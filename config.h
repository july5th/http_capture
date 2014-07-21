#ifndef CONFIG_H
#define CONFIG_H

/* This file contains global options. */

/* global settings defined at compile time */
#define PACKET_MAXSIZE 0x10000	/* maximum total raw packet size */
#define RESOURCE_MAXSIZE 0x400	/* maximum size for http resource identifier */
#define CONNECTION_BUCKETS 100	/* number of buckets in connection hash table */
#define MAX_CONNECTIONS 4000	/* maximum number of simultaneous connections */

#define URL_MAXSIZE 4000
#define HEADER_MAXSIZE 128
#define DATA_MAXSIZE 1024

/* string definitions */
#define HTTP_INDEXFILE "index.html"

/* global settings defined at runtime */
int debug;			/* debugging level */

#endif
