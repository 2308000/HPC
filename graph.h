#pragma once
#include<vector>

namespace graph {
    struct GraphNode {
        char value;

        GraphNode(char v) {
            value = v;
        }
    };

    struct Edge {
        GraphNode *u;
        GraphNode *v;
        int cost;

        Edge(GraphNode *node1, GraphNode *node2, int w) {
            u = node1;
            v = node2;
            cost = w;
        }
    };

    struct Graph {
        int num_nodes = 0;
        std::vector<GraphNode*> nodes;
        std::vector<Edge*> edges;

        Graph() {
            nodes.resize(0);
            edges.resize(0);
            num_nodes = 0;
        }

        Graph(std::vector<GraphNode*> v, std::vector<Edge*> e) {
            nodes = v;
            edges = e;
            num_nodes = nodes.size();
        }

        GraphNode *add_new_node(char value) {
            GraphNode *new_node = new GraphNode(value);
            this->num_nodes++;
            this->nodes.push_back(new_node);
            return new_node;
        }

        void add_new_edge(GraphNode *u, GraphNode *v, int cost) {
            this->edges.push_back(new Edge(u, v, cost));
        }

        void delete_graph() {
            for(auto edge: this->edges) delete(edge);
            for(auto node: this->nodes) delete(node);
            delete this; 
        }
    };
}