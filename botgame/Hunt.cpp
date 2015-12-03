#include "Attack.h"
#include "Capture.h"
#include "Defend.h"
#include "Hunt.h"
#include "Tumbleweed.h"

Hunt* Hunt::pInst = NULL;

Hunt::Hunt()
{

}

Hunt::~Hunt()
{

}

void Hunt::Enter(Bot* pBot)
{
	targTeam = pBot->GetTargetTeam();
	targBot = pBot->GetTargetBot();
	pEnemy = &DynamicObjects::GetInstance()->GetBot(targTeam, targBot);
	pBot->StopAiming();
	pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), pEnemy->GetLocation(), pEnemy->GetVelocity());
	pBot->ReturnBehaviours()->FindPath();
	pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,0,1,1);
}

void Hunt::Execute(Bot* pBot)
{
	pEnemy = &DynamicObjects::GetInstance()->GetBot(targTeam, targBot);
	pBot->ReturnBehaviours()->RefreshBot(pBot->GetLocation(), pBot->GetVelocity());
	pBot->StopAiming();
	if(StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), pEnemy->GetLocation()))
	{	
		pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,1,1,0);
		if((pBot->GetLocation() - pEnemy->GetLocation()).magnitude() <= 300)
		{
			pBot->SetTarget(pEnemy->GetOwnTeamNumber(), pEnemy->GetOwnNumber());
			pBot->ChangeState(Attack::GetInstance());
		}
	}

	pBot->SetAcceleration(pBot->ReturnBehaviours()->ReturnAcceleration());

	if(pEnemy->IsAlive() == false)
	{
		pBot->ChangeToPrevState();
	}
}

void Hunt::Exit(Bot* pBot)
{

}

Hunt* Hunt::GetInstance()
{
	if(!pInst)
		pInst = new Hunt;

	return pInst;
}

void Hunt::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}
