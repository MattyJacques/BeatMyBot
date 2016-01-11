// Title        : Networking.cpp
// Purpose      : Gives network functionality to game


#include <winsock2.h>       // Windows networking
#include "Networking.h"
#include "ErrorLogger.h"    // WriteLn & error reporting

// Initialise instance of the class to null
Networking* Networking::pInstance = nullptr;

Networking::Networking()
{
  port = 65534;
  sock = 0;
  nonblocking = 1;
  flag = 1;
} // Networking()

Networking* Networking::GetInstance()
{
  if (!pInstance)
    pInstance = new Networking;

  return pInstance;
}


bool Networking::WSASetup()
{
  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    ErrorLogger::Writeln(L"WSA FAILURE");
    return false;
  }

  return true;
}

bool Networking::ServerSetup()
{ // Creates a socket and initialises the server address to appropriate
  // values for networking communications. Binds the socket to the address
  // while outputting appropriate error messages if occured

  // Create a socket and check for creation error
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    ErrorLogger::Writeln(L"NetworkSetup() Failure in socket() - server");

  // Inits serverAddr to 0, then defines each member of the struct to the
  // appropriate values for network communication
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(port);

  ioctlsocket(sock, FIONBIO, &nonblocking);
  //setsockopt(sock, IPPROTO_IP, TCP_NODELAY, (char*)&flag, sizeof(int));

  // Binds the created socket to the server address while checking for an
  // error
  if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
  {
    ErrorLogger::Writeln(L"NetworkSetup() Failure in bind()");
    return false;
  }
  return true;

} // ServerSetup()

bool Networking::ConnectToServer(char* ipAddr)
{
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    ErrorLogger::Writeln(L"NetworkSetup() Failure in socket() - client");
    return false;
  }

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(ipAddr);
  serverAddr.sin_port = htons(port);

}

void Networking::Send()
{ // Sends the contents of DynamicObjects over network using a buffer

  char ping[1];

  int rcvlen;
  int clientlen = sizeof(clientAddr);

  if ((rcvlen = recvfrom(sock, ping, sizeof(ping), 0, 
    (struct sockaddr*)&clientAddr, &clientlen)) != -1)
  {
    memcpy(buffer, DynamicObjects::GetInstance(), sizeof(DynamicObjects));

    // Sends the data using the socket and address given as a parameter
    if (sendto(sock, buffer, sizeof(DynamicObjects), 0,
      (sockaddr*)&clientAddr, sizeof(clientAddr)) != sizeof(DynamicObjects))
      ErrorLogger::Writeln(L"SEND WRONG SIZE");
    

  }

} // Send()

DynamicObjects* Networking::Recieve()
{ // Recieves data and places directly within DynamicObjects

  char ping[1] = { '.' };

  int rcvlen;
  int clientlen = sizeof(clientAddr);

  if (sendto(sock, ping, sizeof(ping), 0,
    (sockaddr*)&serverAddr, sizeof(serverAddr)) != sizeof(ping))
    ErrorLogger::Writeln(L"SEND WRONG SIZE");

  if ((rcvlen = recvfrom(sock, buffer, sizeof(DynamicObjects), 0, 
    (sockaddr*)&clientAddr, &clientlen)) < 0)
  { 
    ErrorLogger::Writeln(L"RECIEVE FAILURE");
  }

  if (rcvlen == sizeof(DynamicObjects))
    return (DynamicObjects*)buffer;
  else
  {
    ErrorLogger::Writeln(L"RECIEVE WRONG SIZE");
    return nullptr;
  }

} // Recieve()



void Networking::Release()
{ // If called while pInstance is valid, deletes and defines as nullptr

  pInstance->CloseConnection();

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()

void Networking::CloseConnection()
{
  if (sock)
  {
    closesocket(sock);
    WSACleanup();
  }
}