#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"

int send_msg_to_client(int socketfd, char* data) {

  struct msghdr msg;
  struct iovec iov;
  int s;

  printf("before send - data: %s, length: %d\n",data, (int) strlen(data));
  
  memset(&msg, 0, sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  iov.iov_base = data;
  iov.iov_len = strlen(data)+1;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  s = sendmsg(socketfd, &msg, 0);

  printf("after send - iov_base: %s, length: %d\n", (char *) msg.msg_iov->iov_base, (int) strlen(msg.msg_iov->iov_base));

  if(s < 0){
    perror("sendmsg");
    return 0;
  }

  return s;
}


int main(int argc, char* argv[])
{

        if (argc != 2) {
          printf("Usage: $ %s <data>\n",argv[0]);
          return 0;
        }

	int s, s2, len, slen;
	socklen_t t;
	struct sockaddr_un local, remote;
	char* data = argv[1];

	printf("print data: %s\n",data);

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	
	memset(&local, 0, sizeof(local));
	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, SOCK_PATH);

	unlink(local.sun_path);

	len = strlen(local.sun_path) + sizeof(local.sun_family);
	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		exit(1);
	}

	printf("Waiting for a connection...\n");
	
	t = sizeof(remote);
	if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
		perror("accept");
		exit(1);
	}

	printf("Connected.\n");

	slen = send_msg_to_client(s2, data);

	if(slen < 0)
		perror("send");

	close(s2);

	return 0;
}
