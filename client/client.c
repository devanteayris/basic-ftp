#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include "../common/stream.h"
#include "client.h"

int get_response(int sd) {
    char codestr[MAX_BLOCK_SIZE];
    int code;
    if(readn(sd, codestr, sizeof(codestr)) <= 0) {
        printf("\033[0;31m client: recieve error \033[0m\n ");
        return -1;
    }
    code = atoi(codestr);
    return code;
}

int read_command(char *buf, struct command *cmd) {
    printf("> ");
    fflush(stdout);

    memset(buf, 0, sizeof(buf));
    fgets(buf, MAX_BLOCK_SIZE, stdin);
    int nr = strlen(buf);
    if(buf[nr - 1] == '\n') {
        buf[nr - 1] = '\0';
        --nr;
    }
    
    char *command = strtok(buf, " ");
    char *argument = strtok(NULL, "");

    if(argument != NULL) {
        strncpy(cmd->arg, argument, strlen(argument));
    }

    if(strcmp(command, "pwd") == 0) {
        strcpy(cmd->code, "PATH");
    } else if(strcmp(command, "lpwd") == 0 ) {
        // Local Handling
        strcpy(cmd->code, "LPWD");
    } else if(strcmp(command, "dir") == 0) {
        strcpy(cmd->code, "DIRE");
    } else if(strcmp(command, "ldir") == 0) {
        // Local Handling
        strcpy(cmd->code, "LDIR");
    } else if(strcmp(command, "cd") == 0) {
        strcpy(cmd->code, "CHDR");
    } else if(strcmp(command, "lcd") == 0) {
        // Local handling
        strcpy(cmd->code, "LCDR");
    } else if(strcmp(command, "get") == 0) {
        strcpy(cmd->code, "DOLD");
    } else if(strcmp(command, "put") == 0) {
        strcpy(cmd->code, "UPLD");
    } else if(strcmp(command, "quit") == 0) {
        strcpy(cmd->code, "QUIT");
    } else {
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    strcpy(buf, cmd->code);
    if(cmd->arg != NULL) {
        strcat(buf, " ");
        strcat(buf, cmd->arg);
    }
    //printf("%s\n", buf);
    return 0;   
}

int get_lc_path() {
    char buf[MAX_BLOCK_SIZE];
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));
    strcpy(buf, cwd);

    printf("(\033[0;34m Client \033[0m) %s\n", buf);

    return 0;
}

int get_lc_dir() {
    char buf[MAX_BLOCK_SIZE];
    char cwd[1024];
    DIR *d;
    struct dirent *dir;

    getcwd(cwd, sizeof(cwd));

    d = opendir(cwd);

    if(d) {
        while((dir = readdir(d)) != NULL) {
            char file[1024];

            strcpy(file, dir->d_name);
            strcat(file, "\n");
            strcat(buf, file);
        }
        closedir(d);
    }
    printf("(\033[0;34m Client \033[0m) %s\n", buf);
    memset(buf, 0, sizeof(buf));

    return 0;
}


int cd_lc_dir(char *arg) {
    char buf[MAX_BLOCK_SIZE];
    char cwd[1024];

    if(chdir(arg) != 0) {
        printf("\033[0;33m client: cannot change directory \033[0m\n");
        return -1;
    }

    getcwd(cwd, sizeof(cwd));
    strcpy(buf, cwd);

    printf("(\033[0;34m Client \033[0m) %s\n", buf);
    return 0;
}

int get_serv_path(int sd, struct command *cmd) {
    char buf[MAX_BLOCK_SIZE];

    // Get Response Code 1 for Initiation
    if(get_response(sd) == 1) {
        if((readn(sd, buf, sizeof(buf))) <= 0) {
            printf("\033[0;31m client: recieve error \033[0m\n");
            exit(1);
        }
        printf("(\033[1;35m Server \033[0m) %s\n", buf);
        memset(buf, 0, sizeof(buf));

        // Get Response Code 212 for Completion
        if(get_response(sd) == 212) {
            return 0;
        }
    }
}

int get_serv_dir(int sd, struct command *cmd) {
    char buf[MAX_BLOCK_SIZE];

    // Get Response Code 1 for Initiation
    if(get_response(sd) == 1) {
        memset(buf, 0, sizeof(buf));
        if((readn(sd, buf, sizeof(buf))) <= 0) {
            printf("\033[0;31m client: recieve error \033[0m\n");
            exit(1);
        }
        printf("(\033[1;35m Server \033[0m) \n %s\n", buf);
        memset(buf, 0, sizeof(buf));

        // Get Response Code 212 for Completion
        if(get_response(sd) == 212) {
            return 0;
        }
    }
}

int change_serv_dir(int sd, struct command *cmd) {
    char buf[MAX_BLOCK_SIZE];

    // Get Response Code 1 for Initiation
    if(get_response(sd) == 1) {
        int code = 0;
        code = get_response(sd);

        if(code == 451) {
            printf("\033[01;33m No such directory \033[0m\n");
            return -1;
        }

        if(code == 212) {
            if((readn(sd, buf, sizeof(buf))) <= 0) {
                printf("\033[0;31m client: recieve error \033[0m\n");
                exit(1);
            }
            printf("(\033[1;35m Server \033[0m) %s\n", buf);
        }
        memset(buf, 0, sizeof(buf));


        return 0;
    }
}

int get_file_from_serv(int sd, struct command *cmd) {
    int code = get_response(sd);
    if(code == 150) {
        char buf[MAX_BLOCK_SIZE];
        int size;
        FILE *fp = fopen(cmd->arg, "w");

        while((size = readn(sd, buf, sizeof(buf))) > 0) {
            fwrite(buf, 1, size, fp);
        }

        if(size < 0) {
            perror("\033[0;31m error \033[0m\n");
            exit(1);
        }

        fclose(fp);

        if(get_response(sd) == 226) {
            printf("\033[1;32m File transfer was successful \033[0m \n");
            return 0;
        }

        return 0;
    } else if(code == 550) {
        printf("\033[01;33m File is not valid \033[0m\n");
        return -1;
    }
}

int upload_file_to_serv(int sd, struct command *cmd) {
    FILE *fp = NULL;
    char buf[MAX_BLOCK_SIZE];
    size_t bytes_read;

    fp = fopen(cmd->arg, "r");

    if(!fp) {
        printf("\033[01;33m File is not valid \033[0m\n");
    } else {
        int code = get_response(sd);
        if(code == 350) {
            do {
                bytes_read = fread(buf, 1, MAX_BLOCK_SIZE, fp);

                if(bytes_read < 0) {
                    printf("error");
                }

                if((writen(sd, buf, bytes_read)) < 0) {
                    printf("\033[0;31m client: send error \033[0m \n");
                    exit(1);
                }
            } while(bytes_read > 0);
            fclose(fp);
            if(get_response(sd) == 226) {
                printf("\033[1;32m File transfer was successful \033[0m \n");
                return 0;
            }
        } else if(code == 550) {
            fclose(fp);
            printf("\033[0;33m File cannot be transferred \033[0m \n");
            return -1;
        }
        
    }
}

int send_command_to_serv(struct command *cmd, int sd) {
    char buf[MAX_BLOCK_SIZE];
    sprintf(buf, "%s %s", cmd->code, cmd->arg);
    if((writen(sd, buf, sizeof(buf))) < sizeof(buf)) {
        printf("\033[0;31m client: send error \033[0m\n");
        exit(1);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int sd, n, nr, nw, i = 0;
    char buf[MAX_BLOCK_SIZE], host[60];
    struct sockaddr_in ser_addr;
    struct hostent *hp;

    if(argc == 1) {
        gethostname(host, sizeof(host));
    } else if(argc == 2) {
        strcpy(host, argv[1]);
    } else {
        printf("Usage: %s [<server_host_name> | IP_address ]\n", argv[0]);
        exit(1);
    }

    bzero((char *) &ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SERV_DEFAULT_TCP_PORT);

    if((hp = gethostbyname(host)) == NULL) {
        printf("Host %s not found\n", host);
        exit(1);
    }
    ser_addr.sin_addr.s_addr = * (u_long *) hp->h_addr;

    sd = socket(AF_INET, SOCK_STREAM, 0);

    if(connect(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0) {
        perror("client connect");
        exit(1);
    }

    while(++i) {
        struct command cmd;
        memset(&cmd, 0, sizeof cmd);
        while(read_command(buf, &cmd) != 0) {
            printf("\033[0;31m Invalid command \033[0m\n");
        }

        if(strcmp(cmd.code, "QUIT") == 0) {
            close(sd);
            printf("Client connection closed\nBye\n");
            exit(0);
        }

        if(strcmp(cmd.code, "LPWD") == 0) {
            get_lc_path();
            continue;
        }

        if(strcmp(cmd.code, "LDIR") == 0) {
            get_lc_dir();
            continue;
        }

        if(strcmp(cmd.code, "LCDR") == 0) {
            cd_lc_dir(cmd.arg);
            continue;
        }

        if((nw = writen(sd, buf, (int)strlen(buf)) < 0)) {
            printf("\033[0;31m client: send error \033[0m\n");
            exit(1);
        }

        char codestr[MAX_BLOCK_SIZE];
        
        if((nr = readn(sd, codestr, sizeof(codestr))) <= 0) {
            printf("\033[0;31m client: recieve error \033[0m\n");
            exit(1);
        }

        int code;
        
        if((code = atoi(codestr)) == 0) {
            printf("\033[0;31m client: recieve error \033[0m\n");
            exit(1);
        }

        switch(code) {
            case 502 :
                printf("\033[0;31m Invalid command \033[0m \n");
                break;
            case 221 :
                exit(1);
            default :
                // Run Command
                if(strcmp(cmd.code, "PATH") == 0) {
                    get_serv_path(sd, &cmd);
                    break;
                }

                if(strcmp(cmd.code, "DIRE") == 0) {
                    get_serv_dir(sd, &cmd);
                    break;
                }

                if(strcmp(cmd.code, "CHDR") == 0) {
                    change_serv_dir(sd, &cmd);
                    break;
                }

                if(strcmp(cmd.code, "DOLD") == 0) {
                    get_file_from_serv(sd, &cmd);
                    break;
                }

                if(strcmp(cmd.code, "UPLD") == 0) {
                    upload_file_to_serv(sd, &cmd);
                    break;
                }
                memset(buf, 0, sizeof(buf));
        }
    }
}