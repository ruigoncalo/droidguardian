#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"

#define IPADDR_SIZE 15
#define PORT_SIZE 32
#define PID_SIZE 32
#define PROCNAME_SIZE 64
#define MSG_SIZE (IPADDR_SIZE + PORT_SIZE + PID_SIZE + PROCNAME_SIZE + 4)

struct dg_query {
  	char ip[IPADDR_SIZE];
  	int ip_size;
	unsigned short port;
	int pid;
	char  proc_name[PROCNAME_SIZE];
	int proc_name_size;
	int permission;
	int lifetime;
};


int main(int argc, char* argv[]){

	if(argc != 5){
		perror("argc");
		exit(1);
	}

	int sockfd;
	ssize_t slen, rlen;
	size_t query_size;
	struct sockaddr_un remote;
	struct dg_query *query;

	query = (struct dg_query *) malloc(sizeof(struct dg_query));

	query->ip_size = strlen(argv[1]);
	strncpy(query->ip, argv[1], query->ip_size);
	query->ip[query->ip_size] = '\0';

	query->port= atoi(argv[2]);
	query->pid = atoi(argv[3]);

	query->proc_name_size = strlen(argv[4]);
	strncpy(query->proc_name, argv[4], query->proc_name_size);
	query->proc_name[query->proc_name_size] = '\0';

	query_size = (size_t) sizeof(struct dg_query);

	printf("%s;%d;%hu;%d;%s;%d\n", query->ip, query->ip_size, query->port, query->pid, query->proc_name, query->proc_name_size);
	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	if( sockfd < 1 ){
		perror("socket");
		exit(1);
	}

	printf("Droidguardian: Trying to connect...\n");

	memset(&remote, 0, sizeof(remote));

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);

	if( connect(sockfd, (struct sockaddr *)&remote, sizeof(remote)) == -1 ) {
    	perror("connect");
    	exit(1);
  	}

  	printf("Droidguardian: Connected.\n");

  	slen = send(sockfd, (void *) query, query_size, 0);

  	if(slen <= 0){
  		perror("send");
  		exit(1);
  	}

  	printf("Droidguardian: Query sent.\n");

  	rlen = recv(sockfd, (void *) query, query_size, 0);

  	if(rlen <= 0){
  		perror("recv");
  		exit(1);
  	}

  	printf("Droidguardian: ip: %s, port, %hu, pid: %d, process: %s, permission: %d, lifetime: %d.\n",
  							query->ip, query->port, query->pid, query->proc_name, query->permission, query->lifetime);

  	close(sockfd);

  	return 0;
}
