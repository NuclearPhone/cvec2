test: test.c cvec2.h
	clang -g -o $@ test.c 
	./test
