#include <iostream>
#include <winsock.h>
#include <string>

#pragma comment(lib, "winsock32.lib")

using namespace std;

void ExitWithError(string Message)
{
	cout << Message << "Error:" << WSAGetLastError() << endl;
	WSACleanup();
	exit(1);
}

void PrintMessage(char *Message, int Len)
{
	cout << "Primljena poruka:" << endl;
	for (int i = 0; i < Len; i++)
	{
		cout << Message[i];
	}

	cout << endl;
}

int main(int argc, char *argv[])
{
	WSADATA wsa;

	// 1. Inicijalizacija Winsoketa

	if (WSAStartup(0x0202, &wsa) != 0)
		ExitWithError("Startup failed!");

	// 2. Kreiranje clientsoketa koji ce nam sluziti za komunikaciju

	SOCKET clientsocket;

	clientsocket = socket(AF_INET, SOCK_STREAM, 0);

	if (clientsocket == INVALID_SOCKET)
		ExitWithError("Clinet socket not created!");

	// 3. Kreiramo strukturu sockaddr_in server koja ce nam sluziti da povezemo client soket i server preko connect
	
	cout << "Unesite adresu servera:" << endl;
	char Adresa[20];
	cin >> Adresa;
	cout << "Unesite broj porta:" << endl;
	int Port;
	cin >> Port;

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(Adresa);
	server.sin_port = htons(Port);

	// Za server ovde pre connect unesemo podatke koji su nam potrebni


	// 4. Vrsimo konekciju clientsocketa i servera, pomocu tog clientsocketa cemo moci da komuniciramo posle
	int Len = sizeof(struct sockaddr_in);

	int connectStatus;

	connectStatus = connect(clientsocket, (struct sockaddr*) &server, sizeof(server));

	if (connectStatus < 0)
	{
		ExitWithError("connect() failed!");
	}

	// 5. Deo za komunikaciju RECV i SEND  
	int sendMsgSize,recvMsgSize;

	while(true)
	{
		break;
	}

}