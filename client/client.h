/*
* client.h header file for ftp client
*/


#define SERV_DEFAULT_TCP_PORT 40020

// Hold commands read from User
struct command {
    char arg[255];
    char code[5];
};

/*
 * Function:  get_response
 * --------------------
 * 
 *  To get server response codes after commands have been sent as an request
 *  to determine if the command has been completed successfuly  
 *
 *  sd: socket
 *
 *  returns: response code given by ftp server
 *           returns > 0 if read was sucesss
 *           returns -1 if read was unsuccesful
 */

int get_response(int sd);

/*
 * Function:  read_command
 * --------------------
 * 
 *  Get user input for a command they want to execute to the ftp server
 *
 *  buf: string to store command within
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if command is valid
 *           returns -1 if commoand is invalid
 */
int read_command(char *buf, struct command *cmd);

/*
 * Function:  get_lc_path
 * --------------------
 * 
 *  Get ftp client current directory path
 *
 *
 *  returns: returns 0 if command works
 */
int get_lc_path();


/*
 * Function:  get_lc_dir
 * --------------------
 * 
 *  Get all files listed within the ftp client current directory outputted
 *
 *
 *  returns: returns 0 if command works
 */
int get_lc_dir();

/*
 * Function:  cd_lc_dir
 * --------------------
 * 
 *  Change directory of ftp client
 *
 *  arg: directory to change to
 *
 *  returns: returns 0 if command works
 *           returns -1 if there was an error changing directories
 */
int cd_lc_dir(char *arg);

/*
 * Function:  get_serv_path
 * --------------------
 * 
 *  Get ftp server path of it's directory
 *
 *  sd: socket
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if command works
 * 
 *  exits program if there was a recieve error with the ftp server
 */
int get_serv_path(int sd, struct command *cmd);

/*
 * Function:  get_serv_dir
 * --------------------
 * 
 *  Get ftp server files within it's current directory
 *
 *  sd: socket
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if command works
 * 
 *  exits program if there was a recieve error with the ftp server
 */
int get_serv_dir(int sd, struct command *cmd);

/*
 * Function:  change_serv_dir
 * --------------------
 * 
 *  Change directory on the ftp server
 *
 *  sd: socket
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if command works
 *           returns -1 if directory on ftp client does not exist
 * 
 *  exits program if there was a recieve error with the ftp server
 * 
 */
int change_serv_dir(int sd, struct command *cmd);


/*
 * Function:  get_file_from_serve
 * --------------------
 * 
 *  Get a file from ftp server
 *
 *  sd: socket
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if command works
 *           returns -1 if file requested is not valid
 * 
 *  exits program if there was a recieve error with the ftp server
 * 
 */
int get_file_from_serve(int sd, struct command *cmd);


/*
 * Function:  upload_file_to_serve
 * --------------------
 * 
 *  Upload a file to the ftp server (PUT)
 *
 *  sd: socket
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if command works
 *           returns -1 if file on client cannot be transferred
 *           returns -2 if client could not send bytes to ftp server
 * 
 *  exits if client could not send data to ftp server
 */
int upload_file_to_serve(int sd, struct command *cmd);


/*
 * Function:  get_file_from_serve
 * --------------------
 * 
 *  Send a command to ftp server to execute
 *
 *  sd: socket
 *  cmd: a struct to store a command code and it's argument
 *
 *  returns: returns 0 if the command was sent succesfully
 * 
 *  exits if client could not send data to ftp server
 */
int send_command_to_serv(struct command *cmd, int sd);
