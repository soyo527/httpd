#ifndef HTTP_SERVER_ALL_H
#define HTTP_SERVER_ALL_H

// ��������ģ���ͷ�ļ�
#include "http_server.h"
#include "http_request.h"
#include "utils.h"

// ����������Ҫ�ı�׼��ͷ�ļ�
#include <iostream>
#include <WinSock2.h>
#include <sys/stat.h>
#include<sys/types.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#pragma comment(lib,"WS2_32.lib")

#define PRINTF(str) printf("[%s - %d]"#str"=%s\n",__func__,__LINE__,str);

using namespace std;

#endif // HTTP_SERVER_ALL_H
