#include "http_all.h"

void unimplement(int client)
{
    // ��ָ���׽��ַ�����ʾ

}

void not_found(int client)
{
    // ����404��Ӧ
    char buf[1024];

    strcpy(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "Server: YzkHttpd/0.1\r\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "Content-Type:text/html\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

    // ����404����
    FILE* file = fopen("htdocs/404.html", "r");

    // ��ȡ�ļ����ݲ�����
    while (fgets(buf, sizeof(buf), file) != NULL) {
        send(client, buf, strlen(buf), 0);
    }
    
    fclose(file);
}

void headers(int client)
{
    // ������Ӧ����ͷ��Ϣ
    char buf[1024];

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "Server: YzkHttpd/0.1\r\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "Content-Type:text/html\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

}

void cat(int client, FILE* resource)
{
    char buf[4096];
    int count = 0;

    while (1)
    {
        int ret = fread(buf, sizeof(char), sizeof(buf), resource);
        if (ret <= 0)
        {
            break;
        }
        send(client, buf, ret, 0);
        count += ret;
    }
    cout << "һ������[" << count << "]�ֽڸ������" << endl;
}

void server_file(int client, const char* filename)
{
    int numChars = 1;
    char buf[1024];
    while (numChars > 0 && strcmp(buf, "\n"))
    {
        numChars = get_line(client, buf, sizeof(buf));
        PRINTF(buf);
    }

    FILE* resource = NULL;

    char filename_cmp[5];
    for (int i = strlen(filename), j = 4; j>=0; i--, j--)
    {
        filename_cmp[j] = filename[i];
    }

    if (strcmp(filename_cmp, "html") == 0)
    {
        resource = fopen(filename, "r");
    }
    else
    {
        resource = fopen(filename, "rb");
    }

    if (resource == NULL)
    {
        not_found(client);
    }
    else
    {
        // ��ʽ������Դ�������

        headers(client);

        // �����������Դ����Ϣ
        cat(client, resource);

        cout << "��Դ�������!" << endl;
    }

    fclose(resource);
}
