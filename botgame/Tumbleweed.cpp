#include "Tumbleweed.h"
#include "dynamicObjects.h"
#include "Attack.h"
#include "Capture.h"
#include "Defend.h"
#include "Hunt.h"

Tumbleweed* Tumbleweed::pInst = NULL;

Tumbleweed::Tumbleweed()
{

}

Tumbleweed::~Tumbleweed()
{

}

void Tumbleweed::Enter(Bot* pBot)
{
	//Send even bots to point 0, odd bots to point 1.
	//This is far too hacky. 
	if(pBot->GetOwnNumber()%2 == 0)
		pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(0).m_Location, Vector2D(0,0));
	else
		pBot->ReturnBehaviours()->SetUp(pBot->GetLocation(), pBot->GetLocation(), DynamicObjects::GetInstance()->GetDominationPoint(1).m_Location, Vector2D(0,0));

	//Find a path to the points and set wall avoid and Pathfind on. 
	pBot->ReturnBehaviours()->FindPath();
	pBot->ReturnBehaviours()->ConfigureBehaviours(0,0,0,0,1,1);
}

void Tumbleweed::Execute(Bot* pBot)
{
	pBot->ReturnBehaviours()->RefreshBot(pBot->GetLocation(), pBot->GetVelocity());

	pBot->SetAcceleration(pBot->ReturnBehaviours()->ReturnAcceleration());

	if(OwnedPoints(pBot) < 2)
	{
		pBot->ChangeState(Capture::GetInstance());
	}

	if(OwnedPoints(pBot) >= 2)
	{
		pBot->ChangeState(Defend::GetInstance());
	}
}

void Tumbleweed::Exit(Bot* pBot)
{

}

Tumbleweed* Tumbleweed::GetInstance()
{
	if(!pInst)
		pInst = new Tumbleweed;

	return pInst;
}

void Tumbleweed::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

DominationPoint* Tumbleweed::GetClosestDP(Bot* pBot)
{
	DominationPoint* closestDomPoint;
	double distance = 10000;
	for(int i = 0; i < NUMDOMINATIONPOINTS; i++)
	{
		if((pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude() < distance 
			&& DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber != pBot->GetOwnTeamNumber())
		{
			distance = (pBot->GetLocation() - DynamicObjects::GetInstance()->GetDominationPoint(i).m_Location).magnitude();
			closestDomPoint = &DynamicObjects::GetInstance()->GetDominationPoint(i);
		}
	}
	return closestDomPoint;
}

int Tumbleweed::OwnedPoints(Bot* pBot)
{
	int owned = 0;
	for(int i = 0; i < NUMDOMINATIONPOINTS; i++)
	{
		if(DynamicObjects::GetInstance()->GetDominationPoint(i).m_OwnerTeamNumber == pBot->GetOwnTeamNumber())
			owned++;
	}
	return owned;
}

