package com.rmgoncalo;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver {
	
	private static final String tag = "BroadcastReceiver";

	@Override
	public void onReceive(Context context, Intent intent){
		
		if(intent.getAction().equals("com.rmgoncalo.intent.QUERY")){    
			Intent startServiceIntent = new Intent(context, Daemon.class);
			context.startService(startServiceIntent);
			Log.v(tag, "Service started from boot");
		}
	}
}
