// #include<bits/stdc++.h>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#define MSG_LEN 1024

// #include "encode.hpp"
// #include "decode.hpp"
using namespace std;

string base64Encoder(char input_str[], int len_str)
{

	// Character set of base64 encoding scheme
	char char_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	// Resultant string
	char *res_str = (char *)malloc(MSG_LEN * sizeof(char));

	int index, no_of_bits = 0, padding = 0, val = 0, count = 0, temp;
	int i, j, k = 0;

	// Loop takes 3 characters at a time from
	// input_str and stores it in val
	for (i = 0; i < len_str; i += 3)
	{
		val = 0, count = 0, no_of_bits = 0;

		for (j = i; j < len_str && j <= i + 2; j++)
		{
			// binary data of input_str is stored in val
			val = val << 8;

			// (A + 0 = A) stores character in val
			val = val | input_str[j];

			// calculates how many time loop
			// ran if "MEN" -> 3 otherwise "ON" -> 2
			count++;
		}

		no_of_bits = count * 8;

		// calculates how many "=" to append after res_str.
		padding = no_of_bits % 3;

		// extracts all bits from val (6 at a time)
		// and find the value of each block
		while (no_of_bits != 0)
		{
			// retrieve the value of each block
			if (no_of_bits >= 6)
			{
				temp = no_of_bits - 6;

				// binary of 63 is (111111) f
				index = (val >> temp) & 63;
				no_of_bits -= 6;
			}
			else
			{
				temp = 6 - no_of_bits;

				// append zeros to right if bits are less than 6
				index = (val << temp) & 63;
				no_of_bits = 0;
			}
			res_str[k++] = char_set[index];
		}
	}

	// padding is done here
	for (i = 1; i <= padding; i++)
	{
		res_str[k++] = '=';
	}

	res_str[k] = '\0';
	return res_str;
	string s;
	for (int i = 0; i < k; k++)
	{
		s.push_back(res_str[i]);
	}
	return s;
}

int main(int argc, char *argv[])
{
	int status, valread, clientSocket;
	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(stoi(argv[2]));

	char hello[20] = "Hello from client";
	char buffer[MSG_LEN] = {0};
	if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return 1;
	}
	else
	{
		cout << "Socket created" << endl;
	}

	if ((status = connect(clientSocket, (struct sockaddr *)&serv_addr,
						  sizeof(serv_addr))) < 0)
	{
		cout << "\nConnection Failed \n";
		return -1;
	}
	else
	{
		cout << "Connected Successfully" << endl;
	}

	while (true)
	{
		string message;
		cout << "Enter a message to send (or 'exit' to quit): ";
		getline(cin, message);

		if (message[0] == '3')
		{
			break;
		}
		if (message[0] != '1')
		{
			cout << "Invalid message type" << endl;
			continue;
		}

		int length = message.length();

		char char_array[MSG_LEN];
		strcpy(char_array, message.c_str());
		message = base64Encoder(char_array, length + 1);
		// cout<<message<<endl;

		send(clientSocket, message.c_str(), message.length(), 0);

		char ackBuffer[MSG_LEN];
		int bytesReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
		if (bytesReceived > 0)
		{
			ackBuffer[bytesReceived] = '\0';
			cout << "Server response: " << ackBuffer << endl;
		}
	}
	return 0;
}