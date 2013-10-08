/*
 *  TuxGuardian aux functions
 *
 */

#include "tg_lsm.h"

//---------------------------------------------------------------------

int is_internet_socket(int family) {
if ((family == PF_INET) || (family == PF_INET6))
    return 1;
  else
    return 0;
}

//---------------------------------------------------------------------

void print_socket_info(int family, int type)
{
  switch (family) {

  case PF_LOCAL:  // aka PF_UNIX (the old BSD name) and AF_UNIX/AF_LOCAL
    // sockets for local interprocess communication
    switch (type) {
    case SOCK_STREAM: {
      printk(KERN_INFO "TuxGuardian: (local communication) PF_UNIX SOCK_STREAM\n");
      return;
    }
    case SOCK_DGRAM: {
      printk(KERN_INFO "TuxGuardian: (local communication) PF_UNIX SOCK_DGRAM\n");
      return;
    }
    }
    
  case PF_NETLINK: {
    // netlink is used to transfer information between kernel modules and user space processes
    printk(KERN_INFO "TuxGuardian: (kernel-user communication) PF_NETLINK\n");
    return;
  }

  case PF_PACKET: {
    // packet interface on device level (used to receive or send raw packets at the device driver)
    printk(KERN_INFO "TuxGuardian: (device level communication) PF_PACKET\n");
    return;
  }

  case PF_KEY: {
    // IPSEC stuff.. A user process maintains keyring information on databases that are
    // accessed by sending messages over this socket
    printk(KERN_INFO "TuxGuardian: (keyring db communication - IPSEC) IPSECPF_KEY\n");
    return;
  }


  case PF_INET: {
    // IPv4 communication
    printk(KERN_INFO "TuxGuardian: (IPv4 communication) PF_INET");
    switch (type) {
    case SOCK_STREAM:
      printk(KERN_INFO " SOCK_STREAM\n");
    case SOCK_DGRAM:
      printk(KERN_INFO " SOCK_DGRAM\n");
    case SOCK_RAW:
      printk(KERN_INFO " SOCK_RAW\n");
    }
    return;
  }

 
  case PF_INET6: {
    // IPv6 communication
    printk(KERN_INFO "TuxGuardian: (IPv6 communication) PF_INET6");
    switch (type) {
    case SOCK_STREAM:
      printk(KERN_INFO " SOCK_STREAM\n");
    case SOCK_DGRAM:
      printk(KERN_INFO " SOCK_DGRAM\n");
    case SOCK_RAW:
      printk(KERN_INFO " SOCK_RAW\n");
    }
    return;
  }

  }

}

//--------------------------------------------------------------------


