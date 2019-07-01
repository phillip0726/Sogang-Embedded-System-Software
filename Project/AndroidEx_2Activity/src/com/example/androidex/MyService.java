package com.example.androidex;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Handler;
import android.os.IBinder;
import android.view.Gravity;
import android.widget.TextView;
import android.widget.Toast;

public class MyService extends Service {
	public native void textdriverwrite(String val);
	public native void fnddriverwrite(int sec);
	public native void initialtext();
	static { System.loadLibrary("fnd_driver"); }
	
	private boolean finish;
	private int idx;
    int strlen,i,j;
	String voca[];
	Toast toast;
	@Override
	public int onStartCommand(Intent intent, int flags, int startId){	
		voca = intent.getExtras().getStringArray("array");
		strlen = intent.getExtras().getInt("len");
		System.out.println(strlen);
		return startId;
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	@Override
	public void onCreate(){
		super.onCreate(); 
		Thread clock = new Thread(new Timer());
		clock.start();
	}
	public class Timer implements Runnable {
		private Handler handler = new Handler();
		public void setCustomToast(Context context, String msg){
		    //백그라운드 변경
		    TextView view = new TextView(context);
		    view.setText(msg);
		    view.setBackgroundColor(context.getResources().getColor(android.R.color.white));
		    view.setTextColor(Color.BLACK);
		    view.setTextSize(80);
		    
		    toast = Toast.makeText(context, "",Toast.LENGTH_SHORT);
		    toast.setView(view);
		    toast.setGravity(Gravity.CENTER, 0, 0);
		    toast.show();
		}
		
		@Override
		public void run(){
			while(true){ // per voca 3 times ==> strlen*3
				if(finish) break;
				handler.post(new Runnable(){
					@Override
					public void run(){
						j=voca.length; // 0 case think
						fnddriverwrite(i);
						if(i==0) setCustomToast(getApplicationContext(), "암기모드");
						else if(i%2==0){
							if(idx%j==0) idx = 0;
							setCustomToast(getApplicationContext(), voca[idx]);
							if(idx%2==0) 
								textdriverwrite(voca[idx]); // only eng
							if(i%6==0 && i!=0)
								idx++;
						}
						else toast.cancel();
					}
				});
				try{ 
					Thread.sleep(1000);
					i++;
				}
				catch(InterruptedException e){e.printStackTrace();}
			}
		}
	}
	
	@Override
	public void onDestroy(){
		finish = true;
		toast.cancel();
		fnddriverwrite(0);
		initialtext();
		stopService(new Intent(MyService.this, MainActivity3.class));
		super.onDestroy();
	}
}
