#include "pathfinder.h"
#include "staticmap.h"
#include "Renderer.h"

Pathfinder* Pathfinder::pInst = NULL;

Pathfinder::Pathfinder()
{
	numNodes = 0;
}

Pathfinder::~Pathfinder()
{

}

void Pathfinder::Release()
{
	if(pInst)
	{
		delete pInst;
		pInst = NULL;
	}
}

Pathfinder* Pathfinder::GetInstance()
{
	if(!pInst)
		pInst = new Pathfinder;

	return pInst;
}

void Pathfinder::GenerateNodes()
{
	Rectangle2D rectangle;
	rectangle.PlaceAt(Vector2D(-300, -300), Vector2D(1300, 1300));
	ProcessRectangle(rectangle);
}

void Pathfinder::AddNode(Vector2D newNode)
{
	NodeGraph[numNodes].SetPosition(newNode);
	NodeGraph[numNodes].SetIndexNumber(numNodes);
	numNodes++;
}

void Pathfinder::ProcessRectangle(Rectangle2D rect)
{
	if(rect.GetArea() > 80)
	{
		double height, width;
		height = rect.GetBottomRight().YValue - rect.GetTopRight().YValue;
		width = rect.GetTopRight().XValue - rect.GetTopLeft().XValue;
		if(StaticMap::GetInstance()->IsInsideBlock(rect))
		{
			Rectangle2D rect1;
			rect1.PlaceAt(rect.GetTopLeft(), Vector2D(rect.GetTopLeft().XValue + (width/2), rect.GetTopLeft().YValue + (height/2)));

			Rectangle2D rect2;
			rect2.PlaceAt(Vector2D(rect.GetTopLeft().XValue + (width/2), rect.GetTopLeft().YValue), Vector2D(rect.GetTopRight().XValue, rect.GetTopRight().YValue + (height/2)));

			Rectangle2D rect3;
			rect3.PlaceAt(Vector2D(rect.GetTopLeft().XValue, rect.GetTopLeft().YValue + (height/2)), Vector2D(rect.GetTopLeft().XValue + (width/2), rect.GetTopLeft().YValue + height));

			Rectangle2D rect4;
			rect4.PlaceAt(Vector2D(rect.GetTopLeft().XValue + (width/2), rect.GetTopLeft().YValue + (height/2)), rect.GetBottomRight());

			ProcessRectangle(rect1);
			ProcessRectangle(rect2);
			ProcessRectangle(rect3);
			ProcessRectangle(rect4);	
		}
		else
		{
			AddNode(Vector2D(rect.GetTopLeft().XValue + (width/2), rect.GetTopLeft().YValue + (height/2)));
		}
	}
}

vector<Vector2D> Pathfinder::GeneratePath(Vector2D start, Vector2D goal)
{
	int startInd = FindNode(start);
	int endInd = FindNode(goal);
	int numOpen = 0;
	
	for (int i = 0; i < numNodes; i++)
	{
		NodeGraph[i].Setgscore(0.0);
		NodeGraph[i].Setfscore(0.0);
		NodeGraph[i].Sethscore(0.0);
		NodeGraph[i].Setstate(UNUSED);
		NodeGraph[i].SetParent(nullptr);
	}
	
	NodeGraph[startInd].Setstate(OPEN);
	NodeGraph[startInd].Setgscore(0);
	NodeGraph[startInd].Setfscore(NodeGraph[startInd].Getgscore() + (NodeGraph[startInd].GetPosition() - NodeGraph[endInd].GetPosition()).magnitude());
	numOpen++;

	while(numOpen > 0)
	{
		double lowestfscore = 100000000;
		int indexnumber = NodeGraph[startInd].GetIndexNumber();
		for(int i = 0; i < numNodes; i++)
		{
			if(NodeGraph[i].Getfscore() < lowestfscore && NodeGraph[i].Getfscore() > 0.0 && NodeGraph[i].Getstate() == OPEN)
			{
				lowestfscore = NodeGraph[i].Getfscore();
				indexnumber = NodeGraph[i].GetIndexNumber();
			}
		}

		currentPtr = &NodeGraph[indexnumber];

		if(currentPtr->GetIndexNumber() == endInd)
		{
			return GetPath(currentPtr);
		}
		currentPtr->Setstate(CLOSED);
		numOpen--;

		vector<Edge>::iterator it;
		vector<Edge> tempEdges;
		tempEdges = currentPtr->Getedges();
		for(it = tempEdges.begin(); it != tempEdges.end(); it++)
		{
			double newgscore = 0;
			if(NodeGraph[it->toIndex].Getstate() != CLOSED)
			{
				newgscore = currentPtr->Getgscore() + it->cost;
				if(newgscore < NodeGraph[it->toIndex].Getgscore() || NodeGraph[it->toIndex].Getstate() != OPEN)
				{
					NodeGraph[it->toIndex].SetParent(currentPtr);
					NodeGraph[it->toIndex].Setgscore(newgscore);
					double newfscore = 0;
					newfscore = NodeGraph[it->toIndex].Getgscore() + (NodeGraph[it->toIndex].GetPosition() - NodeGraph[endInd].GetPosition()).magnitude();
					NodeGraph[it->toIndex].Setfscore(newfscore);
					if(NodeGraph[it->toIndex].Getstate() != OPEN)
					{
						NodeGraph[it->toIndex].Setstate(OPEN);
						numOpen++;
					}
				}
			}
		}
	}
	return GetPath(currentPtr);
}

vector<Vector2D> Pathfinder::GetPath(Node* i)
{
	vector<Vector2D> returnPath;
	returnPath.push_back(i->GetPosition());
	Node* loopPtr = i;
	while(loopPtr->GetParent() != nullptr)
	{
		loopPtr = loopPtr->GetParent();
		returnPath.push_back(loopPtr->GetPosition());
	}
	return returnPath;
}

int Pathfinder::FindNode(Vector2D pos)
{
	double lowestDistance = 100000;
	int lowestIndex = 0;
	for (int i = 0; i < numNodes; i++)
	{
		if((NodeGraph[i].GetPosition() - pos).magnitude() < lowestDistance)
		{
			lowestDistance = (NodeGraph[i].GetPosition() - pos).magnitude();
			lowestIndex = NodeGraph[i].GetIndexNumber();
		}
	}
	return lowestIndex;
}

void Pathfinder::GenerateEdgeLists()
{
	for(int i = 0; i < numNodes; i++)
	{
		for(int f = 0; f < numNodes; f++)
		{
			if (StaticMap::GetInstance()->IsLineOfSight(NodeGraph[i].GetPosition(), NodeGraph[f].GetPosition()))
			{
				Edge newEdge;
				newEdge.fromIndex = NodeGraph[i].GetIndexNumber();
				newEdge.toIndex = NodeGraph[f].GetIndexNumber();
				newEdge.cost = (NodeGraph[i].GetPosition() - NodeGraph[f].GetPosition()).magnitude();
				NodeGraph[i].SetEdge(newEdge);
			}
		}
	}
}

void Pathfinder::Debug()
{
	for(int i = 0; i < numNodes; i++)
	{
		Renderer::GetInstance()->DrawDot(NodeGraph[i].GetPosition());
	}
}