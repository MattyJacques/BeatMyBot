#include <winsock2.h>
#include "Networking.h"
#include "ErrorLogger.h"

Networking* Networking::pInstance = nullptr;

Networking::Networking()
{
  port = 65534;
  sock = 0;
}

void Networking::ServerSetup()
{
  if (sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))
    ErrorLogger::Writeln(L"NetworkSetup() Failure in socket()");

  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(port);

  if (bind(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)))
    ErrorLogger::Writeln(L"NetworkSetup() Failure in bind()");
}

void Networking::Send(SOCKADDR_IN* pIPAddr)
{
  char buff[sizeof(DynamicObjects)];
  memcpy(buff, DynamicObjects::GetInstance(), sizeof(DynamicObjects));
	int nSnt = sendto(sock, buff, sizeof(DynamicObjects), 0, 
				            (sockaddr*)pIPAddr, sizeof(pIPAddr));
}

void Networking::Recieve(char* buff, SOCKADDR_IN* pIPAddr)
{
  int fromSize = sizeof(pIPAddr);
  int nBts = recvfrom(sock, buff, sizeof(DynamicObjects), 0, 
                      (sockaddr*)pIPAddr, &fromSize);

  if (nBts)
    ErrorLogger::Writeln(L"Recieve() Failure");
}

void Networking::Release()
{
  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }
}