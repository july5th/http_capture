#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <nids.h>
#include <json/json.h>

#include "config.h"
#include "http_parser.h"

struct stream {
	http_parser request_parser;
	http_parser response_parser;
	//request url
	char url[URL_MAXSIZE + 1];
	//request header key value
	char request_cache[REQUEST_HEADER_MAXSIZE + 1];
	char response_cache[REQUEST_HEADER_MAXSIZE + 1];
        //request post data
	char request_data[REQUEST_DATA_MAXSIZE + 1];
	char response_data[RESPONSE_DATA_MAXSIZE + 1];
	struct tuple4 addr;
        int is_http;
        int tmp;
	json_object *json;
};

int streamCount;

void streamInit();

void streamOpen(struct stream *s, struct tuple4 *addr);
void streamWriteRequest(struct stream *stream, char *data, u_int32_t size);
void streamWriteResponse(struct stream *stream, char *data, u_int32_t size);
void streamClose(struct stream *s);
void streamClean(struct stream *s);
void streamDelete(struct tuple4 *addr);

#endif
