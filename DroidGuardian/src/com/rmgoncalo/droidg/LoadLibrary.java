package com.rmgoncalo.droidg;

public class LoadLibrary {
	
	static {
		System.loadLibrary("dgquery");
	}
	
	public native void startDaemon() throws Exception;
}
