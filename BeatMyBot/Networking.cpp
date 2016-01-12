// Title        : Networking.cpp
// Purpose      : Gives network functionality to game


#include <winsock2.h>       // Windows networking
#include "Networking.h"
#include "ErrorLogger.h"    // WriteLn & error reporting
#include "dynamicObjects.h" // Bot Data


// Initialise instance of the class to null
Networking* Networking::pInstance = nullptr;

bool Networking::isActive = false;


Networking::Networking()
{ // Initialises data to make sure it is not filled with junk

  memset(&data, 0, sizeof(NetData));
  isServer = true;

} // Networking()


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


bool Networking::ServerSetup()
{ // Loads WSA, creates a socket, constructs address struct and then binds the
  // socket to the server address

  bool result = true;

  WSASetup();

  // Create UDP socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    ErrorLogger::Writeln(L"Error: socket() - Exiting");
    result = false;
  }

  memset(&serverAddress, 0, sizeof(serverAddress));     // 0 structure
  serverAddress.sin_family = AF_INET;                   // Internet address
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);    // Any interface
  serverAddress.sin_port = htons(PORT);                 // Local port

  // Bind the socket to the local socket, checking for error
  if (bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
  {
    ErrorLogger::Writeln(L"Error: bind() - Exiting");
    result = false;
  }

  return result;

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


void Networking::ConnectToServer()
{ // Initialises the client and attempts to connect to server asking to
  // retrieve the initial data the server would send

  WSASetup();

  // Create socket
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
  {
    ErrorLogger::Writeln(L"Error: socket() - Exiting");
    Release();
  }

  memset(&serverAddress, 0, sizeof(serverAddress));     // 0 structure
  serverAddress.sin_family = AF_INET;                   // Internet address
  serverAddress.sin_addr.s_addr = inet_addr(IP);        // Server IP
  serverAddress.sin_port = htons(PORT);                 // Local port

  // Create the connect message and send to the server, checking for error
  char connectStr[3] = "hi";
  if (sendto(sock, connectStr, strlen(connectStr), 0, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) != strlen(connectStr))
  {
    ErrorLogger::Writeln(L"Error: sendto() - ConnectToServer() - Exiting");
    Release();
  }

  // Create variables ready for recieving initial data
  int recvlen = 0;
  int addrLen = sizeof(serverAddress);
  char buffer[sizeof(InitialData)];
  memset(&buffer, 0, sizeof(InitialData));

  // Recieve the initial data
  recvlen = recvfrom(sock, buffer, sizeof(InitialData), 0, 
                     (struct sockaddr*) &serverAddress, &addrLen);

  // Create initial data object and init to 0
  InitialData initData;
  memcpy(&initData, &buffer, sizeof(InitialData));

  // Set the timer until the next lot of score is set
  DynamicObjects::GetInstance()->SetScoreTimer(initData.scoreUpdateTimer);

  // Set the scores for the teams, filthy way of doing it however supports any
  // number of teams this way, will change if have time
  for (int i = 0; i < NUMTEAMS; i++)
    DynamicObjects::GetInstance()->m_rgTeams[i].m_iScore = initData.scores[i];

  // Sets the owners of the domination points, one again hacky method but 
  // supports as many domination points as it needs, will change if have time
  for (int i = 0; i < NUMDOMINATIONPOINTS; i++)
    DynamicObjects::GetInstance()->m_rgDominationPoints[i].m_OwnerTeamNumber = initData.dpStates[i];

} // ConnectToServer()


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
    for (UINT j = 0; j < NUMBOTSPERTEAM; j++)
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


bool Networking::Recieve()
{ // Recieves the data from the server

  char buffer[sizeof(NetData)];

  memset(&data, 0, sizeof(NetData));
  memset(&buffer, 0, sizeof(NetData));

  int addrLen = sizeof(serverAddress);

  recvfrom(sock, buffer, sizeof(NetData), 0, (struct sockaddr*) &serverAddress, &addrLen);

  char exitCode[5] = "exit";
  if (strcmp(buffer, exitCode) == 0)
  {
    ErrorLogger::Writeln(L"Exit code recieved - Exiting");
    return false;
  }

  memcpy(&data, &buffer, sizeof(NetData));

  return true;

} // Recieve()


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

  return (int16_t)radians * (180 / PI);

} // RadsToDegrees()


bool Networking::WSASetup()
{ // Starts windows sockets

  bool result = true;

  if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
  {
    ErrorLogger::Writeln(L"WSA FAILURE");
    result = false;
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