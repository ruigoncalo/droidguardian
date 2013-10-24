#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/un.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/socket.h>

#define SOCK_PATH "/tmp/usocket"
#define MAX 100

struct socket *sock;
int create_socket( void );
int connect_socket( void );
int recv_msg(char str[MAX]);


int create_socket( void ) {

  int retval;
  mm_segment_t oldfs;

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  retval = sock_create(AF_UNIX, SOCK_STREAM, 0, &sock);

  set_fs(oldfs);

  return retval;
}

//---------------------------------------------------------

int connect_socket( void ) {

  struct sockaddr_un loc;
  int retval;
  mm_segment_t oldfs;

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  memset(&loc, 0, sizeof(loc));  
  loc.sun_family = AF_UNIX;
  strcpy(loc.sun_path, SOCK_PATH);

  retval = sock->ops->connect(sock, (struct sockaddr *)&loc, sizeof(loc), 0);

  set_fs(oldfs);

  return retval;
}

//----------------------------------------------------------

int recv_msg(char str[MAX]) {

  int retval;
  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;

  memset(&msg, 0, sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_iov->iov_base= str;
  msg.msg_iov->iov_len= strlen(str)+1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  oldfs = get_fs();
  set_fs(KERNEL_DS);

  retval = sock_recvmsg(sock, &msg, strlen(str)+1, 0);

  if(retval == 0){
    printk(KERN_INFO "usocket: error send_msg == 0.\n");
    return -1;
  }
  else
  if(retval < 0){
    printk(KERN_INFO "usocket: error send_msg < 0.\n");
    return -1;
  }

  set_fs(oldfs);

  return 0;
}

//-----------------------------------------------

static int __init usocket_init( void ) {

  int retval;
  char str[MAX];

  retval = create_socket();
  
  if(retval < 0) {
    printk(KERN_INFO "usocket: create_socket error.\n");
    return 0;
  }

  retval = connect_socket();

  if(retval < 0){
    printk(KERN_INFO "usocket: connect_socket error.\n");
    return 0;
  }

  retval = recv_msg(str);
  printk(KERN_INFO "usocket: str: %s", str);


  sock_release(sock);

  return 0;
}

static void __exit usocket_exit( void )
{
  printk(KERN_INFO "usocket exit!\n");
}

 module_init( usocket_init );
 module_exit( usocket_exit );
 MODULE_LICENSE("GPL");
