 #include "network.h"

Network* Network::pInst = NULL;

Network::Network()
{
	currentConnections = 0;
	portNumber = 5555;
	nonblocked = 1;
	flag = 1;
}

Network::~Network()
{
	CloseConnection();
}

void Network::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
	CloseConnection();
}

Network* Network::GetInstance()
{
	if(!pInst)
		pInst = new Network;

	return pInst;
}

bool Network::ConnectToServer(char* IPAdd)
{
	CloseConnection();

	if(!WSASetUp())
		return false;

	SOCKADDR_IN potentialServer;
	potentialServer.sin_family = AF_INET;
	potentialServer.sin_port = htons(portNumber);
	potentialServer.sin_addr.s_addr = inet_addr(IPAdd);

	sSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sSock == INVALID_SOCKET)
	{
		ErrorLogger::Writeln("Socket Invalid (Connect To Server socket()).");
		return false;
	}

	if(connect(sSock, (SOCKADDR*)&potentialServer, sizeof(potentialServer)) == SOCKET_ERROR)
	{
		ErrorLogger::Writeln("Socket Invalid (Connect To Server connect()).");
		ErrorLogger::Writeln(WSAGetLastError());
		return false;
	}
	else
		return true;
}

bool Network::SetUpServer()
{
	CloseConnection();
	//Close attempts to connect to a server.

	if(!WSASetUp())
		return false;

	SOCKADDR_IN serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(portNumber);
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	sSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(sSock == INVALID_SOCKET)
	{
		ErrorLogger::Writeln("Socket Invalid (Set Up Server socket()).");
		return false;
	}

	ioctlsocket(sSock, FIONBIO, &nonblocked);
	setsockopt(sSock, IPPROTO_IP, TCP_NODELAY, (char*)&flag, sizeof(int));
	
	if(bind(sSock, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
	{
		ErrorLogger::Writeln("bind() failed.");
		return false;
	}

	listen(sSock, MAXCONNECTIONS);

	return true;
}

bool Network::SearchForIncoming()
{
	cSock[currentConnections] = accept(sSock, NULL, NULL);
	if(cSock[currentConnections] != INVALID_SOCKET)
	{
		ioctlsocket(cSock[currentConnections], FIONBIO, &nonblocked);
		setsockopt(cSock[currentConnections], IPPROTO_IP, TCP_NODELAY, (char*)&flag, sizeof(int));
		currentConnections++;
		return true;
	}
	else
		return false;
}

bool Network::WSASetUp()
{
	int error = WSAStartup(0x0202, &wsadata);

	if(error)
	{
		ErrorLogger::Writeln("Failed to start WSA.");
		return false;
	}

	if(wsadata.wVersion != 0x0202)
	{
		CloseConnection();
		ErrorLogger::Writeln("Wrong WSA Version.");
		return false;
	}

	return true;
}

void Network::Send(DynamicObjects* dynam)
{
	char sendbuffer[sizeof(DynamicObjects)];
	memcpy(sendbuffer, dynam, sizeof(DynamicObjects));

	int sendSize = 0;
	for(int i = 0; i < currentConnections; i++)
	{
		sendSize = send(cSock[i], sendbuffer, sizeof(DynamicObjects), 0);
	}
}

DynamicObjects* Network::Receive()
{
	char recvbuffer[sizeof(DynamicObjects)];
	memset(recvbuffer, 0, sizeof(DynamicObjects));

	int recvSize;

	recvSize = recv(sSock, recvbuffer, sizeof(DynamicObjects), 0);
	if(recvSize == sizeof(DynamicObjects))
	{
		return (DynamicObjects*)recvbuffer;
	}
	else if(recvSize == SOCKET_ERROR)
	{
		ErrorLogger::Writeln("Recv Socket Error. WSA Error:");
		ErrorLogger::Writeln(WSAGetLastError());
		return NULL;
	}
	else if(recvSize < sizeof(DynamicObjects))
	{
		ErrorLogger::Writeln("Incomplete Receive");
		return NULL;
	}
}

void Network::CloseConnection()
{
	if(sSock)
		closesocket(sSock);

	for(int i = 0; i < MAXCONNECTIONS; i++)
	{
		if(cSock[i])
			closesocket(cSock[i]);
	}

	WSACleanup();
}