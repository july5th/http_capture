HTTP Capture
=======

Compilation
----------------------------
First install libjson-c and libnids
   
Then compile:

   $ make

Run:

   $ ./http_capture -i eth1 -b -p

Output example:

{ "src": "10.10.40.60:39174", "dst": "220.181.112.244:80", "method": "POST", "url": "\/", "user-agent": "curl\/7.19.7 (x86_64-redhat-linux-gnu) libcurl\/7.19.7 NSS\/3.15.3 zlib\/1.2.3 libidn\/1.18 libssh2\/1.4.2", "host": "www.baidu.com", "data": "tkey=test", "code": 302, "time": 1414060183 }
