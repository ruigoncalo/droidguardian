package com.rmgoncalo.droidg;

public class Query {
	
	public static final String tag = "Query";

	private String address;
	private String port;
	private String pid;
	private String processName;
	private String permission;
	private String lifetime;	
	
	public Query(String msg){
		this.setAddress(new String());
		this.setPort(new String());
		this.setPid(new String());
		this.setProcessName(new String());
		
		this.handleString(msg);
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

	private void handleString(String msg){
		
		String[] result = msg.split(";");
		
		setAddress(result[0]);
		setPort(result[1]);
		setPid(result[2]);
		setProcessName(result[3]);	
	}

}
