// #include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <thread>
using namespace std;

void handle_client(int clientSocket)
{
    char buffer[1024];
    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            cout << "Client " << clientSocket << " disconnected" << endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        cout << "Received from client " << clientSocket << ": " << buffer << endl;

        string response;
        // response += buffer;
        cout << "Enter a response: " << endl;
        getline(cin, response);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    close(clientSocket);
}

int main(int argc, char *argv[])
{

    int serverSocket, new_socket, valread;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Error making server socket" << endl;
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(stoi(argv[1]));

    // Binding to the server
    int nbind = ::bind(serverSocket, (sockaddr *)&server_address, sizeof(server_address));
    if (nbind == -1)
    {
        cout << "Bind failed" << endl;
        return 1;
    }
    else
    {
        cout << "Bounded successfully" << endl;
    }

    if (listen(serverSocket, 3) < 0)
    {
        cout << "Listening error" << endl;
        return 1;
    }
    else
    {
        cout << "Listening" << endl;
    }

    while (true)
    {
        struct sockaddr_in clientAddress;
        int addrlen = sizeof(clientAddress);
        if ((new_socket = accept(serverSocket, (struct sockaddr *)&clientAddress,
                                 (socklen_t *)&addrlen)) < 0)
        {
            cout << "Error in accepting" << endl;
            return 1;
        }
        else
        {
            cout << "Accepting from client: " << new_socket << endl;
        }

        thread clientThread(handle_client, new_socket);
        clientThread.detach();
    }

    close(serverSocket);

    return 0;
}