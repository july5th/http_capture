/*
 * HTTP CAPTURE
 * (C)2014 Liu Feiran
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "nids.h"

void mainHelp(char *name) {
	printf(
		"\n"
		"Usage: %s [<options>] <name> [<filter>]\n"
		"\n"
		"Where <options> are:\n"
		"	-i	force <name> to be treated as network interface\n"
		"\n"
		"	-v	verbose - additional v increase output further\n"
		"	-q	quiet - additional q decrease output further\n"
		"	-h	this help page\n"
		"\n"
		"The optional <filter> can be any pcap filter program. See tcpdump man page for\n"
		"details.\n"
		"\n"
		,
		name
	);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	int i, j, result, filterSize = 1;
	char *name = NULL, *filter = NULL;
	
	/* set defaults */
	debug = 1;

	/* parse arguments */
	if (argc < 2)
		mainHelp(argv[0]);
	
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (j = 1; j < strlen(argv[i]); j++) {
				switch (argv[i][j]) {
					case 'h':
						mainHelp(argv[0]);
						break;
	
					case 'i':
						break;

					case 'q':
						if (debug > 0) debug--;
						break;

					case 'v':
						debug++;
						break;
	
					default:
						printf("FATAL: Invalid option: %s\n", argv[i]);
						mainHelp(argv[0]);
						break;
				}
			}
		} else {
			if (!name) {
				name = argv[i];
			} else {
				filterSize += strlen(argv[i]) + 1;
				filter = realloc(filter, filterSize);
				if (!filter) {
					perror("malloc()");
					exit(EXIT_FAILURE);
				}
				strcat(filter, argv[i]);
				strcat(filter, " ");
			}
		}
	}

	
	if (debug > 0) printf("Trying to read from device '%s'...\n", name);
		
	result = nidsDevice(name, filter);
		
	if (result) {
		if (debug > 0) printf("Failed.\n");
		return (EXIT_FAILURE);
	} else {
		if (debug > 0) printf("Finished.\n");
		return (EXIT_SUCCESS);
	}

	return (EXIT_FAILURE); // make compiler happy
}
