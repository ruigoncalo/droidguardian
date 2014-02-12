package com.rmgoncalo.droidg;

//import android.graphics.drawable.ColorDrawable;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.support.v4.app.DialogFragment;
import android.view.LayoutInflater;
import android.widget.Spinner;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
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
	
	/*
	@Override
	public void onCreate(Bundle b){
		super.onCreate(b);
		this.setCancelable(true);
		int style = DialogFragment.STYLE_NORMAL;
		int theme = 0;
		setStyle(style, theme);
	}
	*/
	
	@Override
	public Dialog onCreateDialog(Bundle i){
		
		 String processName = getArguments().getString("process");
		 String pid = getArguments().getString("pid");
		 String address = getArguments().getString("address");
		 String port = getArguments().getString("port");
		 
		 AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
		 
		 LayoutInflater inflater = getActivity().getLayoutInflater();
		 View v = inflater.inflate(R.layout.dialog_fragment, null);
		 
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
		 
		 
		 builder.setView(v)
		 .setTitle(R.string.app_name)
		 .setMessage("Process " + processName + " ("+ pid + 
				 	  ") wants to connect to server " + 
					address + " on port " + port + ".")
		 .setPositiveButton(R.string.allow, new DialogInterface.OnClickListener() {
               @Override
               public void onClick(DialogInterface dialog, int id) {
            	    OnDialogDoneListener act = (OnDialogDoneListener)getActivity();
   					act.onDialogDone("Allow", getLifetime());
   					dismiss();
   					return;
               }
           })
           .setNegativeButton(R.string.deny, new DialogInterface.OnClickListener() {
               public void onClick(DialogInterface dialog, int id) {
            	   	OnDialogDoneListener act = (OnDialogDoneListener)getActivity();
   					act.onDialogDone("Deny", getLifetime());
   					dismiss();
   					return;
               }
           });      
		 
		 
		 
		 return builder.create();
	}
	
	/*
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle icicle){
		View v = inflater.inflate(R.layout.dialog_fragment, container, false);
		//getDialog().getWindow().setBackgroundDrawable(new ColorDrawable(0));
		
		String processName = getArguments().getString("process");
		String pid = getArguments().getString("pid");
		String address = getArguments().getString("address");
		String port = getArguments().getString("port");
		
		TextView dialogMessage = (TextView) v.findViewById(R.id.dialogMessage);
		dialogMessage.setText("Process " + processName + " ("+ pid + ") wants to connect to server " + 
							address + " on port " + port + ".");
		
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
		
		Button btnAllow = (Button) v.findViewById(R.id.buttonAllow);
		btnAllow.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				OnDialogDoneListener act = (OnDialogDoneListener)getActivity();
				act.onDialogDone("Allow", getLifetime());
				dismiss();
				return;
			}
		});
		
		Button btnDeny = (Button) v.findViewById(R.id.buttonDeny);
		btnDeny.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				OnDialogDoneListener act = (OnDialogDoneListener)getActivity();
				act.onDialogDone("Deny", getLifetime());
				dismiss();
				return;
			}
		});
		
		return v;
	}
	*/
}
