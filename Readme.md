[root@freebsd-01 ~/route-bench-kmod-6]# make
Warning: Object directory not changed from original /root/route-bench-kmod-6
[root@freebsd-01 ~/route-bench-kmod-6]# kldunload -v ./route6-bench-kmod.ko
Unloading route6-bench-kmod.ko, id=5
LKM Unloaded
[root@freebsd-01 ~/route-bench-kmod-6]# netstat -6 -rn | wc -l
   73696
[root@freebsd-01 ~/route-bench-kmod-6]# kldload -v ./route6-bench-kmod.ko
Took 0 sec and 33730823 nanosec
843 nanosec/op
Took 0 sec and 28623749 nanosec
715 nanosec/op
Took 0 sec and 25835126 nanosec
645 nanosec/op
Took 0 sec and 23697145 nanosec
592 nanosec/op
Took 0 sec and 20232738 nanosec
505 nanosec/op
Took 0 sec and 19175900 nanosec
479 nanosec/op
LKM Loaded
Loaded ./route6-bench-kmod.ko, id=5
[root@freebsd-01 ~/route-bench-kmod-6]#


