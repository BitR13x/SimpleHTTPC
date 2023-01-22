#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
#define HOST "127.0.0.1"
#define PORT 8400

int main()
{
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;
    int c;

    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Creating socket...\n");
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    printf("Preparing sockaddr_in...\n");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(HOST);
    server.sin_port = htons(PORT);

    printf("Binding socket...\n");
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);
    listen(s, 3);

    c = sizeof(struct sockaddr_in);
    // Receive until the peer closes the connection
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
    {
        int valread = recv(new_socket, recvbuf, recvbuflen, 0);
        if (valread > 0)
        {
            printf("Bytes received: %d\n", valread);
            printf("%s\n", recvbuf);
        }
        else if (valread == 0)
        {
            printf("Connection closed\n");
        }
        else
        {
            printf("recv failed: %d\n", WSAGetLastError());
        }
    }

    if (new_socket == INVALID_SOCKET)
    {
        printf("accept failed with error code : %d", WSAGetLastError());
        return 1;
    }

    closesocket(s);
    WSACleanup();

    return 0;
}
