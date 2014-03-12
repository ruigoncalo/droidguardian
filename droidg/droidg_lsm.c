/*
 * DroidGuardian Security Module
 * version 0.1
 * ruimiguelgoncalo@gmail.com
 */

#include "droidg_lsm.h"

#ifdef CONFIG_SECURITY_NETWORK

// Functions to filter Internet sockets
extern int is_internet_family(int family);
extern int send_query4_to_app(struct socket **sock, struct sockaddr_in **addr, int pid);
extern int send_query6_to_app(struct socket **sock, struct sockaddr_in6 **addr, int pid);
extern int read_answer_from_app(struct socket *sock, struct dg_query *answer);

/* Network hooks */

static int droidg_unix_stream_connect(struct socket *sock, struct socket *other,
				   struct sock *newsk)
{
	return 0;
}

static int droidg_unix_may_send(struct socket *sock, struct socket *other)
{
	return 0;
}

static int droidg_socket_create(int family, int type, int protocol, int kern)
{
	return 0;
}

static int droidg_socket_post_create(struct socket *sock, int family, int type,
				  int protocol, int kern)
{
	return 0;
}

static int droidg_socket_bind(struct socket *sock, struct sockaddr *address,
			   int addrlen)
{
	return 0;
}

static int droidg_socket_connect(struct socket *sock, struct sockaddr *address,
			      int addrlen)
{
  struct dg_query answer;
  struct socket *msock = NULL;
  struct sockaddr_in *addrin = NULL;
  struct sockaddr_in6 *addrin6 = NULL;
  int pid; 
  int retval;
  char process_name[16];

   // current process name
  strcpy(process_name, current->comm);

   // current process id
  pid = current->pid;

  // permit the connection to sockets that are not AF_INET or AF_INET6 family
  if ( address->sa_family != AF_INET && address->sa_family != AF_INET6)
  {
    return 0;
  }

  // permit netd and Captive Portal to access the Internet
  if ( (strcmp(process_name, "CaptivePortalTr") == 0) || (strcmp(process_name, "netd") == 0) )
  {
    return 0;
  }

  switch(address->sa_family)
  {
    case AF_INET:
       addrin = (struct sockaddr_in *) address;
       retval = send_query4_to_app(&msock, &addrin, pid);
       if (retval < 0)
       {
          printk(KERN_INFO "DroidGuardian: '%s' could not send query4 to app.\n", process_name);
          return 0;
       }
       break;

    case AF_INET6:
      addrin6 = (struct sockaddr_in6 *) address;
      retval = send_query6_to_app(&msock, &addrin6, pid);
      if (retval < 0)
      {
        printk(KERN_INFO "DroidGuardian: '%s' could not send query6 to app.\n", process_name);
        return 0;
      }
      break;

    default:
      printk(KERN_INFO "DroidGuardian: '%s' could not define address family.\n", process_name);
      return 0;
  }

  // Read answer from app
  retval = read_answer_from_app(msock, &answer);
  if(retval < 0)
  {
     printk(KERN_INFO "DroidGuardian: '%s' could not read answer from app.\n", process_name);
     return 0;
  }

  if (answer.permission != ALLOW)
  {
     printk(KERN_INFO "DroidGuardian: Internet connection denied to process '%s'. Permission is %d\n", process_name, answer.permission);
     return -EPERM;
  }

  printk(KERN_INFO "DroidGuardian: Internet connection allowed to process '%s'. Permission is %d\n", process_name, answer.permission);
  return 0;
}

static int droidg_socket_listen(struct socket *sock, int backlog)
{
	return 0;
}

static int droidg_socket_accept(struct socket *sock, struct socket *newsock)
{
	return 0;
}

static int droidg_socket_sendmsg(struct socket *sock, struct msghdr *msg, int size)
{
	return 0;
}

static int droidg_socket_recvmsg(struct socket *sock, struct msghdr *msg,
			      int size, int flags)
{
	return 0;
}

static int droidg_socket_getsockname(struct socket *sock)
{
	return 0;
}

static int droidg_socket_getpeername(struct socket *sock)
{
	return 0;
}

static int droidg_socket_setsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

static int droidg_socket_getsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

static int droidg_socket_shutdown(struct socket *sock, int how)
{
	return 0;
}

static int droidg_socket_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	return 0;
}

static int droidg_socket_getpeersec_stream(struct socket *sock,
					char __user *optval,
					int __user *optlen, unsigned len)
{
	return -ENOPROTOOPT;
}

static int droidg_socket_getpeersec_dgram(struct socket *sock,
				       struct sk_buff *skb, u32 *secid)
{
	return -ENOPROTOOPT;
}

static int droidg_sk_alloc_security(struct sock *sk, int family, gfp_t priority)
{
	return 0;
}

static void droidg_sk_free_security(struct sock *sk)
{
}

static void droidg_sk_clone_security(const struct sock *sk, struct sock *newsk)
{
}

static void droidg_sk_getsecid(struct sock *sk, u32 *secid)
{
}

static void droidg_sock_graft(struct sock *sk, struct socket *parent)
{
}

static int droidg_inet_conn_request(struct sock *sk, struct sk_buff *skb,
				 struct request_sock *req)
{
	return 0;
}

static void droidg_inet_csk_clone(struct sock *newsk,
			       const struct request_sock *req)
{
}

static void droidg_inet_conn_established(struct sock *sk, struct sk_buff *skb)
{
}

static void droidg_req_classify_flow(const struct request_sock *req,
				  struct flowi *fl)
{
}
#endif	/* CONFIG_SECURITY_NETWORK */

static struct security_operations droidg_ops = {
	.name =				"droidg",
#ifdef CONFIG_SECURITY_NETWORK
	.unix_stream_connect =		droidg_unix_stream_connect,
	.unix_may_send =		droidg_unix_may_send,

	.socket_create =		droidg_socket_create,
	.socket_post_create =		droidg_socket_post_create,
	.socket_bind =			droidg_socket_bind,
	.socket_connect =		droidg_socket_connect,
	.socket_listen =		droidg_socket_listen,
	.socket_accept =		droidg_socket_accept,
	.socket_sendmsg =		droidg_socket_sendmsg,
	.socket_recvmsg =		droidg_socket_recvmsg,
	.socket_getsockname =		droidg_socket_getsockname,
	.socket_getpeername =		droidg_socket_getpeername,
	.socket_getsockopt =		droidg_socket_getsockopt,
	.socket_setsockopt =		droidg_socket_setsockopt,
	.socket_shutdown =		droidg_socket_shutdown,
	.socket_sock_rcv_skb =		droidg_socket_sock_rcv_skb,
	.socket_getpeersec_stream =	droidg_socket_getpeersec_stream,
	.socket_getpeersec_dgram =	droidg_socket_getpeersec_dgram,
	.sk_alloc_security =		droidg_sk_alloc_security,
	.sk_free_security =		droidg_sk_free_security,
	.sk_clone_security =		droidg_sk_clone_security,
	.sk_getsecid =			droidg_sk_getsecid,
	.sock_graft =			droidg_sock_graft,
	.inet_conn_request =		droidg_inet_conn_request,
	.inet_csk_clone =		droidg_inet_csk_clone,
	.inet_conn_established =	droidg_inet_conn_established,
	.req_classify_flow =		droidg_req_classify_flow,
#endif	/* CONFIG_SECURITY_NETWORK */
};


static __init int droidg_init(void)
{
    if (!security_module_enable(&droidg_ops)) {
        return 0;
    }
 
    printk(KERN_INFO "Droidguardian:  Initializing...\n");
 
    if (register_security(&droidg_ops))
        panic("Droidguardian: Unable to register with kernel.\n");
 
    return 0;
}

security_initcall(droidg_init);
