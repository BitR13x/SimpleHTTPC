#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define HOST "127.0.0.1"
#define PORT 8400

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    // Create a socket
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Connect to server
    struct sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(HOST);
    clientService.sin_port = htons(PORT);
    iResult = connect(ConnectSocket, (SOCKADDR *)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR)
    {
        printf("Failed to connect.\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Send and receive data
    const char *sendbuf = "This is a test";
    int iSendResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iSendResult == SOCKET_ERROR)
    {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %d\n", iSendResult);

    // Cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
