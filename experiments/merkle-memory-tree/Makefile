CFLAGS=-O2

default: test.out

clean:
	rm out/*.o test.out

test.out: out/test.o out/merkle_tree_memory.o out/sha3.o
	gcc ${CFLAGS} -o $@ $^

out/%.o: src/%.c
	gcc ${CFLAGS} -c -o $@ $^

