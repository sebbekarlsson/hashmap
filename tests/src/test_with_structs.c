#include "include/test_with_structs.h"
#include "../../src/include/map.h"
#include "include/list.h"
#include "include/main.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct DOG
{
  char* name;
  int age;
} dog_T;

dog_T* init_dog(const char* name, int age)
{
  dog_T* dog = calloc(1, sizeof(struct DOG));
  dog->age = age;
  dog->name = strdup(name);

  return dog;
}

typedef struct PERSON
{
  char* firstname;
  char* lastname;
  dog_T* pet;
} person_T;

person_T* init_person(const char* firstname, const char* lastname, dog_T* pet)
{
  person_T* person = calloc(1, sizeof(struct PERSON));
  person->firstname = strdup(firstname);
  person->lastname = strdup(lastname);
  person->pet = pet;

  return person;
}

void test_with_structs()
{
  dog_T* dog = init_dog("boo", 4);
  person_T* person = init_person("john", "doe", dog);
  ASSERT(person->pet != 0, "struct in struct is not null");

  map_T* map = NEW_MAP();

  map_set(map, "john", person);

  map_bucket_T* bucket = map_get(map, "john");
  person_T* person_in_map = (person_T*)bucket->value;
  ASSERT(person_in_map != 0, "struct in map is not null");
  ASSERT(person_in_map->pet != 0, "struct in struct is not null");

  ASSERT(person == person_in_map, "struct is the same when getting it back");
}
