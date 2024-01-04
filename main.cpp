#include <cassert>
#include <ctime>
#include <omp.h>
#include "graph.h"
#include "union_find.h"
#include "mst.h"

using std::cout;
using namespace graph;
using namespace mst;

Graph *test_graph_1() {
    Graph *gr = new Graph();
    GraphNode *a = gr->add_new_node('A');
    GraphNode *b = gr->add_new_node('B');
    GraphNode *c = gr->add_new_node('C');
    GraphNode *d = gr->add_new_node('D');
    GraphNode *e = gr->add_new_node('E');
    GraphNode *f = gr->add_new_node('F');
    GraphNode *g = gr->add_new_node('G');

    gr->add_new_edge(a, b, 2);
    gr->add_new_edge(a, c, 3);
    gr->add_new_edge(a, d, 3);
    gr->add_new_edge(b, c, 4);
    gr->add_new_edge(b, e, 3);
    gr->add_new_edge(c, f, 6);
    gr->add_new_edge(c, e, 1);
    gr->add_new_edge(f, g, 9);
    gr->add_new_edge(f, d, 2);
    gr->add_new_edge(f, e, 1);

    return gr;
}

Graph *test_graph_2() {
    Graph *gr = new Graph();
    GraphNode* a = gr->add_new_node('A');
    GraphNode* b = gr->add_new_node('B');
    GraphNode* c = gr->add_new_node('C');
    GraphNode* d = gr->add_new_node('D');
    GraphNode* e = gr->add_new_node('E');
    GraphNode* f = gr->add_new_node('F');

    gr->add_new_edge(a, b, 4);
    gr->add_new_edge(a, c, 2);
    gr->add_new_edge(b, d, 8);
    gr->add_new_edge(c, b, 1);
    gr->add_new_edge(c, e, 4);
    gr->add_new_edge(d, f, 1);
    gr->add_new_edge(e, d, 2);
    gr->add_new_edge(e, f, 7);
    
    return gr;
}

Graph *test_graph_3() {
    Graph *gr = new Graph();
    GraphNode* u = gr->add_new_node('u');
    GraphNode* s = gr->add_new_node('s');
    GraphNode* t = gr->add_new_node('t');
    GraphNode* v = gr->add_new_node('v');
    GraphNode* w = gr->add_new_node('w');

    gr->add_new_edge(s, u, 6);
    gr->add_new_edge(s, t, 3);
    gr->add_new_edge(s, v, 2);
    gr->add_new_edge(v, t, 4);
    gr->add_new_edge(v, u, 4);
    gr->add_new_edge(v, w, 6);
    gr->add_new_edge(t, u, 5);
    gr->add_new_edge(t, w, 5);
    gr->add_new_edge(w, u, 1);

    return gr;
}

Graph *test_graph_4() {
    Graph *gr = new Graph();
    GraphNode* a = gr->add_new_node('A');
    GraphNode* b = gr->add_new_node('B');
    GraphNode* c = gr->add_new_node('C');
    GraphNode* d = gr->add_new_node('D');
    GraphNode* e = gr->add_new_node('E');
    GraphNode* f = gr->add_new_node('F');
    GraphNode* g = gr->add_new_node('G');
    GraphNode* h = gr->add_new_node('H');
    GraphNode* i = gr->add_new_node('I');
    GraphNode* j = gr->add_new_node('J');

    gr->add_new_edge(a, b, 5);
    gr->add_new_edge(a, e, 1);
    gr->add_new_edge(a, d, 9);
    gr->add_new_edge(b, c, 4);
    gr->add_new_edge(b, d, 2);
    gr->add_new_edge(c, h, 4);
    gr->add_new_edge(c, i, 1);
    gr->add_new_edge(c, j, 8);
    gr->add_new_edge(d, e, 2);
    gr->add_new_edge(d, f, 5);
    gr->add_new_edge(d, g, 11);
    gr->add_new_edge(d, h, 2);
    gr->add_new_edge(e, f, 1);
    gr->add_new_edge(f, g, 7);
    gr->add_new_edge(g, h, 1);
    gr->add_new_edge(g, i, 4);
    gr->add_new_edge(h, i, 6);
    gr->add_new_edge(i, j, 0);

    return gr;
}

Graph *performance_test(int node_count, bool is_dense) {
    srand(static_cast<unsigned>(time(NULL)));
    std::vector<GraphNode *> nodes(node_count, nullptr);
    Graph *g = new Graph();

    for(int i = 0; i < node_count; i++) {
        GraphNode* new_node = g->add_new_node('A' + rand() % ('Z' - 'A' + 1));
        //cout << "New node: " << new_node->value << std::endl;
        nodes[i] = new_node;
    }

    for(int i = 0; i < node_count; i++) {
        // +1 ensures that the graph is connected
        int neighbor_count = rand() % 5 + 1;
        if(is_dense) {
            // lots of edges, some will be duplicate between the nodes
            // but this ensures that number of edges is in the upper
            // half of max number of edges
            int half_nodes = (int)(node_count / 2 - 1);
            neighbor_count = half_nodes + rand() % half_nodes;
        }
        
        for(int j = 0; j < neighbor_count; j++) {
            int new_neighbor_idx = rand() % node_count;
            // cannot be a neighbor to itself
            if(new_neighbor_idx == i) {
                if(new_neighbor_idx) {
                    new_neighbor_idx--;
                } else {
                    new_neighbor_idx++;
                }
            }

            g->add_new_edge(nodes[i], nodes[new_neighbor_idx], rand() % 5 + 1);
        }
    }

    if(is_dense) printf("Dense graph: ");
    else printf("Sparse graph: ");
    printf("|V|=%d, |E|=%ld\n\n", g->num_nodes, g->edges.size());

    return g;
}

void sanity_check(Graph *graph, const int assertion) {
    auto mst = prims_algorithm(graph);
    long total_weight_prim = 0;
    for (size_t i = 0; i < mst.size(); ++i) total_weight_prim += mst[i].second;
    cout << "Total weight of MST: " << total_weight_prim << "\n\n";
    assert(total_weight_prim == assertion);

    auto edges = graph->edges;

    auto mst_k = kruskals_algorithm(graph);
    long total_weight_kruskal = 0;
    for(auto edge: mst_k) total_weight_kruskal += edge->cost;

    assert(total_weight_prim == total_weight_kruskal);
    graph->edges = edges;

    auto mst_par_k = par_kruskals_algorithm(graph);
    long total_weight_kruskal_par = 0;
    for(auto edge: mst_par_k) total_weight_kruskal_par += edge->cost;
    assert(total_weight_prim == total_weight_kruskal_par);
    graph->edges = edges;

    auto mst_fk = filter_kruskal(graph);
    long total_weight_filter_kruskal = 0;
    for(auto edge: mst_fk) total_weight_filter_kruskal += edge->cost;
    assert(total_weight_prim == total_weight_filter_kruskal);
}

void performance_check(const int node_count, const bool is_dense) {
    printf("--------------------------------------\n");
    printf("Performance test (%d nodes):\n", node_count);
    int number_of_nodes = 0;
    auto graph = performance_test(node_count, is_dense);

    float end, start = omp_get_wtime();
    auto mst = prims_algorithm(graph);
    end = omp_get_wtime();
    cout << "Time passed (Prim): " << end - start << "\n";

    long total_weight_prim = 0;
    for(auto node_edge: mst) total_weight_prim += node_edge.second;
    
    auto edges = graph->edges;

    end, start = omp_get_wtime();
    auto mst_k = kruskals_algorithm(graph);
    end = omp_get_wtime();
    cout << "Time passed (Kruskal seq): " << end - start << "\n";

    long total_weight_kruskal = 0;
    for(auto edge: mst_k) total_weight_kruskal += edge->cost;
    assert(total_weight_prim == total_weight_kruskal);

    graph->edges = edges;
    end, start = omp_get_wtime();
    auto mst_par_k = par_kruskals_algorithm(graph);
    end = omp_get_wtime();
    cout << "Time passed (Kruskal par): " << end - start << "\n";

    long total_weight_par_kruskal = 0;
    for(auto edge: mst_par_k) total_weight_par_kruskal += edge->cost;
    assert(total_weight_prim == total_weight_par_kruskal);

    graph->edges = edges;
    end, start = omp_get_wtime();
    auto mst_fk = par_kruskals_algorithm(graph);
    end = omp_get_wtime();
    cout << "Time passed (filter Kruskal): " << end - start << "\n";

    long total_weight_filter_kruskal = 0;
    for(auto edge: mst_fk) total_weight_filter_kruskal += edge->cost;
    assert(total_weight_prim == total_weight_filter_kruskal);

    graph->delete_graph();
}

int main() {
    // Sanity checks
    // -------------
    {
        std::vector<std::pair<Graph *, int>> tests;
        tests.push_back(std::make_pair(test_graph_1(), 18));
        tests.push_back(std::make_pair(test_graph_2(), 10));
        tests.push_back(std::make_pair(test_graph_3(), 10));
        tests.push_back(std::make_pair(test_graph_4(), 14));

        printf("Sanity checks:\n\n");
        for(int i = 0; i < tests.size(); i++) {
            printf("Test case %d:\n", i + 1);
            sanity_check(tests[i].first, tests[i].second);
            tests[i].first->delete_graph();
        }
        printf("--------------------------------------\n\n");
    }
    // -------------

    // Performance tests
    // Sprase graphs
    // -----------------
    {
        std::vector<int> num_vercies = {10000, 50000, 150000, 200000, 250000, 300000, 350000};
        for(auto v: num_vercies) {
            performance_check(v, false);
        }
    }
    // Dense graphs
    // -----------------
    {
        std::vector<int> num_vertices = {500, 1500, 2000, 2500, 3000, 3500, 4000};
        for(auto v: num_vertices) {
            performance_check(v, true);
        }
    }
    // -----------------

    return 0;
}


