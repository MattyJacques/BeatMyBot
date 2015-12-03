#include "Attack.h"
#include "Capture.h"
#include "Defend.h"
#include "Hunt.h"
#include "Tumbleweed.h"

Attack* Attack::pInst = NULL;

Attack::Attack()
{

}

Attack::~Attack()
{

}

void Attack::Enter(Bot* pBot)
{
	targTeam = pBot->GetTargetTeam();
	targBot = pBot->GetTargetBot();
	pEnemy = &DynamicObjects::GetInstance()->GetBot(targTeam, targBot);
	pBot->StopAiming();
	pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), pEnemy->GetLocation(), pEnemy->GetVelocity());
	pBot->ReturnBehaviours()->FindPath();
	pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,0,1,1);
}

void Attack::Execute(Bot* pBot)
{
	pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), pEnemy->GetLocation(), pEnemy->GetVelocity());
	pBot->StopAiming();
	if(StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), pEnemy->GetLocation()))
	{	
		pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,1,1,0);
		if((pBot->GetLocation() - pEnemy->GetLocation()).magnitude() <= 200)
		{
			pBot->SetTarget(pEnemy->GetOwnTeamNumber(), pEnemy->GetOwnNumber());
			if(pBot->GetAccuracy() >= 0.7)
				pBot->Shoot();
		}
	}

	pBot->SetAcceleration(pBot->ReturnBehaviours()->ReturnAcceleration());

	if(pEnemy->IsAlive() == false)
	{
		pBot->ChangeState(Tumbleweed::GetInstance());
	}
}

void Attack::Exit(Bot* pBot)
{

}

Attack* Attack::GetInstance()
{
	if(!pInst)
		pInst = new Attack;

	return pInst;
}

void Attack::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

