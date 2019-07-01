package com.example.androidex;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Timer;
import java.util.TimerTask;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import com.example.androidex.R;
//import com.example.androidex.MainActivity.HttpTask;

import android.annotation.SuppressLint;
import android.app.ActionBar.LayoutParams;
import android.app.Activity;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TableLayout;
import android.widget.TextView;



public class MainActivity4 extends Activity{
	private static final Context MainActivity4 = null;
	LinearLayout linear;
	LinearLayout slinear;
	LinearLayout rlinear;
	OnClickListener ltn;
	Timer timer;
	TimerTask timerTask;
	TextView tv[];
	ImageButton bt;
	String s1,s2;
	int getmessage=0;
	public native void initialdot();
	static { System.loadLibrary("fnd_driver"); }
	@SuppressLint("NewApi")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main4);
		linear = (LinearLayout)findViewById(R.id.container);
		
		
		rlinear=new LinearLayout(MainActivity4.this);
		rlinear.setOrientation(LinearLayout.HORIZONTAL);
		
		
		tv=new TextView[4];
		for(int i=0; i<4; i++)
		{
			tv[i]=new TextView(MainActivity4.this);
		}
		tv[0].setGravity(Gravity.CENTER_VERTICAL|Gravity.CENTER_HORIZONTAL);
		tv[0].setTextSize(20);
		tv[0].setText("");
		tv[1].setGravity(Gravity.CENTER);
		tv[1].setTextSize(20);
		tv[1].setText("");
		tv[2].setText("\n번역하고 싶은 문장\n의 사진을 찍으세요");
		tv[2].setTextSize(30);
		tv[3].setText("Take a picture of\n the sentence you\n want to translate");
		tv[3].setTextSize(30);
		bt=new ImageButton(MainActivity4.this);
		tv[0].setLayoutParams(new LayoutParams(1024/8*3, 512));
		/*LinearLayout.LayoutParams params1 = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,(float)1.0);
		rlinear.setLayoutParams(params1);*/
		rlinear.addView(tv[0]);
		
		slinear=new LinearLayout(MainActivity4.this);
		slinear.setLayoutParams(new TableLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT, (float)1.0));
		LinearLayout.LayoutParams params2 = new LinearLayout.LayoutParams(
			LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,(float)1.0);
		slinear.setLayoutParams(params2);
		
		slinear.setOrientation(LinearLayout.VERTICAL);
		tv[2].setLayoutParams(new LayoutParams(1024/8*3, 400/3));
		slinear.addView(tv[2]);
		
		bt.setLayoutParams(new TableLayout.LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT, 1f));
		bt.setScaleType(ImageButton.ScaleType.FIT_XY);
		bt.setBackgroundResource(R.drawable.btn_picture);
		ltn=new OnClickListener(){
			public void onClick(View v){
			
				Log.d("clkb4msg",""+getmessage);
				//if(getmessage==0)
				getmessage=0;
				{
					Log.d("getmsg=0","http://18.191.215.78:3000/press");
					new HttpTask().execute("http://18.191.215.78:3000/press");	//take a picture (have to change)
//				while(true)
//				{
////					try {
//						tv[2].setText("\n번역중입니다... \n잠시만 기다려 주세요");
//						tv[3].setText("Translating... \nwait a second \nplease");
////						Thread.sleep(25000);
////					} catch (InterruptedException e) {
////						// TODO Auto-generated catch block
////						e.printStackTrace();
////					}
//					new HttpTask().execute("http://18.191.215.78:3000/");
//					if(getmessage==1)
//					{
//						break;
//					}
//				}
//				getmessage=0;
					Log.d("b4","b4");
//					try {
//						Thread.sleep(2000);
//					} catch (InterruptedException e) {
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
					Log.d("a4","a4");
					timer = new Timer();
					timerTask=new TimerTask(){
						public void run(){
							MainActivity4.this.runOnUiThread(new Runnable(){
								public void run(){
									tv[2].setText("\n번역중입니다... \n잠시 기다려 주세요");
									tv[3].setText("Translating... \nwait a second \nplease");
									bt.setBackgroundResource(R.drawable.loading);
									new HttpTask().execute("http://18.191.215.78:3000/");
									if(getmessage==1)
									{
										getmessage=0;
										tv[2].setText("\n번역하고 싶은 문장\n의 사진을 찍으세요");
										tv[3].setText("Take a picture of\n the sentence you\n want to translate");
										bt.setBackgroundResource(R.drawable.btn_picture);
										Log.d("clka4",""+getmessage);
										timer.cancel();
									}
								}
							});
						}
					};
					timer.schedule(timerTask,0,2000);
				
					//tv[2].setText("\n번역하고 싶은 문장\n의 사진을 찍으세요");
					//tv[3].setText("Take a picture of\n the sentence you\n want to translate");
				}
			}
		};
		bt.setOnClickListener(ltn);
		
		slinear.addView(bt);
		
		tv[3].setLayoutParams(new LayoutParams(1024/8*3, 400/3));
		slinear.addView(tv[3]);
		
		LinearLayout.LayoutParams params3 = new LinearLayout.LayoutParams(
			LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,(float)1.0);
		rlinear.setLayoutParams(params3);
		rlinear.addView(slinear);
		
		tv[1].setLayoutParams(new LayoutParams(1024/8*3, 512));
		/*LinearLayout.LayoutParams params4 = new LinearLayout.LayoutParams(
				LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,(float)1.0);
			rlinear.setLayoutParams(params4);*/
		rlinear.addView(tv[1]);
		linear.addView(rlinear);
	}
	
	private static String convertStreamToString(InputStream is)
    {
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        StringBuilder sb = new StringBuilder();
        String line = null;
        try
        {
            while ((line = reader.readLine()) != null)
            {
                sb.append(line + "\n");
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
        finally
        {
            try
            {
                is.close();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }
        return sb.toString();
    }

	class HttpTask extends AsyncTask<String , Void , String> {
	  protected String doInBackground(String... params)
	  {
	    InputStream is = getInputStreamFromUrl(params[0]);
	    String result = convertStreamToString(is);

	    return result;
	  }
	  protected void onPostExecute(String result)
	  {
		  int msg=1;
		  String msg1,msg2,msg3;
		  System.out.println(""+result);
		  msg1=result.split("///")[0];
		  s1=result.split("///")[1];
		  s2=result.split("///")[2];
		  Log.d("onpostmsg1",""+msg1);
		  Log.d("s1",""+s1);
		  Log.d("s2",""+s2);
		  //split
		  //tv[0].setText(""+s1);
		  //tv[1].setText(""+s2);
		  if(msg1.equals("0"))
		  {
			  tv[0].setText(""+s1);
			  tv[1].setText(""+s2);
			  getmessage=1;
			  Log.d("onpostmsg2",""+getmessage);
		  }
	   }

	}
	public InputStream getInputStreamFromUrl(String url) {
	  InputStream content = null;
	  try{
	    HttpClient httpclient = new DefaultHttpClient();
	    HttpResponse response = httpclient.execute(new HttpGet(url));
	    content = response.getEntity().getContent();
	  } catch (Exception e) {
	    Log.d("[GET REQUEST]", "Network exception", e);
	  }
	  return content;
	}
	
	public void onDestroy(){
		super.onDestroy();
		initialdot();
	}
}
