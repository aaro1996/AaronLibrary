#ifndef _AARON_HASHMAP_C_
#define _AARON_HASHMAP_C_
#include "aaron_hashmap.h"

aaron_key_val_pair* aaron_key_val_pair_init(void* key, void* val) 
{
	aaron_key_val_pair* pair = malloc(sizeof(aaron_key_val_pair));
	pair->key = key;
	pair->val = val;
	return pair;
}

int aaron_key_val_pair_destroy(aaron_key_val_pair* pair) 
{

	free(pair->key);
	free(pair->val);
	free(pair);
	return 0;
}

aaron_hashmap* aaron_hashmap_init(uint power_of_two, uint (*_hash_function_in_)(void* key), int (*_equality_check_in_)(void* key1, void* key2))
{
	aaron_hashmap* hashmap = malloc(sizeof(aaron_hashmap));
	hashmap->is_growing = 0;
	hashmap->threshold = (1 << (power_of_two > 0 ? power_of_two : 1)); //hashmap size must be at least 4 to start.
	hashmap->current_population = 0;
	hashmap->transfer_location = 0;
	hashmap->buffer = calloc(hashmap->threshold * 8, sizeof(aaron_key_val_pair*));
	hashmap->_hash_function_ = _hash_function_in_;
	hashmap->_equality_check_ = _equality_check_in_;
	return hashmap;
}


int aaron_hashmap_destroy(aaron_hashmap* hashmap) 
{
	free(hashmap->buffer);
	if(hashmap->is_growing) {
		free(hashmap->new_buffer);
	}
	free(hashmap);
	return 0;
}


int aaron_hashmap_free_all(aaron_hashmap* hashmap)
{
	int i;
	if(hashmap->is_growing) {
		for(i = 0; i < hashmap->threshold*4; i++) {
			if(hashmap->buffer[i] != 0x0 && hashmap->buffer[i] != (aaron_key_val_pair*) 0x1)  {
				aaron_key_val_pair_destroy(hashmap->buffer[i]);
			}
		}
		for(i = 0; i < (hashmap->threshold*8); i++) {
			if(hashmap->new_buffer[i] != 0x0 && hashmap->new_buffer[i] != (aaron_key_val_pair*) 0x1)  {
				aaron_key_val_pair_destroy(hashmap->new_buffer[i]);
			}
		}
	} else {
		for(i = 0; i < (hashmap->threshold*8); i++) {
			if(hashmap->buffer[i] != 0x0 && hashmap->buffer[i] != (aaron_key_val_pair*) 0x1) 
			{
				aaron_key_val_pair_destroy(hashmap->buffer[i]);
			}
		}
	}
	return 0;
}

void aaron_hashmap_start_resizing(aaron_hashmap* hashmap)
{
	hashmap->is_growing = 1;
	hashmap->transfer_location = 0;
	hashmap->threshold *= 2;
	hashmap->new_buffer = calloc((hashmap->threshold * 8), sizeof(aaron_key_val_pair*));
	aaron_hashmap_resize(hashmap);
	aaron_hashmap_resize(hashmap);
}


void aaron_hashmap_resize(aaron_hashmap* hashmap) {
	if(hashmap->buffer[hashmap->transfer_location] > (aaron_key_val_pair*) 0x1) {
		aaron_hashmap_put_helper(hashmap, hashmap->new_buffer, hashmap->threshold*8, hashmap->buffer[hashmap->transfer_location]);
		hashmap->buffer[hashmap->transfer_location] = (aaron_key_val_pair*) 
		0x1;
	}
	hashmap->transfer_location++;
	if(hashmap->buffer[hashmap->transfer_location] > (aaron_key_val_pair*) 0x1) {
		aaron_hashmap_put_helper(hashmap, hashmap->new_buffer, hashmap->threshold*8, hashmap->buffer[hashmap->transfer_location]);
		hashmap->buffer[hashmap->transfer_location] = (aaron_key_val_pair*) 0x1;
	}
	hashmap->transfer_location++;
	if(hashmap->transfer_location >= hashmap->threshold*4) {
		aaron_hashmap_finish_resizing(hashmap);
	}

}


void aaron_hashmap_finish_resizing(aaron_hashmap* hashmap) {
	hashmap->is_growing = 0;
	free(hashmap->buffer);
	hashmap->buffer = hashmap->new_buffer;
}

aaron_key_val_pair* aaron_hashmap_remove_helper(aaron_hashmap* hashmap, aaron_key_val_pair** buffer, uint size, void* remove_key)
{
	uint location = (hashmap->_hash_function_(remove_key))%size;
	while(buffer[location] != (aaron_key_val_pair*) 0x0) { //While the buffer location isn't a 0 keep incrementing.
		if((buffer[location] != (aaron_key_val_pair*) 0x1) && ((hashmap->_equality_check_(remove_key, buffer[location]->key)) == 0)) {
			aaron_key_val_pair* retval = buffer[location];
			buffer[location] = (aaron_key_val_pair*) 0x1;
			return retval;
		}
		location = (location+1)%size;
	}
	return (aaron_key_val_pair*) 0x0;
}

aaron_key_val_pair* aaron_hashmap_remove(aaron_hashmap* hashmap, void* remove_key)
{
	aaron_key_val_pair* retval = (aaron_key_val_pair*) 0x0;
	if(hashmap->is_growing) {
		if((retval = aaron_hashmap_remove_helper(hashmap, hashmap->buffer, hashmap->threshold*4, remove_key)) || (retval = aaron_hashmap_remove_helper(hashmap, hashmap->new_buffer, hashmap->threshold*8, remove_key))) {
			hashmap->current_population--;
		}
	} else {
		if((retval = aaron_hashmap_remove_helper(hashmap, hashmap->buffer, hashmap->threshold*8, remove_key))) {
			hashmap->current_population--;
		}
	}
	return retval;
}

aaron_key_val_pair* aaron_hashmap_put_helper(aaron_hashmap* hashmap, aaron_key_val_pair** buffer, uint size, aaron_key_val_pair* input_pair)
{
	uint location = hashmap->_hash_function_(input_pair->key)%size;
	while(buffer[location] > (aaron_key_val_pair*) 0x1) { //While the buffer location isn't either a 0 or a 1, keep incrementing.
		if((buffer[location] != (aaron_key_val_pair*) 0x1) && ((hashmap->_equality_check_(input_pair->key, buffer[location]->key)) == 0)) {
			aaron_key_val_pair* retval = buffer[location];
			buffer[location] = input_pair;
			return retval;
		}
		location = (location+1)%size;
	}
	buffer[location] = input_pair;
	return (aaron_key_val_pair*) 0x0;

}

aaron_key_val_pair* aaron_hashmap_put(aaron_hashmap* hashmap, aaron_key_val_pair* input_pair)
{
	aaron_key_val_pair* retval = (aaron_key_val_pair*) 0x0;
	if(hashmap->is_growing) {
		if((retval = aaron_hashmap_put_helper(hashmap, hashmap->buffer, hashmap->threshold*4, input_pair))){
			aaron_hashmap_put_helper(hashmap, hashmap->new_buffer, hashmap->threshold*8, input_pair);
		} else if ((retval = aaron_hashmap_put_helper(hashmap, hashmap->new_buffer, hashmap->threshold*8, input_pair))) {
		} else {
			++(hashmap->current_population);
		}
		aaron_hashmap_resize(hashmap);
	} else {
		if ((retval = aaron_hashmap_put_helper(hashmap, hashmap->buffer, hashmap->threshold*8, input_pair))) {
		} else {
			++(hashmap->current_population);
		}
		if(hashmap->current_population >= hashmap->threshold) {
			aaron_hashmap_start_resizing(hashmap);
		}
	}
	return retval;
}

aaron_key_val_pair* aaron_hashmap_get_helper(aaron_hashmap* hashmap, aaron_key_val_pair** buffer, uint size, void* key) 
{
	uint location = (uint)(hashmap->_hash_function_(key))%size;
	while(buffer[location] != (aaron_key_val_pair*) 0x0) { //While the buffer location isn't a 0, keep incrementing.
		if((buffer[location] != (aaron_key_val_pair*) 0x1) && ((hashmap->_equality_check_(key, buffer[location]->key)) == 0)) {
			return buffer[location];
		}
		location = (location+1)%size;
	}
	return (aaron_key_val_pair*) 0x0;
}

aaron_key_val_pair* aaron_hashmap_get(aaron_hashmap* hashmap, void* key)
{
	aaron_key_val_pair* retval;
	if(hashmap->is_growing) {
		aaron_key_val_pair* new_buffer_url = aaron_hashmap_get_helper(hashmap, hashmap->new_buffer, hashmap->threshold*8, key);
		retval = ( new_buffer_url ? new_buffer_url : aaron_hashmap_get_helper(hashmap, hashmap->buffer, hashmap->threshold*4, key));
	} else {
		retval = aaron_hashmap_get_helper(hashmap, hashmap->buffer, hashmap->threshold*8, key);
	}
	return retval;

}


#endif