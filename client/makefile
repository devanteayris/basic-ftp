all: myftp

myftp: client.o
	gcc -o myftp client.o ../common/stream.o

client.o: client.c
	gcc -c client.c

client.c: ../common/stream.o
	gcc -c ../common/stream.c -o ../common/stream.o

clean :
	rm client.o myftp ../common/stream.o
