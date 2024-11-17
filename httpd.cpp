#include "http_all.h"

int main(int argc, char const* argv[])
{
    unsigned short port = 80;   // 统一规定端口是一个无符号短整型0~65535
    int server_sock = startup(port);
    cout << "httpd服务已经启动，正在监听" << port << "号端口..." << endl;

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (1)
    {
        // 阻塞式等待用户通过浏览器发起访问
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock < 0)
        {
            error_die("accept");
        }

        // 创建新的线程，完成1对1服务
        DWORD threadId = 0;
        CreateThread(0, 0, accept_request, (void*)client_sock, 0, &threadId);

    }
    closesocket(server_sock);

    system("pause");
    return 0;
}