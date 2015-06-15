HTTP Capture
=======

Compilation
----------------------------
First install libjson-c and libnids

   $ yum install make gcc-c++ json-c json-c-devel libnids libnids-devel libpcap libpcap-devel openssl openssl-devel

Then compile:

   $ make

Run:

   $ ./http_capture -i eth1 -Bbp

   output: { "src": "172.0.0.1:8156", "dst": "172.0.0.1:80", "request.method": "POST", "request.url": "\/test.url", "request.host": "test.host.com", "request.x-forwarded-for": "172.0.0.1", "response.code": 200, "request.body": "{\"test_p1\":\"p1\",\"test_p2\":\"p2\"}", "response.body": "{\"responseCode\":\"0000\",\"responseMessage\":\"请求成功\"}", "time": 1434327902, "request.hc": 1 }

   $ ./http_capture -h
 
   output: ......  
