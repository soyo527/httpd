#include "http_all.h"

// 实现网络初始化,返回服务器端套接字
int startup(unsigned short& port)
{
    // 1.网络通信初始化（Windows特有，Linux不用初始化）
    WSADATA data;
    int ret = WSAStartup(
        MAKEWORD(1, 1),  // 1.1版本的协议
        &data);
    if (ret)
    {
        error_die("WSAStartup");
    }

    // 2.创建套接字
    int server_socket = socket(AF_INET,   // 套接字类型
        SOCK_STREAM,    // 数据流
        IPPROTO_TCP);
    if (server_socket == -1)
    {
        error_die("socket");
    }

    // 3.端口复用（连接关闭后不用等待即可立即复用）
    int opt = 1;
    ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if (ret == -1)
    {
        error_die("setsockopt");
    }

    // 4.绑定套接字
    // 配置服务器端的网络地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        error_die("bind");
    }

    // 动态分配一个客户端
    int nameLen = sizeof(server_addr);
    if (port == 0)
    {
        if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0)
        {
            error_die("getsockname");
        }

        port = server_addr.sin_port;
    }

    // 5.监听
    if (listen(server_socket, 5) < 0)
    {
        error_die("listen");
    }

    return server_socket;
}

// 处理用户请求
DWORD WINAPI accept_request(LPVOID arg)
{
    char buf[1024];

    int client = (SOCKET)arg;   // 客户端套接字

    // 读取一行数据
    int numChars = get_line(client, buf, sizeof(buf));
    PRINTF(buf);

    // 解析方法
    char method[255];
    int i = 0, j = 0;
    while (!isspace(buf[j]) && i < sizeof(method) - 1)
    {
        method[i++] = buf[j++];
    }

    method[i] = 0;
    PRINTF(method);

    // 检测本服务器是否支持请求的方法
    if (stricmp(method, "GET") && stricmp(method, "POST"))
    {
        unimplement(client);
        return 0;
    }

    // 解析资源文件的路径
    char url[255];
    i = 0;
    // 扫到空格就往后一位
    while (isspace(buf[j]) && j < sizeof(buf))
    {
        j++;
    }
    // 取出路径
    while (!isspace(buf[j]) && i < sizeof(url) - 1 && j < sizeof(buf))
    {
        url[i++] = buf[j++];
    }
    url[i] = 0;
    PRINTF(url);

    char path[512] = "";
    sprintf(path, "htdocs%s", url);
    if (path[strlen(path) - 1] == '/')
    {
        strcat(path, "index.html");
    }
    PRINTF(path);

    struct stat status;
    if (stat(path, &status) == -1)
    {
        // 读取请求包的剩余内容
        while (numChars > 0 && strcmp(buf, "\n"))
        {
            numChars = get_line(client, buf, sizeof(buf));
        }
        not_found(client);
    }
    else
    {
        if ((status.st_mode & S_IFMT) == S_IFDIR)
        {
            strcat(path, "/index.html");
        }

        server_file(client, path);
    }

    closesocket(client);
    return 0;
}
