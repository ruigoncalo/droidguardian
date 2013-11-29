#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"

struct dg_query {
  	int server;
	int port;
	int pid;
};

int main(){

	int sockfd, sockfd2;
	ssize_t slen, rlen;
	size_t query_size;
	struct sockaddr_un remote, local;
	socklen_t socklen;
	struct dg_query *query = NULL;

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

	socklen = sizeof(remote);

	if ((sockfd2 = accept(sockfd, (struct sockaddr *)&remote, &socklen)) == -1) {
		perror("accept");
		exit(1);
	}

	query_size = sizeof(struct dg_query);
	query = (struct dg_query *) malloc(sizeof(struct dg_query));

	rlen = recv(sockfd2, (void *) query, query_size, 0);

	if (rlen <= 0){
		perror("recv");
		exit(1);
	}

	printf("DG: received server: %d, port: %d, pid: %d.\n", query->server, query->port, query->pid);

	(query->server)--;
	(query->port)--;
	(query->pid)--;

	slen = send(sockfd2, (void *) query, query_size, 0);

	if (slen <= 0){
		perror("send");
		exit(1);
	}

	close(sockfd);
	close(sockfd2);

	return 0;
}