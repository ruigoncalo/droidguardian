package com.rmgoncalo.droidg;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.Toast;

public class BootActivity extends Activity {
	
	@Override
	public void onCreate(Bundle b){
		super.onCreate(b);
		
		Intent intent = new Intent(this, Daemon.class);
		startService(intent);
		
		Toast.makeText(getApplicationContext(), "Starting DroidGuardian...", Toast.LENGTH_SHORT).show();
		finish();
	}

}
