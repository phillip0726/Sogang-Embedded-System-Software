package com.example.androidex;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.logging.Log;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity3 extends Activity{
	TextView tv; 
	String favor = "apple///사과!!!banana///바나나!!!cake///케이크!!!dragon///용!!!elephant///코끼리!!!fire///불!!!garbage///쓰레기!!!hot///더운!!!ignore///무시하다!!!jinx///징크스!!!king///왕!!!love///사랑!!!move///움직이다!!!novel///소설";
	String afterFavor[];
	int favorlen = 0;
	
	public native void initialdot();
	public native void dotwrite(int mode);
	static { System.loadLibrary("fnd_driver"); }
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		//dotwrite(2);
		super.onCreate(savedInstanceState);
		new HttpTask().execute("http://18.191.215.78:3000/bookmark");
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
//		getstrArray();
//		Intent intent = new Intent(MainActivity3.this, MyService.class);
//		intent.putExtra("array",afterFavor);
//		intent.putExtra("len", favorlen);
//		startService(intent); // service start
		setContentView(R.layout.activity_main3);
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
			favor = result;
			System.out.println("executed");
			getstrArray();
			Intent intent = new Intent(MainActivity3.this, MyService.class);
			intent.putExtra("array",afterFavor);
			intent.putExtra("len", favorlen);
			startService(intent); // service start
		}
	}
	public InputStream getInputStreamFromUrl(String url) 
	{
		InputStream content = null;
		try{
			HttpClient httpclient = new DefaultHttpClient();
			HttpResponse response = httpclient.execute(new HttpGet(url));
			content = response.getEntity().getContent();
		}
		catch (Exception e) {
			System.out.println(e);
		}
		return content;
	}
	
	public void getstrArray(){
		Pattern pattern = Pattern.compile("///");            
	    Matcher matcher = pattern.matcher(favor);
	    while (matcher.find()) {
	    	favorlen++;
	    }
	    if(favorlen == 0) afterFavor =  null ;
	    else afterFavor = favor.split("!!!|///");
	}	
	public void onDestroy(){
		super.onDestroy();
		stopService(new Intent(MainActivity3.this, MyService.class));
		initialdot();
	}
}
