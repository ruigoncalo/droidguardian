#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/data/data/com.rmgoncalo/dgsocket"

struct dg_query {
  	int server;
	int port;
	int pid;
};


int main(){

	int sockfd;
	ssize_t slen, rlen;
	size_t query_size;
	struct sockaddr_un remote;
	struct dg_query *query;

	query = (struct dg_query *) malloc(sizeof(struct dg_query));

	query->server = 1234;
	query->port = 50;
	query->pid = 4321;
	query_size = (size_t) sizeof(struct dg_query);

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	if( sockfd < 1 ){
		perror("socket");
		exit(1);
	}

	printf("DG: Trying to connect...\n");

	memset(&remote, 0, sizeof(remote));

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);

	if( connect(sockfd, (struct sockaddr *)&remote, sizeof(remote)) == -1 ) {
    	perror("connect");
    	exit(1);
  	}

  	printf("DG: Connected.\n");

  	slen = send(sockfd, (void *) query, query_size, 0);

  	if(slen <= 0){
  		perror("send");
  		exit(1);
  	}

  	printf("DG: Query sent.\n");

  	//rlen = recv(sockfd, (void *) query, query_size, 0);

  	//if(rlen <= 0){
  	//	perror("recv");
  	//	exit(1);
  	//}

  	printf("DG: server: %d, port, %d, pid: %d.\n",query->server, query->port, query->pid);

  	close(sockfd);

  	return 0;
}
