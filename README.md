
I. requests.c
---
1. compute_get_or_delete_request
    - Creates through concatenation the specific string for GET or DELETE request (message).
    - Handles the case when a cookie is received and the string should contain it, and the case when an access_token is received and the string should contain it.
    - Due to the fact that the strings for GET and DELETE requests are almost the same, differing only in the first word, which indicates the type of request, I used a variable 'type' to distinguish between these two cases (type = 0 -> GET request, type = 1 -> DELETE request).

2. compute_post_request
    - Creates through concatenation the specific string for POST request (message).
    - Handles the cases for cookie and access_token as I have done for the preceding function.
    - I used a temporary string because strcat requires two strings. This way, I stored the conversion from strlen(s), which is an integer, to a string in the temporary string, conversion made with sprintf.

II. requests.h
---
    
Declares the headers of the functions implemented in requests.c.


III. client.c
---
1. main()
    - Reads until there are no more commands from the console using getline.
    - Checks if the reading has failed for each possible command.
    - For the current command, calls the specific function or exits the program if the current command is exit.
    - Retains the cookie or access_token where necessary (login, enter_library).
    - Handles other error cases that I considered when implementing the application.

2. post_register()
    - Reads the username and password necessary for registration.
    - Checks if there is a space in one of them, which means that there was a mistake with the introduced data.
    - Opens the connection with the server and sends a POST request with the username and password in JSON format.
    - The server sends back a response indicating whether the user has been registered successfully or if the user already exists and could not be registered with the same data.
    - Closes the communication with the server.

3. login()
    - Reads the username and password necessary for login from the console.
    - Checks if the username and password are valid strings.
    - Opens the communication with the server, which provides a message indicating whether the user has successfully logged in.
    - If the login was succesful, extracts the cookie and returns it.
    - Closes the communication with the server.

4. enter_library()
    - Opens communication with the server and sends a request.
    - If the server does not provide a token, it means that the user does not have access to the library. Otherwise, extracts the user's access token, which will be the value returned by the function.

5. get_books()
    - Opens communication with the server and sends a request to access the books in the library.
    - The server checks if the user has access. Otherwise, an error message is displayed.
    - Ends the communication with the server.

6. get_book()
    - The user provides an ID to access the book in the library with the given ID.
    - Opens communication with the server and sends a GET request to the server.
    - The server checks if the user has access to the requested book and displays its details if yes. Otherwise, an error message is displayed.
    - Ends the communication with the server.

7. add_book()
    - The user wants to introduce a new book to the library and provides details about it: title, author, genre, publisher, page_count.
    - Checks if the provided details are valid, meaning that the number of pages of the book should be positive number and that the author, title, genre, and publisher are not empty strings.
    - Opens communication with the server and sends a message with all the details of the book in JSON format.
    - The server checks if the book can be added and returns a message accordingly.
       - Closes the communication with the server.

8. delete_book()
    - The user provides a book ID specific to the book they want to remove from the library.
    - Opens communication with the server and sends a DELETE request.
    - The server checks if the user has this privilege. In each case, a message is displayed on the console.
    - Closes the communication with the server.

9. logout()
    - Opens communication with the server.
    - Sends a GET request to the server, which will check if the user can log out using the access_token kept during login.
    - Returns 0 in case of error or 1 in case of success.
    - Based on the response in the main function, prints either an error message or releases the memory allocated for the cookie and access token.
