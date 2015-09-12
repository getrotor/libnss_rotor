USE
---

Make sure that rotd is running.

```
[varoun@ip-10-0-2-194 libnss_rotd]$ make clean
rm libnss_rotd.so.2
[varoun@ip-10-0-2-194 libnss_rotd]$ make 
gcc -fPIC -shared -o libnss_rotd.so.2 -Wl,-soname,libnss_rotd.so.2 src/client.c src/rotd.c
[varoun@ip-10-0-2-194 libnss_rotd]$ make install
sudo install -m 0644 libnss_rotd.so.2 /lib
sudo /sbin/ldconfig -n /lib /usr/lib
[varoun@ip-10-0-2-194 libnss_rotd]$ getent hosts www.bsd.org
@ _nss_rotd_gethostbyname2_r
Custom lookup
129.128.5.194   www.bsd.org
[varoun@ip-10-0-2-194 libnss_rotd]$ getent hosts www.bsd.org
@ _nss_rotd_gethostbyname2_r
Custom lookup
8.8.178.110     www.bsd.org
[varoun@ip-10-0-2-194 libnss_rotd]$ getent hosts www.bsd.org
@ _nss_rotd_gethostbyname2_r
Custom lookup
149.20.53.86    www.bsd.org
[varoun@ip-10-0-2-194 libnss_rotd]$ diff /etc/nsswitch.conf /etc/nsswitch.conf.orig 
39c39
< hosts:      files rotd dns myhostname
---
> hosts:      files dns myhostname
[varoun@ip-10-0-2-194 libnss_rotd]$ 
```

