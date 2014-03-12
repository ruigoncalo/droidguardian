package com.rmgoncalo.droidg;

import java.io.IOException;
import java.io.OutputStream;

import android.content.Intent;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.FragmentTransaction;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.util.Log;
import android.view.Menu;

public class QueryActivity extends FragmentActivity 
implements OnDialogDoneListener{
	
	private static final String tag = "DG-QueryActivity";
	private static final String internalSocket = "/abstract/internal";
	
	private ClientThread cthread;
	private String address;
	private String port;
	private String pid;
	private String processName;
	private Rule rule;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		Intent intent = getIntent();	
		address = intent.getStringExtra("address");
		port = intent.getStringExtra("port");
		pid = intent.getStringExtra("pid");
		processName = intent.getStringExtra("processName");
			
		Log.v(tag, "onCreate: " + Util.getThreadSignature());
	}
	
	@Override
	protected void onStart() {
		super.onStart();
		showDialog();
	}
	
	@Override
	protected void onStop() {
	    super.onStop();
	}
	  
	@Override
	protected void onDestroy(){
		super.onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	
	private void showDialog(){
		
		DialogWindow dw = DialogWindow.newInstance
				(address, port, pid, processName);
		FragmentManager fm = getSupportFragmentManager();
		FragmentTransaction ft = fm.beginTransaction();
		ft.commit();
		
		dw.show(fm, processName);	
	}
	
	public void onDialogDone(String permission, String lifetime){	
		Log.v(tag, "onDialogDone " + Util.getThreadSignature());
		
		if(! (lifetime.equals(Protocol.ONCE)) )
		{
			rule = new Rule(address, port, pid, processName);
			rule.setPermission(permission);
			rule.setLifetime(lifetime);
			
			RulesList.addRule(rule);
		}
		
		cthread = new ClientThread(permission, lifetime);
		cthread.start();
	}
	   
	/*
	 * ClientThread
	 */
	private class ClientThread extends Thread {
		   
		   String permission;
		   String lifetime;
		   private final Handler handler;
		   private LocalSocket local = new LocalSocket();
		   private OutputStream output;
		   
		   public ClientThread(String permission, String lifetime){
			   handler = new Handler();
			   this.permission = permission;
			   this.lifetime = lifetime;
			   Log.v(tag, "ClientThread: " + Util.getThreadSignature());
		   }
		   
		   private String getResponse(){
			   String res = Protocol.UNDEFINED;
			   
			   if(permission.equals(Protocol.ALLOW))
			   {
				   if(lifetime.equals(Protocol.FOREVER))
					   res = Protocol.ALLOW_FOREVER;
				   else
					   res = Protocol.ALLOW_ONCE;
			   }
			   else
				   if(lifetime.equals(Protocol.FOREVER))
					   res = Protocol.DENY_FOREVER;
				   else
					   res = Protocol.DENY_ONCE;

			   return res;
		   }
		   
		   public void run() {
				onBackground();
				
				handler.post(new Runnable() {
					public void run() {
						onPostExecute();
					}
				});
			}
		   
		   public void onBackground(){
						
				try {		
					LocalSocketAddress addr = 
							new LocalSocketAddress(internalSocket);				
					local.connect(addr);
					output = local.getOutputStream();
					
					String result = getResponse();
					
					output.write(result.getBytes());
					
					output.close();
					local.close();
					
				} catch (IOException e){
					e.printStackTrace();
				}
			}
		   
		   protected void onPostExecute() {
			   Log.v(tag, "onPostExecute");
			   //finish activity
			   finish();
			}   
	   }
}