#ifndef HASHMAP_MAP_H
#define HASHMAP_MAP_H
#include <stdint.h>

#define BIG_PRIME 503

typedef struct MAP_BUCKET
{
  char* key;
  void* value;
  struct MAP* map;
  struct MAP* source_map;
} map_bucket_T;

typedef struct MAP
{
  map_bucket_T** buckets;
  map_bucket_T** used_buckets;
  unsigned int len_used_buckets;
  unsigned int len;
  unsigned int used;
  unsigned int initial_size;
  unsigned int long collisions;
  unsigned int errors;
  char** keys;
  unsigned int nrkeys;

  int64_t item_count;

  unsigned int initialized;
} map_T;

typedef enum {
  MAP_FACTOR_INT,
  MAP_FACTOR_INT64
} MapFactorType;

typedef struct MAP_FACTOR {
  union {
    int int_value;
    int int64_value;
  } as;
  MapFactorType type;
} MapFactor;

map_bucket_T* init_map_bucket(map_T* source_map, char* key, void* value, unsigned int size);

void map_bucket_free(map_bucket_T* bucket);

map_T* init_map(unsigned int size);

void  map_init(map_T* map, unsigned int size);

typedef void (*HashmapFreeFunction)(void* ptr);

void map_free(map_T* map);

void map_clear(map_T* map, HashmapFreeFunction free_func);

unsigned int long map_get_index(map_T* map, char* key);

unsigned int long map_hashfunc(map_T* map, char* key);

void map_resize(map_T* map, unsigned int inc);

unsigned int long map_set(map_T* map, char* key, void* value);

unsigned int long map_set_int(map_T* map, const char* key, int value);
unsigned int long map_set_int64(map_T* map, const char* key, int64_t value);


map_bucket_T* map_get(map_T* map, char* key);

void* map_get_value(map_T* map, char* key);

void* map_get_value_nth(map_T* map, int64_t index);

int map_get_int(map_T* map, const char* key);
int64_t map_get_int64(map_T* map, const char* key);

map_bucket_T* map_find(map_T* map, char* key);

void map_unset(map_T* map, char* key);

void map_unset_int(map_T* map, const char* key);

void map_unset_factor(map_T* map, const char* key);

void map_get_keys(map_T* map, char*** keys, unsigned int* size);

int map_get_values_by_keys(map_T* map, const char* keys[], uint32_t length, uint32_t* out_length, void** out);

map_T* map_copy(map_T* map);

void map_copy_into(map_T* src, map_T* dest);

uint64_t map_get_count(map_T* map);

#define NEW_MAP() init_map(BIG_PRIME)

#endif
