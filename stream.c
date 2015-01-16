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
#include <stdlib.h>
#include <ctype.h>
#include <nids.h>

#include "base64.h"
#include "config.h"
#include "http_parser.h"
#include "stream.h"
#include "hash.h"
#include "output.h"

http_parser_settings request_settings;
http_parser_settings response_settings;

void streamOpen(struct stream *s, struct tuple4 *addr) {
	memset(s, 0, sizeof(struct stream));
	memcpy(&(s->addr), addr, sizeof(struct tuple4));
	streamClean(s);
	s->tmp = 0;
}


void streamClean(struct stream *s) {
	char buffer[1024];
	http_parser_init(&(s->request_parser), HTTP_REQUEST);
	http_parser_init(&(s->response_parser), HTTP_RESPONSE);
        s->is_http = 0;
	s->json = json_object_new_object();
	sprintf(buffer, "%s:%i", int_ntoa(s->addr.saddr), s->addr.source);
	json_object_object_add(s->json, "src", json_object_new_string(buffer));
	sprintf(buffer, "%s:%i", int_ntoa(s->addr.daddr), s->addr.dest);
	json_object_object_add(s->json, "dst", json_object_new_string(buffer));
}

void streamWriteRequest(struct stream *s, char *data, u_int32_t size) {
	if (debug > 2) printf("streamWrite(): %u bytes received\n", size);
	if (debug > 8) printf("request:\n%s\n", data);
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
	json_object_put(s->json);
}

int on_url(http_parser* _, const char* at, size_t length) {
  char *t;
  struct stream* stream = (struct stream*)_;
  size_t real_length = URL_MAXSIZE > length ? length : URL_MAXSIZE;
  memcpy(&(stream->url), at, real_length);
  stream->url[real_length] = '\0';
  stream->is_http = 1;
  json_object_object_add(stream->json, "method", json_object_new_string(http_method_str(stream->request_parser.method)));
  if (base64_output == 1) {
    t = Base64Encode(stream->url, real_length);
    json_object_object_add(stream->json, "url", json_object_new_string(t));
    free(t);
  } else {
    json_object_object_add(stream->json, "url", json_object_new_string(stream->url));
  }
  return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
  struct stream* stream = (struct stream*)_;
  size_t real_length = HEADER_MAXSIZE > length ? length : URL_MAXSIZE;
  int i;
 
  if ((!strncmp(at, "Referer", 7)) 
	|| (!strncmp(at, "Host", 4)) 
	|| (!strncmp(at, "User-Agent", 10))
	|| (!strncmp(at, "Cookie", 6))
	|| (!strncmp(at, "X-Forwarded-For", 15))
	) {
    memcpy(&(stream->cache), at, real_length);
    stream->cache[real_length] = '\0';
    for(i = 0; i < real_length; i++)
        stream->cache[i] = tolower(stream->cache[i]);
  } else {
    if(print_all_request_header == 1) {
      memcpy(&(stream->cache), at, real_length);
      stream->cache[real_length] = '\0';
    } else {
      stream->cache[0] = '\0';
    }
  }
  return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
  char value[HEADER_MAXSIZE], *t;
  size_t real_length = HEADER_MAXSIZE > length ? length : HEADER_MAXSIZE;
  struct stream* stream = (struct stream*)_;
  if(stream->cache[0] == '\0') return 0;
  memcpy(&value, at, real_length);
  value[real_length] = '\0';
  if (base64_output == 1) {
    t = Base64Encode(value, real_length);
    json_object_object_add(stream->json, stream->cache, json_object_new_string(t));
    free(t);
  } else {
    json_object_object_add(stream->json, stream->cache, json_object_new_string(value));
  }

  return 0;
}

int on_status(http_parser* _, const char* at, size_t length) {
  _--;
  struct stream* stream = (struct stream*)_;
  json_object_object_add(stream->json, "code", json_object_new_int(stream->response_parser.status_code));
  //streamDelete(&(stream->addr));
  return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
  struct stream* stream = (struct stream*)_;
  char *t;
  if (DATA_MAXSIZE > length) {
  	memcpy(&(stream->data), at, length);
  	stream->data[length] = '\0';
	if (base64_output == 1) {
		t = Base64Encode(stream->data, length);
  		json_object_object_add(stream->json, "data", json_object_new_string(t));
		free(t);
	} else {
  		json_object_object_add(stream->json, "data", json_object_new_string(stream->data));
	}
  }
  return 0;
}

int session_over(http_parser* _) {
  	_--;
  	struct stream* stream = (struct stream*)_;
	stream->tmp += 1;
	streamClose(stream);
	streamClean(stream);
	return 0;
}

void streamInit() {
	streamCount = 0;

  	memset(&request_settings, 0, sizeof(request_settings));
  	memset(&response_settings, 0, sizeof(response_settings));

	request_settings.on_url = on_url;
	request_settings.on_header_field = on_header_field;
	request_settings.on_header_value = on_header_value;

	if (catch_request_body != 0)
		request_settings.on_body = on_body;

	response_settings.on_status = on_status;
	response_settings.on_message_complete = session_over;
}

void streamDelete(struct tuple4 *addr) {
	struct stream *s;
	if (!(s = hashDelete(addr))) {
		if (debug > 2) printf("nidsCallback(): Oops: Attempted to close unexisting connection. Ignoring.\n");
		return;
	}
	streamClose(s);
	free(s);
}

