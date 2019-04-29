/**
 * Module containing functions that respond to http request
 * sent by browser
 * Created by CHEN LIGUO(student ID: 851090), April 2019
 * for Computer System(COMP30023) project 1
 *
 */

/* libraries required */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "respond.h"

/****************************************************************************/

/* constants used in the program */

static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n";
static char const * const HTTP_200_FORMAT_WITH_COOKIE = "HTTP/1.1 200 OK\r\n\
Set-Cookie: %d\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n";

// maximum size of an html file
#define MAX_FILE 2049

/****************************************************************************/

/* helper functions */

// transfer data from file to sockfd
// return number of bytes sent if successful
size_t fsend(int sockfd, char *filename);
// send response header to browser through sockfd
bool sendheader(int sockfd, char *filename, bool with_cookie, \
                int *cookie_to_use, long added_length);

/****************************************************************************/

// normal response with html file intact
bool respond(int sockfd, char *filename) {
	assert(filename);

	bool status = sendheader(sockfd, filename, false, NULL, 0);
    if(!status)
    	return false;
    // send file
    int n = fsend(sockfd, filename);
    if(n < 0) {
        perror("sendfile");
        return false;
    }
    return true;
}

// response with new cookie sent
bool respond_with_cookie(int sockfd, char *filename, \
	                     int *cookie_to_use) {
	assert(filename);
	assert(cookie_to_use);

	bool status = sendheader(sockfd, filename, true, cookie_to_use, 0);
	if(!status)
		return false;
    int n = fsend(sockfd, filename);
    if(n < 0) {
        perror("sendfile");
        return false;
    }
    return true;
}

// response with something inserted into html file
// to_insert might be username or words entered
bool respond_with_insertion(int sockfd, char *filename, \
                            char *to_insert, int *round_num) {
    assert(filename);

    char header_buff[MAX_FILE], file_buff[MAX_FILE], buffer[MAX_FILE];
    int n, m;

    int filefd = open(filename, O_RDONLY);
    n = read(filefd, buffer, sizeof(buffer));
    if(n < 0) {
        perror("read");
        close(filefd);
        return false;
    }
    close(filefd);
    // adding null byte
    buffer[n] = 0;
    
    if(to_insert != NULL && round_num != NULL)
        // insert round number and (username or words entered)
        n = sprintf(file_buff, buffer, *round_num, to_insert);
    else if(to_insert != NULL && round_num == NULL)
        // insert usename or words entered
        n = sprintf(file_buff, buffer, to_insert);
    else if(to_insert == NULL && round_num != NULL)
        // insert round number
        n = sprintf(file_buff, buffer, *round_num);
    
    long size = strlen(file_buff);
    
    // form response headers
    m = sprintf(header_buff, HTTP_200_FORMAT, size);
    // send header
    if (write(sockfd, header_buff, m) < 0) {
        perror("write");
        return false;
    }
    // send html file
    if(write(sockfd, file_buff, n) < 0) {
        perror("write");
        return false;
    }
    return true;    
}

// transfer data from file to sockfd
// return number of bytes sent if successful
size_t fsend(int sockfd, char *filename) {
    assert(filename);

    char buffer[MAX_FILE];
    bzero(buffer, sizeof(buffer));
    int filefd = open(filename, O_RDONLY);
    int n = read(filefd, buffer, sizeof(buffer));
    if (n < 0) {
        perror("sendfile");
        close(filefd);
        // failed to send file to client
        return -1;
    }
    close(filefd);
    n = write(sockfd, buffer, sizeof(buffer));
    if (n > 0) {
        return sizeof(buffer);
    }
    // failed to send file to client
    return -1;
}

// send response header to browser through sockfd
bool sendheader(int sockfd, char *filename, bool with_cookie, \
	            int *cookie_to_use, long added_length) {
	assert(filename);

	char buff[MAX_FILE];
	// get file size
	int n;
    struct stat st;
    stat(filename, &st);
    long size = st.st_size + added_length;

    if(with_cookie) {
    	assert(cookie_to_use);
	    n = sprintf(buff, HTTP_200_FORMAT_WITH_COOKIE, \
	    	        *cookie_to_use, size);
		(*cookie_to_use)++;
	}
	else
	    n = sprintf(buff, HTTP_200_FORMAT, size);

    if (write(sockfd, buff, n) < 0) {
        perror("write");
        return false;
    }
    return true;
}
