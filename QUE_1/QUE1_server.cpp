// #include <bits/stdc++.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <thread>
#define MSG_LEN 1024

// #include "encode.hpp"
// #include "decode.hpp"
using namespace std;

string base64Decoder(char encoded[], int len_str)
{
    char *decoded_string;

    decoded_string = (char *)malloc(sizeof(char) * MSG_LEN);

    int i, j, k = 0;

    // stores the bitstream.
    int num = 0;

    // count_bits stores current
    // number of bits in num.
    int count_bits = 0;

    // selects 4 characters from
    // encoded string at a time.
    // find the position of each encoded
    // character in char_set and stores in num.
    for (i = 0; i < len_str; i += 4)
    {
        num = 0, count_bits = 0;
        for (j = 0; j < 4; j++)
        {

            // make space for 6 bits.
            if (encoded[i + j] != '=')
            {
                num = num << 6;
                count_bits += 6;
            }

            /* Finding the position of each encoded
            character in char_set
            and storing in "num", use OR
            '|' operator to store bits.*/

            // encoded[i + j] = 'E', 'E' - 'A' = 5
            // 'E' has 5th position in char_set.
            if (encoded[i + j] >= 'A' && encoded[i + j] <= 'Z')
                num = num | (encoded[i + j] - 'A');

            // encoded[i + j] = 'e', 'e' - 'a' = 5,
            // 5 + 26 = 31, 'e' has 31st position in char_set.
            else if (encoded[i + j] >= 'a' && encoded[i + j] <= 'z')
                num = num | (encoded[i + j] - 'a' + 26);

            // encoded[i + j] = '8', '8' - '0' = 8
            // 8 + 52 = 60, '8' has 60th position in char_set.
            else if (encoded[i + j] >= '0' && encoded[i + j] <= '9')
                num = num | (encoded[i + j] - '0' + 52);

            // '+' occurs in 62nd position in char_set.
            else if (encoded[i + j] == '+')
                num = num | 62;

            // '/' occurs in 63rd position in char_set.
            else if (encoded[i + j] == '/')
                num = num | 63;

            // ( str[i + j] == '=' ) remove 2 bits
            // to delete appended bits during encoding.
            else
            {
                num = num >> 2;
                count_bits -= 2;
            }
        }

        while (count_bits != 0)
        {
            count_bits -= 8;

            // 255 in binary is 11111111
            decoded_string[k++] = (num >> count_bits) & 255;
        }
    }

    // place NULL character to mark end of string.
    decoded_string[k] = '\0';

    // return decoded_string;
    string s;
    for (int i = 0; i < k; i++)
    {
        s.push_back(decoded_string[i]);
    }
    return s;
}

void handle_client(int clientSocket)
{
    char buffer[MSG_LEN];
    while (true)
    {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0)
        {
            cout << "Client " << clientSocket << " disconnected" << endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        // cout<<buffer<<endl;
        string s = base64Decoder(buffer, bytesReceived);
        if (s[0] == '3')
        {
            cout << "Client " << clientSocket << " disconnected" << endl;
            break;
        }

        else if (s[0] == '1')
        {
            cout << "Received from client " << clientSocket << ": " << s.substr(2) << endl;
            string response = "Acknowledgment message to a regular message";
            // response += s;
            // cout<<"Enter a response: "<<endl;
            // getline(cin,response);
            send(clientSocket, response.c_str(), response.size(), 0);
        }
        // cout<<s.length()<<endl;
        // cout << "Received from client: "<< s << endl;

        // string response;
        // // response += s;
        // cout<<"Enter a response: "<<endl;
        // getline(cin,response);
        // send(clientSocket, response.c_str(), response.size(), 0);
    }
    close(clientSocket);
}

int main(int argc, char *argv[])
{

    int serverSocket, new_socket, valread;
    struct sockaddr_in server_address;
    // char buffer[1024] = { 0 };

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