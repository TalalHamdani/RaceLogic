#pragma once

#include <vector>
#include <limits>
#include <queue>
#include <functional>
#include <unordered_map>

struct Edge {
    int to;
    double weight;
    
    Edge(int t, double w) : to(t), weight(w) {}
};

class WeightedGraph {
public:
    WeightedGraph(int num_vertices) : num_vertices_(num_vertices) {
        adj_list_.resize(num_vertices);
    }

    void addEdge(int from, int to, double weight) {
        adj_list_[from].push_back(Edge(to, weight));
    }

    void addBidirectionalEdge(int from, int to, double weight) {
        addEdge(from, to, weight);
        addEdge(to, from, weight);
    }

    const std::vector<Edge>& getEdges(int vertex) const {
        return adj_list_[vertex];
    }

    int getNumVertices() const {
        return num_vertices_;
    }

    // Dijkstra's algorithm to find shortest path
    struct DijkstraResult {
        std::vector<double> distances;
        std::vector<int> previous;
        double total_distance;
        std::vector<int> path;
    };

    DijkstraResult dijkstra(int start, int end) const {
        const double INF = std::numeric_limits<double>::infinity();
        DijkstraResult result;
        result.distances.resize(num_vertices_, INF);
        result.previous.resize(num_vertices_, -1);
        result.distances[start] = 0.0;

        using PQElement = std::pair<double, int>; // (distance, vertex)
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
        pq.push({0.0, start});

        while (!pq.empty()) {
            auto [dist, u] = pq.top();
            pq.pop();

            if (dist > result.distances[u]) {
                continue;
            }

            for (const auto& edge : adj_list_[u]) {
                int v = edge.to;
                double weight = edge.weight;
                double new_dist = result.distances[u] + weight;

                if (new_dist < result.distances[v]) {
                    result.distances[v] = new_dist;
                    result.previous[v] = u;
                    pq.push({new_dist, v});
                }
            }
        }

        // Reconstruct path
        result.total_distance = result.distances[end];
        if (result.total_distance != INF) {
            int current = end;
            while (current != -1) {
                result.path.push_back(current);
                current = result.previous[current];
            }
            std::reverse(result.path.begin(), result.path.end());
        }

        return result;
    }

private:
    int num_vertices_;
    std::vector<std::vector<Edge>> adj_list_;
};
