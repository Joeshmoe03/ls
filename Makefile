CFLAGS=-Wall -pedantic

myls: myls.c
	gcc $(CFLAGS) -o myls myls.c

.PHONY: clean
clean:
	rm -f myls
