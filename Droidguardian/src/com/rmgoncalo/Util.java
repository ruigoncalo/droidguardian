package com.rmgoncalo;

import android.os.Bundle;

public class Util {
	
	public static long getThreadId(){
		Thread t = Thread.currentThread();
		return t.getId();
	}
	
	public static String getThreadSignature(){
		Thread t = Thread.currentThread();
		long l = t.getId();
		String name = t.getName();
		long p = t.getPriority();
		String gname = t.getThreadGroup().getName();
		return (name 
				+ ":(id)" + l 
				+ ":(priority)" + p
				+ ":(group)" + gname);
	}
	
	public static void sleepForInSecs(int secs){
		try {
			Thread.sleep(secs * 1000);
		} catch (InterruptedException e){
			throw new RuntimeException("interrupted", e);
		}
	}
	
	public static Bundle getStringAsBundle(String message){
		Bundle b = new Bundle();
		b.putString("message", message);
		return b;
	}
	
	public static String getStringFromBundle(Bundle b){
		return b.getString("message");
	}

}
