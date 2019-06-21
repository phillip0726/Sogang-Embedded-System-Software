package com.example.androidex;

import com.example.androidex.R;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import android.widget.LinearLayout;



public class MainActivity extends Activity{
	
	LinearLayout linear;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

	}
	public void btn_listener(View view){
		Intent intent=new Intent(MainActivity.this, MainActivity2.class);
		startActivity(intent);
	}


}
