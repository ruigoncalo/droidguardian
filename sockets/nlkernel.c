#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;
struct dg_query {
    char ip[16];
    unsigned short port;
    int pid;
    char[32] process;
};

static void hello_nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *skb_out;
    int query_size
    struct dg_query *query;
    struct dg_query *recv;
    int res;

    query = (struct dg_query*) malloc (sizeof (struct dg_query));
    recv = (struct dg_query*) malloc (sizeof (struct dg_query));

    query->ip = "192.168.0.1";
    query->port = (unsigned short) 23;
    query->pid = 234;
    query->process = "process";

    printk(KERN_INFO "Entering: hello_nl_recv_msg.\n");
    
    
    query_size = sizeof(struct dg_query);
    
    // receive query from userspace
    nlh = (struct nlmsghdr*) skb->data;
    recv = (struct dg_query*) nlmsg_data(nlh);


    printk(KERN_INFO "Netlink received msg payload: %s\n", recv->process);

    pid = nlh->nlmsg_pid; /*pid of sending process */

    skb_out = nlmsg_new(query_size, 0);

    if(!skb_out)
    {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return;
    }
    
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, query_size, 0);

    NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    //strncpy(nlmsg_data(nlh),msg,msg_size);

    memcpy(nlmsg_data(nlh), query, query_size);
    
    res = nlmsg_unicast(nl_sk,skb_out,pid);
    
    if(res<0)
        printk(KERN_INFO "Error while sending bak to user\n");
}

static int __init hello_init(void)
{
    printk("Entering: %s\n",__FUNCTION__);
    nl_sk=netlink_kernel_create(&init_net, NETLINK_USER, 0, hello_nl_recv_msg, NULL, THIS_MODULE);
    if(!nl_sk)
    {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }
    return 0;
}

static void __exit hello_exit(void){
    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");