package com.rmgoncalo;

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

public class QueryActivity extends FragmentActivity implements OnDialogDoneListener{
	
	private static final String tag = "QueryActivity";
	private static final String internalSocket = "/abstract/internal";
	private ClientThread cthread;
	private Query query;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		Intent intent = getIntent();	
		query = new Query(intent.getStringExtra("query"));
			
		Log.v(tag, "onCreate: " + Util.getThreadSignature());
	}
	
	@Override
	protected void onStart() {
		super.onStart();
        Log.v(tag, "onStart");
   
        showDialog();
	}

	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void showDialog(){
		
		DialogWindow dw = DialogWindow.newInstance(
				query.getAddress(), 
				query.getPort(), 
				query.getPid(), 
				query.getProcessName());
		FragmentManager fm = getSupportFragmentManager();
		FragmentTransaction ft = fm.beginTransaction();
		ft.commit();
		
		dw.show(fm, "dialog-tag");
		
	}
	
	public void onDialogDone(String permission, String lifetime){	
		
		Log.v(tag, "onDialogDone " + Util.getThreadSignature());
		cthread = new ClientThread(permission, lifetime);
		cthread.start();
	}
	
	   
	   @Override
	   protected void onStop() {
		Log.v(tag, "onStop");
	    super.onStop();
	   }
	  
	   @Override
	   protected void onDestroy(){
		   Log.v(tag, "onDestroy");
		   super.onDestroy();
	   }
	   
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
		   
		   private Integer checkResponse(){
			   Integer res = 0;
			   
			   if (permission.equals("Allow")){
				   if(lifetime.equals("Forever")){
					   res = 1;
				   }
				   else {
					   if(lifetime.equals("Once")){
						   res = 2;
					   }
					   else {
						   //error
						   res = 0;
					   }
				   }
			   }
			   else {
				   if(permission.equals("Deny")){
					   if(lifetime.equals("Forever")){
						   res = 3;
					   }
					   else {
						   if(lifetime.equals("Once")){
							   res = 4;
						   }
						   else {
							   //error
							   res = 0;
						   }
					   }
				   } else {
					   //error
					   res = 0;
				   }
			   }
			   
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
					LocalSocketAddress addr = new LocalSocketAddress(internalSocket);
														
					local.connect(addr);
					
					output = local.getOutputStream();
					
					Integer result = checkResponse();
					
					output.write(result.toString().getBytes());
					
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