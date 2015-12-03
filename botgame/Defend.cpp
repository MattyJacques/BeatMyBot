#include "Defend.h"
#include "Capture.h"
#include "Attack.h"
#include "Hunt.h"
#include "Tumbleweed.h"

Defend* Defend::pInst = NULL;

Defend::Defend()
{

}

Defend::~Defend()
{

}

void Defend::Enter(Bot* pBot)
{
	pEnemy = NULL;
	closestClaimedDP = GetClosestClaimedDP(pBot);
	pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), closestClaimedDP.m_Location, closestClaimedDP.m_Location);
	pBot->ReturnBehaviours()->FindPath();
	pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,0,1,1);
}

void Defend::Execute(Bot* pBot)
{
	closestClaimedDP = GetClosestClaimedDP(pBot);
	pBot->ReturnBehaviours()->RefreshBot(pBot->GetLocation(), pBot->GetVelocity());
	pBot->SetAcceleration(pBot->ReturnBehaviours()->ReturnAcceleration());

	//If domination point is in sight.
	if(StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), closestClaimedDP.m_Location))
	{
		//Seek and defend.
		pBot->ReturnBehaviours()->ConfigureBehaviours(1,0,0,0,1,0);
	}

	pEnemy = IsThreateningEnemy();
	if(pEnemy)
	{
		pBot->SetTarget(pEnemy->GetOwnTeamNumber(), pEnemy->GetOwnNumber());
		pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), pEnemy->GetLocation(), pEnemy->GetVelocity());
		pBot->ChangeState(Attack::GetInstance());
	}

	if(OwnedPoints(pBot) < 2)
	{
		pBot->ChangeState(Capture::GetInstance());
	}
}

void Defend::Exit(Bot* pBot)
{

}

Defend* Defend::GetInstance()
{
	if(!pInst)
		pInst = new Defend;

	return pInst;
}

void Defend::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

DominationPoint Defend::GetClosestClaimedDP(Bot* pBot)
{
	DominationPoint closestDomPoint;
	double distance = 10000;
	for(int i = 0; i < NUMDOMINATIONPOINTS; i++)
	{
		if((pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude() < distance
			&& DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber == pBot->GetOwnTeamNumber())
		{
			distance = (pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude();
			closestDomPoint = DynamicObjects::GetInstance()->GetDominationPoint(i);
		}
	}
	return closestDomPoint;
}

Bot* Defend::IsThreateningEnemy()
{
	for(int i = 0; i < NUMBOTSPERTEAM; i++)
	{
		if((DynamicObjects::GetInstance()->GetBot(1, i).GetLocation() - closestClaimedDP.m_Location).magnitude() < (DOMINATIONRANGE * 12) &&
			DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() == true)
			return &DynamicObjects::GetInstance()->GetBot(1, i);
	}
	return NULL;
}

int Defend::OwnedPoints(Bot* pBot)
{
	int owned = 0;
	for(int i = 0; i < NUMDOMINATIONPOINTS; i++)
	{
		if(DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber == pBot->GetOwnTeamNumber())
			owned++;
	}
	return owned;
}