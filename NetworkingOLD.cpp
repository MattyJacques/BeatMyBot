#include <winsock2.h>
#include "Networking.h"
#include "ErrorLogger.h"

Networking* Networking::pInstance = nullptr;

Networking::Networking()
{
  portNumber = 65534;
  serverSocket = 0;
  clientSocket = 0;
  isConnected = false;
}

Networking* Networking::GetInstance()
{
  if (!pInstance)
    pInstance = new Networking;

  return pInstance;
}

bool Networking::ServerStart()
{
  unsigned long iMode = 1;  // Input mode for socket in ioctlsocket()
  const char optVal = 1;    // Option value for socket option in setsockopt()
  bool isSuccess = true;    // Bool to return if successful

  EndConnection();  // No server found
  StartWSA();       // Start Windows Socket App

  SOCKADDR_IN serverIP;                         // Creates sever address
  serverIP.sin_family = AF_INET;                // Sets family for UDP
  serverIP.sin_port = htons(portNumber);        // Sets port for server
  serverIP.sin_addr.s_addr = htonl(INADDR_ANY); // Sets server address

  // Sets the server socket to created socket
  serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // Checks to make sure the socket is valid
  if (serverSocket == INVALID_SOCKET)
  {
    ErrorLogger::Writeln(L"Invalid socket in ServerStart()");
    isSuccess = false; // Error encountered, change to not successful
  }

  ioctlsocket(serverSocket, FIONBIO, &iMode); // Turn off blocking on socket
  
  // Turns on TCP_NODELAY
  //setsockopt(serverSocket, IPPROTO_IP, TCP_NODELAY, &optVal, sizeof(int));

  if (bind(serverSocket, (LPSOCKADDR)&serverIP, sizeof(serverIP))
      == SOCKET_ERROR)
  { // Attempt to bind the socket to port, if not output error message
    ErrorLogger::Writeln(L"Socket binding failed in ServerStart()");
    isSuccess = false; // Error encountered, change to not successful
  }

  listen(serverSocket, SOMAXCONN); // Listen for connections

  return isSuccess; // Returns where function has been succesful
}

//bool Networking::ServerConnect(char* ipAddr)
//{
//  bool isSuccess = true; // Bool to return if successful
//
//  EndConnection(); // End any outstanding connection attempts
//  StartWSA();      // Start Windows Socket App
//
//  SOCKADDR_IN connectAddress;                         // Potential server addr
//  connectAddress.sin_family = AF_INET;                // Set family for TCP
//  connectAddress.sin_port = htons(portNumber);        // Sets port to use
//  connectAddress.sin_addr.s_addr = inet_addr(ipAddr); // Sets server address
//
//  // Creates socket then sets socket to connect to
//  serverSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP); 
//
//  if (serverSocket == INVALID_SOCKET)
//  { // Checks if server socket is invalid, if so output error
//    ErrorLogger::Writeln(L"Invalid Socket in ServerConnect()");
//    isSuccess = false;  // Error encountered, change to not successful
//  }
//
//  if (connect(serverSocket, (SOCKADDR*)&connectAddress, sizeof(connectAddress))
//      == SOCKET_ERROR)
//  { // Attempts to connect to the server, if not, outputs socket error
//    ErrorLogger::Writeln(L"Socket error in ServerConnect()");
//    ErrorLogger::Writeln(double(WSAGetLastError()));
//    isSuccess = false; // Error encountered, change to not successful
//  }
//
//  return isSuccess; // Returns where function has been succesful
//}

//bool Networking::Search()
//{
//  unsigned long iMode = 1;  // Input mode for socket in ioctlsocket()
//  const char optVal = 1;    // Option value for socket option in setsockopt()
//  bool isSuccess = false;   // Bool to return if successful
//
//  // Accept client connection and set client socket to socket being used
//  clientSocket = accept(serverSocket, NULL, NULL);
//
//  if (clientSocket != INVALID_SOCKET)
//  { // Checks if client sock is valid, if it is, set up the socket options
//    ioctlsocket(clientSocket, FIONBIO, &iMode);
//    //setsockopt(clientSocket, IPPROTO_IP, TCP_NODELAY, &optVal, sizeof(int));
//    isSuccess = true; // No error encountered, function was successful
//  }
//  
//  return isSuccess; // Returns where function has been succesful
//}

void Networking::StartWSA()
{

  // Start WSA, store result for error check
  int error = WSAStartup(MAKEWORD(2,2), &wsaData);

  // If error != 0, output error
  if (error)
    ErrorLogger::Writeln(L"StartWSA() failure");

  if (wsaData.wVersion != MAKEWORD(2, 2))
  { // Correct version check, if not end connection and output error
    EndConnection();
    ErrorLogger::Writeln(L"Wrong WSA version");
  }
}

void Networking::Send(DynamicObjects* data)
{
	char buffer[sizeof(DynamicObjects)];
	memcpy(buffer, DynamicObjects::GetInstance(), sizeof(DynamicObjects));

	int size;

  if (size = recvfrom(serverSocket, buffer, sizeof("@"), 0,  "172))

	size = send(clientSocket, buffer, sizeof(DynamicObjects), 0);
}

DynamicObjects* Networking::Recieve()
{
	DynamicObjects* pData = nullptr;

	char buffer[sizeof(DynamicObjects)];
	memset(buffer, 0, sizeof(DynamicObjects));

	int size;

	size = recv(serverSocket, buffer, sizeof(DynamicObjects), 0);
	if (size == sizeof(DynamicObjects))
	{
		pData = (DynamicObjects*)buffer;
	}
	else if (size == SOCKET_ERROR)
	{
		ErrorLogger::Writeln(L"Socket Error in Recieve(), code: ");
		ErrorLogger::Writeln((double)WSAGetLastError());
	}
	else if (size < sizeof(DynamicObjects))
	{
		ErrorLogger::Writeln(L"Incomplete data in Recieve()");
	}

	return pData;
}

void Networking::Release()
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = nullptr;
	}

	EndConnection();
}

void Networking::EndConnection()
{
  if (serverSocket)
    closesocket(serverSocket);

  if (clientSocket)
    closesocket(clientSocket);
}

Networking::~Networking()
{
  EndConnection();
}