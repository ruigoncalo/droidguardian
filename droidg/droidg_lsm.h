/*
 * Droidguardian Header file
 * ruimiguelgoncalo@gmail.com
 */


#ifndef __HAS_TG_H
#define __HAS_TG_H

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/tracehook.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/security.h>
#include <linux/xattr.h>
#include <linux/capability.h>
#include <linux/unistd.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/spinlock.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/tty.h>
#include <net/icmp.h>
#include <net/ip.h>       /* for local_port_range[] */
#include <net/tcp.h>      /* struct or_callable used in sock_rcv_skb */
#include <net/net_namespace.h>
#include <net/netlabel.h>
#include <linux/uaccess.h>
#include <asm/ioctls.h>
#include <asm/atomic.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>  /* for network interface checks */
#include <linux/netlink.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/dccp.h>
#include <linux/quota.h>
#include <linux/un.h>     /* for Unix socket types */
#include <net/af_unix.h>  /* for Unix socket types */
#include <linux/parser.h>
#include <linux/nfs_mount.h>
#include <net/ipv6.h>
#include <linux/hugetlb.h>
#include <linux/personality.h>
#include <linux/sysctl.h>
#include <linux/audit.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/posix-timers.h>

#define PATH_MODULE "/data/data/com.rmgoncalo.droidg/dg_daemon_server"

#define DG_INET 1
#define DG_INET6 2

#define UNDEFINED -1

#define DENY 0
#define ALLOW 1

#define ONCE 0
#define FOREVER 1

struct dg_query {
    int family;
    struct sockaddr_in addrin;
    struct sockaddr_in6 addrin6;
    int pid;
    int permission;
    int lifetime;
};

#define dg_query_size sizeof(struct dg_query)

int is_internet_socket(int family);
int read_answer_from_app(struct socket *sock,struct dg_query *answer);
int send_query4_to_app(struct socket **sock, struct sockaddr_in **addr, int pid);
int send_query6_to_app(struct socket **sock, struct sockaddr_in6 **addr, int pid);

#endif
