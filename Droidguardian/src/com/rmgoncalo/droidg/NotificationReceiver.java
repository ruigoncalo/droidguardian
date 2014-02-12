
// http://developer.android.com/guide/topics/ui/notifiers/notifications.html
// http://developer.android.com/reference/android/support/v4/app/NotificationCompat.Builder.html

package com.rmgoncalo.droidg;

import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.support.v4.app.NotificationCompat;
import android.support.v4.app.TaskStackBuilder;
import android.util.Log;

public class NotificationReceiver extends BroadcastReceiver {
	
	private static final String tag = "NotificationReceiver";
	
	@Override
	public void onReceive(Context context, Intent intent)
	{
		int id = 0;
		Util.getThreadSignature();
		Log.d(tag, "intent=" + intent);
		int server = intent.getIntExtra("server", 0);
		int port = intent.getIntExtra("port",0);
		int pid = intent.getIntExtra("pid", 0);
		Log.d(tag, server + ":" + port + ":" + pid);
		this.sendNotification(context, server, port, pid, id);
	}
	
	private void sendNotification(Context ctx, int server, int port, int pid, int mId){
		
		NotificationCompat.Builder builder = new NotificationCompat.Builder(ctx)
		.setSmallIcon(R.drawable.ic_launcher)
		.setContentTitle("Droidguardian")
		.setContentText(String.valueOf(server) + ":" + String.valueOf(port) + ":" + String.valueOf(pid))
		.setAutoCancel(true);
		
		Intent resultIntent = new Intent(ctx, QueryActivity.class);
		
		TaskStackBuilder stackBuilder = TaskStackBuilder.create(ctx);
		stackBuilder.addParentStack(QueryActivity.class);
		stackBuilder.addNextIntent(resultIntent);
		
		PendingIntent resultPendingIntent =
		        stackBuilder.getPendingIntent(0, PendingIntent.FLAG_UPDATE_CURRENT);
		builder.setContentIntent(resultPendingIntent);
		
		// passing the notification to the system
		NotificationManager mNotificationManager =
			    (NotificationManager) ctx.getSystemService(Context.NOTIFICATION_SERVICE);
		mNotificationManager.notify(mId, builder.build());		
	}

}
