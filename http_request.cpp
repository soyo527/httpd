#include "http_all.h"

void unimplement(int client)
{
    // 向指定套接字发送提示

}

void not_found(int client)
{
    // 发送404响应
    char buf[1024];

    strcpy(buf, "HTTP/1.0 404 NOT FOUND\r\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "Server: YzkHttpd/0.1\r\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "Content-Type:text/html\n");
    send(client, buf, strlen(buf), 0);

    strcpy(buf, "\r\n");
    send(client, buf, strlen(buf), 0);

    // 发送404内容
    FILE* file = fopen("htdocs/404.html", "r");

    // 读取文件内容并发送
    while (fgets(buf, sizeof(buf), file) != NULL) {
        send(client, buf, strlen(buf), 0);
    }
    
    fclose(file);
}

void headers(int client, const char* type)
{
    // 发送响应包的头信息
    char buf[1024];

    // HTTP响应状态行
    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);

    // 服务器标识
    strcpy(buf, "Server: YzkHttpd/0.1\r\n");
    send(client, buf, strlen(buf), 0);

    // 动态设置Content-Type
    snprintf(buf, sizeof(buf), "Content-Type:%s\n", type);
    send(client, buf, strlen(buf), 0);

    // 空行表示头部结束
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
    cout << "一共发送[" << count << "]字节给浏览器" << endl;
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
    for (int i = strlen(filename), j = 4; j >= 0; i--, j--)
    {
        filename_cmp[j] = filename[i];
    }

    // 判断文件类型
    const char* content_type = "html"; // 默认类型
    if (strcmp(filename_cmp, "html") == 0)
    {
        resource = fopen(filename, "r");
        content_type = "text/html";
    }
    else if (strcmp(filename_cmp, "jpeg") == 0 || strcmp(filename_cmp, "jpg") == 0)
    {
        resource = fopen(filename, "rb");
        content_type = "image/jpeg";
    }
    else if (strcmp(filename_cmp, "png") == 0)
    {
        resource = fopen(filename, "rb");
        content_type = "image/png";
    }
    else if (strcmp(filename_cmp, "mp4") == 0)
    {
        resource = fopen(filename, "rb");
        content_type = "video/mp4";
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
        // 动态设置响应头部
        headers(client, content_type);

        // 发送请求的资源的信息
        cat(client, resource);

        cout << "资源发送完毕!" << endl;
    }

    fclose(resource);
}

