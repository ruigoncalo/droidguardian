package com.rmgoncalo.droidg;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootReceiver extends BroadcastReceiver {
	
	private static final String tag = "BroadcastReceiver";

	@Override
	public void onReceive(Context context, Intent intent){
	
        if (Intent.ACTION_BOOT_COMPLETED.equals(intent.getAction())) {
            Intent startService = new Intent(context, Daemon.class);
            context.startService(startService);
            Log.v(tag, "Service started from boot");
        }
	}

}
