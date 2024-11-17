#include "http_all.h"

// 从客户端套接字读取一行
int get_line(int sock, char* buf, int size)
{
    char c = 0;
    int i = 0;

    while (i < size - 1 && c != '\n')
    {
        int n = recv(sock, &c, 1, 0);
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if (n > 0 && c == '\n')
                {
                    recv(sock, &c, 1, 0);
                }
                else
                {
                    c = '\n';
                }
            }
            buf[i++] = c;
        }
        else
        {
            c = '\n';
        }
    }

    buf[i] = 0;
    return i;
}

void error_die(const char* str)
{
    perror(str);
    exit(1);
}
