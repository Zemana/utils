OBJ    = trie.o wild.o utils.o hash.o
OUT    = matchcore.dll
CFLAGS = -O3 -Wall -Wextra 

all: $(OBJ)
	dllwrap $(OBJ) -s --def matchcore.def --dllname $(OUT)

trie.o: trie.c trie.h
	gcc trie.c $(CFLAGS) -c 

wild.o: wild.c wild.h
	gcc wild.c $(CFLAGS) -c

hash.o: hash.c hash.h
	gcc hash.c $(CFLAGS) -c 

utils.o: utils.c utils.h
	gcc utils.c $(CFLAGS) -c 

clean:
	rm -rf $(OBJ)
	rm -rf $(OUT)