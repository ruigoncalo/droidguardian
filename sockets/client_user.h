#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"
#define PROCESS_SIZE 16

struct dg_query {
  	int server;
	int port;
	int pid;
	char process[PROCESS_SIZE];

};

#define dg_query_size sizeof(struct dg_query)


ssize_t send_query_to_deamon(int sockfd, struct dg_query *query);
int send_question_permit(struct dg_query *query);
int read_answer_from_daemon(int sockfd, struct dg_query *answer);
