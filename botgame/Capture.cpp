#include "Capture.h"
#include "Attack.h"
#include "Defend.h"
#include "Hunt.h"
#include "Tumbleweed.h"

Capture* Capture::pInst = NULL;

Capture::Capture()
{

}

Capture::~Capture()
{

}

void Capture::Enter(Bot* pBot)
{
	pEnemy = NULL;
	closestDP = GetClosestDP(pBot);
	pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), closestDP.m_Location, Vector2D(0,0));
	pBot->ReturnBehaviours()->FindPath();
	pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,0,1,1);
}

void Capture::Execute(Bot* pBot)
{
	closestDP = GetClosestDP(pBot);
	pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), closestDP.m_Location, Vector2D(0,0));

	//If domination point is in sight.
	if(StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), closestDP.m_Location))
	{
		//Seek and capture.
		pBot->ReturnBehaviours()->ConfigureBehaviours(1,0,0,0,1,0);
	}

	pBot->SetAcceleration(pBot->ReturnBehaviours()->ReturnAcceleration());

	pEnemy = IsThreateningEnemy();
	if(pEnemy && StaticMap::GetInstance()->IsLineOfSight(pBot->GetLocation(), pEnemy->GetLocation()))
	{
		pBot->SetTarget(pEnemy->GetOwnTeamNumber(), pEnemy->GetOwnNumber());
		pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetVelocity(), pEnemy->GetLocation(), pEnemy->GetVelocity());
		pBot->ChangeState(Attack::GetInstance());
	}

	if(OwnedPoints(pBot) >= 2)
	{
		pBot->ChangeState(Defend::GetInstance());
	}

	if(OwnedPoints(pBot) < 2)
	{
		//This is bad but it didn't work with pointers and it's reasonably
		//unhacky compared to the alternatives I can think of. 
		DominationPoint	newClosestDP = GetClosestDP(pBot);
		if(newClosestDP.m_Location.XValue != closestDP.m_Location.XValue)
		{
			pBot->ChangeState(Tumbleweed::GetInstance());
		}
	}
}

void Capture::Exit(Bot* pBot)
{

}

Capture* Capture::GetInstance()
{
	if(!pInst)
		pInst = new Capture;

	return pInst;
}

void Capture::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

DominationPoint Capture::GetClosestDP(Bot* pBot)
{
	DominationPoint closestDomPoint;
	double distance = 10000;
	for(int i = 0; i < NUMDOMINATIONPOINTS; i++)
	{
		if((pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude() < distance
			&& DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber != pBot->GetOwnTeamNumber())
		{
			distance = (pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude();
			closestDomPoint = DynamicObjects::GetInstance()->GetDominationPoint(i);
		}
	}
	return closestDomPoint;
}

int Capture::OwnedPoints(Bot* pBot)
{
	int owned = 0;
	for(int i = 0; i < NUMDOMINATIONPOINTS; i++)
	{
		if(DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber == pBot->GetOwnTeamNumber())
			owned++;
	}
	return owned;
}

Bot* Capture::IsThreateningEnemy()
{
	for(int i = 0; i < NUMBOTSPERTEAM; i++)
	{
		if((DynamicObjects::GetInstance()->GetBot(1, i).GetLocation() - closestDP.m_Location).magnitude() < (DOMINATIONRANGE * 5) &&
			DynamicObjects::GetInstance()->GetBot(1, i).IsAlive() == true)
			return &DynamicObjects::GetInstance()->GetBot(1, i);
	}
	return NULL;
}
