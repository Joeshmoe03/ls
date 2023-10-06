CFLAGS = -Wall -pedantic

.PHONY: make

make: myls.c
	gcc $(CFLAGS) -o myls myls.c

.PHONY: clean
clean:
	rm -f ./myls
