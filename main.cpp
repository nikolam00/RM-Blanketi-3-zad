#include<iostream>
#include<stdio.h>
#include<string.h>
#include<winsock.h>
#include<fstream>
#include<io.h>
#include<fcntl.h>
#include<ctime>
#pragma comment(lib,"wsock32.lib")

using namespace std;

void main()   //SMTP
{
	WSAData wsa;
	WSAStartup(0x0202, &wsa);

	SOCKET listenSocket;
	sockaddr_in server;

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == SOCKET_ERROR)
	{
		cout << "Greska prilikom kreiranja listensoketa!" << endl;
		exit(-1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(25);         //SMTP protokol koristi port 25
	server.sin_addr.s_addr = INADDR_ANY;

	int Len = sizeof(sockaddr);

	int bindStatus;

	bindStatus = bind(listenSocket, (struct sockaddr*)&server, Len);

	if (bindStatus == -1)
	{
		cout << "Greska prilikom bind()!" << endl;
		exit(-1);
	}

	sockaddr_in client;
	if (listen(listenSocket, 1) == SOCKET_ERROR)
	{
		cout << "Greksa listen!" << endl;
		exit(-1);
	}
	while(1)
	{
		SOCKET socketComm = accept(listenSocket, (struct sockaddr*)&client, &adrlen);

		char SendMsg[1024];
		ZeroMemory(send_data, 1024);
		char RecvMsg[1024];
		ZeroMemory(recv_data, 1024);

		// Prvo server salje 220 i rm.edu
		strcpy(SendMsg, "220 rm.edu\r\n");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		// Zatim server prima poruku tipa HELO 
		recv(socketComm, RecvMsg, 1024, 0);
		if (strstr(RecvMsg, "HELO") == NULL)
		{
			cout << "Klijent nije poslao poruku HELO ..." << endl;
			exit(-1);
		}

		// Da izvucemo username
		char User[30];
		int i = 0;
		char* sp;
		sp = strstr(RecvMsg, "HELO");
		sp += strlen("HELO ");	// Nakon ovoga sledi username

		while ((*sp) != '\0')
		{
			User[i++] = (*sp);
			sp++;
		}
		User[i] = '\0';

		// Sada server odgovara sa Hello username

		strcpy(SendMsg, "250 Hello ");
		strcat(SendMsg, host);
		strcat(SendMsg, "\r\n");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		// Sada sa klijenta dolazi MAIL FROM mejl

		recv(socketComm, RecvMsg, 1024, 0);

		if (strstr(RecvMsg, "MAIL FROM:") == NULL)
		{
			cout << "Klijent nije poslao poruku MAIL FROM:  ..." << endl;
			exit(-1);
		}

		char Sender[100];
		i = 0;
		sp = strstr(RecvMsg, "MAIL FROM: <");
		sp += strlen("MAIL FROM: <");

		while ((*sp) != '>')
		{
			Sender[i++] = (*sp);
			sp++;
		}
		Sender[i] = '\0';

		strcpy(SendMsg, "250 ");
		strcat(SendMsg, Sender);
		strcat(SendMsg, " Sender OK\r\n");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		// Sada to isto i za primaoca

		recv(socketComm, RecvMsg, 1024, 0);
		if (strstr(RecvMsg, "RCPT TO:") == NULL)
		{
			cout << "Klijent nije poslao poruku RCP TO:  ..." << endl;
			exit(-1);
		}

		char Receiver[100];
		i = 0;
		sp = strstr(RecvMsg, "RCPT TO: <");
		sp += strlen("RCPT TO: <");
		while ((*sp) != '>')
		{
			Receiver[i++] = (*sp);
			sp++;
		}
		Receiver[i] = '\0';

		strcpy(SendMsg, "250 ");
		strcat(SendMsg, Receiver);
		strcat(SendMsg, " Recipient OK\r\n");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		// Sada treba da server dobije DATA

		recv(socketComm, RecvMsg, 1024, 0);
		if (strstr(RecvMsg, "DATA") == 0)
		{
			cout << "Klijent nije poslao poruku DATA" << endl;
			exit(-1);
		};

		// Server salje odgovor na DATA 
		strcpy(SendMsg, "354 Enter mail, end with . on a line by itself\r\n");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		FILE* f;
		f = fopen("Mejlovi.txt", "a");

		fprintf(f, "%s ", User);
		fprintf(f, "%s ", Sender);
		fprintf(f, "%s ", Receiver);

		do
		{
			recv(socketComm, RecvMsg, 1024, 0);
			if (strcmp(RecvMsg, ".") != 0)
				fprintf(f, "%s ", RecvMsg);
		} while (strcmp(RecvMsg, ".") != 0);

		char* Vreme = ctime(&now);

		fprintf(f, "%s\n", Vreme);

		fclose(f);

		// Server salje poruku da je primio poruku . za kraj DATA
		strcpy(SendMsg, "250 OK\r\n");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		// Na kraju server treba da primi poruku QUIT
		recv(socketComm, RecvMsg, 1024, 0);

		strcpy(SendMsg, "221 rm.edu closing connection");
		send(socketComm, SendMsg, strlen(SendMsg) + 1, 0);

		closesocket(socketComm);
	}
	
	closesocket(listenSocket);
	WSACleanup();
}