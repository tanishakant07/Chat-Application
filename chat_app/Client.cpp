#include <iostream>
#include <winsock.h> //Winsock API for TCP/IP that includes newer functions and structures used to retrieve IP addresses.

using namespace std;
#define PORT 9909

int nClientSocket;
struct sockaddr_in srv;

int main()
{
    int nRet = 0;
    //intialise the WSA variables
// environment supporting socket programming on windows
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << endl << "WSA failed to intialize";
        WSACleanup();
        return (EXIT_FAILURE);
    }

    nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (nClientSocket < 0)
    {
        cout << endl << "The socket not opened";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(srv.sin_zero), 0, 8);

    nRet = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0) {
        cout << endl << "Connection failed";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Connected to the server ";
        char buff[255] = { 0, };
        recv(nClientSocket, buff, 255, 0);
        cout << endl << "Just press any key to recieve message from the server";
        getchar();
        cout << endl << buff;

        cout << endl << "Send your message to the server:";
        while (1) {
            fgets(buff, 256, stdin);
            send(nClientSocket, buff, 256, 0);
            cout << endl << "Press any key to get the response from the server";
            getchar();
            recv(nClientSocket, buff, 256, 0);
            cout << endl << buff << endl << "Now send next message:";
        }
    }
}