#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"

#define IPADDR_SIZE 16
#define PROCNAME_SIZE 256


struct dg_query {
  	char ip[IPADDR_SIZE];
  	int ip_size;
	unsigned short port;
	int pid;
	char proc_name[PROCNAME_SIZE];
	int proc_name_size;
	int permission;
	int lifetime;
};

void print_arrays(char a[], char b[]){
	printf("print: %s %s\n", a, b);
}

int get_and_send_message(int sockfd) {

	int sockfd2;
	ssize_t rlen, slen;
	struct sockaddr_un remote;
	size_t query_size;

	struct dg_query *query = NULL;
    	socklen_t socklen = sizeof(remote);

	if ((sockfd2 = accept(sockfd, (struct sockaddr *)&remote, &socklen)) == -1) {
		perror("accept");
		exit(1);
	}
	
	query = (struct dg_query *) malloc(sizeof(struct dg_query));
	
	query_size = sizeof(struct dg_query);
	
	rlen = recv(sockfd2, (void *) query, query_size, 0);

	if (rlen <= 0){
		perror("recv");
		exit(1);
	}

	print_arrays(query->ip, query->proc_name);

	printf("size recv: %d\n", (int) rlen);

	printf("Droidguardian: ip: %s, size: %d port: %hu, pid: %d, proc_name: %s, size: %d\n", 
				query->ip, query->ip_size, query->port, query->pid, query->proc_name, query->proc_name_size);

	query->permission = 0;
	query->lifetime = 1;

	slen = send(sockfd2, (void *) query, query_size, 0);

	if (slen <= 0){
		perror("send");
		exit(1);
	}


	return sockfd2;
}

int main(){

	int sockfd;
	struct sockaddr_un local;

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	if( sockfd < 0 ){
		perror("socket");
		exit(1);
	}

	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);

	unlink(local.sun_path);

	if ( bind(sockfd, (struct sockaddr *)&local, sizeof(local)) == -1 ) {
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, 5) == -1) {
		perror("listen");
		exit(1);
	}

	printf("DG: Waiting for a connection...\n");


	while(1){
		get_and_send_message(sockfd);
	}

	close(sockfd);

	return 0;
}
