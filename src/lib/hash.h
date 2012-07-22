#ifndef __HASH_H
#define __HASH_H

#define DEFAULT_TABLESIZE  4096


typedef struct hashtable {
    unsigned int table_size;        // # of indexes in the table.
    struct hashentry *entries[0];   // XXX: struct-hack
} hashtable_t;

#define HASH_CREATE() create_table(DEFAULT_TABLESIZE)
hashtable_t *create_table(unsigned int);

#define HASH_DELETE(table) delete_table(table)
void delete_table(hashtable_t **);


typedef struct hashentry {
    char *key;
    void *value;
    struct hashentry *next; // TODO: replace w/queue? BUT, what's the advantage?!
} hashentry_t;

#define HASH_ADD(table, key, value) add_entry(table, key, value)
int add_entry(hashtable_t *, char *, void *);

#define HASH_REMOVE(table, key) remove_entry(table, key)
int remove_entry(hashtable_t *, void *);

#define HASH_GET(table, key) get_entry(table, key)
void *get_entry(hashtable_t *, char *);


unsigned int hash_function(char *, unsigned int);


#endif /* __HASH_H */
