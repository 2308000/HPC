#pragma once 
#include <unordered_map>
#include "graph.h"

using namespace graph;

struct UnionFind {
    std::unordered_map<GraphNode*, GraphNode*> parent;
    std::unordered_map<GraphNode*, int> component_size;
    
    UnionFind(Graph *g) {
        for(auto node: g->nodes) {
            parent[node] = node;
            component_size[node] = 1;
        }
    }

    GraphNode *find(GraphNode *p) {
        GraphNode *root = p;
        while(parent[root] != root) root = parent[root];

        while(p != root) {
            GraphNode *next = parent[p];
            parent[p] = root;
            p = next;
        }

        return root;
    }

    bool are_connected(GraphNode *p, GraphNode *q) {
        return find(p) == find(q);
    }

    void unify(GraphNode *p, GraphNode *q) {
        if(are_connected(p, q)) return;

        GraphNode *root1 = find(p);
        GraphNode *root2 = find(q);

        if(component_size[root1] < component_size[root2]) {
            component_size[root2] += component_size[root1];
            component_size[root1] = 0;
            parent[root1] = root2;
            return;
        } 
        component_size[root1] += component_size[root2];
        component_size[root2] = 0;
        parent[root2] = root1;
    }
};
