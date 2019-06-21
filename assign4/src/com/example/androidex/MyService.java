package com.example.androidex;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

public class MyService extends Service {

	Thread counter;
	private class Counter implements Runnable{
		private int sec = 0;
		private int min = 0;
		private Handler handler = new Handler();
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			while(true){
				
				if(isStop){
					break;
				}
				handler.post(new Runnable(){
					@Override
					public void run(){
						String m_s = String.format("%02d", min);
						String s_s = String.format("%02d", sec);
				        final Toast toast = Toast.makeText(getBaseContext(), "" + m_s + ":" + s_s, Toast.LENGTH_SHORT);
				        toast.show();

				        Handler handler = new Handler();
				            handler.postDelayed(new Runnable() {
				               @Override
				               public void run() {
				                   toast.cancel(); 
				               }
				        }, 1000);
					}
				});
				try{
					Thread.sleep(1000);
				} catch(InterruptedException e){
					e.printStackTrace();
				}
				sec++;
				if(sec >= 60){
					min++;
					sec = 0;
				}
				if(min >= 60){
					sec = 0;
				}
			}
		}
		
	}
	public MyService(){
		
	}
	
	@Override
	public void onCreate(){

		super.onCreate();
	}

	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		return null;
	}
	

	@Override
	public int onStartCommand(Intent intent, int flags, int startId){

		counter = new Thread(new Counter());
		counter.start();
		
		
		return START_STICKY;
		
	}
	private boolean isStop;
	
	
	//@SuppressWarnings("deprecation")
	@Override
	public void onDestroy(){
		super.onDestroy();
		isStop=true;
		
	}
	


}
