#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <cstdio>

void unimplement(int client);
void not_found(int client);
void headers(int client);
void cat(int client, FILE* resource);
void server_file(int client, const char* filename);

#endif // HTTP_REQUEST_H
