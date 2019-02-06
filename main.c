
#include <lmdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_LMDB    -1
#define ERR_OPEN    -2
#define ERR_ENV     -3

#define KEY_SIZE    16

MDB_env *env;
MDB_txn *txn;
MDB_dbi dbi;
char last_key[KEY_SIZE];

static char *rand_string(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }

    return str;
}

int open_database() {
    int err;

    err = mdb_env_create(&env);
    if (err != MDB_SUCCESS) {
        printf("Failed to create lmbd env! %d\n", err);
        return ERR_OPEN;
    }

    err = mdb_env_set_maxdbs(env, 1024);
    if (err != MDB_SUCCESS) {
        printf("Could not set maxdbs!\n");
        return ERR_LMDB;
    }

    err = mdb_env_set_mapsize(env, 1024 * 1024 * 1024);
    if (err != MDB_SUCCESS) {
        printf("Error while setting map size!\n");
        return ERR_LMDB;
    }

    err = mdb_env_open(env, ".", 0, 0644);
    if (err != MDB_SUCCESS) {
        printf("Could not open env %d\n", err);
        return ERR_ENV;
    }

    err = mdb_txn_begin(env, NULL, 0, &txn);
    if (err != MDB_SUCCESS) {
        printf("Failed to create lmdb txn! %d\n", err);
        return ERR_LMDB;
    }

    unsigned int dbi_flags = MDB_CREATE;

    err = mdb_dbi_open(txn, "./", dbi_flags, &dbi);
    if (err != MDB_SUCCESS) {
        printf("Failed to open db! %d\n", err);
        return ERR_LMDB;
    }

    return MDB_SUCCESS;
}

int write_to_database(int value_size, int verbose)
{
    MDB_val _key, _val;
    char *key = calloc('0', KEY_SIZE);
    rand_string(key, KEY_SIZE - 1);
    strncpy(last_key, key, KEY_SIZE);

    char *val= calloc('0', value_size);
    rand_string(val, value_size-1);

    _key.mv_data = (void *) key;
    _key.mv_size = KEY_SIZE;
    _val.mv_data = (void *) val;
    _val.mv_size = value_size;

    if (verbose) printf("setting the key : %s\n to value :\n%s\n", key, val);

    int err = mdb_put(txn, dbi, &_key, &_val, 0);

    free(key); free(val);

    if (err != MDB_SUCCESS) {
        printf("Failed to write db : key %s! %d\n", key, err);
        return ERR_LMDB;
    }
    return MDB_SUCCESS;
}

int read_value(char *key)
{
    MDB_val _key, _val;

    _key.mv_data = (void*) key;
    _key.mv_size = KEY_SIZE;

    int err = mdb_get(txn, dbi, &_key, &_val);

    if (err != MDB_SUCCESS) {
        printf("Error occured while getting value of %s\n", key);
        return err;
    }

    printf("The value :\n%s\n", (char*) _val.mv_data);

    return MDB_SUCCESS;
}

int main()
{
char *input;
input = calloc('0', 255);
printf("Commands to use:\nw1k (write 1024 bytes)\nw2k (write 2048 bytes)"
       "\nw512k (write 512 * 1024 bytes)\nw50m (write 50 MB)\nc commit)\ne "
       "(exit)\nr (read last key)\n");

open_database();
printf("$");
int false = 0, true = 1;
    while(fgets(input, 255, stdin)) {
        input[strlen(input) - 1] = '\0';

        if (0 == strcmp(input, "w1k")) {
            write_to_database(1024, true);
        } else if (0 == strcmp(input, "w2k")) {
            write_to_database(1024, true);
        } else if (0 == strcmp(input, "w512k")) {
            write_to_database(1024 * 512, false);
        } else if (0 == strcmp(input, "w50m")) {
            write_to_database(1024 * 1024 * 50, false);
            printf("50 MB written\n");
        } else if (0 == strcmp(input, "c")) {
            printf("commit\n");
            if (txn) {
                mdb_txn_commit(txn);
            }
            open_database();
        } else if (0 == strcmp(input, "r")) {
            read_value(last_key);
        } else if ('r' == input[0] && ' ' == input[1]) {
            read_value(input+2);
        } else if ((0 == strcmp(input, "e")) || (0 == strcmp(input, "q"))) {
            break;
        } else {
            printf("Commands to use:\nw1k (write 1024 bytes)\nw2k (write 2048 bytes)"
                   "\nw512k (write 512 * 1024 bytes)\nw50m (write 50 MB)\nc commit)\ne "
                   "(exit)\nr (read last key)\n");
        }
        printf("$");
    }

free(input);
return 0;
}


