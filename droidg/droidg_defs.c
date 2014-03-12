#include "droidg_lsm.h"

//---------------------------------------------------------------------

int is_internet_socket(int family) {
if ((family == PF_INET) || (family == PF_INET6))
    return 1;
  else
    return 0;
}

//---------------------------------------------------------------------

static int create_socket(struct socket **sock)
{
  int retval;
  retval = sock_create(AF_UNIX, SOCK_STREAM, 0, sock);

  return retval;
}

//---------------------------------------------------------------------


static int connect_socket(struct socket **sock)
{
  struct sockaddr_un loc;
  int retval;
  
  memset(&loc, 0, sizeof(loc));

  loc.sun_family = AF_UNIX;
  strcpy(loc.sun_path, PATH_MODULE);

  retval = (*sock)->ops->connect(*sock, (struct sockaddr*) &loc, sizeof(loc) - 1, 0);

  return retval;
}


//---------------------------------------------------------------------


static int send_query_to_daemon (struct socket **sock, struct dg_query *query)
{
  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;

  int retval;

  if( (*sock) == NULL ){
    printk(KERN_INFO "DroidGuardian: process '%s' lost communication with the daemon (sock is NULL).\n", current->comm);
    return -1;
  }

  memset(&msg, 0, sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_iov->iov_base= (void *) query;
  msg.msg_iov->iov_len= (__kernel_size_t) dg_query_size;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = MSG_NOSIGNAL;

  oldfs = get_fs();
  set_fs(KERNEL_DS);
  
  retval = sock_sendmsg(*sock, &msg, (size_t) dg_query_size);

  if( retval == 0 ){
    printk(KERN_INFO "DroidGuardian: (%s) error %d on sending a query to the daemon.\n", current->comm, retval);
    return -1;
  }

  set_fs(oldfs);

  return 0;
}


//---------------------------------------------------------------------


int send_query4_to_app(struct socket **sock, struct sockaddr_in **addr, int pid) {

  int retval;
  struct dg_query query;
  struct sockaddr_in addrin;  

  retval = create_socket(sock);
  if( retval != 0 ) {
    printk(KERN_INFO "DroidGuardian: %s could not create socket! Error %d.\n", current->comm, retval);
    return -1;
  }

  retval = connect_socket(sock);
   if( retval != 0 ) {
    printk(KERN_INFO "DroidGuardian: %s could not connect to socket! Error %d.\n", current->comm, retval);
    return -1;
  }

  memset(&addrin, 0, sizeof(addrin));
  memcpy(&addrin, *addr, sizeof(struct sockaddr_in));

  query.family = DG_INET;
  query.addrin = addrin;
  query.pid = pid;
  query.permission = UNDEFINED;
  query.lifetime = UNDEFINED;

  retval = send_query_to_daemon(sock, &query);
  if( retval != 0)
    return -1;

  printk(KERN_INFO "DroidGuardian: '%s' (%d) sent query4 to app.\n", current->comm, pid);

  return retval;
}


//---------------------------------------------------------------------


int send_query6_to_app(struct socket **sock, struct sockaddr_in6 **addr, int pid) {

  int retval;
  struct dg_query query;
  struct sockaddr_in6 addrin6;  

  retval = create_socket(sock);
  if( retval != 0 ) {
    printk(KERN_INFO "DroidGuardian: '%s' could not create socket! Error %d.\n", current->comm, retval);
    return -1;
  }

  retval = connect_socket(sock);
   if( retval != 0 ) {
    printk(KERN_INFO "DroidGuardian: '%s' could not connect to socket! Error %d.\n", current->comm, retval);
    return -1;
  }

  memset(&addrin6, 0, sizeof(addrin6));
  memcpy(&addrin6, *addr, sizeof(struct sockaddr_in6));

  query.family = DG_INET6;
  query.addrin6 = addrin6;
  query.pid = pid;
  query.permission = UNDEFINED;
  query.lifetime = UNDEFINED;

  retval = send_query_to_daemon(sock, &query);
  if( retval != 0)
    return -1;

  printk(KERN_INFO "DroidGuardian: '%s' (%d) sent query6 to app.\n", current->comm, pid);

  return retval;
}


//---------------------------------------------------------------------


int read_answer_from_app(struct socket *sock, struct dg_query *answer) {

  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;
  int retval;
  
  memset(&msg, 0 , sizeof(msg));
  memset(&iov, 0, sizeof(iov));

  msg.msg_name = 0;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  msg.msg_iov->iov_base = (void *) answer;
  msg.msg_iov->iov_len  = (__kernel_size_t) dg_query_size;
  
  oldfs = get_fs();
  set_fs(KERNEL_DS);

  retval = sock_recvmsg(sock, &msg, (size_t) dg_query_size, MSG_WAITALL /* 0 flags*/);
  if (retval < 0) {
    printk(KERN_INFO "DoidGuardian: '%s' could not read answer from app. Error %d.\n", current->comm, retval);
  }

  set_fs(oldfs);

  printk(KERN_INFO "DroidGuardian: '%s' (%d) received query from app.\n", current->comm, answer->pid);
  
  return retval;
}
