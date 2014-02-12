package com.rmgoncalo.droidg;

import java.net.InetAddress;
import java.net.UnknownHostException;

import android.util.Log;

public class Network {
	
	public static String getStringToHostname (String str){
		
		InetAddress inetAddress = null;
		String hostname = new String();
		
		Log.v("Network", "what's up?");
		
		try{
			inetAddress = InetAddress.getByName(str);
			hostname = inetAddress.getHostName();
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
		
		if(hostname.equals(null)){
			hostname = str;
		}
		
		Log.v("Network", "network: " + hostname);
	    return hostname;
	}

}
