#include <stdio.h>

#include "hash.h"


int main(int argc, char *argv[])
{
    int a, b, c;
    int a_key, b_key, c_key;
    a = 1;
    a_key = "11";

    b = 3;
    b_key = "3";

    c = 9;
    c_key = "9";
    hashtable_t *mytable = HASH_CREATE();

    HASH_ADD(mytable, a_key, &a);
    HASH_ADD(mytable, b_key, &b);
    HASH_ADD(mytable, c_key, &c);

    HASH_REMOVE(mytable, b_key);
    if ((HASH_GET(mytable, a_key)) != NULL)
        printf("PASS: key %s is %d\n", a_key,
               *(int *)HASH_GET(mytable, a_key));
    else
        printf("FAIL: key %d not in table\n", a_key);

    if ((HASH_GET(mytable, b_key)) != NULL)
        printf("FAIL: key %s is %d\n", b_key,
               *(int *)HASH_GET(mytable, b_key));
    else
        printf("PASS: key %s not in table\n", b_key);

    if ((HASH_GET(mytable, c_key)) != NULL)
        printf("PASS: key %s is %d\n", c_key,
               *(int *)HASH_GET(mytable, c_key));
    else
        printf("FAIL: key %d not in table\n", c_key);
    HASH_DELETE(&mytable);
}
