#pragma once

#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct Vertex {
    int number;
    vector<int> adj;
    vector<int> edge_id;
};

struct Edge {
    int id;
    int from, to;
    int flow;
};

struct Graph {
    int n;
    vector<Vertex> vertices;
    vector<Edge> edges;
};

bool cyclicCut(const vector<vector<int>>& graph, vector<pair<int, int>>& cycle_cut);
