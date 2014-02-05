/*
 *  Local domain sockets - Server kernel module
 *  ruimiguelgoncalo@gmail.com
 *
 *  Relevant information:
 *
 *  struct socket {
 *  	sock_state		state;
 *	short			type;
 *	unsigned long		flags;
 * 	struct socket_wq __rcu	*wq;
 *	struct file		*file;
 * 	struct sock		*sk;
 *	const struct proto_ops	*ops;
 *  }
 *
 *  struct proto_ops {
 *  (...)
 *  	int (* bind)	(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len);
 *	int (* connect)	(struct socket *sock, struct sockaddr *vaddr, int sockaddr_len, int flags);
 *	int (* accept)	(struct socket *sock, struct socket *newsock, int flags);
 *	int (* listen)	(struct socket *sock, int len);
 *  (...)
 *  }
 *
 *  int sock_create (int family, int type, int protocol, struct socket **res);
 *  int sock_recvmsg (struct socket *sock, struct msghdr *msg, size_t size, int flags);
 *  int sock_sendmsg (struct socket *sock, struct msghdr *msg, size_t len);
 *
 *  struct msghdr {
 *  	void*		msg_name;
 *	int		msg_namelen;
 *	struct iovec*	msg_iov;
 *	__kernel_size_t	msg_iovlen;
 *	void*		msg_control;
 *	__kernel_size_t	msg_controllen;
 *	unsigned int	msg_flags;
 *  }
 *
 *  struct iovec {
 *	void*	iov_base;
 *	size_t	iov_len;
 *  }
 *
 *  struct sockaddr_un {
 *  	__kernel_sa_family_t	sun_family; //AF_UNIX
 *	char			sun_path[UNIX_PATH_MAX] //UNIX_PATH_MAX = 108
 *  }
*/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/un.h>
#include <net/sock.h>

#define SOCK_PATH	"/tmp/usocket"
#define LISTEN		10

static struct socket *sock = NULL;
static struct socket *newsock = NULL;

static int __init server_module_init( void ) {

  int retval;
  char* string = "hello_world";

  struct sockaddr_un addr;
  struct sockaddr_un client;
  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;

  // create
  if (sock_create(AF_UNIX, SOCK_STREAM, 0, &sock) < 0){
    printk( KERN_ERR "server: Error creating socket.n" );
    return -EIO;
  }

  // create client
  if (sock_create(AF_UNIX, SOCK_STREAM, 0, &newsock) < 0){
    printk( KERN_ERR "server: Error creating socket.n" );
    return -EIO;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, SOCK_PATH);

  // bind
  retval = sock->ops->bind(sock,(struct sockaddr *)&addr, sizeof(addr));
  if (retval) {
    sock_release(sock);
    return -EIO;
  }

  // listen
  retval = sock->ops->listen(sock, LISTEN);
  if (retval) {
    sock_release(sock);
    return -EIO;
  }

  //accept
  retval = sock->ops->accept(sock, newsock, 0);
  if (retval) {
    sock_release(sock);
    return -EIO;
  }

  //sendmsg
  memset(&msg, 0, sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  memset(&client, 0, sizeof(client));
  client.sun_family = AF_UNIX;
  strcpy(client.sun_path, SOCK_PATH);

  msg.msg_name = &client;
  msg.msg_namelen = sizeof(client);
  msg.msg_iov = &iov;
  msg.msg_iov->iov_base = string;
  msg.msg_iov->iov_len = strlen(string)+1;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  //sendmsg
  oldfs = get_fs();
  set_fs(KERNEL_DS);

  retval = send(newsock, &msg, strlen(string)+1);

  set_fs(oldfs);
  
  return 0;
}

static void __exit server_module_exit( void ) {
  printk(KERN_INFO "Exit usocket.");
  if (sock)
    sock_release(sock);
  if (newsock)
    sock_release(newsock);
}

module_init( server_module_init );
module_exit( server_module_exit );
MODULE_LICENSE("GPL");
