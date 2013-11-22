/*
 * DroidGuardian Client Demo (user space)
 * ruimiguelgoncalo@gmail.com
 */

#include "client_user.h"

ssize_t send_query_to_deamon(int sockfd, struct dg_query *query){
  struct msghdr msg;
  struct iovec iov;
  ssize_t retval;

  memset(&msg, 0, sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  iov.iov_base = (void *) query;
  iov.iov_len = (__kernel_size_t) dg_query_size;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  retval = sendmsg(sockfd, &msg, 0);

  if (retval <= 0) {
    printf("DroidGuardian: Error sending query to daemon.\n");
    perror("sendmsg");
    return -1;
  }

  return retval;
}


int send_question_permit(struct dg_query *query) {
  
  int sockfd;
  ssize_t retval;
  struct sockaddr_un remote;
 
  sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

  if(sockfd == -1){
    printf("DroidGuardian: Error creating socket.\n");
    perror("socket");
  return -1;
  }

  printf("Trying to connect...\n");

  memset(&remote, 0, sizeof(remote));
  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, SOCK_PATH);

  if(connect(s, (struct sockaddr *)&remote, sizeof(remote)) == -1) {
    printf("DroidGuardian: Error connecting socket.\n");
    perror("connect");
    return -1;
  }

  printf("Connected.\n");
  
  retval = send_query_to_deamon(sockfd, &query);

  if(retval <= 0){
    printf("DroidGuardian: Error sending query to daemon.\n");
    perror("send query to daemon");
    return -1;
  }

  return sockfd;
}

int read_answer_from_daemon(int sockfd, struct dg_query *answer){
  struct msghdr msg;
  struct iovec  iov;
  ssize_t retval;
  
  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  msg.msg_iov->iov_base = (void *) answer;
  msg.msg_iov->iov_len  = (__kernel_size_t) tg_query_size;

  retval = recvmsg(sockfd, &msg, 0);

  if (retval <= 0) {
    printf("DroidGuardian: Error receiving answer from daemon.\n");
    perror("recvmsg");
    return -1;
  }

  return 0;
}


int main(int argc, char* argv[])
{
  int sockfd;
  struct dg_query query, answer;
  int dg_server, dg_port, dg_pid, dg_sender;
  char dg_process[PROCESS_SIZE];

  if(argc != 5){
    printf("Usage: $ client_user <server> <port> <pid> <process>\n<server>: int\n<port>: int\n<pid>: int\n<process> char[16]\n");
    perror("argc");
    exit(1);
  }

  if( sizeof(argv[1]) != sizeof(dg_server) || sizeof(argv[2]) != sizeof(dg_port) || sizeof(argv[3]) != sizeof(dg_pid)) {
    printf("argv[1], argv[2] and argv[3] must be int.\n");
    perror("argv not int");
    exit(1);
  }

  if( sizeof(argv[4]) != sizeof(dg_process)){
    printf("argv[4] must be char[16]");
    perror("argv not char");
    exit(1);
  }
  
  dg_server = atoi(argv[1]);
  dg_port = atoi(argv[2]);
  dg_pid = atoi(argv[3]);
  dg_process = argv[4];

  query.server = dg_server;
  query.port = dg_port;
  query.pid = dg_pid;
  query.process = process;

  sockfd = send_question_permit(&query);

  if(sockfd == -1){
    printf("DroidGuardian: Error sending question permit.\n");
    perror("question permit");
    exit(1);
  }

  if(read_answer_from_daemon(sockfd, &answer) == -1){
    printf("DroidGuardian: Error reading answer from daemon.\n");
    perror("read answer");
    exit(1);
  }

  return 0;
}
