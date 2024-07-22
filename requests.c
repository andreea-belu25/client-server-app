#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

//  type = 0 => get_request, type = 1 => delete_request

char *compute_get_or_delete_request(int type, char *host, char *url, char *cookie, char *access_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    
    if (type == 0)
        strcpy(message, "GET ");
    else if (type == 1)
        strcpy(message, "DELETE ");

    strcat(message, url);
    strcat(message, " HTTP/1.1\r\n");

    strcat(message, "Host: ");
    strcat(message, host);
    strcat(message, "\r\n");

    if (cookie != NULL) {
        strcat(message, "Cookie: ");
        strcat(message, cookie);
        strcat(message, "\r\n");
    }
    
    if (access_token != NULL) {
        strcat(message, "Authorization: Bearer ");
        strcat(message, access_token);
        strcat(message, "\r\n");
    }

    strcat(message, "Connection: close\r\n\r\n");

    int length = strlen(message);
    message[length] = '\0';
    return message;
}

char *compute_post_request(char *host, char *url, char *data, char *cookie, char *access_token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    
    strcpy(message, "POST ");

    strcat(message, url);
    strcat(message, " HTTP/1.1\r\n");

    strcat(message, "Host: ");
    strcat(message, host);
    strcat(message, "\r\n");
    strcat(message, "Content-Type: application/json\r\n");
    strcat(message, "Content-Length: ");

    char *temp = calloc (strlen(data) + 1, sizeof(char));
    sprintf(temp, "%zu", strlen(data));
    strcat(message, temp);
    free(temp);
    strcat(message, "\r\n");

    if (cookie != NULL) {
        strcat(message, "Cookie: ");
        strcat(message, cookie);
        strcat(message, "\r\n");
    }
    
    if (access_token != NULL) {
        strcat(message, "Authorization: Bearer ");
        strcat(message, access_token);
        strcat(message, "\r\n");
    }

    strcat(message, "Connection: close\r\n\r\n");
    strcat(message, data);

    int length = strlen(message);
    message[length] = '\0';
    return message;
}
