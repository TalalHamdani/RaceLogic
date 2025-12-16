#ifndef RACE_GRAPH_H
#define RACE_GRAPH_H

#include "BSTMap.h"
#include "LinkedList.h"
#include "Queue.h"
#include <limits>


// Replaces std::pair logic
struct PQNode {
  float cost;
  int node;

  // Overload operators for PriorityQueue comparison
  bool operator<(const PQNode &other) const { return cost < other.cost; }
  bool operator>(const PQNode &other) const { return cost > other.cost; }
  bool operator==(const PQNode &other) const { return cost == other.cost; }
};

struct Edge {
  int to;
  float baseTime;   // Seconds
  float gripFactor; // 1.0 = Standard, Higher = More affected by rain
};

class RaceGraph {
private:
  int numNodes;
  // Map<NodeID, List<Edge>>
  BSTMap<int, LinkedList<Edge>> adj;

public:
  RaceGraph(int nodes) : numNodes(nodes) {}

  void addEdge(int u, int v, float time, float grip) {
    adj[u].push_back({v, time, grip});
  }

  // Dijkstra's Algorithm to find shortest time (Ideal Lap)
  // Weather: 0.0 (Dry) to 1.0 (Heavy Rain)
  float calculateIdealLapTime(int startNode, int endNode, float weather) {
    BSTMap<int, float> dist;
    for (int i = 0; i < numNodes; ++i)
      dist[i] = std::numeric_limits<float>::max();

    PriorityQueue<PQNode> pq;

    dist[startNode] = 0.0f;
    pq.push({0.0f, startNode});

    while (!pq.empty()) {
      PQNode top = pq.top();
      pq.pop();

      float d = top.cost;
      int u = top.node;

      if (d > dist[u])
        continue;
      if (u == endNode)
        return d;

      // Iterate through LinkedList using iterator
      // Note: adj[u] returns reference to LinkedList
      for (auto &edge : adj[u]) {
        // Dynamic Weight Calculation
        // Weight = BaseTime * (1.0 + (WeatherFloat * GripPenalty))
        float weight = edge.baseTime * (1.0f + (weather * edge.gripFactor));

        if (dist[u] + weight < dist[edge.to]) {
          dist[edge.to] = dist[u] + weight;
          pq.push({dist[edge.to], edge.to});
        }
      }
    }
    return -1.0f; // Unreachable
  }
};

#endif // RACE_GRAPH_H
