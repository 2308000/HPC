#include <omp.h>
#include <algorithm>
#include <vector>
#include <stack>
#include <stdio.h>
#include "graph.h"

using namespace graph;

const int EDGE_THRESHOLD = 2<<12;

namespace q_sort {
    void seq_quick_sort(std::vector<Edge*>::iterator start, std::vector<Edge*>::iterator end) {
        auto edge_comparator = [](const Edge *a, const Edge *b) { return a->cost < b->cost; };
        std::sort(start, end, edge_comparator);
    }

    void par_quick_sort(std::vector<Edge*>::iterator start, std::vector<Edge*>::iterator end) {
        // Source: https://en.cppreference.com/w/cpp/algorithm/partition
        if (start == end) return;
        auto edge_count = std::distance(start, end);

        if (edge_count <= EDGE_THRESHOLD) {
            seq_quick_sort(start, end);
            return;
        }

        auto pivot = *std::next(start, edge_count / 2);

        auto lighter = std::partition(start, end, [pivot](const Edge *edge) {
            return edge->cost < pivot->cost;
        });
        auto heavier = std::partition(lighter, end, [pivot](const Edge *edge) {
            return !(pivot->cost < edge->cost);
        });

        #pragma omp task
        par_quick_sort(start, lighter);
        #pragma omp task
        par_quick_sort(heavier, end);
        #pragma omp taskwait
    }
}
