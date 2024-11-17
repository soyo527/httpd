#include "http_all.h"

// ʵ�������ʼ��,���ط��������׽���
int startup(unsigned short& port)
{
    // 1.����ͨ�ų�ʼ����Windows���У�Linux���ó�ʼ����
    WSADATA data;
    int ret = WSAStartup(
        MAKEWORD(1, 1),  // 1.1�汾��Э��
        &data);
    if (ret)
    {
        error_die("WSAStartup");
    }

    // 2.�����׽���
    int server_socket = socket(AF_INET,   // �׽�������
        SOCK_STREAM,    // ������
        IPPROTO_TCP);
    if (server_socket == -1)
    {
        error_die("socket");
    }

    // 3.�˿ڸ��ã����ӹرպ��õȴ������������ã�
    int opt = 1;
    ret = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if (ret == -1)
    {
        error_die("setsockopt");
    }

    // 4.���׽���
    // ���÷������˵������ַ
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        error_die("bind");
    }

    // ��̬����һ���ͻ���
    int nameLen = sizeof(server_addr);
    if (port == 0)
    {
        if (getsockname(server_socket, (struct sockaddr*)&server_addr, &nameLen) < 0)
        {
            error_die("getsockname");
        }

        port = server_addr.sin_port;
    }

    // 5.����
    if (listen(server_socket, 5) < 0)
    {
        error_die("listen");
    }

    return server_socket;
}

// �����û�����
DWORD WINAPI accept_request(LPVOID arg)
{
    char buf[1024];

    int client = (SOCKET)arg;   // �ͻ����׽���

    // ��ȡһ������
    int numChars = get_line(client, buf, sizeof(buf));
    PRINTF(buf);

    // ��������
    char method[255];
    int i = 0, j = 0;
    while (!isspace(buf[j]) && i < sizeof(method) - 1)
    {
        method[i++] = buf[j++];
    }

    method[i] = 0;
    PRINTF(method);

    // ��Ȿ�������Ƿ�֧������ķ���
    if (stricmp(method, "GET") && stricmp(method, "POST"))
    {
        unimplement(client);
        return 0;
    }

    // ������Դ�ļ���·��
    char url[255];
    i = 0;
    // ɨ���ո������һλ
    while (isspace(buf[j]) && j < sizeof(buf))
    {
        j++;
    }
    // ȡ��·��
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
        // ��ȡ�������ʣ������
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
