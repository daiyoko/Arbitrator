/* hash.c
 *
 * Implementation of seperate-chaining hash. Please use the #define HASH_
 * macros when using this API.
 *
 * TODO: Possible enhancements:
 *  - List keys/values without looking at every possible entry slot.
 *  - Dynamically increase hashsize when %in-use threshold capacity hit.
 *
 */
#include <stdlib.h>
#include <string.h>

#include "hash.h"


struct hashtable *
create_table(unsigned int table_s)
{
    size_t hashtable_s;

    hashtable_s = sizeof(hashtable_t) + sizeof(hashentry_t) * table_s;

    hashtable_t *table = (hashtable_t *)malloc(hashtable_s);
    memset(table, 0, sizeof(hashentry_t) * table_s);

    table->table_size = table_s;
    return table;
}


void
delete_table(hashtable_t **table)
{
    int i;
    struct hashentry *hash_entry;

    for (i = 0; i < (*table)->table_size; i++) {
        if ( (hash_entry = (*table)->entries[i]) != NULL) {
            printf("REMOVING KEY %s\n", hash_entry->key);
            remove_entry(*table, hash_entry->key);
        }
    }
    free(*table);

    // XXX: stale-pointer slapper...
    *table = NULL;
}


/*
 * Adds hashentry into the hashtable. Upon hash collicions, prepend new
 * hashentry to the chain.
 */
int
add_entry(hashtable_t *table, char *key, void *value)
{
    unsigned int hash_index;

    hashentry_t *new_entry = (hashentry_t *)malloc(sizeof(hashentry_t));

    new_entry->value = value;
    new_entry->key = malloc(sizeof(char) * strlen(key));
    memcpy(new_entry->key, key, strlen(key));

    hash_index = hash_function(new_entry->key, table->table_size);

    new_entry->next = &table->entries[hash_index];//FIXME:&
    table->entries[hash_index] = new_entry;
    return 0;
}


int
remove_entry(hashtable_t *table, void *key)
{
    unsigned int hash_index;
    struct hashentry *hash_entry, *temp_entry;

    hash_index = hash_function(key, table->table_size);
    if ( (hash_entry = table->entries[hash_index]) == NULL) {
        // no-such-entry in table
        return -1;
    }
    else if (!memcmp(hash_entry->key, key, sizeof(*key))) {
        table->entries[hash_index] = table->entries[hash_index]->next;
    }
    else {
        // walk-the-chain...
        while (hash_entry->next != NULL) {
            if (!memcmp(hash_entry->next->key, key, sizeof(*key))) {
                temp_entry = hash_entry->next;
                hash_entry->next = hash_entry->next->next;
                hash_entry = temp_entry;
                break;
            }
            hash_entry = hash_entry->next;
        }
        // no-such-entry in table
        return -1;
    }
    free(hash_entry);
    return 0;
}


void *
get_entry(hashtable_t *table, char *key)
{
    unsigned int hash_index;
    struct hashentry *hash_entry;

    hash_index = hash_function(key, table->table_size);
    hash_entry = table->entries[hash_index];

    // walk-the-chain...
    while (hash_entry != NULL) {
        if (!memcmp(hash_entry->key, key, sizeof(*key))) {
            return hash_entry->value;
        }
        hash_entry = hash_entry->next;
    }
    // no-such-entry in table
    return NULL;
}


/*
 * @param key: NULL terminated string. Change to "char *" if for sure.
 * @param maxsize: Total slots in the hashtable.
 */
unsigned int
hash_function(char *key, unsigned int maxsize)
{
    int sum, i;

    sum = 0;
    for (i = 0; i < strlen(key); i++) {
        sum += key[i];
    }
    return (unsigned int)sum % maxsize;
}
