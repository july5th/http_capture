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

#include "stream.h"
#include "output.h"

void output(struct stream *s)
{
  json_object_set_new(s->json, "code", json_integer(s->response_parser.status_code));
  //printf("%s m:%s u:%s r:%d o:%s\n", adres(s->addr), http_method_str(s->request_parser.method), s->url, s->response_parser.status_code, json_dumps(s->json, 0));
  printf("%s\n", json_dumps(s->json, JSON_PRESERVE_ORDER));
}

