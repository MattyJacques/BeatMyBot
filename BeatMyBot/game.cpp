#include "game.h"
#include "staticmap.h"
#include "Renderer.h"
#include "errorlogger.h"
#include "myinputs.h"
#include "dynamicobjects.h"
#include "Pathfinder.h"
#include "Behaviours.h"
#include "Networking.h"
#include "State.h"
#include "Capture.h"
#include "Attack.h"
#include "Reload.h"
#include "Defend.h"


Game* Game::pInst = NULL;

Game* Game::GetInstance()
{
	if(pInst == NULL)
	{
		pInst= new Game();
	}

	return pInst;
}


void Game::Release()		// Static
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}


Game::Game()
{
	InitialiseScript();
}

Game::~Game()
{
	
}

ErrorType Game::Start()
{

	m_timer.mark();
	m_timer.mark();
	m_State = RUNNING;

	return SUCCESS;
}

ErrorType Game::RunInterface()
{
	ErrorType answer=SUCCESS;

	Renderer* pTheRenderer = Renderer::GetInstance();

	// Handle mouse
	static int mouseX = 0;
	static int mouseY = 0;
	static float screenCentreX = 0.0f;
	static float screenCentreY = 0.0f;
	static float zoom = 0.5f;
	MyInputs* pInputs = MyInputs::GetInstance();
	pInputs->SampleMouse();

	mouseX += int(pInputs->GetMouseDX()/zoom);
	mouseY -= int(pInputs->GetMouseDY()/zoom);

	if(mouseX<screenCentreX-200/zoom)
		screenCentreX-=((screenCentreX-200/zoom)-mouseX)/(50*zoom);
	if(mouseX>screenCentreX+200/zoom)
		screenCentreX+=(mouseX-(screenCentreX+200/zoom))/(50*zoom);
	if(mouseY>screenCentreY+200/zoom)
    screenCentreY += (mouseY - (screenCentreY+200/zoom)) / (50 * zoom);
	if(mouseY<screenCentreY-200/zoom)
    screenCentreY -= ((screenCentreY-200/zoom)-mouseY) / (50 * zoom);

	// Handle viewport
	pTheRenderer->SetViewScale(zoom);
	pTheRenderer->SetViewCentre(Vector2D(screenCentreX,screenCentreY));

	pTheRenderer->DrawMousePointer( Vector2D(float(mouseX), float(mouseY)));

	if(pInputs->GetMouseDZ()>0)
		zoom*=1.05f;
	if(pInputs->GetMouseDZ()<0)
		zoom/=1.05f;

	// Display the scores
	for(int i=0;i<NUMTEAMS;i++)
	{
		int score=(DynamicObjects::GetInstance()->GetScore(i));
		Vector2D pos(10.0f, i*25.0f+10.0f);
		pTheRenderer->DrawTextAt(pos, L"Team ");
		pos.set(75.0f, i*25.0f+10.0f);
		pTheRenderer->DrawNumberAt(pos, i+1);
		pos.set(105.0f, i*25.0f+10.0f);
		pTheRenderer->DrawNumberAt(pos, score);
	}

  // Display Bot States
  if (Networking::GetInstance()->isServer)
  {
    for (int i = 0; i < NUMBOTSPERTEAM; i++)
    {
      wchar_t stateName[11];
      if (DynamicObjects::GetInstance()->GetBot(0, i).pCurrentState)
      {
        mbstowcs(stateName, DynamicObjects::GetInstance()->GetBot(0, i).pCurrentState->GetStateName(), 11);
      }
      else
      {
        char name[11];
        sprintf(name, "nullptr");
        mbstowcs(stateName, name, 11);
      }

      Vector2D pos(10.0f, i*50.0f + 100.0f);
      pTheRenderer->DrawTextAt(pos, L"Bot ");
      pos.set(42.5f, i*50.0f + 100.0f);
      pTheRenderer->DrawNumberAt(pos, i);
      pos.set(60.5f, i * 50.0f + 100.0f);

      if (DynamicObjects::GetInstance()->GetBot(0, i).IsAlive())
        pTheRenderer->DrawTextAt(pos, L"Alive");
      else
        pTheRenderer->DrawTextAt(pos, L"Dead");

      pos.set(110.0f, i*50.0f + 100.0f);
      pTheRenderer->DrawTextAt(pos, stateName);
      pos.set(200.0f, i*50.0f + 100.0f);
      pTheRenderer->DrawNumberAt(pos, DynamicObjects::GetInstance()->GetBot(0, i).domTarget);
      pos.set(240.0f, i*50.0f + 100.0f);
      pTheRenderer->DrawNumberAt(pos, DynamicObjects::GetInstance()->GetBot(0, i).botTarget);
      pos.set(280.0f, i*50.0f + 100.0f);
      pTheRenderer->DrawNumberAt(pos, DynamicObjects::GetInstance()->GetBot(0, i).GetAmmo());

    }
  }


	return answer;
}

ErrorType Game::Update()
{
	m_timer.mark();
	// Cap the frame time
	if(m_timer.m_fFrameTime>0.05f)
		m_timer.m_fFrameTime=0.05f;
	ErrorType answer=SUCCESS;

	Renderer* pTheRenderer = Renderer::GetInstance();


	// If not paused or minimised
	if(m_State == RUNNING)
	// Update Dynamic objects
	{
    DynamicObjects::GetInstance()->Update(m_timer.m_fFrameTime);
    if (Networking::GetInstance()->isServer)
    {
      MyInputs* pInput = MyInputs::GetInstance();
      pInput->SampleKeyboard();
      if (pInput->KeyPressed(DIK_C))
      {
        Networking::Release();
        Networking::GetInstance()->isServer = false;
        //Networking::GetInstance()->WSASetup();
        Networking::GetInstance()->ConnectToServer();
      }
      else
      {
        //DynamicObjects::GetInstance()->Update(m_timer.m_fFrameTime);
        Networking::GetInstance()->ConnectToClients();
        Networking::GetInstance()->Send();
      }
    }
    //else
    //{
      //if (Networking::GetInstance()->Recieve())
        //memcpy(DynamicObjects::GetInstance(), &Networking::GetInstance()->data, sizeof(DynamicObjects));

    //}
		
	}

	// Render
	if(pTheRenderer->GetIsValid()==false)
	{
		//m_State = MINIMISED;			// Or some other reason why invalid
	}
	else
	{
		if(m_State == MINIMISED)
			m_State = PAUSED;			// No longer minimised. Return to pause state.
		if(StaticMap::GetInstance()->Render()==FAILURE)
		{
			ErrorLogger::Writeln(L"Static map failed to render. Terminating.");
			answer = FAILURE;
		}
		if(DynamicObjects::GetInstance()->Render()==FAILURE)
		{
			ErrorLogger::Writeln(L"Dynamic objects failed to render. Terminating.");
			answer = FAILURE;
		}

		if(pTheRenderer->DrawFX(m_timer.m_fFrameTime)==FAILURE)
		{
			ErrorLogger::Writeln(L"Render failed to draw FX.");
			// Continue. Non-critical error.
		}
		if(pTheRenderer->EndScene()==FAILURE)
		{
			ErrorLogger::Writeln(L"Render failed to end scene. Terminating.");
			answer = FAILURE;
		}
	}

	if(RunInterface()==FAILURE)
	{
		ErrorLogger::Writeln(L"Interface reports failure.");
		// Non-critical error
	}

  if (Networking::GetInstance()->isServer)
   Networking::GetInstance()->Send();

	return answer;
}

ErrorType Game::End()
{
  Networking::Release();
	Renderer::Release();
  StaticMap::Release();
  Attack::Release();
  Capture::Release();
  Defend::Release();
  Reload::Release();
  Pathfinder::Release();
  DynamicObjects::Release();

	return SUCCESS;
}


ErrorType Game::InitialiseScript()
{
	// Hard code for now
	StaticMap* pMap = StaticMap::GetInstance();

	// Add some rectangles
	Rectangle2D r;
	r.PlaceAt(1000, -1000, 700, -700);
	pMap->AddBlock(r);
  r.PlaceAt(-700, 700, -1000, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(1000, -700, 900, 0);
  pMap->AddBlock(r);
  r.PlaceAt(-900, 0, -1000, 700);
  pMap->AddBlock(r);

  r.PlaceAt(-700, -1000, -1400, -700);
  pMap->AddBlock(r);
  r.PlaceAt(1400, 700, 700, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(300, -1000, -300, -850);
  pMap->AddBlock(r);
  r.PlaceAt(300, 850, -300, 1000);
  pMap->AddBlock(r);

  r.PlaceAt(500, -450, 200, -150);
  pMap->AddBlock(r);
  r.PlaceAt(-200, 150, -500, 450);
  pMap->AddBlock(r);

  r.PlaceAt(-200, -500, -750, -400);
  pMap->AddBlock(r);
  r.PlaceAt(750, 400, 200, 500);
  pMap->AddBlock(r);

  r.PlaceAt(700, -200, 600, 200);
  pMap->AddBlock(r);
  r.PlaceAt(-600, -200, -700, 200);
  pMap->AddBlock(r);


	// Add two spawn points
	pMap->AddSpawnPoint(Vector2D(-1250, 0));
	pMap->AddSpawnPoint(Vector2D(1250, 00));

  // Add four supply points
  pMap->AddSupplyPoint(Vector2D(900, 500));
  pMap->AddSupplyPoint(Vector2D(900, -500));
  pMap->AddSupplyPoint(Vector2D(-900, 500));
  pMap->AddSupplyPoint(Vector2D(-900, -500));

	// Place the domination points
	DynamicObjects* pDynObjects = DynamicObjects::GetInstance();

	pDynObjects->PlaceDominationPoint(Vector2D( -200,-850 ));
	pDynObjects->PlaceDominationPoint(Vector2D( 0,0 ));
	pDynObjects->PlaceDominationPoint(Vector2D( 200,850 ));

	pDynObjects->Initialise();
  Pathfinder::GetInstance()->GenerateNodes();


  if (!Networking::GetInstance()->ServerSetup())
    ErrorLogger::Writeln(L"Error: ServerSetup() Failure");
  else
    Networking::GetInstance()->CreateThread();

	return SUCCESS;
}