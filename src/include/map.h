#ifndef MAP_H
#define MAP_H

#define BIG_PRIME 503
#define DEFAULT_BUCKET_MAP_SIZE BIG_PRIME

typedef struct MAP_BUCKET
{
  char* key;
  void* value;
  struct MAP_BUCKET* next;
  struct MAP* map;
  struct MAP* source_map;
} map_bucket_T;

typedef struct MAP
{
  map_bucket_T** buckets;
  unsigned int len;
  unsigned int mask;
  unsigned int used;
  unsigned int initial_size;
  unsigned int long collisions;
  unsigned int errors;
} map_T;

map_bucket_T* init_map_bucket(map_T* source_map, char* key, void* value, unsigned int size);

map_T* init_map(unsigned int size);

unsigned int long map_get_index(map_T* map, char* key);

unsigned int long map_hashfunc(map_T* map, char* key);

void map_resize(map_T* map, unsigned int inc);

unsigned int long map_set(map_T* map, char* key, void* value);

map_bucket_T* map_get(map_T* map, char* key);

map_bucket_T* map_find(map_T* map, char* key);

void map_unset(map_T* map, char* key);

#define NEW_MAP() init_map(BIG_PRIME)

#endif
