#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "image.h"
#include <fstream>
#include <list>
#include <string>
#include<mutex>
#define PORT 9988
using namespace std;

mutex mtx;
bool SendingMatrix(SOCKET _socket, Image& my_image)
{
    int rows = my_image.get_rows(), cols = my_image.get_cols();
    int** arr = new int* [rows];
    for (int val = 0; val < rows; val++)
    {
        arr[val] = new int[cols];
        for (int j = 0; j < cols; j++) {
            arr[val][j] = my_image[val][j];

        }
        if (send(_socket, (char*)arr[val], cols * 4, 0) == SOCKET_ERROR)
        {
            cerr << "Failed to send file data to server........(:/)\n" << endl;
            return false;
        }
    }

    cout << "Image  has been UPLOADED successfully" << endl;

    return true;
}

int main()
{
    WSADATA _wsData;
    if (WSAStartup(MAKEWORD(2, 2), &_wsData) != 0) {
        cerr << "Failed to initialize Winsock" << endl;
        return 1;
    }

    SOCKET ClientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ClientSocket == INVALID_SOCKET) {
        cerr << "Failed to create socket" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in AddressofServer;
    AddressofServer.sin_family = AF_INET;
    AddressofServer.sin_addr.s_addr = inet_addr("127.0.0.1");
    AddressofServer.sin_port = htons(PORT);
    mtx.lock();
    if (connect(ClientSocket, (struct sockaddr*)&AddressofServer, sizeof(AddressofServer)) == SOCKET_ERROR) {
        cerr << "Failed to connect to the server" << endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    Image my_image("Image5.jpg");

    // my_image.show();
     // Sending Image rows
    int rows = my_image.get_rows(), cols = my_image.get_cols();

    if (int i = send(ClientSocket, (char*)&rows, sizeof(int), 0) == INVALID_SOCKET)
    {
        cout << i << endl;
        int j = recv(ClientSocket, (char*)&j, 4, 0);
        cout << j;
        cerr << "Failed to send Number of rows to server" << endl; return 1;
    }
    else
    {
        // Sending number of cols of row
        if (send(ClientSocket, (char*)&cols, sizeof(int), 0) == INVALID_SOCKET)
        {

            cerr << "Failed to send Number of cols to server" << endl; return 1;
        }
        int j = recv(ClientSocket, (char*)&j, 4, 0);
        cout << j;
    }
    SendingMatrix(ClientSocket, my_image);


    int char_count;
    if (recv(ClientSocket, (char*)&char_count, sizeof(int), 0) == SOCKET_ERROR)
    {
        cerr << "Failed to receive Number of characters from server" << endl; return 1;
    }
    cout << "Number of characters in the picture are  :  " << char_count << endl;
    my_image.show();
    mtx.unlock();
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}
