package com.rmgoncalo.droidg;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Service;
import android.content.Intent;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class Daemon extends Service {

	private static final String tag = "DG-Daemon";
	public static String socketAddress = "/abstract/usocket";
	public static String internalSocket = "/abstract/internal";

	private ClientTask clientTask;
	private ServerTask serverTask;
	private String appOnDialog;

	static {
		System.loadLibrary("dgquery");
	}

	public native void startDaemon() throws Exception;

	@Override
	public void onCreate() {
		super.onCreate();
		Toast.makeText(getApplicationContext(), "Starting DroidGuardian...",
				Toast.LENGTH_SHORT).show();

		serverTask = new ServerTask();
		serverTask.start();
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);

		clientTask = new ClientTask();
		clientTask.start();

		return START_STICKY;
	}

	@Override
	public void onDestroy() {
		Log.d(tag, "in onDestroy().");
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
	@SuppressWarnings("resource")
	private String getResponseFromDialog() throws Exception {

		String result = null;
		int bufferReadSize = 1;
		byte[] bufferRead = new byte[bufferReadSize];

		LocalServerSocket server = null;
		LocalSocket receiver = null;
		InputStream input;

		try {
			server = new LocalServerSocket(internalSocket);
			Log.d(tag, "InternalSocket: localServerSocket");

			receiver = new LocalSocket();
			receiver = server.accept();
			Log.d(tag, "InternalSocket: receiver accepted");

			input = receiver.getInputStream();

			int bytesRead = input.read(bufferRead);
			Log.d(tag, "bytesRead: " + bytesRead);

			result = new String(bufferRead, "UTF-8");

			input.close();

			receiver.close();
			server.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		return result;
	}

	/*
	 * launchActivity
	 */
	private void launchActivity(String address, String port, String pid,
			String processName) {

		Intent intent = new Intent(getBaseContext(), QueryActivity.class);
		intent.putExtra("address", address);
		intent.putExtra("port", port);
		intent.putExtra("pid", pid);
		intent.putExtra("processName", processName);
		intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
		getApplication().startActivity(intent);
	}

	/*
	 * processQuery
	 */
	private String processQuery(String message) {

		String result = null;
		boolean updated_permission = false;
		String processName = null;
		String address = null;
		String port = null;
		String pid = null;
		String permission = null;

		Query query = new Query(message);
		processName = query.getProcessName();
		address = query.getAddress();
		port = query.getPort();
		pid = query.getPid();

		if (processName.equals(appOnDialog)) {
			Log.d(tag, "Application " + processName + " is on dialog ("
					+ appOnDialog + ")");

			while (!updated_permission) {
				permission = RulesList.getPermission(processName);
				Log.d(tag, "permission is : " + permission);
				if (!permission.equals(Protocol.UNDEFINED))
					updated_permission = true;
			}
			updated_permission = false;
			return permission;
		} else {
			Log.d(tag, "Application " + processName + " is not on dialog.");
		}

		if (RulesList.isNewQuery(processName)) {
			appOnDialog = processName;
			launchActivity(address, port, pid, processName);
			try {
				// result = "-1" or "1" or "2" or "3" or "4"
				result = getResponseFromDialog();
			} catch (Exception e) {
				e.printStackTrace();
			}
			appOnDialog = null;
		} else {
			result = RulesList.getPermission(processName);
		}

		return result;
	}

	/*
	 * startLocalServer()
	 */
	private void startLocalServer() throws Exception {

		int bufferReadSize = 128;
		byte[] bufferRead = new byte[bufferReadSize];
		int bytesRead = 0;

		LocalServerSocket server = null;
		LocalSocket receiver = new LocalSocket();
		InputStream input;
		OutputStream output;

		try {
			server = new LocalServerSocket(socketAddress);
		} catch (IOException e) {
			e.printStackTrace();
		}

		try {
			// LocalSocketAddress localSocketAddress =
			// server.getLocalSocketAddress();
			// Log.d(tag, "localSocketAddress:" + localSocketAddress.getName());

			while (true) {
				if (server == null) {
					Log.d(tag, "The localSocketServer is null.");
					break;
				}

				receiver = server.accept();

				input = receiver.getInputStream();
				output = receiver.getOutputStream();

				bytesRead = input.read(bufferRead);

				if (bytesRead >= 0) {
					byte[] aux = new byte[bytesRead];
					for (int i = 0; i < bytesRead; i++) {
						aux[i] = bufferRead[i];
						bufferRead[i] = '\0';
					}

					String str = new String(aux);
					Log.d(tag, "The msg is : " + str);

					String sendtoKernel = processQuery(str);

					Log.d(tag, "sendtoKernel: " + sendtoKernel);

					output.write(sendtoKernel.getBytes());

					bytesRead = 0;
				}

				input.close();
				output.close();
			}

			receiver.close();
			server.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/*
	 * Server task
	 */

	private class ServerTask extends Thread {

		private final Handler handler;

		public ServerTask() {
			this.handler = new Handler();
		}

		public void run() {
			onBackground();

			handler.post(new Runnable() {
				public void run() {
					onPostExecute();
				}
			});
		}

		private void onBackground() {
			Log.d(tag, "ServerTask:" + Util.getThreadSignature());

			try {
				startLocalServer();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		// stop the service
		private void onPostExecute() {
			stopSelf();
		}
	}

	/**
	 * Client task.
	 */
	private class ClientTask extends Thread {

		public void run() {
			Log.d(tag, "ClientTask:" + Util.getThreadSignature());

			try {
				startDaemon();
			} catch (Exception e) {
				e.printStackTrace();
			}

		}
	}

}
