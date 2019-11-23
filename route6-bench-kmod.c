#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/socket.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_var.h>
#include <net/route.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include <sys/time.h>

#include "ipv6_ips.h"

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
        if (sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in*)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

static void bench_route_lookup() {
        //struct flow_entry *fle;
        //struct sockaddr_in sin; //, sin_mask;
        struct sockaddr_in6 sin; //, sin_mask;
        //struct sockaddr_in6 sin6_mask;; //, sin_mask;
        //struct sockaddr_dl rt_gateway;
        struct rt_addrinfo info;
        //struct sockaddr rt_gateway;
        struct sockaddr_dl rt_gateway;
        //uint32_t pfx_len;
        char gw_s[128];

	struct bintime btb, bte;

        bzero(&sin, sizeof(sin));
        //sin.sin_len = sizeof(struct sockaddr_in);
        sin.sin6_len = sizeof(struct sockaddr_in6);
        //sin.sin_family = AF_INET;
        sin.sin6_family = AF_INET6;
        //inet_aton("172.16.18.57", &sin.sin_addr); 
        //inet_aton("2006::1", &sin.sin6_addr); 
        //inet_pton(sin.sin6_family, "2009::1", &sin.sin6_addr); 



        //rt_gateway.sa_len = sizeof(struct sockaddr);
        rt_gateway.sdl_len = sizeof(rt_gateway);
        //info.rti_info[RTAX_NETMASK] = (struct sockaddr *)&sin6_mask;
	for (int c=0; c<6; ++c) {
	bintime(&btb);
        for (int i=0; i<IPCOUNT-1; ++i) {
        sin.sin6_addr.s6_addr32[0]=test_ips[i][0];
        sin.sin6_addr.s6_addr32[1]=test_ips[i][1];
        sin.sin6_addr.s6_addr32[2]=test_ips[i][2];
        sin.sin6_addr.s6_addr32[3]=test_ips[i][3];
        bzero(&info, sizeof(info));
        info.rti_info[RTAX_GATEWAY] = (struct sockaddr *)&rt_gateway;
        int error = rib_lookup_info(RT_DEFAULT_FIB, (struct sockaddr *)&sin, 0, 0, &info);
        if (error == 0) {
                //uprintf("Lookup succseeded\n");
                if (info.rti_flags & RTAX_GATEWAY) {
                                inet_ntop( info.rti_info[RTAX_GATEWAY]->sa_family,  get_in_addr(info.rti_info[RTAX_GATEWAY]), gw_s, 127  );
                                //uprintf("we have RTAX_GATEWAY %s\n", gw_s);
                                
                        }
                } else {
                        uprintf("Error %d\n", error);
                }

        }
	bintime(&bte);
	bintime_sub(&bte, &btb);
	struct timespec ts;
	bintime2timespec(&bte, &ts);
	uprintf("Took %ld sec and %ld nanosec\n", ts.tv_sec, ts.tv_nsec);
	uprintf("%ld nanosec/op\n", ts.tv_nsec/IPCOUNT);
	}
}

static int lkm_event_handler(struct module *module, int event_type, void *arg) {

  int retval = 0;                   // function returns an integer error code, default 0 for OK

  switch (event_type) {             // event_type is an enum; let's switch on it
    case MOD_LOAD:                  // if we're loading
      bench_route_lookup();
      uprintf("LKM Loaded\n");      // spit out a loading message
      break;

    case MOD_UNLOAD:                // if were unloading
      uprintf("LKM Unloaded\n");    // spit out an unloading messge
      break;

    default:                        // if we're doing anything else
      retval = EOPNOTSUPP;          // return a 'not supported' error
      break;
  }

  return(retval);                   // return the appropriate value

}

static moduledata_t lkm_data = {
  "freebsd_lkm",            // Name of our module
  lkm_event_handler,        // Name of our module's 'event handler' function
  NULL                      // Ignore for now :)
};

// Register the module with the kernel using:
//  the module name
//  our recently defined moduledata_t struct with module info
//  a module type (we're daying it's a driver this time)
//  a preference as to when to load the module
DECLARE_MODULE(freebsd_lkm, lkm_data, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);

