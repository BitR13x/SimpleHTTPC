#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#define HOST "127.0.0.1"
#define PORT 8001

void sendHelloWorld(SOCKET clientSocket, char *buffer)
{
    char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>Hello World</body></html>";
    printf("%s\n", buffer);

    //? splitting buffer output
    char *method = strtok(buffer, " \t\r\n");
    char *url = strtok(NULL, " \t");
    char *prot = strtok(NULL, " \t\r\n");
    printf("%s | %s | %s \n", method, url, prot);

    send(clientSocket, response, strlen(response), 0);
    // shutdown(clientSocket, 0);
    shutdown(clientSocket, SD_SEND);
    closesocket(clientSocket);
}

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
    while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
    {
        // handle new connection here
        printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        //? reading request
        char buffer[1024] = {0};
        int valread = recv(new_socket, buffer, 1024, 0);
        if (valread > 0)
        {
            sendHelloWorld(new_socket, buffer);
        }
        else if (valread == 0)
        {
            printf("Client disconnected\n");
        }
        else
        {
            printf("recv failed with error code : %d", WSAGetLastError());
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
