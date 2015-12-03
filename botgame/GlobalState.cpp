#include "GlobalState.h"
#include "Attack.h"
#include "Capture.h"
#include "Defend.h"
#include "Hunt.h"
#include "Tumbleweed.h"

GlobalState* GlobalState::pInst = NULL;

GlobalState::GlobalState()
{
}

GlobalState::~GlobalState()
{
}

void GlobalState::Enter(Bot* pBot)
{
}

void GlobalState::Execute(Bot* pBot)
{
	pBot->ReturnBehaviours()->RefreshBot(pBot->GetLocation(), pBot->GetVelocity());

	pEnemy = IsAimingAt(pBot);
	if(pEnemy != pBot && pBot->GetCurrentState() == Defend::GetInstance())
	{
		pBot->SetTarget(pEnemy->GetOwnTeamNumber(), pEnemy->GetOwnNumber());
		pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), pEnemy->GetLocation(), pEnemy->GetVelocity());
		pBot->ChangeState(Hunt::GetInstance());
	}
}

void GlobalState::Exit(Bot* pBot)
{
}

Bot* GlobalState::IsAimingAt(Bot* pBot)
{
	for(int i = 0; i < NUMBOTSPERTEAM; i++)
	{
		if(DynamicObjects::GetInstance()->GetBot(1, i).GetTargetBot() == pBot->GetOwnNumber() &&
			DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() == true)
		{
			return &DynamicObjects::GetInstance()->GetBot(1, i);
		}
	}
	return pBot;
}

GlobalState* GlobalState::GetInstance()
{
	if(!pInst)
		pInst = new GlobalState;

	return pInst;
}

void GlobalState::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

