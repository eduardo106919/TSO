#include "metaindex.h"
#include <stdio.h>

// For exercise 1.3 see man pages for
// int pthread_mutex_lock(pthread_mutex_t *mutex)
// int pthread_mutex_unlock(pthread_mutex_t *mutex);
// int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
// int pthread_cond_signal(pthread_cond_t *cond);
// int pthread_cond_broadcast(pthread_cond_t *cond);

void destroy_filemeta(void *ptr)
{
	if (ptr != NULL)
	{
		Filemeta *meta = (Filemeta *)ptr;
		free(meta->content);
		free(meta);
	}
}

Index *index_init()
{
	// allocate memory for the index
	Index *index = malloc(sizeof(Index));

	// GHashTable initialization
	index->htable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, destroy_filemeta);

	// Useful for exercise 1.3
	// mutex variable initialization
	// int pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr);
	// attr can be used to define non-default attributes (e.g., recursive lock)
	pthread_mutex_init(&index->mutex, NULL);

	// Useful for exercise 1.3
	// condition variable initialization
	// int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
	// attr can be used to define non-default attributes (e.g., recursive lock)
	pthread_cond_init(&index->cond, NULL);
	return index;
}

// Note: remember that memory allocation and copying must be done here
int index_add(Index *index, char *filename, Filemeta meta)
{
	if (index == NULL || filename == NULL)
	{
		return -1;
	}

	struct filemeta *data = malloc(sizeof(Filemeta));
	if (data == NULL)
	{
		return -1;
	}

	data->refs = meta.refs;
	data->size = meta.size;
	data->content = strdup(meta.content);

	gboolean out = FALSE;

	// acquire the lock
	pthread_mutex_lock(&(index->mutex));

	out = g_hash_table_insert(index->htable, (gpointer)strdup(filename), (gpointer)data);

	// release the lock
	pthread_mutex_unlock(&(index->mutex));

	if (out == FALSE)
	{
		free(data);
		return -1;
	}

	return 0;
}

int index_get(Index *index, char *filename, Filemeta *meta)
{
	int out = -1;
	// invalid input
	if (index == NULL || filename == NULL || meta == NULL)
	{
		return out;
	}

	// acquire the lock
	pthread_mutex_lock(&(index->mutex));

	gpointer data = g_hash_table_lookup(index->htable, (gconstpointer)filename);
	if (data != NULL)
	{
		Filemeta *temp = (Filemeta *)data;
		meta->refs = temp->refs;
		meta->size = temp->size;
		meta->content = strdup(temp->content);
		out = 0;
	}

	// release the lock
	pthread_mutex_unlock(&(index->mutex));

	return out;
}

int index_increfs(Index *index, char *filename)
{
	int out = -1;
	// invalid input
	if (index == NULL || filename == NULL)
	{
		return out;
	}

	// acquire the lock
	pthread_mutex_lock(&(index->mutex));

	gpointer data = g_hash_table_lookup(index->htable, (gconstpointer)filename);
	if (data != NULL)
	{
		Filemeta *temp = (Filemeta *)data;
		out = temp->refs++;

		// signal other threads
		pthread_cond_broadcast(&(index->cond));
	}

	// release the lock
	pthread_mutex_unlock(&(index->mutex));

	return out;
}

// Exercise 1.3
int index_wait_refs(Index *index, char *filename, int nrefs)
{
	int out = -1;
	if (index == NULL || filename == NULL || nrefs < 1)
	{
		return out;
	}

	// acquire the lock
	pthread_mutex_lock(&(index->mutex));

	gpointer data = g_hash_table_lookup(index->htable, (gconstpointer)filename);
	if (data != NULL)
	{
		out = 0;
		Filemeta *temp = (Filemeta *)data;

		while (temp->refs < nrefs)
		{
			pthread_cond_wait(&(index->cond), &(index->mutex));
		}
	}

	// release the lock
	pthread_mutex_unlock(&(index->mutex));

	return out;
}

int index_remove(Index *index, char *filename)
{
	int out = -1;
	if (index == NULL || filename == NULL)
	{
		return out;
	}

	// acquire the lock
	pthread_mutex_lock(&(index->mutex));

	gboolean removed = g_hash_table_remove(index->htable, (gconstpointer)filename);
	if (removed == TRUE)
	{
		out = 0;
	}

	// release the lock
	pthread_mutex_unlock(&(index->mutex));

	return out;
}

void index_destroy(Index *index)
{
	// destroy hashtable
	g_hash_table_destroy(index->htable);

	// Useful for exercice 1.3
	// destroy mutex and cond variables
	pthread_mutex_destroy(&index->mutex);
	pthread_cond_destroy(&index->cond);

	free(index);
}
