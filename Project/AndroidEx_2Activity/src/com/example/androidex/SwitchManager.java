package com.example.androidex;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class SwitchManager extends Service {
	public native int switchwrite();
	public native void dotwrite(int mode);
	public native void initialtext();
	static { System.loadLibrary("fnd_driver"); }
	
	private boolean finish;
	private int num;
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId){	
		return 0;
	}
	
	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	
	@Override
	public void onCreate(){
		super.onCreate(); 
		Thread check = new Thread(new Runnable(){
			@Override
			public void run() {
				while(true){
					try{
						num = switchwrite(); // get pushed switch number
						if(num == 1){
							dotwrite(1);
							Intent intent = new Intent(SwitchManager.this, MainActivity2.class);
							intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
							startActivity(intent);
						}
						if(num == 2){
							dotwrite(2);
							Intent intent = new Intent(SwitchManager.this, MainActivity3.class);
							intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
							startActivity(intent);
						}
						if(num == 3){
							dotwrite(3);
							Intent intent = new Intent(SwitchManager.this, MainActivity4.class);
							intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
							startActivity(intent);
						}
						if(num == 6){
							dotwrite(4); initialtext();
							Intent intent = new Intent(SwitchManager.this, MainActivity.class);
							stopService(new Intent(SwitchManager.this, MyService.class));
							intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
							startActivity(intent);
						}
						System.out.println(num +" switch pushed");
						Thread.sleep(300); // 0.3 sec
					}
					catch(Throwable t){}	
				}	
			}
		});
		check.start();
	}
	
	@Override
	public void onDestroy(){
		super.onDestroy();
	}
}