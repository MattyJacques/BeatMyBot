// Title        : Networking.cpp
// Purpose      : Gives network functionality to game


#include <winsock2.h>       // Windows networking
#include "Networking.h"
#include "ErrorLogger.h"    // WriteLn & error reporting
#include "dynamicObjects.h" // Bot Data


// Initialise instance of the class to null
Networking* Networking::pInstance = nullptr;

Networking * Networking::GetInstance()
{ // Returns a pointer to the current instance of the class, if none currently
  // exists, create on then return that pointer

  if (!pInstance)
  {
    pInstance = new Networking;
    isActive = true;
  }

  return pInstance;

} // GetInstance()


void Networking::ServerSetup()
{ // Loads WSA, creates a socket, constructs address struct and then binds the
  // socket to the server address

  WSASetup();

  // Create UDP socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    ErrorLogger::Writeln(L"Error: socket() - Exiting");
    Release();
  }

  memset(&serverAddress, 0, sizeof(serverAddress));     // 0 structure
  serverAddress.sin_family = AF_INET;                   // Internet address
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);    // Any interface
  serverAddress.sin_port = htons(PORT);                 // Local port

  // Bind the socket to the local socket, checking for error
  if (bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
  {
    ErrorLogger::Writeln(L"Error: bind() - Exiting");
    Release();
  }

} // ServerSetup()


void* Networking::ConnectToClients()
{ // Secondary thread loops forever until told to quit, waits for new clients,
  // if a new client is found, store the new client and send initial data

  while (!false)
  {
    // Create the address for the client and get the length of the address, init
    // to 0
    sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    int clientLength = sizeof(clientAddress);

    // Create buff for the hello message from client, init to 0
    char buffer[3];
    memset(&buffer, 0, sizeof(buffer));

    // Recieve the message and place into buffer
    recvfrom(sock, buffer, sizeof(buffer), 0,
             (struct sockaddr*) &clientAddress, &clientLength);

    // Create the char array to check if the client is actually valid
    char check[3] = "hi";

    if (strcmp(buffer, check) == 0)
    { // Client send correct message, add to the client list

      ErrorLogger::Writeln(L"Found client");
      StoreClient(clientAddress);

      // Set the score timer of the initial data
      InitialData initData;
      initData.scoreUpdateTimer = DynamicObjects::GetInstance()->GetScoreTimer();

      // Set the states of the domination points 
      for (int i = 0; i < NUMDOMINATIONPOINTS; i++)
        initData.dpStates[i] = DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber;

      // Set scores for the teams
      for (int i = 0; i < NUMTEAMS; i++)
        initData.scores[i] = DynamicObjects::GetInstance()->GetScore(i);

      // Send the initial data to the new client
      if (sendto(sock, (char*) &initData, sizeof(InitialData), 0, (struct sockaddr*) &clientAddress, sizeof(clientAddress)))
       ErrorLogger::Writeln(L"Initial Data Sento() Failed");
    }

    if (!isActive)
    { // If the server is not active, close the thread
      ErrorLogger::Writeln(L"Server not active, ending thread");
      _endthread();
    }

  }

} // ConnectToClients()


void Networking::CreateThread()
{ // Makes a new thread to check for clients

  _beginthreadex(NULL, 0, (unsigned int(__stdcall *)(void*))ConnectToClients(), 0, 0, (unsigned int*)&thread);

} // CreateThread()


void Networking::StoreClient(sockaddr_in address)
{  // Stores the client in the vector of clients

  clients.push_back(address);

} // StoreClient()


void Networking::Send()
{ // Sends data to all clients within the vector

  // Set location data for bots
  for (int i = 0; i < NUMTEAMS; i++)
  {
    for (int j = 0; j < NUMBOTSPERTEAM; j++)
    {
      data.teams[i].bots[j].xValue = (int16_t)DynamicObjects::GetInstance()->
                                        GetBot(i, j).GetLocation().XValue;
      data.teams[i].bots[j].yValue = (int16_t)DynamicObjects::GetInstance()->
                                        GetBot(i, j).GetLocation().YValue;
      data.teams[i].bots[j].dir = RadsToDegrees(DynamicObjects::GetInstance()->
                                        GetBot(i, j).GetDirection());
      data.teams[i].bots[j].isAlive = DynamicObjects::GetInstance()->
                                        GetBot(i, j).IsAlive();
    }
  }

  // Set shot data for bots
  for (int i = 0; i < NUMTEAMS; i++)
  {
    for (UINT i = 0; i < NUMBOTSPERTEAM; i++)
    {
      data.teams[i].shots[j].team = (int8_t)DynamicObjects::GetInstance()->
                                       GetBot(i, j).GetTargetTeam();
      data.teams[i].shots[j].bot = (int8_t)DynamicObjects::GetInstance()->
                                       GetBot(i, j).GetTargetBot();
      data.teams[i].shots[j].damage = (int8_t)DynamicObjects::GetInstance()->
                                       GetBot(i, j).GetDamage();
      data.teams[i].shots[j].firing = DynamicObjects::GetInstance()->
                                       GetBot(i, j).GetFiring();
    }
  }

  // Send data to all clients
  for (int i = 0; i < clients.size(); i++)
  {
    if (sendto(sock, (char*)&data, sizeof(NetData), 0, (struct sockaddr*) &clients[i], sizeof(clients[i])))
      ErrorLogger::Writeln(L"Error: sendto() - Send() - Wrong size - Exiting");
  }

} // Send()


void Networking::Release()
{ // If called while pInstance is valid, deletes and defines as nullptr

  if (pInstance)
  {
    pInstance->CloseConnections();
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()


void Networking::SendClientExit()
{ // When server is closed, tells clients to exit so they do not crash while
  // waiting to recieve

  char exitCode[5] = "exit";

  for (int i = 0; i < clients.size(); i++)
  {
    if (sendto(sock, exitCode, sizeof(exitCode), 0, (struct sockaddr*) &clients[i], sizeof(clients[i])))
      ErrorLogger::Writeln(L"Error: sendto() - SendClientExit() - Wrong size - Exiting");
  }

} // SendClientExit()


int16_t Networking::RadsToDegrees(double radians)
{ // Converts radians to degrees

  return radians * (180 / PI);

} // RadsToDegrees()


bool Networking::WSASetup()
{ // Starts windows sockets

  bool result = true;

  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    ErrorLogger::Writeln(L"WSA FAILURE");
    return false;
  }

  return result;

} // WSASetup()


void Networking::CloseConnections()
{ // Sends exit code to any clients, closes socket and closes windows sockets

  SendClientExit();
  isActive = false;
  closesocket(sock);
  WSACleanup();

} // CloseConnections()
