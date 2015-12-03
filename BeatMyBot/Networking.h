#pragma once

#include "dynamicObjects.h"

class Networking
{

public:
  int sock;
  SOCKADDR_IN serverAddr;
  SOCKADDR_IN clientAddr;
  unsigned short port;
  
  

  static Networking* GetInstance();
  Networking();
  void ServerSetup();
  void Send(SOCKADDR_IN* pIPAddr);
  void Recieve(char* buff, SOCKADDR_IN* pIPAddr);
  static void Release();

protected:
	static Networking* pInstance;

};

