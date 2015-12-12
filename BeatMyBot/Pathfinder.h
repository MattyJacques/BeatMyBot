#pragma once

#include "Shapes.h"
#include <vector>


enum state { UNUSED, OPEN, CLOSED };

struct Edge
{
  int edgeTo;
  float cost;
};


struct Node
{
  int nodeID;
  Vector2D position;
  float gScore;
  float fScore;
  Node* from;
  state currentState;
  std::vector<Edge> edges;
};


class Pathfinder
{

public:

  //int numOfNodes;
  static Pathfinder* pInstance;
  std::vector<Node> nodeList;

  Pathfinder();
  static Pathfinder* GetInstance();
  void GenerateNodes();
  void PlaceNode(Vector2D nodePos);
  void GetRect(Rectangle2D rect);
  std::vector<Vector2D> Pathfinder::GeneratePath(Vector2D start, 
                                                 Vector2D goal);
  int GetClosestNode(Vector2D position);
  std::vector<Vector2D> GetPath(Node* endNode);
  void GenerateEdges();
  static void Release();
  void PathDebug(std::vector<Vector2D> drawPath);

};

