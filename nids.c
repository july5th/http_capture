/*
 * HTTP CAPTURE
 * (C)2014 Liu Feiran
 *
 */

#include <string.h>
#include <nids.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "hash.h"
#include "stream.h"
#include "output.h"

/* the following line has been stolen from the official libnids example :)
   i'm not sure why libnids isn't using struct in_addr if their own example relies on its existence */
#define int_ntoa(x)     inet_ntoa(*((struct in_addr *)&x))

void nidsCallback(struct tcp_stream *ns, void **param) {
	struct stream *s;
	struct half_stream *hs;
	struct tuple4 a;
	
	char buffer[65535];

	if (debug > 5) printf("\n*** Callback from libnids ***\n");    
	
	memcpy(&a, &ns->addr, sizeof(struct tuple4));
	
	switch (ns->nids_state) {

		case NIDS_JUST_EST:
			//if (ns->addr.dest != 80 && ns->addr.dest != 443) break;
			//if (ns->addr.dest != 80) break;
			ns->client.collect++;
			ns->server.collect++;
			
			if ((s = hashFind(&a))) {
				printf("hashfind\n");
				if (!(s = hashDelete(&(s->addr)))) {
                			if (debug > 2) printf("nidsCallback(): Oops: Attempted to close unexisting connection. Ignoring.\n");
                			return;
        			}
				json_object_put(s->json);
				free(s);
			}

			if (!(s = malloc(sizeof(struct stream)))) {
				if (debug > 2) printf("nidsCallback(): Oops: Could not allocate stream memory.\n");
				return;
			}

			if(!hashAdd(&a, s)) {
				if (debug > 2) printf("nidsCallback(): Oops: Connection limit exceeded.\n");
				free(s);
				return;
			}
			streamOpen(s, &a);
			
			break;

		case NIDS_DATA:
			if (!(s = hashFind(&a))) {
				if (debug > 2) printf("nidsCallback(): Oops: Received data for unexisting connection. Ignoring.\n");
				break;
			}
			if (ns->client.count_new) {
				hs = &ns->client;
                                //printf("client receive %i\n", hs->count_new);
				memcpy(buffer, (char *)hs->data, hs->count_new);
				streamWriteResponse(s, buffer, hs->count_new);
			} else {
				hs = &ns->server;
                                //printf("server receive %i\n", hs->count_new);
				memcpy(buffer, (char *)hs->data, hs->count_new);
				streamWriteRequest(s, buffer, hs->count_new);
			}
			break;

		default:
			streamDelete(&a);
	}
}

static void hc_syslog(int type, int errnum, struct ip *iph, void *data) {
	return;
}

int nidsRun(char *filter) {
	nids_params.pcap_filter = filter;
	nids_params.n_tcp_streams = (MAX_CONNECTIONS * 2) / 3 + 1; /* libnids allows 3/4 * n simultaneous twin connections */

	struct nids_chksum_ctl temp;
	temp.netaddr = 0;
	temp.mask = 0;
	temp.action = 1;
	nids_register_chksum_ctl(&temp,1);

	nids_params.syslog = hc_syslog;

	if (!nids_init())
		return (-1);

	hashInit();
	streamInit();
	output_init();
	nids_register_tcp(nidsCallback);
        fprintf(stderr, "start nids run\n");
	nids_run();
        fprintf(stderr, "finish nids run\n");
	
	return (0);
}

int nids_device(char *dev, char *filter) {
	nids_params.device = dev;
	return (nidsRun(filter));
}

int nids_file(char *filename, char *filter) {
        nids_params.filename = filename;
        return (nidsRun(filter));
}

