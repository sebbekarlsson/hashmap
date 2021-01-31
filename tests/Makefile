exec = test.out
sources = $(wildcard src/*.c)
objects = $(sources:.c=.o)
flags = -I./include -L./ -lhashmap -g -Wall -lm -ldl -fPIC -rdynamic
LPATH = $(HOME)/.local

$(exec): $(objects) libhashmap.a
	gcc $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h libhashmap.a
	gcc -c $(flags) $< -o $@

libhashmap.a:
	cd .. && make && cp ./libhashmap.a tests/.

clean:
	-rm -rf .tmp
	-rm *.out
	-rm *.o
	-rm *.a
	-rm src/*.o

lint:
	clang-tidy src/*.c src/include/*.h
