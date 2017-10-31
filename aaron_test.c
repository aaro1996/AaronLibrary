#ifndef _AARON_TEST_C_
#define _AARON_TEST_C_

#include "aaron_test.h"
#include "aaron_hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) 
{
	hashmap_test();
}

int test_print_hashmap(aaron_hashmap* hashmap)
{
	char buf[20];
	uint size = (hashmap->threshold*8)>>(hashmap->is_growing);
	for(uint i = 0; i < size; ++i) {
		if (hashmap->buffer[i] > ((aaron_key_val_pair*) 0x1)) {
			sprintf(buf, "<%d, %d>", *((int*)(hashmap->buffer[i]->key)), *((int*)(hashmap->buffer[i]->val)));
		} else if (hashmap->buffer[i] == ((aaron_key_val_pair*) 0x1))
		{
			sprintf(buf, "Removed");
		} else {
			sprintf(buf, "Empty");
		}
		printf("%d: %s \n", i, buf);
	}
	if (hashmap->is_growing) {
		printf("Resizing hashmap, temp buffer follows \n");
		size *= 2;
		for(uint i = 0; i < size; ++i) {
			if (hashmap->new_buffer[i] > ((aaron_key_val_pair*) 0x1)) {
				sprintf(buf, "<%d, %d>", *((int*)(hashmap->new_buffer[i]->key)), *((int*)(hashmap->new_buffer[i]->val)));
			} else if (hashmap->new_buffer[i] == ((aaron_key_val_pair*) 0x1))
			{
				sprintf(buf, "Removed");
			} else {
				sprintf(buf, "Empty");
			}
			printf("%d: %s \n", i, buf);
		}
	}
	return 0;
}

int test_equality_check(void* a, void* b)
{
	int* i = (int*) a;
	int* j = (int*) b;
	return !((*i) == (*j));
}

uint test_hash_function(void* a)
{
	uint* i = (uint*) a;
	return *i;
}

int hashmap_test()
{
	char buf[10];
	size_t* buf2_size = malloc(sizeof(size_t));
	*buf2_size = 10;
	char** buf2 = malloc(sizeof(char*));
	*buf2 = malloc(*buf2_size*sizeof(char));
	int i, j;
	aaron_hashmap * hashmap = aaron_hashmap_init(1, test_hash_function, test_equality_check);
	while(1){
		getline(buf2, buf2_size, stdin);
		int n = sscanf(*buf2, "%s %d %d", buf, &i, &j);
		if ((n >= 2) && !strcmp(buf, "get")) {
			aaron_key_val_pair* pair = aaron_hashmap_get(hashmap, &i);
			if (pair != (aaron_key_val_pair*) 0x0) {
				int ok = *((int*)(pair->key));
				int ov = *((int*)(pair->val));
				printf("Got <%d, %d> \n", ok, ov);
			} else {
				printf("Couldn't find key %d \n", i);
			}
		} else if ((n >= 3) && !strcmp(buf, "put")) {
			int* key = malloc(sizeof(int));
			int* val = malloc(sizeof(int));
			*key = i;
			*val = j;
			aaron_key_val_pair* in_pair = aaron_key_val_pair_init((void*)key, (void*)val);
			aaron_key_val_pair* out_pair = aaron_hashmap_put(hashmap, in_pair);
			if (out_pair != (aaron_key_val_pair*) 0x0) {
				int ok = *((int*)(out_pair->key));
				int ov = *((int*)(out_pair->val));
				printf("Overwrote <%d, %d>  with <%d, %d>\n", ok, ov, *key, *val);
			} else {
				printf("Wrote <%d, %d> Successfully \n", *key, *val);
			}
		} else if ((n >= 2) && !strcmp(buf, "remove")) {
			aaron_key_val_pair* pair = aaron_hashmap_remove(hashmap, &i);
			if (pair != (aaron_key_val_pair*) 0x0) {
				int ok = *((int*)(pair->key));
				int ov = *((int*)(pair->val));
				printf("Removed <%d, %d> \n", ok, ov);
				aaron_key_val_pair_destroy(pair);
			} else {
				printf("Couldn't find key %d \n", i);
			}
		} else if ((n >= 1) && !strcmp(buf, "print")) {
			test_print_hashmap(hashmap);
		} else if ((n >= 1) && !strcmp(buf, "quit")) {
			break;
		}
	}
	free(*buf2);
	free(buf2);
	free(buf2_size);
	aaron_hashmap_free_all(hashmap);
	aaron_hashmap_destroy(hashmap);
	return 0;
}

#endif