package com.rmgoncalo;

public class Query {
	
	static {
		System.loadLibrary("dgquery");
	}
	
	private int server;
	private int port;
	private int pid;
	private int lifetime;
	private int permission;
	
	public Query(){
		this.setServer(0);
		this.setPort(0);
		this.setPid(0);	
		this.setLifetime(0);
		this.setPermission(0);
	}
	
	public native void updateVars() throws Exception;
	
	public int getServer() {
		return server;
	}

	public void setServer(int server) {
		this.server = server;
	}

	public int getPort() {
		return port;
	}
	
	public int getLifetime(){
		return lifetime;
	}
	
	public int getPermission(){
		return permission;
	}

	public void setPort(int port) {
		this.port = port;
	}

	public int getPid() {
		return pid;
	}

	public void setPid(int pid) {
		this.pid = pid;
	}
	
	public void setLifetime(int lifetime){
		this.lifetime = lifetime;
	}
	
	public void setPermission(int permission){
		this.permission = permission;
	}

}
