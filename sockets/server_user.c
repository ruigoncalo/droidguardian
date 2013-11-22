#include "server_user.h"

int recv_query_from_module(int sockfd, struct dg_query *query){
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

  msg.msg_iov->iov_base = (void *) query;
  msg.msg_iov->iov_len  = (__kernel_size_t) tg_query_size;

  retval = recvmsg(sockfd, &msg, 0);

  if (retval <= 0) {
    printf("DroidGuardian: Error receiving query from module.\n");
    perror("recvmsg");
    return -1;
  }

  return 0;
}


ssize_t send_answer_to_module(int sockfd, struct dg_query *query){
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
    printf("DroidGuardian: Error sending answer to module.\n");
    perror("sendmsg");
    return -1;
  }

  return retval;
}


int main(int argc, char* argv[])
{

	int s, s2;
	socklen_t t;
	struct sockaddr_un local, remote;
	struct dg_query query;


	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);

	unlink(local.sun_path);

	if (bind(s, (struct sockaddr *)&local, sizeof(local)) == -1) {
		perror("bind");
		exit(1);
	}
	

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	printf("Waiting for a connection...\n");

	for(;;) {

		t = sizeof(remote);
		if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
			perror("accept");
			exit(1);
		}

		printf("Connected.\n");

		recv_query_from_module(s2, &query);

		send_answer_to_module(s2, &query);
	}

	return 0;
}