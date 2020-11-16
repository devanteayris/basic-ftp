/*
* stream.h head file for stream read and stream write.
*/


#define MAX_BLOCK_SIZE (1024 * 5)


/*
* purpose: read a message from "fd" and place the string in the
* message to "buf".
* pre: 1) size of buf bufsize >= MAX_BLOCK_SIZE,
* post: 1) buf contains the byte stream;
* 2) return value > 0 : number of bytes read
* = 0 : connection closed
* = -1 : read error
* = -2 : protocol error
* = -3 : buffer too small
*/ 

int readn(int fd, char *buf, int bufsize);


/*
* purpose: constructing a message containing "nbytes" bytes from "buf".
* Send the message to "fd".
* pre: 1) nbytes <= MAX_BLOCK_SIZE,
* post: 1) nbytes bytes from buf written to fd;
* 2) return value = nbytes : number ofbytes written
* = -3 : too many bytes to send
* otherwise: write error
*/ 
int writen(int fd, char *buf, int nbytes);
