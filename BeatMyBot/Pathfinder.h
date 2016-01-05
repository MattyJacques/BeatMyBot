// Title        : Pathfinder.h
// Purpose      : A* pathfinding


#pragma once


#include "Shapes.h"   // Rectangle and Circle objects
#include <vector>     // Vector definition for list of nodes etc


// For node tagging while generating path
enum state { UNUSED, OPEN, CLOSED };


// Holds a line of sight from one node to another along with distance
// for travel to that node from containing node
struct Edge
{ 
  int edgeTo;
  float cost;
};


// Holds information on each node needed for A*
struct Node
{
  int nodeID;                 // Holds a unique ID for the node
  Vector2D position;          // Position of node
  float gScore;               // Holds travel cost from start node to this
  float fScore;               // Holds potential travel cost
  Node* from;                 // Holds pointer to which node we came from
  state currentState;         // Current state of the node
  std::vector<Edge> edges;    // Vector full of line of sights to other nodes
};


class Pathfinder
{
private:
  static Pathfinder* pInstance; // Holds the current instance of the class
  std::vector<Node> nodeList;   // Vector containing all nodes on map

  // Private constructor to avoid unauth access
  Pathfinder();

  // Creates a node at the given position and adds the node to the list
  void PlaceNode(Vector2D nodePos);

  // Recursive function that checks the area of a rect for any collisions. If
  // no collisions then calls PlaceNode otherwise split rect and try again
  void GetRect(Rectangle2D rect);

  // Returns the ID of the closen node to the given position
  int GetClosestNode(Vector2D position);

  // Generates the line of sight list for every node and stores within the node
  void GenerateEdges();

  // Takes the given node and traces the route back to the starting node using
  // the from element of the node structs, returning a vector full of nodes
  std::vector<Vector2D> GetPath(Node* endNode);

public:
  // Returns the instance of the class, if none currently exists, creates one
  static Pathfinder* GetInstance();

  // Starts the node generation process by calling GetRect covering the map,
  // generates edges afterwards
  void GenerateNodes();

  // Generates a path from the starting position to the end position using A*
  // pathfinding
  std::vector<Vector2D> Pathfinder::GeneratePath(Vector2D start, 
                                                 Vector2D goal);

  // If pInstance is valid, delete and define as null
  static void Release();

  // Various debug outputs
  void PathDebug(std::vector<Vector2D> drawPath);

};

