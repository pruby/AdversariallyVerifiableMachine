default: test.out

clean:
	rm out/*.o test.out

test.out: out/test.o out/merkle_tree_memory.o out/sha3.o
	gcc -O2 -o $@ $^

out/%.o: src/%.c
	gcc -O2 -c -o $@ $^

