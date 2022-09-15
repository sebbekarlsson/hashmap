#include <hashmap/macros.h>
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

void map_bucket_clear(map_bucket_T* bucket, HashmapFreeFunction free_func)
{
  if (!bucket)
    return;

  if (bucket->map) {
    map_clear(bucket->map, free_func);
  }
  map_bucket_free(bucket);
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

void map_init(map_T* map, unsigned int size)
{
  if (map->initialized)
    return;
  map->initialized = 1;
  map->len = size;
  map->initial_size = map->len;
  map->used = 0;
  map_resize(map, map->len);
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

void map_clear(map_T* map, HashmapFreeFunction free_func)
{
  char** keys = 0;
  unsigned int len;

  if (map->keys) {
    map_get_keys(map, &keys, &len);

    for (unsigned int i = 0; i < len; i++) {
      char* k = keys[i];
      if (!k) {
        continue;
      }

      map_bucket_T* bucket = map_get(map, k);

      if (bucket) {
        if (bucket->value && free_func) {
          free_func(bucket->value);
          bucket->value = 0;
        }

        map_unset(map, k);
        map_bucket_free(bucket);
        free(k);
      }
    }

    free(map->keys);
    map->keys = 0;
    map->nrkeys = 0;
  }

  if (map->buckets) {
    free(map->buckets);
    map->buckets = 0;
  }

  if (map->used_buckets) {
    free(map->used_buckets);
    map->used_buckets = 0;
  }

  map->len = 0;
  map->len_used_buckets = 0;
  map->item_count = 0;
  map->collisions = 0;
  map->errors = 0;

  map->used = 0;
}

HashMapIndex map_get_index(map_T* map, char* key)
{
  if (!key) {
    printf("map_get_index error, key is null.\n");
    exit(1);
  }

  if (!map) {
    printf("map_get_index error, map is null. (key=%s)\n", key);
    exit(1);
  }

  HashMapIndex hash = map_hashfunc(map, key);
  HashMapIndex index = hash % map->len;

  return index;
}

/**
 * From: http://www.cse.yorku.ca/~oz/hash.html
 */
HashMapIndex map_hashfunc(map_T* map, char* key)
{
  if (!map) {
    printf("map_hashfunc error, map is null. (key=%s)\n", key);
    exit(1);
  }
  if (!key) {
    printf("map_hashfunc error, key is null.\n");
    exit(1);
  }

  unsigned char* str = (unsigned char*)key;
  HashMapIndex hash = 0;
  int c = 0;

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
  if ((map->used > map->len) || map->buckets == 0) {
    unsigned int old_len = map->len;

    if (!map->len) {
      map->len = map->initial_size;
    }

    if (!map->len) {
      fprintf(stderr, "_map_resize: len == 0.\n");
      return;
    }

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

HashMapIndex map_set(map_T* map, char* key, void* value)
{
  if (!key) {
    exit(1);
    printf("map_set: key cannot be null.\n");
  }

  _map_resize(map, map->initial_size);

  if (!map->buckets) {
    fprintf(stderr, "map_set: No buckets!\n");
    return 0;
  }

  HashMapIndex index = map_get_index(map, key);

  map_bucket_T* bucket = map->buckets[index];

  if (!bucket) {
    bucket = map_get(map, key);//map_find(map, key);
    map->errors += 1;
  }


  if (bucket && (strcmp(bucket->key, key) != 0)) {
    HashMapIndex i = map_set(bucket->map, key, value);
    map->collisions += 1;

    map->nrkeys += 1;
    map->keys = maybe_realloc(&map->keys, map->nrkeys);
    map->keys[map->nrkeys - 1] = strdup(key);

    return i;
  } else if (bucket && (strcmp(bucket->key, key) == 0)) {
    bucket->value = value;
    return index;
  } else if (!bucket && !map->buckets[index]) {
    // then it's a completely new key

    map->buckets[index] = init_map_bucket(map, key, value, MAX(128, map->initial_size));
    map->used += 1;

    map->nrkeys += 1;
    map->keys = maybe_realloc(&map->keys, map->nrkeys);
    map->keys[map->nrkeys - 1] = strdup(key);

    map->len_used_buckets += 1;
    map->used_buckets = maybe_realloc_buckets(&map->used_buckets, map->len_used_buckets);
    map->used_buckets[map->len_used_buckets - 1] = map->buckets[index];

    map->item_count += 1;

    return index;
  } else {
    fprintf(stderr, "(Hashmap): Should not get here.\n");
  }

  return index;
}

HashMapIndex map_set_int(map_T* map, const char* key, int value)
{
  if (!map)
    return 0;
  if (!key)
    return 0;
  MapFactor* factor = calloc(1, sizeof(MapFactor*));
  factor->type = MAP_FACTOR_INT;
  factor->as.int_value = value;

  return map_set(map, (char*)key, factor);
}

HashMapIndex map_set_int64(map_T* map, const char* key, int64_t value)
{
  if (!map)
    return 0;
  if (!key)
    return 0;
  MapFactor* factor = calloc(1, sizeof(MapFactor*));
  factor->type = MAP_FACTOR_INT64;
  factor->as.int64_value = value;

  return map_set(map, (char*)key, factor);
}

map_bucket_T* map_get(map_T* map, char* key)
{
  if (!map) {
    printf("map_get error, map is null. (key=%s)\n", key);
    exit(1);
  }
  if (!key) {
    printf("map_get error, key is null.\n");
    exit(1);
  }

  if (!map->buckets)
    return 0;

  HashMapIndex index = map_get_index(map, key);

  map_bucket_T* bucket = map->buckets[index];

  if (bucket && bucket->key != 0 && strcmp(bucket->key, key) != 0 && bucket->map != 0) {
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

int map_get_int(map_T* map, const char* key)
{
  if (!map)
    return 0;
  if (!key)
    return 0;
  MapFactor* factor = (MapFactor*)map_get_value(map, (char*)key);
  if (!factor)
    return 0;

  return factor->as.int_value;
}

int64_t map_get_int64(map_T* map, const char* key)
{
  if (!map)
    return 0;
  if (!key)
    return 0;
  MapFactor* factor = (MapFactor*)map_get_value(map, (char*)key);
  if (!factor)
    return 0;

  switch (factor->type) {
    case MAP_FACTOR_INT64: return (int64_t)factor->as.int64_value; break;
    case MAP_FACTOR_INT: return (int64_t)factor->as.int_value; break;
  }

  return factor->as.int64_value;
}

void map_unset(map_T* map, char* key)
{
  map_bucket_T* bucket = map_get(map, key);
  if (!bucket)
    return;

  int index = map_get_index(bucket->source_map, key);
  bucket->source_map->buckets[index] = 0;

  map->item_count -= 1;
  map->item_count = MAX(0, map->item_count);
}

void map_unset_factor(map_T* map, const char* key)
{
  if (!map)
    return;
  if (!key)
    return;
  MapFactor* factor = (MapFactor*)map_get_value(map, (char*)key);
  if (!factor)
    return;

  free(factor);
  factor = 0;

  return map_unset(map, (char*)key);
}

void map_unset_int(map_T* map, const char* key)
{
  return map_unset_factor(map, key);
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

void map_copy_into(map_T* src, map_T* dest)
{
  if (!src)
    return;
  if (!dest)
    return;

  char** keys = 0;
  uint32_t len = 0;

  map_get_keys(src, &keys, &len);

  for (uint32_t i = 0; i < len; i++) {
    char* key = keys[i];
    if (!key)
      continue;
    void* value = map_get_value(src, key);
    if (!value)
      continue;

    map_set(dest, key, value);
  }
}

int map_get_values_by_keys(map_T* map, const char* keys[], uint32_t length, uint32_t* out_length,
                           void** out)
{
  if (!map)
    return 0;
  if (!out)
    return 0;
  if (!keys)
    return 0;
  if (length <= 0)
    return 0;

  uint32_t count = 0;
  for (uint32_t i = 0; i < length; i++) {
    const char* key = keys[i];
    if (!key)
      continue;

    void* value = map_get_value(map, (char*)key);
    if (!value)
      continue;

    out[count] = value;
    count++;
  }

  *out_length = count;
  return count > 0;
}

uint64_t map_get_count(map_T* map)
{
  return (uint64_t)MAX(0, map->item_count);
}

void* map_get_value_nth(map_T* map, int64_t index)
{
  if (!map->buckets)
    return 0;
  if (!map->keys)
    return 0;

  index = index % map->nrkeys;

  return map_get_value(map, map->keys[index]);
}
