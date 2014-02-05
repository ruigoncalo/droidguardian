package com.rmgoncalo;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Service;
import android.content.Intent;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

public class Daemon extends Service {
	
	private static final String tag = "Daemon";
	public static String socketAddress = "/abstract/usocket";
	public static String internalSocket = "/abstract/internal";
	
	private ClientTask clientTask;
	private ServerTask serverTask;
	private LoadLibrary lb;
	
	@Override
	public void onCreate(){
		super.onCreate();
		lb = new LoadLibrary();
		
		serverTask = new ServerTask();
		serverTask.start();
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId){
		super.onStartCommand(intent, flags, startId);
		
		clientTask = new ClientTask();
		clientTask.start();
		
		return START_STICKY;
	}
	
	@Override
	public void onDestroy(){
		Log.v(tag, "in onDestroy().");
		serverTask.interrupt();
		clientTask.interrupt();
		
		super.onDestroy();
	}
	
	@Override
	public IBinder onBind(Intent arg0) {
		return null;
	}
	
	/*
	 * Internal Socket
	 */
	private String getResponseFromDialog() throws Exception {
		
		String result = null;
		int bufferReadSize = 1;
	    byte[] bufferRead =  new byte[bufferReadSize];
	    
		LocalServerSocket server = null;
		LocalSocket receiver = new LocalSocket();
		InputStream input;
		
		try{
			server = new LocalServerSocket(internalSocket);
			Log.v(tag, "InternalSocket: localServerSocket");
		} catch(IOException e){
			e.printStackTrace();
		}
		
		try{
			receiver = server.accept();
			Log.v(tag, "InternalSocket: receiver accepted");
		} catch (IOException e){
			e.printStackTrace();
		}
		
		try{
			input = receiver.getInputStream();
			
			int bytesRead = input.read(bufferRead);
			Log.v(tag, "bytesRead: " + bytesRead);

			result = new String(bufferRead, "UTF-8");
			
			input.close();
			receiver.close();
			server.close();
		} catch (IOException e){
			e.printStackTrace();
		}
		
		return result;		
	}
	
	/*
	 * startLocalServer()
	 */
	private void startLocalServer() throws Exception {
		
		int bufferReadSize = 128;
	    byte[] bufferRead =  new byte[bufferReadSize];
	    int bytesRead = 0;
	    
	    LocalServerSocket server = null;
	    LocalSocket receiver = new LocalSocket();
	    InputStream input;
	    OutputStream output;
	      
	    try {
	    	server = new LocalServerSocket(socketAddress);
	    } catch (IOException e){
	    	e.printStackTrace();
	    }
	    
		try{
			LocalSocketAddress localSocketAddress = server.getLocalSocketAddress();
			Log.v(tag, "localSocketAddress:" + localSocketAddress.getName());
			
			while(true) {
				if (server == null){
                    Log.v(tag, "The localSocketServer is null.");
                    break;
                }
				
				receiver = server.accept();
				Log.v(tag, "localSocketServer accepted.");
				
				input = receiver.getInputStream();
				output = receiver.getOutputStream();
				
				
					bytesRead = input.read(bufferRead);
					
					if (bytesRead >= 0) {
                        Log.d(tag, "Receive data from socket, bytesRead = " + bytesRead);
                        
                        byte[] aux = new byte[bytesRead];
                        for(int i = 0; i < bytesRead; i ++){
                        	aux[i] = bufferRead[i];
                        	bufferRead[i] = '\0';
                        }
                        
                        String str = new String(aux);
                        
                        Log.d(tag, "The msg is : " + str);
                        
                        launchActivity(str);
                        
                        String sendtoKernel = getResponseFromDialog(); 
                        Log.v(tag, "sendtoKernel: " + sendtoKernel);
                        
                        output.write(sendtoKernel.getBytes());
                        
                        bytesRead = 0;
                    }
					
				input.close();
				output.close();	
			}
			
			receiver.close();
			server.close();
		}
		catch (IOException e){
			e.printStackTrace();
		}
	}
	
	private void launchActivity(String msg){
		Intent intent = new Intent(getBaseContext(), QueryActivity.class);
		intent.putExtra("query", msg);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		getApplication().startActivity(intent);
		Log.v(tag, "launchActivity:" + msg);
	}
	
	/*
	 * Server task
	 */
	
	private class ServerTask extends Thread {
		
		private final Handler handler;
		
		public ServerTask(){
			this.handler = new Handler();
		}
		
		public void run(){
			onBackground();
			
			handler.post(new Runnable() {
				public void run() {
					onPostExecute();
				}
			});
		}
		
		private void onBackground(){
			Log.v(tag, "ServerTask:" + Util.getThreadSignature());
			
			try {
				startLocalServer();
			} catch(Exception e){
				e.printStackTrace();
			}
		}
		
		//stop the service
		private void onPostExecute(){
			stopSelf();
		}
	}
	
	/**
	 * Client task.
	 */
	private class ClientTask extends Thread {	
			
			public void run() {
				Log.v(tag, "ClientTask:" + Util.getThreadSignature());
				
				try {
					lb.startDaemon();
				} catch(Exception e) {
					e.printStackTrace();
				}

			}
	}
		
		
}
