package com.example.androidex;

import com.example.androidex.R;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.EditText;
import android.widget.LinearLayout.LayoutParams;
import java.util.Random;


public class MainActivity2 extends Activity{
	LinearLayout linear;
	Button[][] mButton = null;

	int[][] random_text;
	EditText data;
	final int WIDTH = 1024;
	final int HEIGHT = 400;
	int row;
	int col;
	int btn_id = 1;

	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		linear = (LinearLayout)findViewById(R.id.container);		
		data=(EditText)findViewById(R.id.editText1);

	}
	@Override
	protected void onDestroy(){
		super.onDestroy();
		stopService(new Intent(this, MyService.class));
	}
	public void btn2_listener(View view){
		String row_col = data.getText().toString();
		
		row = Integer.parseInt(row_col.split(" ")[0]);
		col = Integer.parseInt(row_col.split(" ")[1]);
		
		random_text = new int[row][col];
		for(int i=0;i<row;i++){
			for(int j=0;j<col;j++){
				random_text[i][j] = i * col + j + 1;
			}
		}

		mButton = new Button[row][col];
		while(true){
			shuffle();			
			if(!isfirstEnd()){
				break;
			}
		}
		makeButton();
		startService(new Intent(this, MyService.class));
	}
	protected boolean isfirstEnd(){
		boolean result = true;
		int count = 1;
		for(int i=0;i<row;i++){
			for(int j=0;j<col;j++){
				if(count != random_text[i][j]){
					result = false;
					break;
				}
				count++;
			}
		}
		return result;
	}
	protected void shuffle(){
		Random random = new Random();

		int black_r = row - 1;
		int black_c = col - 1;
		
		for(int i=0;i<1000;i++){
			int tmp;
			int flag = random.nextInt(4);
			switch(flag){
				case 0:				//  up
					if(black_r - 1 >= 0){
						tmp = random_text[black_r-1][black_c];
						random_text[black_r-1][black_c] = random_text[black_r][black_c];
						random_text[black_r][black_c] = tmp;
						black_r = black_r - 1;
					}
					break;
				case 1:				// down
					
					if(black_r + 1 < row){
						tmp = random_text[black_r+1][black_c];
						random_text[black_r+1][black_c] = random_text[black_r][black_c];
						random_text[black_r][black_c] = tmp;
						black_r = black_r + 1;
					}					
					break;
				case 2:				// right
					if(black_c + 1 < col){
						tmp = random_text[black_r][black_c+1];
						random_text[black_r][black_c+1] = random_text[black_r][black_c];
						random_text[black_r][black_c] = tmp;
						black_c = black_c + 1;
					}
					break;
				case 3:				// left
					if(black_c - 1 >= 0){
						tmp = random_text[black_r][black_c-1];
						random_text[black_r][black_c-1] = random_text[black_r][black_c];
						random_text[black_r][black_c] = tmp;
						black_c = black_c - 1;
					}
					break;
			}
		}
		
		
	}

	protected void makeButton(){

		for(int i=0;i<row;i++){
			LinearLayout l = new LinearLayout(this);

			LayoutParams pm = new LayoutParams(
					LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT
			);
			
			l.setLayoutParams(pm);			
			for(int j=0;j<col;j++){
				int idx = i*col + j;
				mButton[i][j] = new Button(this);
				LayoutParams btm_pm = new LayoutParams( WIDTH / col, HEIGHT / row);
				mButton[i][j].setLayoutParams(btm_pm);
				mButton[i][j].setId(idx);
				
				OnClickListener listener2=new OnClickListener(){
					public void onClick(View v){
						swapButton(v.getId());
						if(isEnd()){
							stopService(new Intent(MainActivity2.this, MyService.class));
							Intent intent=new Intent(MainActivity2.this, MainActivity.class);
							startActivity(intent);
							MainActivity2.this.finish();
						}
					}
				};
				if(random_text[i][j] == row * col){
					mButton[i][j].setBackgroundColor(Color.BLACK);
				}

				mButton[i][j].setText("" + random_text[i][j] );
				btn_id++;
				mButton[i][j].setOnClickListener(listener2);
				
				l.addView(mButton[i][j]);
			}
			linear.addView(l);
		}
	}
	
	@SuppressLint("NewApi")
	protected void swapButton(int v_id){
		int i = (int)(v_id / col);
		int j = (int)(v_id % col);
		
		if(i - 1 >= 0){
			
			if(Integer.parseInt((String) mButton[i-1][j].getText()) == (row * col)){

				CharSequence tmp = mButton[i-1][j].getText();
				
				mButton[i-1][j].setBackground(mButton[i][j].getBackground());
				mButton[i-1][j].setText(mButton[i][j].getText());
				
				mButton[i][j].setText(tmp);
				mButton[i][j].setBackgroundColor(Color.BLACK);
				return;
			}
		}
		if(i + 1 < row){
			if(Integer.parseInt((String) mButton[i+1][j].getText()) == (row * col)){
				CharSequence tmp = mButton[i+1][j].getText();
				mButton[i+1][j].setBackground(mButton[i][j].getBackground());
				mButton[i+1][j].setText(mButton[i][j].getText());
				
				mButton[i][j].setText(tmp);
				mButton[i][j].setBackgroundColor(Color.BLACK);
				return;
			}			
		}
		if(j - 1 >= 0){
			if(Integer.parseInt((String) mButton[i][j-1].getText()) == (row * col)){
				CharSequence tmp = mButton[i][j-1].getText();
				mButton[i][j-1].setBackground(mButton[i][j].getBackground());
				mButton[i][j-1].setText(mButton[i][j].getText());
				
				mButton[i][j].setText(tmp);
				mButton[i][j].setBackgroundColor(Color.BLACK);
				return;
			}			
		}
		if(j + 1 < col){
			if(Integer.parseInt((String) mButton[i][j+1].getText()) == (row * col)){
				CharSequence tmp = mButton[i][j+1].getText();
				mButton[i][j+1].setBackground(mButton[i][j].getBackground());
				mButton[i][j+1].setText(mButton[i][j].getText());
				
				mButton[i][j].setText(tmp);
				mButton[i][j].setBackgroundColor(Color.BLACK);
				return;
			}				
		}
	}
	
	protected boolean isEnd(){
		int count = 1;
		boolean result = true;
		for(int i=0;i<row;i++){
			for(int j=0;j<col;j++){
				int num = Integer.parseInt((String)mButton[i][j].getText());
				if(num != count){
					result = false;
				}
				count++;
			}
		}
		return result;
	}
}
