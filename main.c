/*
 * HTTP CAPTURE
 * (C)2014 Liu Feiran
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <getopt.h>

#include "main.h"
#include "config.h"
#include "nids.h"

void usage(char *name) {
	printf(
		"\n"
		"Usage: %s [<options>] <name> [<filter>]\n"
		"\n"
		"Where <options> are:\n"
		"	-i	force <name> to be treated as network interface\n"
		"	-f	pcap file name\n"
		"	-a	catch all request header\n"
		"	-b	catch request body\n"
		"	-r	use base64 format\n"
		"	-d	run as dameo\n"
		"	-p	print ouput to screen instead of redis\n"
		"\n"
		"	-v	verbose - additional v increase output further\n"
		"	-q	quiet - additional q decrease output further\n"
		"	-h	this help page\n"
		"\n"
		,
		name
	);
	exit(EXIT_FAILURE);
}

void to_daemon() {
	int i, max_fd;
	pid_t pid;
	signal(SIGHUP, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	
	pid = fork();
	if(pid > 0) exit(0);
	setsid();
	pid = fork();
	if(pid > 0) exit(0);
	setpgrp();
	max_fd = sysconf(_SC_OPEN_MAX);
	for(i=0; i < max_fd; i++)
		close(i);
	umask(0);
	chdir("/");
}

int main(int argc, char **argv) {
	char device_or_pcap_name[1024];

	/* set defaults */
	int run_mode = MODE_DEVICE;
	int daemon = 0;
	debug = 1;
	catch_request_body = 0;
	redis_output = 1;
	body_base64_output = 0;
	print_all_request_header = 0;

        int result;
        int arg_options;
        const char *short_options = "f:i:abrdpvqh";
	char filter_string[256] = " ";

        const struct option long_options[] = {
                {"pcap-file", required_argument, NULL, 'f'},
                {"interface", required_argument, NULL, 'i'},
                {"print-request-body", required_argument, NULL, 'b'},
                {"dameo", required_argument, NULL, 'd'},
                {"print", required_argument, NULL, 'p'},
                {"verbose", required_argument, NULL, 'v'},
                {"quite", required_argument, NULL, 'q'},
                {"help", no_argument, NULL, 'h'},
                {NULL, 0, NULL, 0}
        };

        while ((arg_options =
                        getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {

                switch (arg_options) {

                case 'f':
			strcpy(device_or_pcap_name, optarg);
			run_mode = MODE_PCAP;
                        break;
                case 'i':
			strcpy(device_or_pcap_name, optarg);
			run_mode = MODE_DEVICE;
                        break;
                case 'a':
			print_all_request_header = 1;
                        break;
                case 'b':
			catch_request_body = 1;
                        break;
                case 'r':
			body_base64_output = 1;
                        break;
                case 'd':
                        daemon = 1;
                        break;
                case 'p':
                        redis_output = 0;
                        break;
                case 'v':
                        debug++;
                        break;
                case 'q':
                        if (debug > 0) debug--;;
                        break;
                case 'h':
                        usage(argv[0]);
                        return 0;
                        break;

                default:
                        printf("CMD line Options Error\n\n");
                        break;
                }
	}

	if (daemon != 0) to_daemon();
	if (debug > 0) printf("Trying to read from '%s'...\n", device_or_pcap_name);
		
	if(run_mode == MODE_DEVICE)
		result = nids_device(device_or_pcap_name, filter_string);
	else if(run_mode == MODE_PCAP)
		result = nids_file(device_or_pcap_name, filter_string);
		
	if (result) {
		if (debug > 0) printf("Failed.\n");
		return (EXIT_FAILURE);
	} else {
		if (debug > 0) printf("Finished.\n");
		return (EXIT_SUCCESS);
	}

	return (EXIT_FAILURE); // make compiler happy
}
