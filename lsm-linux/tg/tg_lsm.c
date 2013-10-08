/*
 * TuxGuardian test (http://tuxguardian.sourceforge.net/)
 * ruimiguelgoncalo@gmail.com
 */

#include "tg_lsm.h"

#ifdef CONFIG_SECURITY

#ifdef CONFIG_SECURITY_NETWORK

extern int is_internet_socket(int family);
extern void print_socket_info(int family, int type);

static int tg_socket_unix_stream_connect(struct sock *sock, struct sock *other,
				   struct sock *newsk)
{
	return 0;
}

static int tg_socket_unix_may_send(struct socket *sock, struct socket *other)
{
	return 0;
}

static int tg_socket_create(int family, int type, int protocol, int kern)
{
	
	if(is_internet_socket(family)) {
	  printk(KERN_INFO "TuxGuardian: create socket - process %d '%s'.\n", current->pid, current->comm);
	  print_socket_info(family,type);
	}	
	else
	  return 0;
	
	return 0;
}

static int tg_socket_post_create(struct socket *sock, int family, int type,
				  int protocol, int kern)
{	
	return 0;
}

static int tg_socket_bind(struct socket *sock, struct sockaddr *address,
			   int addrlen)
{
	return 0;
}

static int tg_socket_connect(struct socket *sock, struct sockaddr *address,
			      int addrlen)
{
	return 0;
}

static int tg_socket_listen(struct socket *sock, int backlog)
{
	return 0;
}

static int tg_socket_accept(struct socket *sock, struct socket *newsock)
{
	return 0;
}

static int tg_socket_sendmsg(struct socket *sock, struct msghdr *msg, int size)
{
	return 0;
}

static int tg_socket_recvmsg(struct socket *sock, struct msghdr *msg,
			      int size, int flags)
{
	return 0;
}

static int tg_socket_getsockname(struct socket *sock)
{
	return 0;
}

static int tg_socket_getpeername(struct socket *sock)
{
	return 0;
}

static int tg_socket_setsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

static int tg_socket_getsockopt(struct socket *sock, int level, int optname)
{
	return 0;
}

static int tg_socket_shutdown(struct socket *sock, int how)
{
	return 0;
}

static int tg_socket_sock_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	return 0;
}

static int tg_socket_getpeersec_stream(struct socket *sock,
					char __user *optval,
					int __user *optlen, unsigned len)
{
	return 0;
}

static int tg_socket_getpeersec_dgram(struct socket *sock,
				       struct sk_buff *skb, u32 *secid)
{
	return 0;
}

static int tg_sk_alloc_security(struct sock *sk, int family, gfp_t priority)
{
	return 0;
}

static void tg_sk_free_security(struct sock *sk)
{
}

static void tg_sk_clone_security(const struct sock *sk, struct sock *newsk)
{
}

static void tg_sk_getsecid(struct sock *sk, u32 *secid)
{
}

static void tg_sock_graft(struct sock *sk, struct socket *parent)
{
}

static int tg_inet_conn_request(struct sock *sk, struct sk_buff *skb,
				 struct request_sock *req)
{
	return 0;
}

static void tg_inet_csk_clone(struct sock *newsk,
			       const struct request_sock *req)
{
}

static void tg_inet_conn_established(struct sock *sk, struct sk_buff *skb)
{
}

static int tg_secmark_relabel_packet(u32 secid)
{
	return 0;
}

static void tg_secmark_refcount_inc(void)
{
}

static void tg_secmark_refcount_dec(void)
{
}

static void tg_req_classify_flow(const struct request_sock *req,
				  struct flowi *fl)
{
}

static int tg_tun_dev_alloc_security(void **security)
{
	return 0;
}

static void tg_tun_dev_free_security(void *security)
{
}

static int tg_tun_dev_create(void)
{
	return 0;
}

static int tg_tun_dev_attach_queue(void *security)
{
	return 0;
}

static int tg_tun_dev_attach(struct sock *sk, void *security)
{
	return 0;
}

static int tg_tun_dev_open(void *security)
{
	return 0;
}

static void tg_skb_owned_by(struct sk_buff *skb, struct sock *sk)
{
}

#endif	 /* CONFIG_SECURITY_NETWORK */


static struct security_operations tg_ops = {
	.name =				"tg",

/* Common capabilities
   source code in security/commoncap.c */

	.ptrace_access_check =		cap_ptrace_access_check,
	.ptrace_traceme =		cap_ptrace_traceme,
	.capget =			cap_capget,
	.capset =			cap_capset,
	.capable =			cap_capable,
	.settime =			cap_settime,
	.vm_enough_memory =		cap_vm_enough_memory,
	.netlink_send =			cap_netlink_send,
	.bprm_set_creds =		cap_bprm_set_creds,
	.bprm_secureexec =		cap_bprm_secureexec,
	.inode_need_killpriv =		cap_inode_need_killpriv,
	.inode_killpriv =		cap_inode_killpriv,
	.inode_setxattr =		cap_inode_setxattr,
	.inode_removexattr =		cap_inode_removexattr,
	.mmap_file =			cap_mmap_file,
	.mmap_addr =			cap_mmap_addr,
	.task_fix_setuid =		cap_task_fix_setuid,
	.task_setnice =			cap_task_setnice,
	.task_setioprio =		cap_task_setioprio,
	.task_setscheduler =		cap_task_setscheduler,
	.task_prctl =			cap_task_prctl,

/* Hook functions - network */

#ifdef CONFIG_SECURITY_NETWORK
	.unix_stream_connect =		tg_socket_unix_stream_connect,
	.unix_may_send =		tg_socket_unix_may_send,

	.socket_create =		tg_socket_create,
	.socket_post_create =		tg_socket_post_create,
	.socket_bind =			tg_socket_bind,
	.socket_connect =		tg_socket_connect,
	.socket_listen =		tg_socket_listen,
	.socket_accept =		tg_socket_accept,
	.socket_sendmsg =		tg_socket_sendmsg,
	.socket_recvmsg =		tg_socket_recvmsg,
	.socket_getsockname =		tg_socket_getsockname,
	.socket_getpeername =		tg_socket_getpeername,
	.socket_getsockopt =		tg_socket_getsockopt,
	.socket_setsockopt =		tg_socket_setsockopt,
	.socket_shutdown =		tg_socket_shutdown,
	.socket_sock_rcv_skb =		tg_socket_sock_rcv_skb,
	.socket_getpeersec_stream =	tg_socket_getpeersec_stream,
	.socket_getpeersec_dgram =	tg_socket_getpeersec_dgram,
	.sk_alloc_security =		tg_sk_alloc_security,
	.sk_free_security =		tg_sk_free_security,
	.sk_clone_security =		tg_sk_clone_security,
	.sk_getsecid =			tg_sk_getsecid,
	.sock_graft =			tg_sock_graft,
	.inet_conn_request =		tg_inet_conn_request,
	.inet_csk_clone =		tg_inet_csk_clone,
	.inet_conn_established =	tg_inet_conn_established,
	.secmark_relabel_packet =	tg_secmark_relabel_packet,
	.secmark_refcount_inc =		tg_secmark_refcount_inc,
	.secmark_refcount_dec =		tg_secmark_refcount_dec,
	.req_classify_flow =		tg_req_classify_flow,
	.tun_dev_alloc_security =	tg_tun_dev_alloc_security,
	.tun_dev_free_security =	tg_tun_dev_free_security,
	.tun_dev_create =		tg_tun_dev_create,
	.tun_dev_attach_queue =		tg_tun_dev_attach_queue,
	.tun_dev_attach =		tg_tun_dev_attach,
	.tun_dev_open =			tg_tun_dev_open,
	.skb_owned_by =			tg_skb_owned_by,

#endif /* CONFIG_SECURITY_NETWORK */
};

static __init int tg_init(void)
{
    if (!security_module_enable(&tg_ops)) {
        return 0;
    }
 
    printk(KERN_INFO "TuxGuardian:  Initializing.\n");
 
    if (register_security(&tg_ops))
        panic("TuxGuardian: Unable to register with kernel.\n");
 
    return 0;
}

security_initcall(tg_init);

#endif /* CONFIG_SECURITY */
