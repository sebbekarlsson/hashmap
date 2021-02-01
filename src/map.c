#include "include/map.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

map_bucket_T* init_map_bucket(map_T* source_map, char* key, void* value, unsigned int size)
{
  map_bucket_T* bucket = calloc(1, sizeof(struct MAP_BUCKET));
  bucket->key = strdup(key);
  bucket->value = value;
  bucket->map = init_map(size);
  bucket->source_map = source_map;
  return bucket;
}

map_T* init_map(unsigned int size)
{
  map_T* map = calloc(1, sizeof(struct MAP));
  map->len = size;
  map->initial_size = map->len;
  map->used = 0;
  map_resize(map, map->len);
  return map;
}

unsigned int long map_get_index(map_T* map, char* key)
{
  unsigned int long hash = map_hashfunc(map, key);
  unsigned int long index = hash % map->len;

  return index;
}

/**
 * From: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned int long map_hashfunc(map_T* map, char* key)
{
  unsigned char* str = (unsigned char*)key;
  unsigned int long hash = 0;
  int c;

  while ((c = *str++)) {
    hash = (c + (hash << 6) + (hash << 16) - hash);
  }

  return hash;
}

void map_resize(map_T* map, unsigned int inc)
{
  map->len += inc;

  if (map->buckets) {
    map->buckets = realloc(map->buckets, (map->len * sizeof(map_bucket_T*)));
  } else {
    map->buckets = calloc(map->len, sizeof(map_bucket_T*));
  }
}

static void _map_resize(map_T* map, unsigned int inc)
{
  if ((map->used > map->len)) {
    unsigned int old_len = map->len;

    map_resize(map, inc);

    map_bucket_T** new_buckets = calloc(map->len, sizeof(map_bucket_T*));

    for (unsigned int i = 0; i < old_len; i++) {
      map_bucket_T* old_bucket = map->buckets[i];
      if (!old_bucket)
        continue;
      if (!old_bucket->key)
        continue;

      int new_index = map_get_index(map, old_bucket->key);
      new_buckets[new_index] = old_bucket;
    }

    free(map->buckets);
    map->buckets = new_buckets;
  }
}

unsigned int long map_set(map_T* map, char* key, void* value)
{
  _map_resize(map, map->initial_size);

  unsigned int long index = map_get_index(map, key);

  map_bucket_T* bucket = map->buckets[index];

  if (!bucket) {
    bucket = map_find(map, key);
    map->errors += 1;
  }

  if (bucket && (strcmp(bucket->key, key) != 0)) {
    int i = map_set(bucket->map, key, value);
    map->collisions += 1;
    return i;
  } else if (bucket && (strcmp(bucket->key, key) == 0)) {
    bucket->value = value;
  } else if (!bucket && !map->buckets[index]) {
    map->buckets[index] = init_map_bucket(map, key, value, DEFAULT_BUCKET_MAP_SIZE);
    map->used += 1;
  }

  return index;
}

map_bucket_T* map_get(map_T* map, char* key)
{
  unsigned int long index = map_get_index(map, key);

  map_bucket_T* bucket = map->buckets[index];

  if (bucket && strcmp(bucket->key, key) != 0) {
    bucket = map_get(bucket->map, key);
  }

  return bucket;
}

void map_unset(map_T* map, char* key)
{
  map_bucket_T* bucket = map_get(map, key);
  if (!bucket)
    return;

  int index = map_get_index(bucket->source_map, key);
  bucket->source_map->buckets[index] = 0;
  map_resize(bucket->source_map, -1);
}

map_bucket_T* map_find(map_T* map, char* key)
{
  for (unsigned int i = 0; i < map->len; i++) {
    map_bucket_T* bucket = map->buckets[i];
    if (!bucket)
      continue;
    if (!bucket->key)
      continue;

    if (strcmp(bucket->key, key) == 0)
      return bucket;

    bucket = map_get(bucket->map, key);
    if (bucket)
      return bucket;
  }

  return 0;
}
