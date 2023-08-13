// #include "bits/stdc++.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <thread>
using namespace std;

void handle_client(int clientSocket, struct sockaddr_in clientAddress)
{
    char buffer[1024];
    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            cout << "Client on port " << ntohs(clientAddress.sin_port) << " closed connection" << endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        cout << "Received from client on port " << ntohs(clientAddress.sin_port) << ": " << buffer << endl;

        // string response = "Server received: ";

        int spaces = 0;
        string s1 = "";
        string s2 = "";
        string s3 = "";

        for (int i = 0; i < bytesReceived; i++)
        {
            if (buffer[i] == ' ')
            {
                spaces++;
                continue;
            }

            if (spaces == 0)
                s1.push_back(buffer[i]);
            else if (spaces == 1)
                s2.push_back(buffer[i]);
            else if (spaces == 2)
                s3.push_back(buffer[i]);
        }

        float ans = 0;
        if (s2 == "+")
        {
            ans = stof(s1) + stof(s3);
        }
        else if (s2 == "-")
        {
            ans = stof(s1) - stof(s3);
        }
        else if (s2 == "/")
        {
            ans = stof(s1) / stof(s3);
        }
        else if (s2 == "*")
        {
            ans = stof(s1) * stof(s3);
        }
        else if (s2 == "^")
        {
            ans = pow(stof(s1), stof(s3));
        }

        string output = to_string(ans);
        cout << "Sending to client on port " << ntohs(clientAddress.sin_port) << ": " << output << endl;

        send(clientSocket, output.c_str(), output.size(), 0);
    }
    close(clientSocket);
}

int main(int argc, char *argv[])
{

    int serverSocket, new_socket, valread;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
        // cout<<"Bind failed"<<endl;
        return 1;
    }
    else
    {
        // cout<<"Bounded successfully"<<endl;
    }

    if (listen(serverSocket, 3) < 0)
    {
        cout << "Listening error" << endl;
        return 1;
    }
    else
    {
        cout << "Server Listening on port: " << argv[1] << endl;
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
            char clientIp[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIp, INET_ADDRSTRLEN);
            int client_port = ntohs(clientAddress.sin_port);
            cout << "Client " << clientIp << " on port " << client_port << " connected" << endl;
        }

        thread clientThread(handle_client, new_socket, clientAddress);
        clientThread.detach();
        // handle_client(new_socket);
    }

    close(serverSocket);

    return 0;
}
