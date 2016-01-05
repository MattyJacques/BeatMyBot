// Title        : Networking.h
// Purpose      : Gives network functionality to game


#pragma once


#include "dynamicObjects.h"


class Networking
{

private:
  int sock;                         // Holds the socket number
  SOCKADDR_IN serverAddr;           // Holds address for server
  SOCKADDR_IN clientAddr;           // Holds address for client
  unsigned short port;              // Holds port number for communication
  static Networking* pInstance;     // Holds current instance of the class
  

  // Sets up port, nulls socket ready for creation
  Networking();

  // Creates a socket and initialises the server address to appropriate
  // values for networking communications. Binds the socket to the address
  // while outputting appropriate error messages if occured
  void ServerSetup();

public:
  // Returns a pointer to the current instance of the class, if none currently
  // exists, create on then return that pointer
  static Networking* GetInstance();
  
  // Sends the contents of DynamicObjects over network using a buffer
  void Send(SOCKADDR_IN* pIPAddr);

  // Recieves data and places directly within DynamicObjects
  void Recieve(char* buff, SOCKADDR_IN* pIPAddr);

  // If called while pInstance is valid, deletes and defines as nullptr
  static void Release();

};

