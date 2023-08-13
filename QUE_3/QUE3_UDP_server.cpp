// #include "bits/stdc++.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
using namespace std;

void handle_client(int serverSocket, struct sockaddr_in clientAddress)
{
    char buffer[1024];
    socklen_t nlen = sizeof(struct sockaddr);
    ssize_t bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr *)&clientAddress, &nlen);
    buffer[bytesReceived] = '\0';

    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIp, INET_ADDRSTRLEN);
    int client_port = ntohs(clientAddress.sin_port);

    if (buffer[0] == '-' && buffer[1] == '1' && buffer[2] == '\0')
    {
        cout << "Client " << clientIp << " on port " << client_port << " closed communication" << endl;

        return;
    }

    cout << "Client " << clientIp << " on port " << client_port << " connected" << endl;

    cout << "Received from client on port " << client_port << ": " << buffer << endl;

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
    cout << "Sending to client on port " << client_port << ": " << output << endl;

    sendto(serverSocket, output.c_str(), output.size(), 0, (sockaddr *)&clientAddress, nlen);
}

int main(int argc, char *argv[])
{

    int serverSocket, new_socket, valread;
    struct sockaddr_in server_address;

    // Creating socket file descriptor
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
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

    cout << "Server listening on port " << argv[1] << endl;

    while (true)
    {
        char buffer[1024];
        struct sockaddr_in clientAddress;
        // socklen_t addrlen = sizeof(clientAddress);
        handle_client(serverSocket, clientAddress);
    }
    close(serverSocket);

    return 0;
}
