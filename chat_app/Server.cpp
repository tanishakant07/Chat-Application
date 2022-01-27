#include <iostream>
#include <winsock.h> //Winsock API for TCP/IP that includes newer functions and structures used to retrieve IP addresses.

using namespace std;
#define PORT 9909

/*struct sockaddr_in{
    short sin_family;
    u_short sin_port; //port for listening
    struct in_addr sin_addr; //IP address
    char sin_zero[8]; //it has space to store information during program execution
};*/

struct sockaddr_in srv;
fd_set fr, fw, fe; //fr : socket descriptors for reading
                   //fw : socket descriptors for writing
                   //fe : socket descriptors for exceptions
int nMaxFd;
int nSocket;
int nArrClient[5];

void ProcessNewMessage(int nClientSocket) {
    cout << endl << " Processing new message for client socket:" << nClientSocket;
}

void ProcessTheNewRequest() {
    //New connection Request
    if (FD_ISSET(nSocket, &fr))
    {
        int nLen = sizeof(struct sockaddr);
        int nClientSocket = accept(nSocket, NULL, &nLen);
        if (nClientSocket > 0) {
            //Put it into the client fd_set
            int nIndex = 0;
            for (int nIndex = 0; nIndex < 5; nIndex++) {
                if (nArrClient[nIndex] == 0) {
                    nArrClient[nIndex] = nClientSocket;
                    send(nClientSocket, "Got the connection successfully", 37, 0);
                    break;
                }
            }
            if (nIndex == 5) {
                cout << endl << "No space for a new connection";
            }
        }
    }
    else {
        for (int nIndex = 0; nIndex < 5; nIndex++) {
            if (FD_ISSET(nArrClient[nIndex], &fr)) {
                //new message from the client 
            }
        }
    }
}

int main()
{
    int nRet = 0;
    //intialise the WSA variables
// environment supporting socket programming on windows
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        cout << endl << "WSA failed to intialize";
    }
    else {
        cout << endl << "WSA intialized";
    }

    //intialize the socket  
    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //nSocket is the socket ID
    if (nSocket < 0)
    {
        cout << endl << "The socket not opened";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << " The socket opened successfully" << nSocket;
    }
    //intialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY; //take in the IP address of the system
    memset(&(srv.sin_zero), 0, 8);

    //setsocket

    int nOptVal = 0;
    int nOptLen = sizeof(nOptVal);
    nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
    if (!nRet)
    {
        cout << endl << "The setsockopt call successful";
    }
    else {
        cout << endl << "The setsockopt call failed";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    //Blocking vs non blocking sockets
    // optval = 0 means it is a blocking socket and != 0 means a non blocking socket
   /* u_long optval = 0;
    nRet = ioctlsocket (nSocket, FIONBIO, &optval);
    if (nRet != 0){
        cout << endl << " ioctlsocket call failed";
    }
    else {
        cout << endl << " ioctlsocket call passed";
    }*/

    //bind the socket to the local port
    nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRet < 0) {
        cout << endl << "Fail to bind to local port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Successfully bound to local port";
    }


    //listen the request from client (queues the requests)
    nRet = listen(nSocket, 5); //5 is the backlog which tells the server number of active requests 
    if (nRet < 0) {
        cout << endl << "Failed to start listen to loacl port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else {
        cout << endl << "Started listening to local port";
    }

    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (1) {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);

        for (int nIndex = 0; nIndex < 5; nIndex++) {
            if (nArrClient[nIndex] != 0) {
                FD_SET(nArrClient[nIndex], &fr);
                FD_SET(nArrClient[nIndex], &fe);
            }
        }

        //keep waiting for new requests and proceed as per the request
        nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);
        if (nRet > 0) {
            //When someone coonects or coomunicates with a message over a dedicate dconnection
            cout << endl << "data on port... processing now ...";
            //process the request
            /*if (FD_ISSET (nSocket, &fe)){
                cout << endl <<"thers is an exception";
            }
            if (FD_ISSET(nSocket, &fw)){
                 cout << endl <<"ready to write";
            }
            if (FD_ISSET (nSocket, &fr)){
                cout << endl <<"ready to read";
            }
            break;*/
            ProcessTheNewRequest();
        }
        else if (nRet == 0) {
            // none of the socket descriptors are ready hence no connection or communication request made
            cout << endl << "Nothing on port: " << PORT;
        }
        else {
            // fails and application shows some useful message
            cout << endl << "Failed";
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        cout << endl << "After the select call : " << fr.fd_count;
        Sleep(2000);
    }
}