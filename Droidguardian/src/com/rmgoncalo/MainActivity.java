package com.rmgoncalo;

import android.os.Bundle;
import android.app.AlertDialog;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;
import android.support.v4.app.DialogFragment;
import android.support.v4.app.FragmentActivity;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.widget.AdapterView.OnItemSelectedListener;


public class MainActivity extends FragmentActivity {
	
	public static Query query = new Query();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		try {
			query.updateVars();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		Button buttonShowDialog = (Button) findViewById(R.id.button_show_dialog);
		buttonShowDialog.setOnClickListener(new OnClickListener() {	
			@Override
			public void onClick(View v) {
				showDialog();
			}
		});
		TextView tv = (TextView) findViewById(R.id.text_query);
		
		tv.setText("Server: " + query.getServer() + "\nPort: " + query.getPort() + "\nPid: " + query.getPid());
	}
	
	private void showDialog(){
		DialogFragment newFragment = ExampleDialogFragment.newInstance();
		newFragment.show(getSupportFragmentManager(), "dialog");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public static String getDialogMessage(){
		return "Application (pid: " + query.getPid() + ") wants to connect to server " + query.getServer() +
				" on port " + query.getPort() + ".";
	}
	
	public static class ExampleDialogFragment extends DialogFragment {
			
		public static ExampleDialogFragment newInstance(){
			ExampleDialogFragment frag = new ExampleDialogFragment();
			return frag;
		}
		
		@Override
		public Dialog onCreateDialog(Bundle savedInstanceState){
			
			AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(getActivity());
			View view = getActivity().getLayoutInflater().inflate(R.layout.dialog_fragment, null);
			Context context = getActivity().getApplicationContext();
			
			Spinner spinner = (Spinner) view.findViewById(R.id.lifetimeSpinner);
			ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(context,
					R.array.lifetime_options, android.R.layout.simple_spinner_item);
			adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
			spinner.setAdapter(adapter);
			spinner.setOnItemSelectedListener(new CustomOnItemSelectedListener());
            
            alertDialogBuilder.setView(view);
            alertDialogBuilder.setTitle(getString(R.string.dialog_title));
            alertDialogBuilder.setMessage(getDialogMessage());
            alertDialogBuilder.setPositiveButton(
            		getString(R.string.dialog_positive_text), new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Toast.makeText(getActivity(), 
                    		getString(R.string.dialog_positive_toast_message), Toast.LENGTH_SHORT).show();
                    query.setPermission(1);
                    dialog.dismiss();
                }
            });
            alertDialogBuilder.setNegativeButton(
            		getString(R.string.dialog_negative_text), new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    Toast.makeText(getActivity(), 
                    		getString(R.string.dialog_negative_toast_message), Toast.LENGTH_SHORT).show();
                    query.setPermission(2);
                    dialog.cancel();
                }
            });
            return alertDialogBuilder.create();
		}
		
		@Override
        public void onSaveInstanceState(Bundle outState) {
            
            super.onSaveInstanceState(outState);
        }
	}
	
	public static class CustomOnItemSelectedListener implements OnItemSelectedListener{
		
		public void onItemSelected(AdapterView<?> parent, View view, int pos,long id) {
			query.setLifetime(pos);
		}
		
		 @Override
		  public void onNothingSelected(AdapterView<?> arg0) {
			// TODO Auto-generated method stub
		  }
		
	}

}
