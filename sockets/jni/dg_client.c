#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <netdb.h>
#include <sys/un.h>

#define SOCK_PATH "/data/data/com.rmgoncalo/dg_daemon_server"
#define DG_INET 1
#define DG_INET6 2

struct dg_query {
   int family;
   struct sockaddr_in addrin;
   struct sockaddr_in6 addrin6;
   int pid;
   char process[16];
   int permission;
   int lifetime;
};


// usage: dg_client 1 port pid process
// usage: dg_client 2 port pid process
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
	unsigned short port;
	struct sockaddr_in addrin;
	struct sockaddr_in6 addrin6;
	char ip[INET_ADDRSTRLEN];
	char ip6[INET6_ADDRSTRLEN];

	query = (struct dg_query *) malloc(sizeof(struct dg_query));
	memset(&addrin, 0, sizeof(addrin));
	memset(&addrin6, 0, sizeof(addrin6));

	port = (unsigned short) strtoul(argv[2], NULL, 0);

	int family = atoi(argv[1]);

	if(family == DG_INET)
	{
		query->family = DG_INET;

		addrin.sin_family = AF_INET;
		addrin.sin_port = htons(port);
		addrin.sin_addr.s_addr = htonl(INADDR_ANY);

		query->addrin = addrin;
	}
	else
	{
		if(family == DG_INET6)
		{
			query->family = DG_INET6;

			addrin6.sin6_family = AF_INET6;
			addrin6.sin6_port = htons(port);
			addrin6.sin6_addr =  in6addr_any;
			addrin6.sin6_flowinfo = 0;

			query->addrin6 = addrin6;
		}
		else
		{
			printf("Error DG family.\n");
			return 0;
		}
	}

	query->pid = atoi(argv[3]);
	strcpy(query->process, argv[4]);

	query_size = (size_t) sizeof(struct dg_query);

	if(query->family == DG_INET)
	{
		inet_ntop(AF_INET, &(query->addrin.sin_addr), ip, INET_ADDRSTRLEN);
		printf("ipv4:%s-port:%hu-:pid:%d-process:%s\n", ip, query->addrin.sin_port, query->pid, query->process);
	}
	else
	{
		if(query->family == DG_INET6)
		{
			inet_ntop(AF_INET6, &(query->addrin6.sin6_addr), ip6, INET6_ADDRSTRLEN);
			printf("ipv6:%s-port:%hu-:pid:%d-process:%s\n", ip6, query->addrin6.sin6_port, query->pid, query->process);
		}
		else
		{
			printf("Error Query family.\n");
			return 0;
		}
	}


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

  	slen = write(sockfd, query, query_size);

  	if(slen <= 0)
  	{
  		perror("send");
  		exit(1);
  	}

  	printf("Droidguardian: Query sent.\n");

  	rlen = read(sockfd, query, query_size);

  	if(rlen <= 0)
  	{
  		perror("recv");
  		exit(1);
  	}

  	printf("--\n\nPermission: %d\n Lifetime: %d.\n", query->permission, query->lifetime);

  	close(sockfd);

  	return 0;
}
