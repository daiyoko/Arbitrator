/* Mon May 28 5:48PM
 *
 * XXX: sudo! port 80
 * forwards requests
 *
 * netstat -a | grep <port-number>
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define LISTENQ         1024

#define MAXLINE         4096
#define SERV_PORT       9877
#define FORWARD_ADDRESS "192.168.1.3"


void
sig_chld(int signo) {
    pid_t pid;
    int stat;

    while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}


int
main(int argc, char *argv[])
{
    int listenfd, connfd, fwdfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr, fwdaddr;
    char buf[MAXLINE], testbuf[MAXLINE];
    int n;

    // setup servaddr
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    // bind servaddr
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    signal(SIGCHLD, sig_chld);
    for (;;) {
        clilen = sizeof(cliaddr);
        if ( (connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) > 0){
            printf("accept error!\n");
        }
        if ( (childpid = fork()) == 0) { // is child
            // setup fwdaddr
            fwdfd = socket(AF_INET, SOCK_STREAM, 0);
            memset(&fwdaddr, 0, sizeof(fwdaddr));
            fwdaddr.sin_family = AF_INET;
            fwdaddr.sin_port = htons(SERV_PORT);
            inet_pton(AF_INET, FORWARD_ADDRESS, &fwdaddr.sin_addr);
            connect(fwdfd, (struct sockaddr *)&fwdaddr, sizeof(fwdaddr));

            close(listenfd);
            /*
             *  client      =>      proxy       =>      server
             *  [connfd]                                [fwdfd]
             *
             *  REQUEST "GET /" ...
             */
            memset(buf, 0, sizeof(char) * MAXLINE);
            n = read(connfd, buf, MAXLINE);
            printf("%s", buf);
            write(fwdfd, buf, strlen(buf));

            /*
             *  client      <=      proxy       <=      server
             *  [connfd]                                [fwdfd]
             *
             *                                  ...     HTTP/1.0 200 OK
             */
            memset(buf, 0, sizeof(char) * MAXLINE);
            while ( (n = read(fwdfd, buf, MAXLINE)) > 0) {
                printf("%s", buf);
                write(connfd, buf, n);
            }

            // clean up sockets
            close(fwdfd);
            close(connfd);
            exit(0);
        }
    }
    close(listenfd);
}
