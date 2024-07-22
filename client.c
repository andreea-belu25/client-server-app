#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"

#define HOST "34.246.184.49"
#define PORT 8080

char *send_message(char *message) {
    int server = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    send_to_server(server, message);
    char *received_information = receive_from_server(server);

    close_connection(server);
    return received_information;
}

void post_register() {
    printf("username=");

    char *username = NULL;
    unsigned long size = 0;

    if (getline(&username, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
   
    int length = strlen(username);
    username[length - 1] = '\0';

    printf("password=");

    char *password = NULL;
    size = 0;

    if (getline(&password, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
   
    length = strlen(password);
    password[length - 1] = '\0';

    if (strchr(username, ' ') != 0 || strchr(password, ' ') != 0) {
        printf("Error: There was a mistake with credentials\n");
        free(username);
        free(password);
        return;
    }
	

    char *dest = calloc(1024, sizeof(char));
    strcat(dest, "{\"username\":\"");
    strcat(dest, username);
    strcat(dest, "\",\"password\":\"");
    strcat(dest, password);
    strcat(dest, "\"}");

    int len = strlen(dest);
    dest[len] = '\0';

    free(username);
    free(password);

	char *data = compute_post_request(HOST, "/api/v1/tema/auth/register", dest, NULL, NULL);
	char *received_information = send_message(data);
	free(data);
	free(dest);

	if (strstr(received_information, "{\"") == NULL)
		printf("User has registered with success!\n");
	else
		printf("Error: Existing user!\n");
	
	free(received_information);
}

char *login() {
    printf("username=");

    char *username = NULL;
    unsigned long size = 0;

    if (getline(&username, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return NULL;
    }
   
    int length = strlen(username);
    username[length - 1] = '\0';

    printf("password=");

    char *password = NULL;
    size = 0;

    if (getline(&password, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return NULL;
    }
   
    length = strlen(password);
    password[length - 1] = '\0';

    if (strchr(username, ' ') != 0 || strchr(password, ' ') != 0) {
        printf("Error: There was a mistake with credentials\n");
        free(username);
        free(password);
        return NULL;
    }
    
	
    char *dest = calloc(1024, sizeof(char));
    strcat(dest, "{\"username\":\"");
    strcat(dest, username);
    strcat(dest, "\",\"password\":\"");
    strcat(dest, password);
    strcat(dest, "\"}");
	
    int len = strlen(dest);
    dest[len] = '\0';

    free(username);
    free(password);

	char *data = compute_post_request(HOST, "/api/v1/tema/auth/login", dest, NULL, NULL);
    char *received_information = send_message(data);
	free(data);
	free(dest);

    char *cookie = NULL;
	if (strstr(received_information, "{\"") == NULL) {
        if (strstr(received_information, "Set-Cookie: ") == NULL) {
            printf("Error: Login failed: Invalid details!");
            return NULL;
        }

        int length = strstr(strstr(received_information, "Set-Cookie: "), ";") 
        - (strstr(received_information, "Set-Cookie: ") + strlen("Set-Cookie: "));
        cookie = calloc(length + 1, sizeof(char));

        strncpy(cookie, strstr(received_information, "Set-Cookie: ")
        + strlen("Set-Cookie: "), length);

        printf("User has logged in with success!\n");
    } else {
		printf("Error: Login failed: Invalid details!\n");
    }

	free(received_information);
    return cookie;
}

char *enter_library(char *cookie) {
	char *data = compute_get_or_delete_request(0, HOST, "/api/v1/tema/library/access", cookie, NULL);
	char *received_information = send_message(data);
	free(data);
	    
    char *start_token = strstr(received_information, "\"token\":\"");
    char *access_token = NULL;
    if (start_token == NULL) {
        printf("Error: User cannot join the library!\n");
    } else {
        start_token = start_token + strlen("\"token\":\"");
        char *end_token = strstr(start_token, "\"");
        int length_token = end_token - start_token;
        access_token = calloc(length_token + 1, sizeof(char));
        strncpy(access_token, start_token, length_token);
        printf("Entered library with success!\n");
    }
    
	free(received_information);
    return access_token;
}

void get_books(char *cookie, char *access_token) {
	char *data = compute_get_or_delete_request(0, HOST, "/api/v1/tema/library/books", cookie, access_token);
	char *received_information = send_message(data);
    free(data);

	if (strstr(received_information, "[") != NULL) {
        printf("%s\n", strstr(received_information, "["));
    } else {
        printf("Error: User cannot access books!\n");
    }

    free(received_information);
}

void get_book(char *cookie, char *access_token) {
    printf("id=");
    char *id = NULL;
    unsigned long size = 0;
    if (getline(&id, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length = strlen(id);
    id[length - 1] = '\0';

    char *dest = calloc(1024, sizeof(char));
    strcpy(dest, "/api/v1/tema/library/books/");
    strcat(dest, id);
    int len = strlen(dest);
    dest[len] = '\0';

	char *message = compute_get_or_delete_request(0, HOST, dest, cookie, access_token);
	char *received_information = send_message(message);
	free(message);

    char *str = strstr(received_information, "{\"");
    if (str != NULL) {
        if (strstr(str, "\"error\"") != NULL) {
            printf("Error: The book with id %s cannot be accessed!\n", id);
        } else {
            printf("%s\n", str);
        }
    } else {
		printf("Error: The book with id %s cannot be accessed!\n", id);
    }

    free(id);
	free(received_information);
}

int valid_string(char *string) {
    return strcmp(string, "") != 0;
}

void add_book(char *cookie, char *access_token) {
    printf("title=");
    char *title = NULL;
    unsigned long size = 0;
    if (getline(&title, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length_title = strlen(title);
    title[length_title - 1] = '\0';

    printf("author=");
    char *author = NULL;
    size = 0;
    if (getline(&author, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length_author = strlen(author);
    author[length_author - 1] = '\0';

    printf("genre=");
    char *genre = NULL;
    size = 0;
    if (getline(&genre, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length_genre = strlen(genre);
    genre[length_genre - 1] = '\0';

    printf("publisher=");
    char *publisher = NULL;
    size = 0;
    if (getline(&publisher, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length_publisher = strlen(publisher);
    publisher[length_publisher - 1] = '\0';

    printf("page_count=");
    char *page_count_s = NULL;
    size = 0;
    if (getline(&page_count_s, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length = strlen(page_count_s);
    page_count_s[length - 1] = '\0';
    int page_count = atoi(page_count_s);

    if (page_count <= 0 || !valid_string(title) || !valid_string(author) || !valid_string(genre) || !valid_string(publisher)) {
        free(title);
        free(author);
        free(genre);
        free(publisher);
        free(page_count_s);
        printf ("Error: Book cannot be added!, invalid details\n");
        return;
    }

    char *message = calloc(1024, sizeof(char));
    strcat(message, "{\"title\":\"");
    strcat(message, title);
    strcat(message, "\",\"author\":\"");
    strcat(message, author);
    strcat(message, "\",\"genre\":\"");
    strcat(message, genre);
    strcat(message, "\",\"publisher\":\"");
    strcat(message, publisher);
    strcat(message, "\",\"page_count\":");
    strcat(message, page_count_s);
    strcat(message, "}");

    int len = strlen(message);
    message[len] = '\0';

    free(title);
    free(author);
    free(genre);
    free(publisher);
    free(page_count_s);
    
	char *data = compute_post_request(HOST, "/api/v1/tema/library/books", message, cookie, access_token);
	char *received_response = send_message(data);
    
	free(message);
    free(data);
    
	if (!strstr(received_response, "{\"")) {
        printf("The book has been added with success!\n");
    } else {
		printf("Error: The book cannot be added!\n");
    }

	free(received_response);	
}

void delete_book(char *cookie, char *access_token) {
	printf("id=");
    char *id = NULL;
    unsigned long size = 0;
    if (getline(&id, &size, stdin) == -1) {
        printf("Error : Read failed\n");
        return;
    }
    int length = strlen(id);
    id[length - 1] = '\0';

    char *dest = calloc(1024, sizeof(char));
    strcpy(dest, "/api/v1/tema/library/books/");
    strcat(dest, id);
    int len = strlen(dest);
    dest[len] = '\0';

	char *message = compute_get_or_delete_request(1, HOST, dest, cookie, access_token);
	char *received_response = send_message(message);
	free(message);

	if (strstr(received_response, "{\"") == NULL) {
        printf("The book with id %s has been deleted with succes!\n", id);
    } else {
		printf("Error: The book with is %s has failed to be deleted!\n", id);
    }

    free(id);
	free(received_response);
}

int logout(char *cookie, char *access_token) {
	char *info = compute_get_or_delete_request(0, HOST, "/api/v1/tema/auth/logout", cookie, access_token);
	char *received_response = send_message(info);
    free(info);

    int success = 0;
	if (strstr(received_response, "{\"") == NULL)
        success = 1;

    free(received_response);
    return success;
}

int main(int argc, char *argv[])
{
    char *cookie = NULL, *access_token = NULL;

    while(1) {
        char *command = NULL;
        unsigned long size = 0;
        if (getline(&command, &size, stdin) == -1) {
            printf("Error : Read failed\n");
            return 0;
        }
        int length = strlen(command);
        command[length - 1] = '\0';

        if (strcmp("register", command) == 0) {
            post_register();
            free(command);
            continue;
        } 

        if (strcmp("login", command) == 0) {
            char *aux = login();
            if (aux != NULL) {
                if (cookie != NULL)
                    free(cookie);
                cookie = calloc(strlen(aux) + 1, sizeof(char));
                strcpy(cookie, aux);
                free(aux);
            }
            free(command);
            continue;
        }

        if (strcmp("enter_library", command) == 0) {
            char *aux = NULL;
            if (cookie != NULL) 
                aux = enter_library(cookie);
            else
                printf("Error: Log in before you can enter library\n");

            if (aux != NULL) {
                if (access_token != NULL)
                    free(access_token);
                access_token = calloc(strlen(aux) + 1, sizeof(char));
                strcpy(access_token, aux);
                free(aux);
            }
            free(command);
            continue;
        }

        if (strcmp("get_books", command) == 0) {
            if (cookie == NULL) {
                free(command);
                printf("Error: you must be logged in, in order to get the books information.\n");
                continue;
            }
            if (access_token == NULL) {
                free(command);
                printf("Error: you must be enter the library first, in order to get the books information.\n");
                continue;
            }

            get_books(cookie, access_token);
            free(command);
            continue;
        }

        if (strcmp("get_book", command) == 0) {
            if (cookie == NULL) {
                free(command);
                printf("Error: you must be logged in, in order to get a book\n");
                continue;
            }
            if (access_token == NULL) {
                free(command);
                printf("Error: you must be enter the library first, in order to get a book\n");
                continue;
            }
            get_book(cookie, access_token);
            free(command);   
            continue;
        }

        if (strcmp("add_book", command) == 0) {
            if (cookie == NULL) {
                free(command);
                printf("Error: you must be logged in, in order to add a book\n");
                continue;
            }
            if (access_token == NULL) {
                free(command);
                printf("Error: you must be enter the library first, in order to add a book\n");
                continue;
            }

            add_book(cookie, access_token);
            free(command);
            continue;
        }

        if (strcmp("delete_book", command) == 0) {
            if (cookie == NULL) {
                free(command);
                printf("Error: you must be logged in, in order to delete a book\n");
                continue;
            }
            if (access_token == NULL) {
                free(command);
                printf("Error: you must be enter the library first, in order to delete a book\n");
                continue;
            }

            delete_book(cookie, access_token);
            free(command);
            continue;
        }

        if (strcmp("logout", command) == 0) {
            if (cookie == NULL) {
                free(command);
                printf("Error: you must be logged in, in order to log out\n");
                continue;
            }

            int logout_successfully = logout(cookie, access_token);

            if (logout_successfully) {
                printf("User has logged out with success!\n");
                if (cookie != NULL)
                    free(cookie);
                if (access_token != NULL)
                    free(access_token);
                
                cookie = NULL;
                access_token = NULL;
            } else {
                printf("Error: User has failed to log out!\n");
            }
            free(command);
            continue;
        }

        if (strcmp("exit", command) == 0) {
            free(command);
            break;
        }    
    }
    return 0;
}
