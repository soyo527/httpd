#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <WinSock2.h>


int startup(unsigned short& port);
DWORD WINAPI accept_request(LPVOID arg);

#endif // HTTP_SERVER_H
