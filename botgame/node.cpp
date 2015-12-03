#include "Node.h"

Node::~Node()
{
}

void Node::SetEdge(Edge e)
{
	edgeList.push_back(e);
}

double Node::GetEdgeCost(int toIndex)
{
	vector<Edge>::iterator it;
	for(it = edgeList.begin(); it != edgeList.end(); it++)
	{
		if(it->toIndex == toIndex)
			return it->cost;
	}
	return -1;
}

//Get/Set Functions
Vector2D Node::GetPosition()
{
	return position;
}

int Node::GetIndexNumber()
{
	return indexNumber;
}

void Node::SetPosition(Vector2D pos)
{
	position = pos;
}

void Node::SetIndexNumber(int ind)
{
	indexNumber = ind;
}

void Node::SetParent(Node* par)
{
	parent = par; 
}

Node* Node::GetParent()
{
	return parent;
}

void Node::Setfscore(double newf)
{
	f_score = newf;
}

void Node::Setgscore(double newg)
{
	g_score = newg;
}

void Node::Sethscore(double newh)
{
	h_score = newh;
}

double Node::Getfscore()
{
	return f_score;
}

double Node::Getgscore()
{
	return g_score;
}

double Node::Gethscore()
{
	return h_score;
}

void Node::Setstate(NodeState newState)
{
	nodeState = newState;
}

NodeState Node::Getstate()
{
	return nodeState;
}

vector<Edge> Node::Getedges()
{
	return edgeList;
}