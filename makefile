.PHONY: conway

conway: conway.c
	gcc -g -ggdb -o conway conway.c

run: conway
	./conway

debug: conway
	gdb conway

clean:
	rm conway
