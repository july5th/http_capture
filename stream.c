/*
 * HTTP CAPTURE
 * (C)2014 Liu Feiran
 *
 */
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <nids.h>

#include "config.h"
#include "http_parser.h"
#include "stream.h"
#include "hash.h"
#include "output.h"

http_parser_settings request_settings;
http_parser_settings response_settings;

void streamOpen(struct stream *s, struct tuple4 *addr) {
	char buffer[1024];

	memset(s, 0, sizeof(struct stream));

	memcpy(&(s->addr), addr, sizeof(struct tuple4));
	http_parser_init(&(s->request_parser), HTTP_REQUEST);
	http_parser_init(&(s->response_parser), HTTP_RESPONSE);
	s->json = json_object();

	//add ip information
	sprintf(buffer, "%s:%i", int_ntoa(addr->saddr), addr->source);
        json_object_set_new(s->json, "src", json_string(buffer));
	sprintf(buffer, "%s:%i", int_ntoa(addr->daddr), addr->dest);
        json_object_set_new(s->json, "dst", json_string(buffer));
}

void streamWriteRequest(struct stream *s, char *data, u_int32_t size) {
	if (debug > 2) printf("streamWrite(): %u bytes received\n", size);
	if (debug > 8) printf("response: %s\n", data);
	
	if (size != 0) {
		http_parser_execute(&(s->request_parser), &request_settings, data, size);
	}
	
}

void streamWriteResponse(struct stream *s, char *data, u_int32_t size) {
	if (debug > 2) printf("streamWrite(): %u bytes received\n", size);
	if (debug > 8) printf("response: %s\n", data);
	
	if (size != 0) {
		http_parser_execute(&(s->response_parser), &response_settings, data, size);
	}
	
}

void streamClose(struct stream *s) {
	output(s);
	json_decref(s->json);
}

int on_url(http_parser* _, const char* at, size_t length) {
  struct stream* stream = (struct stream*)_;
  size_t real_length = URL_MAXSIZE > length ? length : URL_MAXSIZE;
  memcpy(&(stream->url), at, real_length);
   
  json_object_set_new(stream->json, "method", json_string(http_method_str(stream->request_parser.method)));
  json_object_set_new(stream->json, "url", json_string(stream->url));
  return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
  struct stream* stream = (struct stream*)_;
  size_t real_length = HEADER_MAXSIZE > length ? length : URL_MAXSIZE;
  if ((!strncmp(at, "Referer", 7)) || (!strncmp(at, "Host", 4)) || (!strncmp(at, "User-Agent", 10))) {
  	memcpy(&(stream->cache), at, real_length);
  	stream->cache[real_length] = '\0';
  } else {
  	stream->cache[0] = '\0';
  }
  return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
  char value[1024];
  size_t real_length = 1024 > length ? length : 1024;
  struct stream* stream = (struct stream*)_;

  if(stream->cache[0] == '\0') return 0;
  memcpy(&value, at, real_length);
  value[real_length] = '\0';
  
  json_object_set_new(stream->json, stream->cache, json_string(value));
  return 0;
}

int on_status(http_parser* _, const char* at, size_t length) {
  _--;
  struct stream* stream = (struct stream*)_;
  streamClose(stream);
  hashDelete(&(stream->addr));
  return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
  struct stream* stream = (struct stream*)_;
  if (DATA_MAXSIZE > length) {
  	memcpy(&(stream->data), at, length);
  	stream->data[length] = '\0';
  	json_object_set_new(stream->json, "data", json_string(stream->data));
  }
  return 0;
}

void streamInit() {
	streamCount = 0;

  	memset(&request_settings, 0, sizeof(request_settings));
  	memset(&response_settings, 0, sizeof(response_settings));

	request_settings.on_url = on_url;
	request_settings.on_header_field = on_header_field;
	request_settings.on_header_value = on_header_value;
	request_settings.on_body = on_body;
	response_settings.on_status = on_status;
}
