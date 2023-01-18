test: test.c
	clang -g -o $@ test.c 
	./test
