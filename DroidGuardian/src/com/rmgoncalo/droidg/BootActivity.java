package com.rmgoncalo.droidg;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class BootActivity extends Activity {

	@Override
	public void onCreate(Bundle b) {
		super.onCreate(b);

		// starting service from activity
		Intent intent = new Intent(this, Daemon.class);
		startService(intent);

		finish();
	}

}
