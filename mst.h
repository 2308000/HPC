#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <queue>
#include <stdlib.h>
#include <omp.h>
#include <algorithm>
#include "graph.h"
#include "union_find.h"
#include "quick_sort.h"
#include <ctime>
#include <cassert>

using namespace graph;
using std::cout;

const int KRUSKAL_THRESHOLD = 2<<20;

namespace mst {
    std::vector<std::pair<GraphNode*, int>> prims_algorithm(Graph *g) {
        auto edge_comparator = [](std::pair<GraphNode*, int> a, std::pair<GraphNode*, int> b) { 
            return a.second > b.second; 
        };

        std::priority_queue<std::pair<GraphNode*, int>, std::vector<std::pair<GraphNode*, int>>, decltype(edge_comparator)> pq(edge_comparator);
        std::unordered_set<GraphNode*> visited;
        std::vector<std::pair<GraphNode*, int>> result;
        std::unordered_map<GraphNode*, std::vector<std::pair<GraphNode*, int>>> adj_list;

        for(auto edge: g->edges) {
            auto u = edge->u;
            auto v = edge->v;
            adj_list[u].push_back(std::make_pair(v, edge->cost));
            adj_list[v].push_back(std::make_pair(u, edge->cost));
        }

        auto source = g->nodes[0];
        while (visited.size() < g->num_nodes) {
            if (visited.find(source) == visited.end()) {
                visited.insert(source);
                for (auto edge : adj_list[source]) pq.push(edge);
                while (!pq.empty() && visited.find(pq.top().first) != visited.end()) pq.pop(); 

                if (!pq.empty()) {
                    result.push_back(std::make_pair(source, pq.top().second));
                    source = pq.top().first;
                    pq.pop();
                }
            }

            result.push_back(std::make_pair(source, 0));
        }
        
        return result;
    }

    std::vector<Edge*> kruskals_algorithm(Graph *g) {
        std::vector<Edge*> result;
        q_sort::seq_quick_sort(g->edges.begin(), g->edges.end());

        UnionFind uf(g);
        for (auto edge: g->edges) {
            GraphNode *set_u = uf.find(edge->u);
            GraphNode *set_v = uf.find(edge->v);

            if (set_u != set_v) {
                result.push_back(edge);
                uf.unify(set_u, set_v);
            }
        }

        return result;
    }

    std::vector<Edge*> par_kruskals_algorithm(Graph *g) {
        std::vector<Edge*> result;
        #pragma omp parallel
        {
            #pragma omp single nowait
            q_sort::par_quick_sort(g->edges.begin(), g->edges.end());
        }
        
        UnionFind uf(g);
        for (auto edge: g->edges) {
            GraphNode *set_u = uf.find(edge->u);
            GraphNode *set_v = uf.find(edge->v);

            if (set_u != set_v) {
                result.push_back(edge);
                uf.unify(set_u, set_v);
            }
        }

        return result;
    }

    std::vector<Edge*>::iterator filter(std::vector<Edge*>::iterator start, 
                                        std::vector<Edge*>::iterator end, 
                                        UnionFind& uf) {
        auto are_in_diff_components = [&uf](const Edge *edge) { 
            GraphNode *set_u = uf.find(edge->u);
            GraphNode *set_v = uf.find(edge->v);
            return uf.are_connected(set_u, set_v);
        };

        return std::partition(start, end, are_in_diff_components);
    }

    std::vector<Edge*> filter_kruskal_util(Graph *g,
                                            UnionFind& uf,
                                            std::vector<Edge*>::iterator start,
                                            std::vector<Edge*>::iterator end) {
        auto edge_count = std::distance(g->edges.begin(), g->edges.end());
        if(edge_count <= KRUSKAL_THRESHOLD) {
            return par_kruskals_algorithm(g);
        }
        auto pivot = *std::next(g->edges.begin(), edge_count / 2);
        auto middle = std::partition(g->edges.begin(), g->edges.end(), [pivot](const Edge *edge) {return edge->cost <= pivot->cost; });

        std::vector<Edge*> lighter_edges, heavier_edges;
        std::vector<Edge*>::iterator heavier_edges_start;

        #pragma omp task 
        lighter_edges = filter_kruskal_util(g, uf, start, middle);
        #pragma omp task 
        heavier_edges_start = filter(middle, end, uf);
        #pragma omp taskwait

        #pragma omp task 
        heavier_edges = filter_kruskal_util(g, uf, heavier_edges_start, end);

        lighter_edges.insert(lighter_edges.end(), heavier_edges.begin(), heavier_edges.end());
        return lighter_edges;
    }

    std::vector<Edge*> filter_kruskal(Graph *g) {
        std::vector<Edge*> result;
        UnionFind uf(g);
        
        #pragma omp parallel
        {
            #pragma omp single nowait
            result = filter_kruskal_util(g, uf, g->edges.begin(), g->edges.end());
        }

        return result;
    }
}
