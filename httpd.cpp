#include "http_all.h"

int main(int argc, char const* argv[])
{
    unsigned short port = 80;   // ͳһ�涨�˿���һ���޷��Ŷ�����0~65535
    int server_sock = startup(port);
    cout << "httpd�����Ѿ����������ڼ���" << port << "�Ŷ˿�..." << endl;

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (1)
    {
        // ����ʽ�ȴ��û�ͨ��������������
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_sock < 0)
        {
            error_die("accept");
        }

        // �����µ��̣߳����1��1����
        DWORD threadId = 0;
        CreateThread(0, 0, accept_request, (void*)client_sock, 0, &threadId);

    }
    closesocket(server_sock);

    system("pause");
    return 0;
}