#ifndef HASH_H
#define HASH_H

void hashInit();
void hashDebug();

void *hashFind(void *key);
void *hashFindAll();
int hashAdd(void *key, void *data);
void *hashDelete(void *key);

#endif
