#include "Pathfinder.h"
#include "staticmap.h"
#include "Renderer.h"
#include "dynamicObjects.h"


Pathfinder* Pathfinder::pInstance = nullptr;


Pathfinder::Pathfinder()
{
  //numOfNodes = 0;
} // Pathfinder()


Pathfinder* Pathfinder::GetInstance()
{
  if (!pInstance)
    pInstance = new Pathfinder;

  return pInstance;
} // GetInstance()


void Pathfinder::GenerateNodes()
{
  Rectangle2D rect;
  rect.PlaceAt(Vector2D(-1300, -1300), Vector2D(1300, 1300));
  GetRect(rect);
} // GenerateNodes()


void Pathfinder::PlaceNode(Vector2D nodePos)
{
  Node aNode;
  aNode.position = nodePos;

  if (nodeList.size())
    aNode.nodeID = nodeList.size();
  //else
    //aNode.nodeID = 0;

  nodeList.push_back(aNode);
} // PlaceNode()


void Pathfinder::GetRect(Rectangle2D rect)
{
  if (rect.GetArea() > 120)
  {
    float height = rect.GetBottomRight().YValue - rect.GetTopRight().YValue;
    float width = rect.GetTopRight().XValue - rect.GetTopLeft().XValue;

    if (StaticMap::GetInstance()->IsInsideBlock(rect))
    {
      Rectangle2D rect1;
      rect1.PlaceAt(rect.GetTopLeft(), Vector2D(rect.GetTopLeft().XValue + 
                    (width / 2), rect.GetTopLeft().YValue + (height / 2)));

      Rectangle2D rect2;
      rect2.PlaceAt(Vector2D(rect.GetTopLeft().XValue + (width / 2), 
                    rect.GetTopLeft().YValue), 
                    Vector2D(rect.GetTopRight().XValue,
                    rect.GetTopRight().YValue + (height / 2)));

      Rectangle2D rect3;
      rect3.PlaceAt(Vector2D(rect.GetTopLeft().XValue, rect.GetTopLeft().YValue
                    + (height / 2)), Vector2D(rect.GetTopLeft().XValue + 
                    (width / 2), rect.GetTopLeft().YValue + height));

      Rectangle2D rect4;
      rect4.PlaceAt(Vector2D(rect.GetTopLeft().XValue + (width / 2), 
                    rect.GetTopLeft().YValue + (height / 2)),
                    rect.GetBottomRight());

      GetRect(rect1);
      GetRect(rect2);
      GetRect(rect3);
      GetRect(rect4);
    }
    else
    {
      Vector2D nodePos(rect.GetTopLeft().XValue + (width / 2),
                       rect.GetTopLeft().YValue + (height / 2));
      PlaceNode(nodePos);
    }
  }
} // GetRect()

int Pathfinder::GetClosestNode(Vector2D position)
{
  float lowDist = FLT_MAX;
  int lowID = 0;

  for (Node& i : nodeList)
  {
    if ((i.position - position).magnitude() < lowDist)
    {
      lowDist = ((i.position - position).magnitude());
      lowID = i.nodeID;
    }
  }

  return lowID;
} // GetClosestNode()

std::vector<Vector2D> Pathfinder::GeneratePath(Vector2D start, Vector2D goal)
    {
  int startID = GetClosestNode(start);
  int endID = GetClosestNode(goal);
  int openNum = 0;
  Node* currentNode = nullptr;

  for (Node& i : nodeList)
  {
    i.gScore = 0;
    i.fScore = 0;
    i.currentState = UNUSED;
    i.from = nullptr;
  }
  
  nodeList[startID].currentState = OPEN;
  nodeList[startID].fScore = (nodeList[startID].position - 
                              nodeList[endID].position).magnitude();
  openNum++;

  while (openNum)
  {
    float lowFScore = FLT_MAX;
    int index = startID;

    for (Node& i : nodeList)
    {
      if (i.fScore < lowFScore && i.currentState == OPEN && i.fScore > 0)
      {
        lowFScore = i.fScore;
        index = i.nodeID;
      }
    }

    currentNode = &nodeList[index];
    if (currentNode->nodeID == endID)
    {
      return GetPath(currentNode);
    }
    else
    {
      currentNode->currentState = CLOSED;
      openNum--;

      for (Edge& i : currentNode->edges)
      {
        float gScore = 0;
        float fScore = 0;
        if (nodeList[i.edgeTo].currentState != CLOSED)
        {
          gScore = currentNode->gScore + i.cost;
          if (gScore < nodeList[i.edgeTo].gScore ||
            nodeList[i.edgeTo].currentState != OPEN)
          {
            nodeList[i.edgeTo].from = currentNode;
            nodeList[i.edgeTo].gScore = gScore;

            nodeList[i.edgeTo].fScore = nodeList[i.edgeTo].gScore + 
                                        (nodeList[i.edgeTo].position - 
                                        nodeList[endID].position).magnitude();

            if (nodeList[i.edgeTo].currentState != OPEN)
            {
              nodeList[i.edgeTo].currentState = OPEN;
              openNum++;
            }
          }
        }
      }
    }
  } // while (openNum)

} // GeneratePath()


std::vector<Vector2D> Pathfinder::GetPath(Node* endNode)
{
  std::vector<Vector2D> path;
  Node* pNode = endNode;
  path.push_back(pNode->position);

  while (pNode->from)
  {
    pNode = pNode->from;
    path.push_back(pNode->position);
  }

  return path;
} // GetPath()


void Pathfinder::GenerateEdges()
{
  StaticMap* pStaticMap = StaticMap::GetInstance();

  for (Node& i : nodeList)
  {
    for (Node& j : nodeList)
    {
      if (pStaticMap->IsLineOfSight(i.position, j.position))
      {
        Edge anEdge;
        anEdge.edgeTo = j.nodeID;
        anEdge.cost = (i.position - j.position).magnitude();
        i.edges.push_back(anEdge);
      }
    }
  }
} // GenerateEdges()


void Pathfinder::Release()
{
  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }
} // Release()


void Pathfinder::PathDebug(Vector2D start, Vector2D finish)
{
  std::vector<Vector2D> testPath;
  static bool pathExists = false;
  int i = 0;

  if (!pathExists)
  {
    testPath = GeneratePath(start, finish);
  }
  
  while (i <= testPath.size())
  { 
    
    Renderer::GetInstance()->DrawLine(testPath[i], testPath[i + 1]);
    i++;
  }
  /*for (Node i : nodeList)
  {
    Renderer::GetInstance()->DrawDot(i.position);
  }*/

} // PathDebug()
