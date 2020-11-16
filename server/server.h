/*
* server.h header file for ftp server
*/


#define SERV_DEFAULT_TCP_PORT 40020

/*
 * Function:  write_to_log
 * --------------------
 * 
 *  Write client interaction to log
 *
 *  str: client interaction as a string
 *
 */
void write_to_log(char *str);

/*
 * Function:  send_serv_response
 * --------------------
 * 
 *  Send response code to ftp client connected
 *
 *  sd: socket
 *  code: code
 *
 *  returns: returns 0 if command works
 *           returns -1 if server could not send response to client
 */
int send_serv_response(int sd, int code);

/*
 * Function:  serve_pwd
 * --------------------
 * 
 *  Get current directory path and send it to ftp client connected
 *
 *  sd: socket
 *
 *  returns: returns 0 if command works
 *           returns -1 if server could not send response to client
 */
int serve_pwd(int sd);

/*
 * Function:  serve_dir
 * --------------------
 * 
 *  Get all files and directories within the current directory and send it
 *  to the ftp client connected
 *
 *  sd: socket
 *
 *  returns: returns 0 if command works
 *           returns -1 if server could not send response to client
 */
int serve_dir(int sd);

/*
 * Function:  serve_cd_dir
 * --------------------
 * 
 *  Change the current directory according to arg
 *
 *  sd: socket
 *  arg: directory to change to
 *
 *  returns: returns 0 if command works
 *           returns -1 if server could not send response to client
 */
int serve_cd_dir(int sd, char *arg);

/*
 * Function:  serve_get_file
 * --------------------
 * 
 *  Send a file to the ftp client connected
 *
 *  sd: socket
 *  arg: file to send
 *
 *  returns: returns 0 if command works
 *           returns -1 if server could not send response to client
 */
int serve_get_file(int sd, char *arg);

/*
 * Function:  serve_put_file
 * --------------------
 * 
 *  Get file from ftp client connected
 *
 *  sd: socket
 *  arg: file to download
 *
 *  returns: returns 0 if command works
 *           returns -1 if server could not read file given by ftp client
 */
int serve_put_file(int sd, char *arg);

/*
 * Function:  claim_children
 * --------------------
 * 
 *  Remove child zombie processes from process table
 *
 */
void claim_children();

/*
 * Function:  daemon_init
 * --------------------
 * 
 *  Create a daemon and fork child process so it runs concurrently
 *
 */
void daemon_init(void);

/*
 * Function:  serve_a_client
 * --------------------
 * 
 *  Get new socket created to serve a new client when they connect to ftp server
 * 
 *  sd: socket
 *
 */
void serve_a_client(int sd);

/*
 * Function:  set_up_server
 * --------------------
 * 
 * Sets up how the ftp server will run, forks new childs when new clients
 * connect to ftp server
 *
 */
void set_up_server();
