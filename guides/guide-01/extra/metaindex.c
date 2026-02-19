/* metaindex.c */

#include "metaindex.h"
#include <stdlib.h>
#include <stdio.h>

void destroy_filemeta(void *ptr)
{
    if (ptr != NULL)
    {
        Filemeta *temp = ptr;
        free(temp->content);

        pthread_mutex_destroy(&temp->mutex);
        pthread_cond_destroy(&temp->cond);

        free(temp);
    }
}

Index *index_init()
{
    // allocate memory for the index
    Index *index = malloc(sizeof(Index));

    // GHashTable initialization
    index->htable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, destroy_filemeta);

    pthread_rwlock_init(&index->rwlock, NULL);

    return index;
}

int index_add(Index *index, char *filename, Filemeta meta)
{
    int out = -1;
    if (index == NULL || filename == NULL)
        return out;

    // acquire the write lock
    pthread_rwlock_wrlock(&index->rwlock);

    // check if the key exists
    if (g_hash_table_contains(index->htable, filename) == FALSE)
    {
        Filemeta *temp = malloc(sizeof(Filemeta));
        if (temp != NULL)
        {
            temp->size = meta.size;
            temp->refs = meta.refs;
            temp->content = strdup(meta.content);
            temp->deleted = 0;
            pthread_mutex_init(&temp->mutex, NULL);
            pthread_cond_init(&temp->cond, NULL);

            g_hash_table_insert(index->htable, strdup(filename), temp);
            out = 0;
        }
    }

    // release the lock
    pthread_rwlock_unlock(&index->rwlock);

    return out;
}

int index_get(Index *index, char *filename, Filemeta *meta)
{
    int out = -1;
    if (index == NULL || filename == NULL || meta == NULL)
        return out;

    // acquire the read lock
    pthread_rwlock_rdlock(&index->rwlock);

    Filemeta *temp = g_hash_table_lookup(index->htable, filename);
    if (temp != NULL)
    {
        pthread_mutex_lock(&(temp->mutex));

        // release the lock
        pthread_rwlock_unlock(&index->rwlock);

        out = 0;

        meta->refs = temp->refs;
        meta->size = temp->size;
        meta->content = strdup(temp->content);

        pthread_mutex_unlock(&(temp->mutex));
    }
    else
    {
        // release the lock
        pthread_rwlock_unlock(&index->rwlock);
    }

    return out;
}

int index_increfs(Index *index, char *filename)
{
    int out = -1;
    if (index == NULL || filename == NULL)
        return out;

    // acquire the read lock
    pthread_rwlock_rdlock(&index->rwlock);

    Filemeta *temp = g_hash_table_lookup(index->htable, filename);
    if (temp != NULL)
    {
        pthread_mutex_lock(&(temp->mutex));

        // release the lock
        pthread_rwlock_unlock(&index->rwlock);

        out = temp->refs++;

        pthread_mutex_unlock(&(temp->mutex));
    }
    else
    {
        // release the lock
        pthread_rwlock_unlock(&index->rwlock);
    }

    return out;
}

static void index_signal_to_remove(Index *index, char *filename)
{
    // acquire the read lock
    pthread_rwlock_rdlock(&index->rwlock);

    Filemeta *temp = g_hash_table_lookup(index->htable, filename);
    if (temp != NULL)
    {
        pthread_mutex_lock(&(temp->mutex));

        temp->deleted = 1;
        // signal other threads that the entry was removed
        pthread_cond_broadcast(&(temp->cond));

        pthread_mutex_unlock(&(temp->mutex));
    }

    // release the lock
    pthread_rwlock_unlock(&index->rwlock);
}

int index_remove(Index *index, char *filename)
{
    int out = -1;
    if (index == NULL || filename == NULL)
        return out;

    // tell other threads this entry will be removed
    index_signal_to_remove(index, filename);

    // acquire the write lock
    pthread_rwlock_wrlock(&index->rwlock);

    g_hash_table_remove(index->htable, filename);

    // release the lock
    pthread_rwlock_unlock(&index->rwlock);

    return out;
}

void index_destroy(Index *index)
{
    if (index != NULL)
    {
        // destroy hashtable
        g_hash_table_destroy(index->htable);

        pthread_rwlock_destroy(&index->rwlock);

        free(index);
    }
}

static void index_lock_entries(gpointer key, gpointer value, gpointer user_data)
{
    Filemeta *temp = value;
    // acquire lock
    pthread_mutex_lock(&(temp->mutex));
}

static void index_unlock_entries(gpointer key, gpointer value, gpointer user_data)
{
    Filemeta *temp = value;
    // release lock
    pthread_mutex_unlock(&(temp->mutex));
}

static void show_entry(gpointer key, gpointer value, gpointer user_data)
{
    Filemeta *meta = value;
    printf("[key: %s, size: %lu, refs: %d, content: %s]\n", (char *)key, meta->size, meta->refs, meta->content);
}

void index_show(Index *index)
{
    if (index != NULL)
    {
        // acquire the read lock
        pthread_rwlock_rdlock(&index->rwlock);

        // acquire the locks of each entry
        g_hash_table_foreach(index->htable, index_lock_entries, NULL);

        // release the read lock
        pthread_rwlock_unlock(&index->rwlock);

        // show each entry
        g_hash_table_foreach(index->htable, show_entry, NULL);

        // release the locks of each entry
        g_hash_table_foreach(index->htable, index_unlock_entries, NULL);
    }
}

int index_wait_refs(Index *index, char *filename, int nrefs)
{
    int out = -1;
    if (index == NULL || filename == NULL || nrefs < 1)
        return out;

    // acquire the read lock
    pthread_rwlock_rdlock(&index->rwlock);

    Filemeta *temp = g_hash_table_lookup(index->htable, filename);
    if (temp != NULL)
    {
        pthread_mutex_lock(&(temp->mutex));

        while (!temp->deleted && temp->refs < nrefs)
            pthread_cond_wait(&(temp->cond), &(temp->mutex));

        out = !temp->deleted ? -1 : 0;

        pthread_mutex_unlock(&(temp->mutex));
    }

    // release the lock
    pthread_rwlock_unlock(&index->rwlock);

    return out;
}

void index_inc_entry(gpointer key, gpointer value, gpointer user_data)
{
    Filemeta *meta = value;

    // increment references
    meta->refs++;

    // signal all other thread waiting
    pthread_cond_broadcast(&(meta->cond));
}

int index_multi_inc_refs(Index *index, char **filenames, int size)
{
    int out = -1;
    if (index == NULL || filenames == NULL || size < 0)
        return out;

    out = 0;
    // acquire the read lock
    pthread_rwlock_rdlock(&index->rwlock);

    Filemeta *metas[size];
    Filemeta *temp = NULL;
    for (int i = 0; out == 0 && i < size; i++)
    {
        temp = g_hash_table_lookup(index->htable, filenames[i]);
        // filename does not exist in the hash table
        if (temp == NULL)
        {
            out = -1;
            // release previous locks
            for (int j = 0; j < i; j++)
                pthread_mutex_unlock(&(metas[j]->mutex));

            // release the read lock
            pthread_rwlock_unlock(&index->rwlock);
        }
        else
        {
            // acquire lock
            pthread_mutex_lock(&(temp->mutex));
            metas[i] = temp;
        }
    }

    if (out == 0)
    {
        // increment references
        for (int i = 0; i < size; i++)
            metas[i]->refs++;

        // release locks
        for (int j = 0; j < size; j++)
            pthread_mutex_unlock(&(metas[j]->mutex));
    }

    return out;
}
