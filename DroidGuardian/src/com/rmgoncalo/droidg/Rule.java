package com.rmgoncalo.droidg;

public class Rule {
	
	public static final String tag = "DG-Rule";

	private String address;
	private String port;
	private String pid;
	private String processName;
	private String permission;
	private String lifetime;
	
	public Rule(){
		this.address = null;
		this.port = null;
		this.pid = null;
		this.processName = null;
		this.permission = null;
		this.lifetime = null;
	}
	
	public Rule(String address, String port, String pid, String processName){
		this.address = address;
		this.port = port;
		this.pid = pid;
		this.processName = processName;
		this.permission = null;
		this.lifetime = null;
	}
	
	public Rule(String address, String port, String pid, String processName, 
			String permission, String lifetime) {
		this.address = address;
		this.port = port;
		this.pid = pid;
		this.processName = processName;
		this.permission = permission;
		this.lifetime = lifetime;
	}

	public String getAddress() {
		return address;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	public String getPort() {
		return port;
	}

	public void setPort(String port) {
		this.port = port;
	}

	public String getPid() {
		return pid;
	}

	public void setPid(String pid) {
		this.pid = pid;
	}

	public String getProcessName() {
		return processName;
	}

	public void setProcessName(String processName) {
		this.processName = processName;
	}

	public String getPermission() {
		return permission;
	}

	public void setPermission(String permission) {
		this.permission = permission;
	}

	public String getLifetime() {
		return lifetime;
	}

	public void setLifetime(String lifetime) {
		this.lifetime = lifetime;
	}

}
