#include "Pathfinder.h"
#include "staticmap.h"              // For IsInsideBlock()
#include "Renderer.h"               // DEBUG DRAWING


// Initialise to nullptr
Pathfinder* Pathfinder::pInstance = nullptr;


Pathfinder::Pathfinder()
{

} // Pathfinder()


Pathfinder* Pathfinder::GetInstance()
{ // Returns the instance of the class, if none currently exists, creates one

  if (!pInstance)
    pInstance = new Pathfinder;

  return pInstance;

} // GetInstance()


void Pathfinder::GenerateNodes()
{ // Starts the node generation process by calling GetRect covering the map,
  // generates edges afterwards

  Rectangle2D rect;
  rect.PlaceAt(Vector2D(-1300, -1300), Vector2D(1300, 1300));
  GetRect(rect);

  GenerateEdges();

} // GenerateNodes()


void Pathfinder::PlaceNode(Vector2D nodePos)
{ // Creates a node at the given position and adds the node to the list

  Node aNode;
  aNode.position = nodePos;

  aNode.nodeID = nodeList.size();

  nodeList.push_back(aNode);

} // PlaceNode()


void Pathfinder::GetRect(Rectangle2D rect)
{ // Recursive function that checks the area of a rect for any collisions. If
  // no collisions then calls PlaceNode otherwise split rect and try again

  if (rect.GetArea() > 1000)
  { // If the rect area is above predetermined value check for node placement

    // Work out the width and height of the rect to use incase of collision
    float height = rect.GetBottomRight().YValue - rect.GetTopRight().YValue;
    float width = rect.GetTopRight().XValue - rect.GetTopLeft().XValue;

    if (StaticMap::GetInstance()->IsInsideBlock(rect))
    { // Check for collision with block, if so make rect smaller for recursion

      // Make a new rect for next node placement attempt
      Rectangle2D rect1;
      rect1.PlaceAt(rect.GetTopLeft(), Vector2D(rect.GetTopLeft().XValue + 
                    (width / 2), rect.GetTopLeft().YValue + (height / 2)));

      // Make a new rect for next node placement attempt
      Rectangle2D rect2;
      rect2.PlaceAt(Vector2D(rect.GetTopLeft().XValue + (width / 2), 
                    rect.GetTopLeft().YValue), 
                    Vector2D(rect.GetTopRight().XValue,
                    rect.GetTopRight().YValue + (height / 2)));

      // Make a new rect for next node placement attempt
      Rectangle2D rect3;
      rect3.PlaceAt(Vector2D(rect.GetTopLeft().XValue, rect.GetTopLeft().YValue
                    + (height / 2)), Vector2D(rect.GetTopLeft().XValue + 
                    (width / 2), rect.GetTopLeft().YValue + height));

      // Make a new rect for next node placement attempt
      Rectangle2D rect4;
      rect4.PlaceAt(Vector2D(rect.GetTopLeft().XValue + (width / 2), 
                    rect.GetTopLeft().YValue + (height / 2)),
                    rect.GetBottomRight());

      // Try place new node at each new rect
      GetRect(rect1);
      GetRect(rect2);
      GetRect(rect3);
      GetRect(rect4);
    }
    else
    {
      // Create a Vector2D in centre of rect and call PlaceNode()
      Vector2D nodePos(rect.GetTopLeft().XValue + (width / 2),
                       rect.GetTopLeft().YValue + (height / 2));
      PlaceNode(nodePos);
    }

  } // If area > 1000

} // GetRect()

int Pathfinder::GetClosestNode(Vector2D position)
{ // Returns the ID of the closen node to the given position

  float lowDist = FLT_MAX;    // Holds lowest dist, init to max float
  int lowID = 0;              // Will hold the Id of closest node

  for (Node& i : nodeList)
  { // Cycle through entire node list

    if ((i.position - position).magnitude() < lowDist)
    { // if the distance to current node being check is lower than current min
      // store new ID and distance

      lowDist = ((i.position - position).magnitude());
      lowID = i.nodeID;

    }
  }

  return lowID;

} // GetClosestNode()

std::vector<Vector2D> Pathfinder::GeneratePath(Vector2D start, Vector2D goal)
{ // Generates a path from the starting position to the end position using A*
  // pathfinding

  int startID = GetClosestNode(start);  // Store nodeID of closest node to start
  int endID = GetClosestNode(goal);     // Store nodeID of closest node to end
  int openNum = 0;                      // Store number of nodes that are open
  Node* currentNode = nullptr;          // Store pointer to current node

  for (Node& i : nodeList)
  { // Initialise the node list elements for A* to null/0
    i.gScore = 0;
    i.fScore = 0;
    i.currentState = UNUSED;
    i.from = nullptr;
  }
  
  // Change the starting node to open and work out the first f score
  nodeList[startID].currentState = OPEN;
  nodeList[startID].fScore = (nodeList[startID].position - 
                              nodeList[endID].position).magnitude();

  openNum++; // Increment the number of open nodes

  while (openNum)
  { // While there are open nodes, complete A* process

    float lowFScore = FLT_MAX;    // Holds lowest dist, init to max float
    int index = startID;          // Will hold the Id of closest node

    for (Node& i : nodeList)
    { // Loops through nodeList looking for lowest f score

      if (i.fScore < lowFScore && i.currentState == OPEN && i.fScore > 0)
      {
        lowFScore = i.fScore;
        index = i.nodeID;
      }

    }

    currentNode = &nodeList[index];   // Define current node as lowest fscore

    if (currentNode->nodeID == endID)
    { // Return path if we have found the end
      return GetPath(currentNode);
    }
    else
    { // If not the end, change node to closed then look at edges to next nodes
      currentNode->currentState = CLOSED;
      openNum--;

      for (Edge& i : currentNode->edges)
      { // Loops through edges checking for next node to go to

        float gScore = 0;
        float fScore = 0;
        if (nodeList[i.edgeTo].currentState != CLOSED)
        { // If node is not closed, store new g score and check rest of gscores

          gScore = currentNode->gScore + i.cost;
          if (gScore < nodeList[i.edgeTo].gScore ||
            nodeList[i.edgeTo].currentState != OPEN)
          { // If gscore is lower or state is not open then store gscore + node

            nodeList[i.edgeTo].from = currentNode;
            nodeList[i.edgeTo].gScore = gScore;

            // Work out new fscore
            nodeList[i.edgeTo].fScore = nodeList[i.edgeTo].gScore + 
                                        (nodeList[i.edgeTo].position - 
                                        nodeList[endID].position).magnitude();

            if (nodeList[i.edgeTo].currentState != OPEN)
            { // if the edge to node is not open, open node and increment
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
{ // Takes the given node and traces the route back to the starting node using
  // the from element of the node structs, returning a vector full of nodes

  std::vector<Vector2D> path;         // Holds the path
  Node* pNode = endNode;              // Current node to work with
  path.push_back(pNode->position);    // Push current node on to the path

  while (pNode->from)
  { // While there is still a node to be traced, push the node onto the list
    // and trace back another node

    pNode = pNode->from;
    path.push_back(pNode->position);

  }

  return path;

} // GetPath()


void Pathfinder::GenerateEdges()
{ // Generates the line of sight list for every node and stores within the node

  // Create pointer for use in loops
  StaticMap* pStaticMap = StaticMap::GetInstance();

  for (Node& i : nodeList)
  {
    for (Node& j : nodeList)
    {
      if (pStaticMap->IsLineOfSight(i.position, j.position))
      { // If there is a line of sight, create and store the edge
        Edge anEdge;
        anEdge.edgeTo = j.nodeID;
        anEdge.cost = (i.position - j.position).magnitude();
        i.edges.push_back(anEdge);
      }
    }
  }

} // GenerateEdges()


void Pathfinder::Release()
{ // If pInstance is valid, delete and define as null

  if (pInstance)
  {
    delete pInstance;
    pInstance = nullptr;
  }

} // Release()


void Pathfinder::PathDebug(std::vector<Vector2D> drawPath)
{ // DELETE BEFORE HAND IN
  int i = 0;

  if (drawPath.size() > 0)
  {
    while (i < drawPath.size() - 1)
    {

      Renderer::GetInstance()->DrawLine(drawPath[i], drawPath[i + 1]);
      i++;
    }
  }

  /*for (Node i : nodeList)
  {
    Renderer::GetInstance()->DrawDot(i.position);
  }*/

} // PathDebug()
