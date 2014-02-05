package com.rmgoncalo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Toast;

public class BootActivity extends Activity {
	
	@Override
	public void onCreate(Bundle b){
		super.onCreate(b);
		
		Intent intent = new Intent();
		intent.setAction("com.rmgoncalo.intent.QUERY");
		sendBroadcast(intent);
		
		Toast.makeText(getApplicationContext(), "Starting DroidGuardian...", Toast.LENGTH_SHORT).show();
		finish();
	}

}
