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
  WSADATA wsaData;
  u_long nonblocking;
  int flag;
  char buffer[sizeof(DynamicObjects)];
  

  // Sets up port, nulls socket ready for creation
  Networking();


  

public:
  bool ConnectToServer(char* ipAddr);
  bool WSASetup();
  void CloseConnection();

  // Creates a socket and initialises the server address to appropriate
  // values for networking communications. Binds the socket to the address
  // while outputting appropriate error messages if occured
  bool ServerSetup();


  // Returns a pointer to the current instance of the class, if none currently
  // exists, create on then return that pointer
  static Networking* GetInstance();
  
  // Sends the contents of DynamicObjects over network using a buffer
  void Send();

  // Recieves data and returns pointer to data
  DynamicObjects* Recieve();

  // If called while pInstance is valid, deletes and defines as nullptr
  static void Release();

};

