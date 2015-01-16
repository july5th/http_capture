#ifndef CONFIG_H
#define CONFIG_H

/* This file contains global options. */

/* global settings defined at compile time */
#define PACKET_MAXSIZE 0x10000	/* maximum total raw packet size */
#define RESOURCE_MAXSIZE 0x400	/* maximum size for http resource identifier */
#define CONNECTION_BUCKETS 8000	/* number of buckets in connection hash table */
#define MAX_CONNECTIONS 655350	/* maximum number of simultaneous connections */

#define URL_MAXSIZE 2048
#define HEADER_MAXSIZE 4096
#define DATA_MAXSIZE 8192

/* string definitions */
#define HTTP_INDEXFILE "index.html"

/* global settings defined at runtime */
int debug;			/* debugging level */

int catch_request_body;
int redis_output;
int body_base64_output;
int print_all_request_header;

#define REDIS_HOST "172.17.81.123"
#define REDIS_PORT 6379

#endif
