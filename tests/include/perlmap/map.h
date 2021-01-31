#ifndef MAP_H
#define MAP_H

typedef struct MAP_SECTION
{
  unsigned long int hash;
  char* key;
  void* value;
} map_section_T;

typedef struct MAP
{
  map_section_T** sections;
  unsigned int len;
  unsigned int mask;
} map_T;

map_section_T* init_map_section(unsigned long int hash, char* key, void* value);

map_T* init_map();

void* map_set(map_T* map, const char* key, void* value);

void* map_get(map_T* map, const char* key);

void* map_unset(map_T* map, const char* key);

int map_get_index(map_T* map, const char* key);

int mkhash(map_T* map, const char* k);
#endif
