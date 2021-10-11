#include <hashmap/map.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) a > b ? a : b

char** maybe_realloc(char*** keys, unsigned int extra_size)
{
  *keys = realloc(*keys, extra_size * sizeof(char*));

  return *keys;
}

map_bucket_T** maybe_realloc_buckets(map_bucket_T*** buckets, unsigned int extra_size)
{
  *buckets = realloc(*buckets, extra_size * sizeof(char*));

  return *buckets;
}

map_bucket_T* init_map_bucket(map_T* source_map, char* key, void* value, unsigned int size)
{
  map_bucket_T* bucket = calloc(1, sizeof(struct MAP_BUCKET));
  bucket->key = strdup(key);
  bucket->value = value;
  bucket->map = init_map(size);
  bucket->source_map = source_map;
  return bucket;
}

void map_bucket_free(map_bucket_T* bucket)
{
  if (bucket->key)
    free(bucket->key);
  if (bucket->map)
    map_free(bucket->map);
  free(bucket);
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

void map_free(map_T* map)
{
  char** keys = 0;
  unsigned int len;

  if (map->keys) {
    map_get_keys(map, &keys, &len);

    for (unsigned int i = 0; i < len; i++) {
      char* k = keys[i];
      if (!k)
        continue;

      free(k);
    }

    free(map->keys);
  }

  if (map->used_buckets) {
    for (unsigned int i = 0; i < map->len_used_buckets; i++) {
      map_bucket_T* bucket = map->used_buckets[i];
      if (!bucket)
        continue;
      map_bucket_free(bucket);
    }

    free(map->used_buckets);
  }

  if (map->buckets) {
    free(map->buckets);
  }

  free(map);
}

unsigned int long map_get_index(map_T* map, char* key)
{
  if (!key) {
    printf("map_get_index error, key is null.\n");
    exit(1);
  }

  if (!map) {
    printf("map_get_index error, map is null.\n");
    exit(1);
  }

  unsigned int long hash = map_hashfunc(map, key);
  unsigned int long index = hash % map->len;

  return index;
}

/**
 * From: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned int long map_hashfunc(map_T* map, char* key)
{
  if (!map) {
    printf("map_hashfunc error, map is null.\n");
    exit(1);
  }
  if (!key) {
    printf("map_hashfunc error, key is null.\n");
    exit(1);
  }

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
  if (!key) {
    exit(1);
    printf("map_set: key cannot be null.\n");
  }

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

    map->nrkeys += 1;
    map->keys = maybe_realloc(&map->keys, map->nrkeys);
    map->keys[map->nrkeys - 1] = strdup(key);

    return i;
  } else if (bucket && (strcmp(bucket->key, key) == 0)) {
    bucket->value = value;
    return index;
  } else if (!bucket && !map->buckets[index]) {
    map->buckets[index] = init_map_bucket(map, key, value, MAX(128, map->initial_size));
    map->used += 1;

    map->nrkeys += 1;
    map->keys = maybe_realloc(&map->keys, map->nrkeys);
    map->keys[map->nrkeys - 1] = strdup(key);

    map->len_used_buckets += 1;
    map->used_buckets = maybe_realloc_buckets(&map->used_buckets, map->len_used_buckets);
    map->used_buckets[map->len_used_buckets - 1] = map->buckets[index];
    return index;
  }

  return index;
}

map_bucket_T* map_get(map_T* map, char* key)
{
  if (!map) {
    printf("map_get error, map is null.\n");
    exit(1);
  }
  if (!key) {
    printf("map_get error, key is null.\n");
    exit(1);
  }

  unsigned int long index = map_get_index(map, key);

  map_bucket_T* bucket = map->buckets[index];

  if (bucket && strcmp(bucket->key, key) != 0) {
    bucket = map_get(bucket->map, key);
  }

  return bucket;
}

void* map_get_value(map_T* map, char* key)
{
  map_bucket_T* bucket = map_get(map, key);

  if (!bucket)
    return 0;

  return bucket->value;
}

void map_unset(map_T* map, char* key)
{
  map_bucket_T* bucket = map_get(map, key);
  if (!bucket)
    return;

  int index = map_get_index(bucket->source_map, key);
  bucket->source_map->buckets[index] = 0;
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

map_T* map_copy(map_T* map)
{
  if (!map->len)
    return map;

  map_T* new_map = calloc(1, sizeof(struct MAP));
  new_map->used = map->used;
  new_map->len = map->len;
  new_map->initial_size = map->initial_size;
  new_map->nrkeys = map->nrkeys;
  new_map->keys = calloc(map->nrkeys, sizeof(char*));
  memcpy(&new_map->keys, &map->keys, sizeof(map->keys));
  new_map->used_buckets = map->used_buckets;
  new_map->len_used_buckets = map->len_used_buckets;

  map_bucket_T** buckets = calloc(map->len, sizeof(map_bucket_T*));

  if (buckets && map->buckets) {
    for (unsigned int i = 0; i < map->len; i++) {
      map_bucket_T* b = map->buckets[i];
      buckets[i] = b;
    }
  }

  new_map->buckets = buckets;

  return new_map;
}

void map_get_keys(map_T* map, char*** keys, unsigned int* size)
{
  *keys = map->keys;
  *size = map->nrkeys;
}
