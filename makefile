run: all

all: main.c
	gcc -g -o slast main.c
	
.PHONY: clean

clean:
	rm -f slast
