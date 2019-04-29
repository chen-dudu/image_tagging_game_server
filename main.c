/**
 * part of the code in this file is 
 * taken from the sample code from LMS
 * Modified by CHEN LIGUO(student ID: 851090), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

/* libraries required */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include "handle.h"
#include "list.h"

/****************************************************************************/

int main(int argc, char * argv[])
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s ip port\n", argv[0]);
        return 0;
    }
    printf("image_tagger server is now running at IP: %s on port %s\n", 
           argv[1], argv[2]);
    // create TCP socket which only accept IPv4
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // reuse the socket if possible
    int const reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, \
                   &reuse, sizeof(int)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // create and initialise address we will listen on
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // if ip parameter is not specified
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // bind address to socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, \
             sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen on the socket
    listen(sockfd, 5);

    // initialise an active file descriptors set
    fd_set masterfds;
    FD_ZERO(&masterfds);
    FD_SET(sockfd, &masterfds);
    // record the maximum socket number
    int maxfd = sockfd;

    // the next unused cookie, which will 
    // be assigned to the next new client
    int next_cookie = 0;
    int round_num = 1;
    List *player_list = new_list();

    while (1) {
        // monitor file descriptors
        fd_set readfds = masterfds;
        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // loop all possible descriptor
        for (int i = 0; i <= maxfd; ++i)
            // determine if the current file descriptor is active
            if (FD_ISSET(i, &readfds)) {   
                // create new socket if there is 
                // new incoming connection request
                if (i == sockfd) {
                    struct sockaddr_in cliaddr;
                    socklen_t clilen = sizeof(cliaddr);
                    int newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr,\
                    	                   &clilen);
                    if (newsockfd < 0)
                        perror("accept");
                    else { 
                        // add the socket to the set
                        FD_SET(newsockfd, &masterfds);
                        // update the maximum tracker
                        if (newsockfd > maxfd)
                            maxfd = newsockfd;
                    }
                }
                // a request is sent from the client
                else if (!handle_http_request(i, &next_cookie, &round_num, \
                                              player_list)) {
                    close(i);
                    FD_CLR(i, &masterfds);
                }
            }
    }
    // free all player info stored in the server
    free_list(player_list);
    return 0;
}
