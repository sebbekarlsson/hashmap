#include <hashmap/map.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int age;
} Person;

int main(int argc, char* argv[]) {
  map_T m = {0};
  map_init(&m, 512);


  Person* p = (Person*)calloc(1, sizeof(Person));
  Person* p2 = (Person*)calloc(1, sizeof(Person));
  p->age = 33;
  p2->age = 25;


  map_set(&m, "person1", p);



  Person* yo = map_get_value(&m, "person1");


  printf("%d\n", yo->age);


  map_clear(&m, free);


  Person* yo2 = map_get_value(&m, "person1");

  printf("%p\n", yo2);


  map_set(&m, "person2", p2);

  Person* yo3 = map_get_value(&m, "person2");

  printf("%p\n", yo3);
  printf("%d\n", yo3->age);


  map_clear(&m, free);
  //printf("%d\n", yo2->age);


 return 0;

}
