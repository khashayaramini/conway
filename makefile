.PHONY: conway

conway: conway.c
	gcc -g -ggdb -o conway conway.c

run: conway
	./conway $(ARGS)

debug: conway
	gdb --args conway $(ARGS)

clean:
	rm conway
