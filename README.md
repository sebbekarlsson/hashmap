# hashmap
> Generic hashmap implemented with the **sdbm** hash function.  
> Read more about it here: [http://www.cse.yorku.ca/~oz/hash.html](http://www.cse.yorku.ca/~oz/hash.html)

## Usage
> Here is an example:
```C
#include <hashmap/map.h>

map_T* map = NEW_MAP();

// set
map_set(map, "xyz", strdup("123"));

// get
map_bucket_T* bucket = map_get(map, "xyz");
printf("%s\n", (char*)bucket->value) // "123"

// unset
map_unset(map, "xyz");
```

## Installing
> Just clone down the repository and run:
```bash
make && make install
```

## Using it
> To use it in a project, simply **install** it first, then compile your
> program with the following flags:
```
-lm -lhashmap
```
> Then just:
```C
#include <hashmap/map.h>
```
