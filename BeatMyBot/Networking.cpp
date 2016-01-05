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
} // Networking()

void Networking::ServerSetup()
{ // Creates a socket and initialises the server address to appropriate
  // values for networking communications. Binds the socket to the address
  // while outputting appropriate error messages if occured

  // Create a socket and check for creation error
  if (sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))
    ErrorLogger::Writeln(L"NetworkSetup() Failure in socket()");

  // Inits serverAddr to 0, then defines each member of the struct to the
  // appropriate values for network communication
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(port);

  // Binds the created socket to the server address while checking for an
  // error
  if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)))
    ErrorLogger::Writeln(L"NetworkSetup() Failure in bind()");

} // ServerSetup()

void Networking::Send(SOCKADDR_IN* pIPAddr)
{ // Sends the contents of DynamicObjects over network using a buffer

  // Create buffer the size of the data then copy the data into the buffer
  char buff[sizeof(DynamicObjects)];
  memcpy(buff, DynamicObjects::GetInstance(), sizeof(DynamicObjects));

  // Sends the data using the socket and address given as a parameter
	int nSnt = sendto(sock, buff, sizeof(DynamicObjects), 0, 
				            (sockaddr*)pIPAddr, sizeof(pIPAddr));

} // Send()

void Networking::Recieve(char* buff, SOCKADDR_IN* pIPAddr)
{ // Recieves data and places directly within DynamicObjects

  // Define size of data to recieve
  int fromSize = sizeof(pIPAddr);

  // Recieves the data and places within DynamicObjects
  int nBts = recvfrom(sock, buff, sizeof(DynamicObjects), 0, 
                      (sockaddr*)pIPAddr, &fromSize);

  // If greater than 0, output error
  if (nBts)
    ErrorLogger::Writeln(L"Recieve() Failure");

} // Recieve()

void Networking::Release()
{ // If called while pInstance is valid, deletes and defines as nullptr

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()