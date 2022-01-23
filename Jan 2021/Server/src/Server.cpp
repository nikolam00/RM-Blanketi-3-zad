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

	// 2. Kreiranje listensoketa

	SOCKET listensocket;

	listensocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listensocket == INVALID_SOCKET)
		ExitWithError("Listening socket not created!");

	// 3. Sada treba da povezemo listensocket sa sockaddr_in strukturom
	// Za ovu strukturu pravimo strukturu nazovimo je klijent i tu treba da postavimo broj porta na kome osluskujemo

	cout << "Unesite broj porta: ";
	int Port;
	cin >> Port;

	sockaddr_in Server;

	Server.sin_family = AF_INET;
	Server.sin_addr.s_addr = INADDR_ANY;
	Server.sin_port = htons(Port);

	// Kreirali smo strukturu server i podesili smo koji port zelimo da koristimo

	// Sada imamo i strukturu server i lestining socket treba ta dva da povezemo preko bind()

	int bindStatus;

	bindStatus = bind(listensocket, (struct sockaddr *)&Server, sizeof(Server));

	if (bindStatus < 0)
		ExitWithError("Greska prilikom bind()");

	//4. Listen

	// Povezali smo listensocket i strukturu sada treba da stavimo samo da se osluskuje preko tog soketa, da ga pustimo da radi

	int listenStatus;

	listenStatus = listen(listensocket, 2);

	if (listenStatus < 0)
		ExitWithError("Greska prilikom listen()");

	// Drugi parametar je koliko mozemo da konektujemo na taj server

	//5. Accept, Send i Receive

	// Uvek kada se javi neki klijent i zatrazi konekciju on mora da kreira novi soket za njega i da izvrsi konekciju tog soketa i klijenta koji se javio
	// Za svakog klijenta imamo strukturu sockaddr_in

	while (true)
	{
		SOCKET clientSocket;
		sockaddr_in Klijent;

		// Ne vrsimo popunjavanje podataka za Klijenta, jer ce to sama funkcija accept da odradi!

		int Len = sizeof(Klijent);
		clientSocket = accept(listensocket, (struct sockaddr *)&Klijent, &Len);

		if (clientSocket == INVALID_SOCKET)
		{
			ExitWithError("Greska prilikom accept()");
		}

		// Sada smo napravili soket preko koga cemo vrsiti komunikaciju klijenta iservera, preko ovog soketa sada saljemo poruke i primamo poruke.

		int recvMsgSize;
		int sendMsgSize;

		char PorukaRcv[100];

		int i = 0;

		while (1)
		{
			recvMsgSize = recv(clientSocket, PorukaRcv, sizeof(PorukaRcv), 0);

			if (recvMsgSize < 0)
			{
				ExitWithError("Greska prilikom recv()");
			}

			if (recvMsgSize == 0)
				break;

			cout << "Primljena poruka: ";
			PrintMessage(PorukaRcv, recvMsgSize);
			cout << endl;

			char PorukaSnd[100];

			if (i == 0)
			{
				// Deo da izvucem username
				int p = 0;
				while (PorukaRcv[p] != ' ')
					p++;

				p++;

				char Username[20];
				int j = 0;
				while (p < strlen(PorukaRcv))
				{
					Username[j] = PorukaRcv[p];
					j++;
					p++;
				}

				strcpy(PorukaSnd, "Hi ");
				strcat(PorukaSnd, Username);
			}
			else
			{
				strcpy(PorukaSnd, "MSG-" + i);
				strcat(PorukaSnd, PorukaRcv);
			}

			sendMsgSize = send(clientSocket, PorukaSnd, sizeof(PorukaSnd), 0);

			if (sendMsgSize < 0)
			{
				ExitWithError("Greska prilikom send()");
			}

			i++;
		}

		closesocket(clientSocket);
	}

	closesocket(listensocket);
	WSACleanup();
}