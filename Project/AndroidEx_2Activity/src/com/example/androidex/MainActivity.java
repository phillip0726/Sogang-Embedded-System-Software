package com.example.androidex;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageButton;
import android.widget.LinearLayout;



public class MainActivity extends Activity{
	
	LinearLayout linear;
	public native int dotdriveropen();
	public native void dotdriverwrite(int fd, int mode);
	public native void dotdriverclose(int fd);
	int desc;
	static { System.loadLibrary("fnd_driver"); }
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		Intent intent = new Intent(MainActivity.this, SwitchManager.class);
		startService(intent); // switch manager service start
		setContentView(R.layout.activity_main);
		linear = (LinearLayout)findViewById(R.id.container);
		                                             
		/*try{
			File file = new File("engkor.TXT");
			FileReader filereader = new FileReader(file);
			int singlech=0;
			int len=0;
			while((singlech = filereader.read())!=-1){
				System.out.println((char)singlech);
				len++;
			}
			Log.d("len",""+len);
			System.out.println(len);
			filereader.close();
		}catch(FileNotFoundException e){
			
		}catch(IOException e){
			System.out.println(e);
		}*/
		
		
		ImageButton btn1=(ImageButton)findViewById(R.id.newactivity1);
		ImageButton btn2=(ImageButton)findViewById(R.id.newactivity2);
		ImageButton btn3=(ImageButton)findViewById(R.id.newactivity3);
		OnClickListener listener1=new OnClickListener(){
			public void onClick(View v){
				Intent intent=new Intent(MainActivity.this, MainActivity2.class);
				startActivity(intent);	
				desc = dotdriveropen();
				dotdriverwrite(desc,1); // mode 1
				dotdriverclose(desc);
			}
		};
		btn1.setOnClickListener(listener1);
		OnClickListener listener2=new OnClickListener(){
			public void onClick(View v){
				Intent intent=new Intent(MainActivity.this, MainActivity3.class);
				startActivity(intent);
				desc = dotdriveropen();
				dotdriverwrite(desc,2); // mode 2
				dotdriverclose(desc);
			}
		};
		btn2.setOnClickListener(listener2);
		OnClickListener listener3=new OnClickListener(){
			public void onClick(View v){
				Intent intent=new Intent(MainActivity.this, MainActivity4.class);
				startActivity(intent);
				desc = dotdriveropen();
				dotdriverwrite(desc,3); // mode 3
				dotdriverclose(desc);
			}
		};
		btn3.setOnClickListener(listener3);
	}
}
