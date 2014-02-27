package com.rmgoncalo.droidg;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.AdapterView.OnItemSelectedListener;

public class DialogWindow extends DialogFragment {
	
	private Spinner spinner;
	private String lifetime;

	public static DialogWindow newInstance(String address, String port, String pid, String process){
		DialogWindow dw = new DialogWindow();
		Bundle b = new Bundle();
		b.putString("address", address);
		b.putString("port", port);
		b.putString("pid", pid);
		b.putString("process", process);
		dw.setArguments(b);
		return dw;
	}

	public String getLifetime(){
		return this.lifetime;
	}
	
	public void setLifetime(String s){
		this.lifetime = s;
	}
	
	
	public Dialog onCreateDialog(Bundle i){
		AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
		
		LayoutInflater inflater = getActivity().getLayoutInflater();
		View v = inflater.inflate(R.layout.dialog_fragment, null);
		
		String processName = getArguments().getString("process");
		String pid = getArguments().getString("pid");
		String address = getArguments().getString("address");
		String port = getArguments().getString("port");
		String message = "Process " + processName + 
						 " ("+ pid + ") wants to connect to address " + 
						 address + " on port " + port + ".";
		
		spinner = (Spinner) v.findViewById(R.id.lifetimeSpinner);
		ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(
													getActivity(), 
													R.array.lifetime_options,
													android.R.layout.simple_spinner_item);
		adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		spinner.setAdapter(adapter);
		
		spinner.setOnItemSelectedListener(new OnItemSelectedListener() {
			
			public void onItemSelected(AdapterView<?> parent, View view, int pos, long id) {
		        setLifetime(parent.getItemAtPosition(pos).toString());
		    }

		    public void onNothingSelected(AdapterView<?> parent) {
		        setLifetime("Forever");
		    }
		});
		
		builder.setView(v).
		setIcon(R.drawable.ic_launcher).
		setTitle(R.string.app_name).
		setMessage(message).
		setPositiveButton(R.string.allow, new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				OnDialogDoneListener act = (OnDialogDoneListener)getActivity();
				act.onDialogDone("Allow", getLifetime());
				dismiss();
				return;
				
			}
		}).
		setNegativeButton(R.string.deny, new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				OnDialogDoneListener act = (OnDialogDoneListener)getActivity();
				act.onDialogDone("Deny", getLifetime());
				dismiss();
				return;
				
			}
		});
		
		return builder.create();
		
	}
}
