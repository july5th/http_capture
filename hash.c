/*
 * HTTP CAPTURE
 * (C)2004-2005 Michael Poppitz
 *
 * Maintains hash table for fast connection lookups.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"
#include "hash.h"

#define HASH_KEYSIZE 12			/* two IP addresses and two ports (2*4+2*2) */

struct hashEntry {
	u_char key[HASH_KEYSIZE];
	void *data;
	struct hashEntry *next;
};

struct hashEntry *hashTable[CONNECTION_BUCKETS];
int hashTableEntries;

/* debugging function - outputs hash table contents */
void hashDebug() {
	struct hashEntry *tmp;
	int i;
		
	for (i = 0; i < CONNECTION_BUCKETS; i++) {
		printf("%2i:", i);
		for (tmp = hashTable[i]; tmp; tmp = tmp->next) {
			printf("* ");
		}
		printf("\n");
	}
}

/* helper function - calculates hash of a key */
u_int hashCalc(void *key) {
	int i;
	u_int hash;

	/* no need to do elaborated hashing - port numbers and ips are random enough */
	hash = 0;
	for (i = 0; i < HASH_KEYSIZE; i++) {
		hash += ((u_char *)key)[i];
	}
	return (hash % CONNECTION_BUCKETS);
}

/* initializes hash buckets */
void hashInit() {
	int i;
	
	for (i = 0; i < CONNECTION_BUCKETS; i++)
		hashTable[i] = NULL;

	hashTableEntries = 0;
}

/* finds entry with given key */
void *hashFind(void *key) {
	struct hashEntry *tmp;
	
	tmp = hashTable[hashCalc(key)];
	while (tmp) {
		if (memcmp(&tmp->key, key, HASH_KEYSIZE) == 0) {
			return (tmp->data);
		}
		tmp = tmp->next;
	}
	return (NULL);
}

/* finds first entry in hash table and returns its key */
void *hashFindAll() {
	int i;
	
	for (i = 0; i < CONNECTION_BUCKETS; i++)
		if (hashTable[i])
			return (&hashTable[i]->key);
	return (NULL);
}

/* attempts to add an entry to the hash table */
int hashAdd(void *key, void *data) {
	struct hashEntry *tmp, *new;
	int bucket;

	if (hashTableEntries >= MAX_CONNECTIONS)
		return (0);

	if (!(new = (struct hashEntry *)malloc(sizeof(struct hashEntry))))
		return (0);
		
	memcpy(new->key, key, HASH_KEYSIZE);
	new->data = data;
	new->next = NULL;
	
	bucket = hashCalc(key);
	
	if(!hashTable[bucket]) {
		hashTable[bucket] = new;
	} else {
		for (tmp = hashTable[bucket]; tmp->next; tmp = tmp->next);
		tmp->next = new;
	}

	hashTableEntries++;
	return (1);
}

void *hashDelete(void *key) {
	struct hashEntry *tmp, *tmp2;
	void *data;
	int bucket;

	bucket = hashCalc(key);
	tmp = hashTable[bucket];
	
	if (!tmp)
		return (NULL);
	
	if (memcmp(&tmp->key, key, HASH_KEYSIZE) == 0) {
		hashTable[bucket] = tmp->next;
		data = tmp->data;
		free(tmp);
		hashTableEntries--;
		return (data);
	
	} else {
		while (tmp->next && (memcmp(&tmp->next->key, key, HASH_KEYSIZE) != 0)) {
			tmp = tmp->next;
		}
		if (tmp->next) {
			tmp2 = tmp->next;
			data = tmp->next->data;
			tmp->next = tmp->next->next;
			free(tmp2);
			hashTableEntries--;
			return (data);
		}
	}

	return (NULL);
}
