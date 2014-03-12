package com.rmgoncalo.droidg;

import java.util.Comparator;
import java.util.Map;
import java.util.TreeMap;

import android.util.Log;

public class RulesList {
	
	private static final String tag = "DG-RulesList";
	
	public static Map<String, Rule> rules = 
			new TreeMap<String, Rule>(new Comparator<String>() {
		 public int compare(String o1, String o2) {
		        return o1.compareTo(o2);
		    }
	});
	
	public static void addRule(Rule r){
		rules.put(r.getProcessName(), r);
		Log.v(tag, "Added new rule:\n Name " + r.getProcessName() +
								  "\n Pid: " + r.getPid() +
								  "\n Address: " + r.getAddress() +
								  "\n Port: " + r.getPort() +
								  "\n Permission: " + r.getPermission() +
								  "\n Lifetime: " + r.getLifetime());
	}
	
	public static Rule getRule(Rule r){
		
		for(Map.Entry<String, Rule> e : rules.entrySet()){
			if(e.getKey().equals(r.getProcessName()))
				return e.getValue();
		}
		return null;
	}
	
	public static boolean isNewQuery(String processName){
		
		Log.v(tag, "Searching for " + processName);
		for(Map.Entry<String, Rule> e : rules.entrySet()){
			if(e.getKey().equals(processName))
				return false;
		}
		return true;
	}
	
	public static void setPermission(Rule r, String permission){
		for(Map.Entry<String, Rule> e : rules.entrySet())
			if(e.getKey().equals(r.getProcessName()))
				e.getValue().setPermission(permission);
	}
	
	public static void setLifetime(Rule r, String lifetime){
		for(Map.Entry<String, Rule> e : rules.entrySet())
			if(e.getKey().equals(r.getProcessName()))
				e.getValue().setLifetime(lifetime);
	}
	
	public static String getPermission(String processName){
		for(Map.Entry<String, Rule> e : rules.entrySet())
			if(e.getKey().equals(processName))
				return e.getValue().getPermission();
		
		return Protocol.UNDEFINED;
	}
	
	public static Map<String,Rule> getAllRules(){
		Map<String, Rule> result = new TreeMap<String, Rule>(new Comparator<String>() {
			 public int compare(String o1, String o2) {
			        return o1.compareTo(o2);
			    }
		});
		
		for(Map.Entry<String, Rule> e : rules.entrySet()){
			result.put(e.getKey(), e.getValue());
		}
		
		return result;
	}

}
