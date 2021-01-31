exec = hashmap.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -g -Wall -lm -ldl -fPIC -rdynamic
LPATH = $(HOME)/.local


libhashmap.a: $(objects)
	ar rcs $@ $^

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

install:
	make
	make libhashmap.a
	mkdir -p $(LPATH)/include/hashmap
	cp -r ./src/include/* $(LPATH)/include/hashmap/.
	cp ./libhashmap.a $(LPATH)/lib/.

clean:
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

lint:
	clang-tidy src/*.c src/include/*.h
