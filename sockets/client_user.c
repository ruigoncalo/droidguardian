#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH "/tmp/usocket"
#define MAX 100

int recv_msg_from_server(int socketfd, char data[MAX]) {

  struct msghdr msg;
  struct iovec iov;
  int s;
  
  memset(&msg, 0, sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  iov.iov_base = data;
  iov.iov_len = MAX;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;


  s = recvmsg(socketfd, &msg, 0);

  printf("after recv - iov_base: %s, length: %d\n", (char *) msg.msg_iov->iov_base, (int) strlen(msg.msg_iov->iov_base));

  if(s < 0){
    perror("recvmsg");
  }

  return s;
}


int main(void)
{
	int s, len, slen;
	struct sockaddr_un remote;
	char data[MAX];

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        	perror("socket");
        	exit(1);
    	}

	printf("Trying to connect...\n");

	memset(&remote, 0, sizeof(remote));

	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, SOCK_PATH);
	len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
        	perror("connect");
        	exit(1);
    	}

	printf("Connected.\n");

	slen = recv_msg_from_server(s, data);

	if (slen < 0) {
		perror("recvmsg");
	}

	//printf("print data received > %s\n", data);
    
	close(s);

	return 0;
}
