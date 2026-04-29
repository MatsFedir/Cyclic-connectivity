#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <set>

using namespace std;

struct Vertex {
    int number;
    vector<int> adj;
    vector<int> flow;
};

struct Edge {
    int id;
    int u, v;
};

struct Graph {
    int n;
    vector<Vertex> vertices;
    vector<Edge> edges;
    int girth;
    int connectivity;
};

struct Tree {
    int root;
    int depth;
    vector<int> vertices;
    set<int> vertex_set;
    vector<int> last;   // current leaves
};

struct CycleCut {
    int size;
    vector<int> edge_ids;
};

struct CutResult {
    vector<int> part;
    vector<int> cut;   // edges of cut
};

// premade graphs / graph constructors
Graph makeEmptyGraph(int n);
void addEdge(Graph& G, int u, int v);
Graph buildPetersen();
Graph buildHexagonalPrism();
Graph buildMobiusLadder10();
Graph special4();
Graph largeOneGirth3();

Tree initTree(const Graph& G, int root);
void growTree(const Graph& G, Tree& T);
Tree buildTree(const Graph& G, int root, int maxDepth);
bool disjoint(const Tree& A, const Tree& B);

CycleCut girth(const Graph& G);
vector<vector<int>> buildA0(const Graph& G, int g);
int cyclicCut(const Graph& G);

Graph buildFlowGraph(const Graph& G, const vector<int>& A, const vector<int>& Tv_nodes, int& S_out, int& T_out);
void pushFlow(Graph& G, const vector<int>& parent, int S, int T);
bool augment_bfs(Graph& G, int S, int T, vector<int>& parent);
bool augmentOnce(Graph& G, int S, int T);
int maxflow(Graph& G, int S, int T);

#endif
