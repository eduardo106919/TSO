/* metaindex.h */

#include <glib.h>
#include <stdint.h>
#include <pthread.h>

/**
 * @brief Size of keys at the Hashtable
 *
 */
#define KEY_SIZE 100
#define CONTENT_SIZE 10*1024*1024

/**
 * @brief struct defining the values of the Hashtable
 *
 */
typedef struct filemeta
{
    uint64_t size;
    int refs;
    char *content;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    char deleted;
} Filemeta;

/**
 * @brief struct containing the hashtable structure, global mutex and condition variable
 *
 */
typedef struct index
{
    GHashTable *htable;
    pthread_rwlock_t rwlock;
} Index;

/**
 * @brief Initializes the index structure
 *
 * @return NULL in case of failure and a pointer to the struct otherwise
 */
Index *index_init();

/**
 * @brief Adds a new key-value entry (filename-meta) into the Hashtable
 *
 * @param index
 * @param filename
 * @param meta
 * @return -1 in case of failure (or if the key already exists) and 0 otherwise
 */
int index_add(Index *index, char *filename, Filemeta meta);

/**
 * @brief Get the value (meta) for a specific key (filename)
 *
 * @param index
 * @param filename
 * @param meta
 * @return Returns -1 in case of failure (or if the key does not exists) and 0 otherwise
 */
int index_get(Index *index, char *filename, Filemeta *meta);

/**
 * @brief Increment by 1 the number of references for a given key (filename)
 *
 * @param index
 * @param filename
 * @return Returns -1 in case of failure (or if the key does not exists) and the updated number of references otherwise
 */
int index_increfs(Index *index, char *filename);

/**
 * @brief Remove a key-value entry from the Hashtable
 *
 * @param index
 * @param filename
 * @return -1 in case of failure (or if the key does not exists) and 0 otherwise
 */
int index_remove(Index *index, char *filename);

/**
 * @brief Destroys the index structure
 *
 * @param index
 */
void index_destroy(Index *index);

/**
 * @brief Shows the index structure
 *
 * @param index
 */
void index_show(Index *index);

/**
 * @brief Blocks until the number of references of a key (filename) reaches a given thredshold (nrefs)
 *
 * @param index
 * @param filename
 * @param nrefs
 * @return Returns -1 in case of failure (or if the key does not exists) and 0 otherwise
 */
int index_wait_refs(Index *index, char *filename, int nrefs);

/**
 * @brief increments the reference count (by 1) of a set of keys received as argument
 *
 * @param index
 * @param filenames
 * @param size
 * @return -1 in case of failure (or if at least one key does not exist) and 0 otherwise
 */
int index_multi_inc_refs(Index *index, char **filenames, int size);
