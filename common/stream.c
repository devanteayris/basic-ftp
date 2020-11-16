/*
* stream.c
* routines for stream read and write.
*/



#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "stream.h"

int readn(int fd, char *buf, int bufsize) {
    short data_size;
    int n, nr, len;


    if(bufsize < MAX_BLOCK_SIZE) {
        return (-3);
    }

    if(read(fd, (char *) &data_size, 1) != 1) {
        return (-1);
    }

    if(read(fd, (char *) (&data_size) + 1, 1) != 1) {
        return (-1);
    }

    len = (int) ntohs(data_size);

    for(n = 0; n < len; n += nr) {
        if((nr = read(fd, buf + n, len - n)) <= 0) {
            return (nr); // Return Read Error
        }
    }
    return (len);
}


int writen(int fd, char *buf, int nbytes) {
    short data_size = nbytes;
    int n, nw;

    if(nbytes > MAX_BLOCK_SIZE) {
        return (-3);
    }

    data_size = htons(data_size);

    if(write(fd, (char *) &data_size, 1) != 1) {
        return (-1);
    }

    if(write(fd, (char *) (&data_size) + 1, 1) != 1) {
        return (-1);
    }

    for(n = 0; n < nbytes; n += nw) {
        if((nw = write(fd, buf + n, nbytes - n)) <= 0) {
            return (nw); // Return Write Error
        }
    }
    return (n);
}