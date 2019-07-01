package com.example.androidex;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.annotation.SuppressLint;
import android.app.ActionBar.LayoutParams;
import android.app.Activity;
import android.media.MediaPlayer;
import android.os.AsyncTask;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;



@SuppressLint("NewApi")
public class MainActivity2 extends Activity implements OnInitListener{
	LinearLayout linear;
	LinearLayout slinear;
	LinearLayout rlinear;
	EditText data;
	TextView tv;
	TextView tvd[];
	Button btn;
	ImageButton btnf[];
	Button btns[];
	OnClickListener ltn;
	int i,j,flag=0;
	String tmp;
	int fd1;
	String s[];
	public native int textdriveropen();
	public native void textdriverwrite(int fd, String val);
	public native void textdriverclose(int fd);
	public native void initialdot();
	public native void initialtext();
	MediaPlayer mp1, mp2;
	private TextToSpeech myTTS;
	static { System.loadLibrary("fnd_driver"); }
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main2);
		linear = (LinearLayout)findViewById(R.id.container);
		
		data=(EditText)findViewById(R.id.editText1);
		slinear=new LinearLayout(MainActivity2.this);
		s= new String[16];
		
		Button btn=(Button)findViewById(R.id.button1);
		btnf = new ImageButton[4];
		tvd = new TextView[8];
		btns = new Button[4];
		myTTS = new TextToSpeech(this, this);
		ltn=new OnClickListener(){
			public void onClick(View v){
				tmp=data.getText().toString(); // get string
				fd1 = textdriveropen();
				textdriverwrite(fd1,tmp);
				textdriverclose(fd1);
				if(flag==0)
				{
					//flag=1;
					new HttpTask().execute("http://18.191.215.78:3000/send?eng="+data.getText().toString());
//					try {
//						Thread.sleep(5000);
//					} catch (InterruptedException e) {
//						Log.d("error",""+e);
//						e.printStackTrace();
//					}
					slinear.removeAllViews();
					slinear=new LinearLayout(MainActivity2.this);
					slinear.setOrientation(LinearLayout.VERTICAL);
					/*LinearLayout.LayoutParams sparams = new LinearLayout.LayoutParams(
						LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,(float)1.0);
					slinear.setLayoutParams(sparams);*/
					/*setContentView(R.layout.activity_main2);
					linear = (LinearLayout)findViewById(R.id.container);
					data=(EditText)findViewById(R.id.editText1);
					tv=(TextView)findViewById(R.id.textView1);
					Button btn=(Button)findViewById(R.id.button1);*/
					for(i=0;i<4;i++)
					{
						rlinear=new LinearLayout(MainActivity2.this);
						LinearLayout.LayoutParams rparams = new LinearLayout.LayoutParams(
							LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT,(float)1.0);
						rlinear.setLayoutParams(rparams);
						btnf[i]=new ImageButton(MainActivity2.this);
						btnf[i].setId(i);
						LinearLayout.LayoutParams col_param = new LinearLayout.LayoutParams(
							LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT,(float) 1.0);
						btnf[i].setLayoutParams(col_param);
						btnf[i].setScaleType(ImageButton.ScaleType.FIT_XY);
						
						btnf[i].setLayoutParams(new LayoutParams(1024/6, 400/4));
						
						btnf[i].setBackgroundResource(R.drawable.btn_star);
						
						rlinear.addView(btnf[i]);
					
						btnf[i].setOnClickListener(new View.OnClickListener() {
				        
							public void onClick(View v) {
								int i=v.getId();
								if(!btnf[i].isSelected())
								{
									btnf[i].setSelected(true);
									Log.d("123","http://18.191.215.78:3000/star?idx="+s[i*4+3]+"&star=1");
									new HttpTask().execute("http://18.191.215.78:3000/star?idx="+s[i*4+3]+"&star=1");
								}
								else
								{
									btnf[i].setSelected(!btnf[i].isSelected());
									new HttpTask().execute("http://18.191.215.78:3000/star?idx="+s[i*4+3]+"&star=0");
								}
							}
						});
					
						tvd[i*2]=new TextView(MainActivity2.this);
						tvd[i*2].setText("");
						tvd[i*2].setTextSize(20);
						tvd[i*2].setGravity(Gravity.CENTER_VERTICAL);
						LinearLayout.LayoutParams col_param2 = new LinearLayout.LayoutParams(
							LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT,(float) 2.5);
						tvd[i*2].setLayoutParams(col_param2);
						tvd[i*2].setLayoutParams(new LayoutParams(1024/3, 400/4));
						rlinear.addView(tvd[i*2]);
						
						tvd[i*2+1]=new TextView(MainActivity2.this);
						tvd[i*2+1].setText("");
						tvd[i*2+1].setTextSize(20);
						tvd[i*2+1].setGravity(Gravity.CENTER_VERTICAL);
						LinearLayout.LayoutParams col_param1 = new LinearLayout.LayoutParams(
							LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT,(float) 2.5);
						tvd[i*2+1].setLayoutParams(col_param1);
						tvd[i*2+1].setLayoutParams(new LayoutParams(1024/3, 400/4));
						rlinear.addView(tvd[i*2+1]);
					
						btns[i]=new Button(MainActivity2.this);
						btns[i].setId(i);
						LinearLayout.LayoutParams col_param3 = new LinearLayout.LayoutParams(
							LayoutParams.WRAP_CONTENT, LayoutParams.MATCH_PARENT,(float) 1.0);
						btns[i].setLayoutParams(col_param3);
						btns[i].setBackgroundResource(R.drawable.btn_speaker);
						btns[i].setLayoutParams(new LayoutParams(1024/6, 400/4));
						
						btns[i].setOnClickListener(new View.OnClickListener() {
					        
							public void onClick(View v) {
								int i=v.getId();
								myTTS.speak(s[4*i], TextToSpeech.QUEUE_FLUSH, null);
								System.out.println("abcde");
							}
						});
						
						rlinear.addView(btns[i]);
						slinear.addView(rlinear);
									
					}
					linear.addView(slinear);
				}
				//String temp=data.getText().toString();
				//tv.setText(temp);
				//new HttpTask().execute("http://18.191.215.78:3000/send?eng=apple");
			}
		};
		btn.setOnClickListener(ltn);
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
		  // System.out.println(""+result+"123"+result);
		  if(result.contains("///"))
		  {
			  for(int i=0;i<4;i++)
			  {
				  s[i*4+3]=result.split("!!!")[i];
				  Log.d("!!!",""+s[i*4+3]);
				  for(j=0;j<4;j++)
				  {
					  s[j+i*4]=s[i*4+3].split("///")[j];
					  Log.d("///",""+s[j+i*4]);
				  }
			  }
			  s[15]=s[15].split("\n")[0];
			  for(int i=0;i<4;i++)
			  {
				  tvd[i*2].setText("   "+s[i*4]);
				  tvd[i*2+1].setText("   "+s[i*4+1]);
				  if(s[i*4+2].equals("1"))
					  btnf[i].setSelected(true);
			  }
		  // new HttpTask().execute("http://18.191.215.78:3000?eng=aaa&star=0");
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
		initialtext();
		//myTTS.shutdown();
	}
	@Override
	public void onInit(int status) {
		// TODO Auto-generated method stub
		
	}
}
