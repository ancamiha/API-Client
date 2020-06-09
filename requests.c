#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <string.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char* host, char* url, char* query_params, 
                            char* cookies, char* token) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // scrierea numelui metodei, URL-ului si a protocolului
    if (query_params != NULL) {
        sprintf(line, "GET %s/%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // adaugarea hostului
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaugare cookies si token daca exista
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    // adaugare linie noua la final
    compute_message(message, "");
    return message;
}

char *compute_post_request(char* host, char* url, char* content_type, char* body_data,
                            char* cookies, char* token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // scrierea numelui metodei, URL-ului si a protocolului
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // adaugarea hostului
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // se adauga headerele necesare
    int length = strlen(body_data);

    sprintf(line, "Content-Length: %d", length);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    // adaugare cookies si token daca exista
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    if (cookies != NULL) {
       sprintf(line, "Cookie: %s", cookies);
       compute_message(message, line);
    }

    // adaugare linie noua la finalul headerului
    compute_message(message, "");

    // adaugarea payload-ului
    strcat(body_data_buffer, body_data);

    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}

char *compute_delete_request(char* host, char* url, char* query_params, 
                            char* cookies, char* token) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // scrierea numelui metodei, URL-ului si a protocolului
    if (query_params != NULL) {
        sprintf(line, "DELETE %s/%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // adaugarea hostului
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaugare cookies si token daca exista
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    if (cookies != NULL) {
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}
