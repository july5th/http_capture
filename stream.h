#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <nids.h>
#include <jansson.h>

#include "config.h"
#include "http_parser.h"

struct stream {
	http_parser request_parser;
	http_parser response_parser;
	//request url
	char url[URL_MAXSIZE + 1];
	//request header key value
	char cache[HEADER_MAXSIZE + 1];
        //request post data
	char data[DATA_MAXSIZE + 1];
	struct tuple4 addr;
	json_t *json;
};

int streamCount;

void streamInit();

void streamOpen(struct stream *s, struct tuple4 *addr);
void streamWriteRequest(struct stream *stream, char *data, u_int32_t size);
void streamWriteResponse(struct stream *stream, char *data, u_int32_t size);
void streamClose(struct stream *s);

#endif
