#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../common/stream.h"
#include "server.h"

void write_to_log(char *str) {
    time_t now = time(&now);
    
    if (now == -1) {
        
        puts("The time() function failed");
    }
        
    struct tm *ptm = gmtime(&now);
    
    if (ptm == NULL) {
        
        puts("The gmtime() function failed");
    }   

    char buf[1024];
    strcpy(buf, asctime(ptm));
    strcat(buf, " ");
    strcat(buf, str);
    strcat(buf, "\n");

    FILE *fp = fopen("/tmp/myftpd_log.txt", "a");
    if(fp == NULL) {
        perror("Error opening log file");
    } else {
        fprintf(fp, "%s", buf);
    }
    fclose(fp);
}

int send_serv_response(int sd, int code) {
    char buf[MAX_BLOCK_SIZE];

    snprintf(buf, sizeof(buf) + 1, "%d", code);

    if((writen(sd, buf, sizeof(buf))) < sizeof(buf)) {
        printf("server: send error\n");
        return -1;
    }
    return 0;
}

int serve_pwd(int sd) {
    char buf[MAX_BLOCK_SIZE];
    char cwd[1024];

    send_serv_response(sd, 1);
    getcwd(cwd, sizeof(cwd));
    strcpy(buf, cwd);

    if((writen(sd, buf, sizeof(buf))) < sizeof(buf)) {
        printf("server: send error\n");
        return -1;
    }

    send_serv_response(sd, 212);

    write_to_log("Client used pwd");

    return 0;
}

int serve_dir(int sd) {
    char buf[MAX_BLOCK_SIZE];


    char cwd[1024];
    DIR *d;
    struct dirent *dir;

    send_serv_response(sd, 1);

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
    
    if((writen(sd, buf, sizeof(buf))) < sizeof(buf)) {
        printf("server: send error\n");
        return -1;
    }

    send_serv_response(sd, 212);

    write_to_log("Client used dir");

    return 0;
}

int serve_cd_dir(int sd, char *arg) {
    char buf[MAX_BLOCK_SIZE];
    char cwd[1024];

    send_serv_response(sd, 1);

    if(chdir(arg) != 0) {
        printf("server: cannot change directory\n");
        send_serv_response(sd, 451);
        return -1;
    }

    getcwd(cwd, sizeof(cwd));
    strcpy(buf, cwd);
    send_serv_response(sd, 212);

    char log[1024];
    strcpy(log, "Client has changed directory to: ");
    strcat(log, arg);
    write_to_log(log);

    if((writen(sd, buf, sizeof(buf))) < sizeof(buf)) {
        printf("server: send error\n");
        return -1;
    }

    return 0;
}

int serve_get_file(int sd, char *arg) {
    FILE *fp = NULL;
    char buf[MAX_BLOCK_SIZE];
    size_t bytes_read;

    fp = fopen(arg, "r");

    if(!fp) {
        send_serv_response(sd, 550);
    } else {
        send_serv_response(sd, 150);

        do {
            bytes_read = fread(buf, 1, MAX_BLOCK_SIZE, fp);

            if(bytes_read < 0) {
                printf("error");
            }

            if((writen(sd, buf, bytes_read)) < 0) {
                printf("server: send error\n");
                return -1;
            }
        } while(bytes_read > 0);

        send_serv_response(sd, 226);

        char log[1024];
        strcpy(log, "Client has downloaded: ");
        strcat(log, arg);
        write_to_log(log);

        fclose(fp);

        return 0;
    }
}

int serve_put_file(int sd, char *arg) {
    char buf[MAX_BLOCK_SIZE];
    int size;
    FILE *fp = fopen(arg, "w");

    if(!fp) {
        send_serv_response(sd, 550);
    } else {
        send_serv_response(sd, 350);
        while((size = readn(sd, buf, sizeof(buf))) > 0) {
            fwrite(buf, 1, size, fp);
        }

        if(size < 0) {
            perror("error\n");
            return -1;
        }

        fclose(fp);

        send_serv_response(sd, 226);

        char log[1024];
        strcpy(log, "Client has uploaded: ");
        strcat(log, arg);
        write_to_log(log);
        return 0;
    }
    fclose(fp);
}

void claim_children() {
    pid_t pid = 1;

    while(pid > 0) {
        pid = waitpid(0 , (int *) 0, WNOHANG);
    }
}

void daemon_init(void) {
    pid_t pid;
    struct sigaction act;

    if((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    } else if(pid > 0) {
        printf("Server PID: %d\n", pid);
        exit(0);
    }

    setsid();
    umask(0);

    act.sa_handler = claim_children;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, (struct sigaction *) 0);
}



void serve_a_client(int sd) {
    write_to_log("New Client Connected");
	char cmd[5];
	char arg[MAX_BLOCK_SIZE];

    while(1) {
        int code = 200;
        int nr, nw;
        char buf[MAX_BLOCK_SIZE];

        memset(buf, 0, MAX_BLOCK_SIZE);
        memset(cmd, 0, 5);
        memset(arg, 0, MAX_BLOCK_SIZE);

        if((nr = readn(sd, buf, sizeof(buf))) < 0) {
            return;
        }

        buf[nr] = '\0';

        strncpy(cmd, buf, 4);

        if(strcmp(cmd, "PATH") == 0) {
            code = 200;
        } else if(strcmp(cmd, "DIRE") == 0) {
            code = 200;
        } else if(strcmp(cmd, "CHDR") == 0) {
            code = 200;
        } else if(strcmp(cmd, "DOLD") == 0) {
            code = 200;
        } else if(strcmp(cmd, "UPLD") == 0) {   
            code = 200;
        } else {
            code = 502;
        }

        send_serv_response(sd, code);

        if(code == 200) {
            char *tmp = buf + 5;
            strcpy(arg, tmp);
            
            if(strcmp(cmd, "PATH") == 0) {
                serve_pwd(sd);
            } else if(strcmp(cmd, "DIRE") == 0) {
                serve_dir(sd);
            } else if(strcmp(cmd, "CHDR") == 0) {
                serve_cd_dir(sd, arg);
            } else if(strcmp(cmd, "DOLD") == 0) {
                serve_get_file(sd, arg);
            } else if(strcmp(cmd, "UPLD") == 0) {   
                serve_put_file(sd, arg);
            } else {

            }
        }     
    }
}

void set_up_server() {
    int sd, nsd, n, cli_addrlen;
    pid_t pid;
    struct sockaddr_in ser_addr, cli_addr;

    daemon_init();

    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("server:socket");
        exit(1);
    }

    bzero((char *) &ser_addr, sizeof(ser_addr));

    // Server Listening
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(SERV_DEFAULT_TCP_PORT);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr)) < 0) {
        perror("server bind");
        exit(1);
    }

    listen(sd, 5);

    while(1) {
        cli_addrlen = sizeof(cli_addr);
        nsd = accept(sd, (struct sockaddr *) &cli_addr, (socklen_t *) &cli_addrlen);
        if(nsd < 0) {
            if(errno == EINTR) {
                continue;
            }
            perror("server:accept");
            exit(1);
        }

        if((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } else if(pid > 0) {
            close(nsd);
            continue;
        }

        close(sd);
        serve_a_client(nsd);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    if(argc == 2) {
        printf("%s ", argv[1]);
        if(chdir(argv[1]) != 0) {
            printf("Not a valid directory\n");
            exit(1);
        }
    } else if (argc > 2) {
        printf("Usage: %s [<initial_current_directory>]\n", argv[0]);
        exit(1);
    } else {
        // Default Directory
        chdir("/");
    }

    // Log File Creation
    FILE *fp = fopen("/tmp/myftpd_log.txt", "r");
    if(fp == NULL) {
        // Check if it current exists
        FILE *fp2 = fopen("/tmp/myftpd_log.txt", "w+");
        fclose(fp2);
    }
    fclose(fp);

    set_up_server();
}