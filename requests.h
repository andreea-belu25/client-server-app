#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_or_delete_request(int type, char *host, char *url, char *cookie, char *token);
char *compute_post_request(char *host, char *url, char *data, char *cookie, char *token);

#endif
