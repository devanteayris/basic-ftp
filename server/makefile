all: myftpd

myftpd: server.o
	gcc -o myftpd server.o ../common/stream.o

server.o: server.c
	gcc -c server.c

server.c: ../common/stream.o
	gcc -c ../common/stream.c -o ../common/stream.o

clean :
	rm server.o myftpd ../common/stream.o
