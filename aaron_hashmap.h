#ifndef _AARON_HASHMAP_H_
#define _AARON_HASHMAP_H_
#define _AARON_HASHMAP_LOAD_FACTOR_
#include <stdlib.h>
typedef struct{
	void* key;
	void* val;
} aaron_key_val_pair;

typedef struct{
	aaron_key_val_pair** buffer;//When buffer hits threshold, begins moving things to the new buffer.  hashmap is using open addressing, so a pointer of null means nothing has ever been there, a pointer of 1 means something has been.  Something being there means something is there.
	aaron_key_val_pair** new_buffer;    //Uses a load factor of .125
	uint transfer_location; //Where the transfer currently is in the old buffer.
	uint current_population; //Current number of things in the hashmap
	uint threshold; //Threshold to begin resizing.
	uint (*_hash_function_)(void* key); //Hash function over whatever type the keys are
	int (*_equality_check_)(void* key1, void* key2); //Equality check function over whatever type the keys are
	unsigned char is_growing; //Whether the hashmap is resizing right now
} aaron_hashmap;

aaron_key_val_pair* aaron_key_val_pair_init(void* key, void* val);
int aaron_key_val_pair_destroy(aaron_key_val_pair* pair); 

aaron_hashmap* aaron_hashmap_init(uint power_of_two, uint (*_hash_function_in_)(void* key), int (*_equality_check_in_)(void* key1, void* key2));
int aaron_hashmap_destroy(aaron_hashmap* hashmap);
int aaron_hashmap_free_all(aaron_hashmap* hashmap);

void aaron_hashmap_start_resizing(aaron_hashmap* hashmap);
void aaron_hashmap_resize(aaron_hashmap* hashmap);
void aaron_hashmap_finish_resizing(aaron_hashmap* hashmap);

aaron_key_val_pair* aaron_hashmap_remove_helper(aaron_hashmap* hashmap, aaron_key_val_pair** buffer, uint size, void* remove_key);
aaron_key_val_pair* aaron_hashmap_remove(aaron_hashmap* hashmap, void* remove_key);

aaron_key_val_pair* aaron_hashmap_put_helper(aaron_hashmap* hashmap, aaron_key_val_pair** buffer, uint size, aaron_key_val_pair* input_pair);
aaron_key_val_pair* aaron_hashmap_put(aaron_hashmap* hashmap, aaron_key_val_pair* input_pair);

aaron_key_val_pair* aaron_hashmap_get_helper(aaron_hashmap* hashmap, aaron_key_val_pair** buffer, uint size, void* key);
aaron_key_val_pair* aaron_hashmap_get(aaron_hashmap* hashmap, void* key);

#endif