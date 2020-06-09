#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"


// printeaza raspunsul serverului
void getMessage(char* response) {
    char* p = strtok(response, "\n");
    while (p != NULL) {
        char* copie = p;
        p = strtok(NULL, "\n");
        if (p == NULL) {
            printf("%s\n\n", copie);
        }
    }
}

int main(int argc, char *argv[])
{
    char* message;
    char* response;
    int sockfd;
    char* host = calloc(50, sizeof(char));
    // salvez host ul
    sprintf(host, "%s", "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com");

    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    char command[30], username[30], password[30];
    char title[50], author[50], genre[50], publisher[50], bookId[15];
    int page_count;
    char* cookie = NULL;
    char* token = NULL;
    bool logged_in = false;
    bool acces_in = false;

    while (1) {
        // primire comanda
        scanf("%s", command);

        // daca comanda este register
        if (strncmp(command, "register", 8) == 0) {
            // deschidem o conexiuna noua 
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

            // se asteapta username-ul si parola
            printf("username=");
            scanf("%s", username);
            json_object_set_string(root_object, "username", username);
            printf("password=");
            scanf("%s", password);
            json_object_set_string(root_object, "password", password);
            // realizam stringul de tip json
            serialized_string = json_serialize_to_string_pretty(root_value);

            // compunem mesajul corespunzator
            message = compute_post_request(host, "/api/v1/tema/auth/register", "application/json",
                                    serialized_string, NULL, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            close_connection(sockfd); 
        
            // afisez mesajul intors de server
            getMessage(response);    
        } 

        // daca comanda este login
        if (strncmp(command, "login", 5) == 0) {
            if (!logged_in) {
                // deschidem o conexiuna noua 
                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

                // se asteapta username-ul si parola
                // construire JSON
                printf("username=");
                scanf("%s", username);
                json_object_set_string(root_object, "username", username);
                printf("password=");
                scanf("%s", password);
                json_object_set_string(root_object, "password", password);
                // realizam stringul de tip json
                serialized_string = json_serialize_to_string_pretty(root_value);

                // compunem mesajul corespunzator
                message = compute_post_request(host, "/api/v1/tema/auth/login", "application/json",
                                            serialized_string, NULL, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                // daca raspunsul nu este o eroare inseamna ca login-ul
                // s-a efectuat cu succes
                if (strstr(response, "error") == NULL) {
                    logged_in = true;
                }

                // afisez mesajul intors de server
                getMessage(response);
            } else {
                printf("Already logged in!\n");
            }
        } 

        // daca se cere acces in biblioteca
        if (strncmp(command, "enter_library", 14) == 0) {
            if (logged_in) {
                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

                // compunem mesajul de login pentru a obtine un cookie valid
                message = compute_post_request(host, "/api/v1/tema/auth/login", "application/json",
                                                serialized_string, NULL, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd); 
                close_connection(sockfd);

                // salvez cookie-ul
                char* p = strstr(response, "connect.sid=");
                if (p != NULL) {
                    cookie = strtok(p, ";");     
                }

                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                // compunem mesajul corespunzator
                message = compute_get_request(host,
                        "/api/v1/tema/library/access", NULL, cookie, NULL);
                send_to_server(sockfd, message);
                response = basic_extract_json_response(receive_from_server(sockfd));
                close_connection(sockfd);

                acces_in = true;
                // afisez mesajul intors de server
                printf("%s\n\n", response);

                // extrag token-ul
                p = strstr(response, "token");
                p = p + 8;
                token = strtok(p, "\"");  
            } else {
                printf("{\"You are not logged in!\"} \n\n");
            }
        } 

        // daca se cere afisarea tuturor cartilor de pe server
        if (strncmp(command, "get_books", 10) == 0) {
            if (acces_in && logged_in) {
                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                // compunem mesajul corespunzator
                message = compute_get_request(host,
                    "/api/v1/tema/library/books", NULL, NULL, token);

                send_to_server(sockfd, message);
                response = basic_extract_json_response(receive_from_server(sockfd));
                close_connection(sockfd);

                // afisez mesajul intors de server
                printf("%s \n\n", response);
            } else {
                printf("{\"You don't have acces!\"} \n\n");
            }
        } 

        // daca se cere informatie despre o carte
        if (strncmp(command, "get_book", 9) == 0) {
            if (acces_in && logged_in) {
                // citesc id-ul dorit
                printf("id=");
                scanf("%s", bookId);
                // creez url-ul
                char url[50] = "/api/v1/tema/library/books/";
                strcat(url, bookId);

                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                // compunem mesajul corespunzator
                message = compute_get_request(host, url, NULL, NULL, token);

                send_to_server(sockfd, message);
                response = basic_extract_json_response(receive_from_server(sockfd));
                printf("%s\n\n", response);
                
                close_connection(sockfd);
            } else {
                printf("{\"You don't have acces!\"} \n\n");
            }
        } 

        // daca se doreste adaugarea unei carti
        if (strncmp(command, "add_book", 9) == 0) {
            if (acces_in && logged_in) {
                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

                // construire JSON
                printf("title=");
                scanf("%s", title);
                json_object_set_string(root_object, "title", title);
                printf("author=");
                scanf("%s", author);
                json_object_set_string(root_object, "author", author);
                printf("genre=");
                scanf("%s", genre);
                json_object_set_string(root_object, "genre", genre);
                printf("page_count=");
                scanf("%d", &page_count);
                json_object_set_number(root_object, "page_count", page_count);
                printf("publisher=");
                scanf("%s", publisher);
                json_object_set_string(root_object, "publisher", publisher);
                // realizam stringul de tip json
                serialized_string = json_serialize_to_string_pretty(root_value);

                // compunem mesajul corespunzator
                message = compute_post_request(host, "/api/v1/tema/library/books", "application/json",
                                        serialized_string, NULL, token);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                // afisez mesajul intors de server
                getMessage(response);
            } else {
                printf("{\"You don't have acces!\"} \n\n");
            }
        }

        // daca se doreste stergerea unei carti
        if (strncmp(command, "delete_book", 12) == 0) {
            if (acces_in && logged_in) {
                // citesc id-ul dorit
                printf("id=");
                scanf("%s", bookId);
                // creez url-ul
                char url[50] = "/api/v1/tema/library/books/";
                strcat(url, bookId);

                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                // compunem mesajul corespunzator
                message = compute_delete_request(host, url, NULL, NULL, token);

                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                // afisez mesajul intors de server
                getMessage(response);
            } else {
                printf("{\"You don't have acces!\"} \n\n");
            }
        }

        // daca se primeste logout
        if (strncmp(command, "logout", 7) == 0) {
            if (logged_in) {
                logged_in = false;

                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

                // compunem mesajul de login pentru a obtine un cookie valid
                message = compute_post_request(host, "/api/v1/tema/auth/login", "application/json",
                                                serialized_string, NULL, NULL);
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd); 
                close_connection(sockfd);

                // salvez cookie-ul
                char* p = strstr(response, "connect.sid=");
                if (p != NULL) {
                    cookie = strtok(p, ";");     
                }

                sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
                // compunem mesajul corespunzator
                message = compute_get_request(host,
                    "/api/v1/tema/auth/logout", NULL, cookie, NULL);

                send_to_server(sockfd, message);
                response = receive_from_server(sockfd);
                close_connection(sockfd);

                // afisez mesajul intors de server
                getMessage(response);
            }
        }

        // daca se primeste exit
        if (strncmp(command, "exit", 4) == 0) {
            close_connection(sockfd);
            break;
        }
    }

    // eliberam memoria
    free(host);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
    return 0;
}
