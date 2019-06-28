#include "sys_inc.h"
#include "log_trace.h"
#include "usr_uthash.h"

int uthash_put(hashtable_t **hashHead, char *ht_key, void *ht_value, _free_call_ f)
{
	hashtable_t *ht = (hashtable_t *)calloc(sizeof(hashtable_t), 1);
	if (! ht)
	{
		LOG_TRACE_PERROR("calloc error !\n");
		return -1;
	}
	
	ht->key = calloc(1, strlen(ht_key) + 1);
	if (! ht->key)
	{
		LOG_TRACE_PERROR("calloc error !\n");
		free(ht);
		return -1;
	}
	
	memcpy(ht->key, ht_key, strlen(ht_key));
	
	ht->value = ht_value;
	ht->f = f;

	HASH_ADD_STR((*hashHead), key, ht);

	return 0;
}

void *uthash_find(hashtable_t **hashHead, char *ht_key)
{
	hashtable_t *out = NULL;

	HASH_FIND_STR((*hashHead), ht_key, out);
	if (! out)
	{
		//LOG_TRACE_NORMAL("the key(%s) is not exist in hash!\n", ht_key);
		return NULL;
	}

	return out->value;
}

int uthash_count(hashtable_t **hashHead)
{
	return HASH_COUNT((*hashHead));
}

void uthash_delete(hashtable_t **hashHead, hashtable_t **current)
{
	if (! (*current))
		return;
	
	if ((*current) && (*current)->key)
	{
		free((*current)->key);
		(*current)->key = NULL;
	}
	
	if ((*current) && (*current)->f)
	{
		(*current)->f((*current)->value);
		(*current)->value = NULL;
	}

	HASH_DEL((*hashHead), (*current));
	
	if ((*current))
		free((*current));

	(*current) = NULL;
}

void uthash_destroy(hashtable_t **hashHead)
{
	hashtable_t *current = NULL;
	hashtable_t *tmp = NULL;
	
	HASH_ITER(hh, (*hashHead), current, tmp)
	{
		//LOG_TRACE_NORMAL("[safe traversal]key:%s, value addr:%d\n", current->key, (intptr_t) current->value);
		uthash_delete(hashHead, &current);
		current = NULL;
	}
}

void uthash_delete_by_key(hashtable_t **hashHead, char *key)
{
	hashtable_t *current = NULL;
	hashtable_t *tmp = NULL;
	
	HASH_ITER(hh, (*hashHead), current, tmp)
	{
		if (key && current->key 
			&& strcmp(key, current->key) == 0)
		{
			//LOG_TRACE_NORMAL("[safe traversal]key:%s, value addr:%d\n", current->key, (intptr_t) current->value);
			uthash_delete(hashHead, &current);
			current = NULL;
			break;
		}
	}
}