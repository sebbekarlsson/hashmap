#include <hashmap/map.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct {
  int age;
} Person;

int main(int argc, char* argv[]) {
  map_T* map = NEW_MAP();

  Person* p1 = (Person*)calloc(1, sizeof(Person));
  map_set(map, "apa_person1", p1);
  map_set(map, "apa_person1", p1);
  map_set(map, "apa_person1", p1);
  map_set(map, "niltonyo0", (Person*)calloc(1, sizeof(Person)));
  map_set(map, "niltonyo1", (Person*)calloc(1, sizeof(Person)));
  map_set(map, "niltonyo2", (Person*)calloc(1, sizeof(Person)));
  printf("count: %ld\n", map_get_count(map));

  return 0;

}
