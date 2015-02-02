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
#include <signal.h>
#include <time.h>

#include "stream.h"
#include "output.h"
#include "config.h"

int output_fd;
int output_size = 0;

void create_conn() {
  struct sockaddr_in sin = {0};
  int x;
  output_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (output_fd == -1) {
    fprintf(stderr, "redis: socket() failed to create socket\n");
    exit(1);
  }
  sin.sin_addr.s_addr = inet_addr(REDIS_HOST);
  sin.sin_port = htons((unsigned short)REDIS_PORT);
  sin.sin_family = AF_INET;
  x = connect(output_fd, (struct sockaddr*)&sin, sizeof(sin));
  if (x != 0) {
    fprintf(stderr, "redis: connect() failed\n");
    exit(1);
  }
}

void handle_pipe(int sig)
{
  printf("receive sig: %i\n", sig);
  create_conn(); 
}

void output_init() {
  if (redis_output == 1) {
  	struct sigaction action;
  	action.sa_handler = handle_pipe;
  	sigemptyset(&action.sa_mask);
  	action.sa_flags = 0;
  	create_conn();
  	sigaction(SIGPIPE, &action, NULL);
  }
  return;
}

int output(struct stream *s)
{
  char line[60000];
  time_t tt = time(NULL);
  if(s->is_http == 0)
    return 0;
  json_object_object_add(s->json, "time", json_object_new_int(tt));
  json_object_object_add(s->json, "hc", json_object_new_int(s->tmp));

  const char * tmp = json_object_to_json_string(s->json);

  if (redis_output == 1) {
    	snprintf(line, sizeof(line),
            "*3\r\n"
            "$5\r\nRPUSH\r\n"
            "$%u\r\n%s\r\n"
            "$%u\r\n%s\r\n"
            ,
            (unsigned)strlen("sec_input_queue"), "sec_input_queue",
            (unsigned)strlen(tmp), tmp
            );

  	int count = send(output_fd, line, (int)strlen(line), 0);

  	if (count != strlen(line)) {
    		printf("output socket error\n");
    		create_conn();
    		return 0;
  	}
  	return count;
  } else {
  	printf("%s\n", tmp);
	return 1;
  }
}

