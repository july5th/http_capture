#ifndef CONFIG_H
#define CONFIG_H

/* This file contains global options. */

/* global settings defined at compile time */
#define PACKET_MAXSIZE 0x10000	/* maximum total raw packet size */
#define RESOURCE_MAXSIZE 0x400	/* maximum size for http resource identifier */
#define CONNECTION_BUCKETS 8000	/* number of buckets in connection hash table */
#define MAX_CONNECTIONS 655350	/* maximum number of simultaneous connections */

#define URL_MAXSIZE 2048

#define RESPONSE_HEADER_MAXSIZE 2048
#define REQUEST_HEADER_MAXSIZE 4096
#define REQUEST_DATA_MAXSIZE 2048
#define RESPONSE_DATA_MAXSIZE 4096

/* string definitions */
#define HTTP_INDEXFILE "index.html"

/* global settings defined at runtime */
int debug;			/* debugging level */
int test;			/* test mode */

int catch_request_body;
int catch_response_body;
int redis_output;
int base64_output;
int print_all_request_header;

//int all_send_data;
//int start_time;

char redis_key[256];

#define REDIS_HOST "127.0.0.1"
#define REDIS_PORT 6379

#endif
