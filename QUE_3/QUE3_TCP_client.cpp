// #include "bits/stdc++.h"
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <netdb.h>

using namespace std;

int main(int argc, char *argv[])
{
	int status, valread, clientSocket;
	struct sockaddr_in serv_addr;

	// Hostname to IP address:
	string hostname = argv[1];
	struct hostent *host_entry = gethostbyname(hostname.c_str());

	if (host_entry != nullptr)
	{
		struct in_addr **addr_list = (struct in_addr **)host_entry->h_addr_list;
		argv[1] = inet_ntoa(*addr_list[0]);
	}
	else
	{
		cout << "Please enter a valid hostname" << endl;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(stoi(argv[2]));

	if ((clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
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
		cout << "TCP Client connected to " << argv[1] << " on port " << argv[2] << endl;
	}

	while (true)
	{
		string message;
		cout << "Enter an expression in the following format:" << endl
			 << "operand1 operator operand2" << endl
			 << "Valid operators are + - * / ^." << endl
			 << "To quit, enter -1." << endl;
		getline(cin, message);

		if (message == "-1")
		{
			cout << "Bye!" << endl;
			break;
		}

		send(clientSocket, message.c_str(), message.length(), 0);

		char ackBuffer[1024];
		int bytesReceived = recv(clientSocket, ackBuffer, sizeof(ackBuffer), 0);
		if (bytesReceived > 0)
		{
			ackBuffer[bytesReceived] = '\0';
			cout << "ANS: " << ackBuffer << endl;
		}
	}
	return 0;
}
