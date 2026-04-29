#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <sstream> 
#include <queue>
#include <list>
#include <stack>
#include <set>
#include <cmath>

#include "common.h"
using namespace std;

CycleCut girth(const Graph& G) {
	int best = -1;
	vector<int> best_cycle;

	for (int s = 0; s < G.n; s++) {
		vector<int> dist(G.n, -1);
		vector<int> parent(G.n, -1);
		queue<int> q;

		dist[s] = 0;
		q.push(s);

		while (!q.empty()) {
			int v = q.front(); q.pop();

			if (best != -1 && dist[v] >= best) continue;

			for (int i = 0; i < G.vertices[v].adj.size(); i++) {
				int u = (G.edges[G.vertices[v].adj[i]].v == v) ? (G.edges[G.vertices[v].adj[i]].u) : (G.edges[G.vertices[v].adj[i]].v);

				if (dist[u] == -1) {
					dist[u] = dist[v] + 1;
					parent[u] = v;
					q.push(u);
				}
				else if (parent[v] != u) {
					int len = dist[v] + dist[u] + 1;
					if (best != -1 && len >= best) continue;

					vector<int> pv, pu;
					int x = v, y = u;

					while (dist[x] > dist[y]) { pv.push_back(x); x = parent[x]; }
					while (dist[y] > dist[x]) { pu.push_back(y); y = parent[y]; }
					while (x != y) {
						pv.push_back(x);
						pu.push_back(y);
						x = parent[x];
						y = parent[y];
					}
					pv.push_back(x);

					vector<int> cycle = pv;
					reverse(pu.begin(), pu.end());
					cycle.insert(cycle.end(), pu.begin(), pu.end());

					best = len;
					best_cycle = cycle;
				}
			}
		}
	}

	vector<int> cut_edges;
	for (int i = 0; i < best_cycle.size(); i++) {
		int a = best_cycle[i];
		int b = best_cycle[(i + 1) % best_cycle.size()];

		for (const Edge& e : G.edges) {
			if ((e.u == a && e.v == b) || (e.u == b && e.v == a)) {
				cut_edges.push_back(e.id);
				break;
			}
		}
	}

	return { best, cut_edges };
}




void pushFlow(Graph& G, const vector<int>& parent, int S, int T) {
	int v = T;

	while (v != S) {
		int u = parent[v];


		for (int i = 0; i < G.vertices[u].adj.size(); i++) {
			int eid = G.vertices[u].adj[i];
			const Edge& e = G.edges[eid];
			int neighbor = (e.u == u) ? e.v : e.u;

			if (neighbor == v) {
				G.vertices[u].flow[i]++;
				break;
			}
		}


		for (int i = 0; i < G.vertices[v].adj.size(); i++) {
			int eid = G.vertices[v].adj[i];
			const Edge& e = G.edges[eid];
			int neighbor = (e.u == v) ? e.v : e.u;

			if (neighbor == u) {
				G.vertices[v].flow[i]--;
				break;
			}
		}

		v = u;
	}
}





bool augment_bfs(Graph& G, int S, int T, vector<int>& parent) {
	fill(parent.begin(), parent.end(), -1);
	queue<int> q;
	parent[S] = S;
	q.push(S);

	while (!q.empty()) {
		int v = q.front(); q.pop();

		for (int i = 0; i < G.vertices[v].adj.size(); i++) {
			// had a problem with it on the start, so I asked ChatGpt and he said that this const Edge is the most effective, so I kept it
			int eid = G.vertices[v].adj[i];
			const Edge& e = G.edges[eid];

			// find neighbor
			int u = (e.u == v) ? e.v : e.u;

			// residual check
			if (parent[u] == -1 && G.vertices[v].flow[i] < 1) {
				parent[u] = v;
				if (u == T) return true;
				q.push(u);
			}
		}
	}
	return false;
}


void resetFlows(Graph& G) {
	for (auto& vertex : G.vertices) {
		fill(vertex.flow.begin(), vertex.flow.end(), 0);
	}
}


bool augmentOnce(Graph& G, int S, int T) {
	vector<int> parent(G.n, -1);
	if (!augment_bfs(G, S, T, parent))
		return false;

	pushFlow(G, parent, S, T);
	return true;
}


int maxflow(Graph& G, int S, int T) {
	for (auto& vertex : G.vertices)
		fill(vertex.flow.begin(), vertex.flow.end(), 0);

	int flow = 0;
	vector<int> parent(G.n, -1);

	while (augment_bfs(G, S, T, parent)) {
		pushFlow(G, parent, S, T);
		flow++;
	}

	return flow;
}



Graph buildFlowGraph(const Graph& G, const vector<int>& A, const vector<int>& Tv_nodes, int& S_out, int& T_out) {
	Graph G2 = G;
	int originalN = G2.n;


	S_out = originalN;
	Vertex S;
	S.number = S_out;
	G2.vertices.push_back(S);

	// connect S 
		for (int u : A) {
		int eid = G2.edges.size();
		G2.edges.push_back({ eid, S_out, u });


		G2.vertices[S_out].adj.push_back(eid);
		G2.vertices[S_out].flow.push_back(0);


		G2.vertices[u].adj.push_back(eid);
		G2.vertices[u].flow.push_back(0);
	}

	//connecting sink
	T_out = originalN + 1;
	Vertex T;
	T.number = T_out;
	G2.vertices.push_back(T);


	for (int v : Tv_nodes) {
		int eid = G2.edges.size();
		G2.edges.push_back({ eid, v, T_out });


		G2.vertices[v].adj.push_back(eid);
		G2.vertices[v].flow.push_back(0);


		G2.vertices[T_out].adj.push_back(eid);
		G2.vertices[T_out].flow.push_back(0);
	}

	G2.n += 2;
	return G2;
}


Tree initTree(const Graph& G, int root) {
	Tree T;
	T.root = root;
	T.depth = 0;
	T.vertices.push_back(root);
	T.vertex_set.insert(root);
	T.last.push_back(root);
	return T;
}

void growTree(const Graph& G, Tree& T) {

	if (T.last.empty())
		return;

	vector<int> new_last;

	for (int u : T.last) {

		for (int eid : G.vertices[u].adj) {

			int v = (G.edges[eid].u == u)
				? G.edges[eid].v
				: G.edges[eid].u;


			if (T.vertex_set.find(v) == T.vertex_set.end()) {
				T.vertices.push_back(v);
				T.vertex_set.insert(v);
				new_last.push_back(v);
			}
		}
	}

	T.last = new_last;
	T.depth += 1;
}



Tree buildTree(const Graph& G, int root, int maxDepth) {
	vector<int> level(G.n, -1);
	queue<int> q;

	Tree T;
	T.root = root;
	T.depth = maxDepth;

	level[root] = 0;
	q.push(root);
	T.vertices.push_back(root);
	T.vertex_set.insert(root);

	while (!q.empty()) {
		int v = q.front(); q.pop();
		if (level[v] == maxDepth) continue;

		for (int eid : G.vertices[v].adj) {
			int u = (G.edges[eid].u == v)
				? G.edges[eid].v
				: G.edges[eid].u;

			if (level[u] == -1) {
				level[u] = level[v] + 1;
				q.push(u);
				T.vertices.push_back(u);
				T.vertex_set.insert(u);
			}
		}
	}
	return T;
}


bool disjoint(const Tree& A, const Tree& B) {
	for (int v : A.vertices)
		if (B.vertex_set.count(v))
			return false;
	return true;
}


vector<vector<int>> buildA0_edges(const Graph& G, int g) {
	vector<vector<int>> A0;
	set<pair<int, int>> used;

	for (int v = 0; v < G.n && A0.size() < g; v++) {
		for (int eid : G.vertices[v].adj) {
			const Edge& e = G.edges[eid];
			int u = (e.u == v) ? e.v : e.u;
			if (v < u && !used.count({ v,u })) {
				A0.push_back({ v,u });
				used.insert({ v,u });
			}
		}
	}
	return A0;
}

vector<vector<int>> buildA0_depth2(const Graph& G, int g) {
	vector<vector<int>> A0;
	for (int v = 0; v < G.n && A0.size() < g; v++) {
		Tree T = buildTree(G, v, 2);  // 2-level BFS tree
		A0.push_back(T.vertices);
	}
	return A0;
}

vector<vector<int>> buildA0_binary(const Graph& G, int g) {
	vector<vector<int>> A0;
	int d = (int)ceil(log2((g - 1) / 2.0));
	for (int v = 0; v < G.n && A0.size() < g; v++) {
		Tree T = buildTree(G, v, d);
		A0.push_back(T.vertices);
	}
	return A0;
}

vector<vector<int>> buildA0(const Graph& G, int g) {
	if (g <= 4) return buildA0_edges(G, g);
	if (g <= 12) return buildA0_depth2(G, g);
	return buildA0_binary(G, g);
}



// Finds edge-disjoint paths between Tv and Tw and appends them to paths.
// Tv and Tw are Trees rooted at two different vertices, containing vertex lists and sets.
// paths is a vector of paths, where each path is represented as a vector of edge IDs.
void findpaths(const Graph& G, const Tree& Tv, const Tree& Tw, vector<vector<int>>& paths) {

	int m = G.edges.size();
	// in each edge: flow = 0 -> free, flow = 1 -> used
	vector<int> flow(m, 0);

	// Marking already used edges
	for (const auto& p : paths) {
		for (int eid : p) flow[eid] = 1;
	}

	while (true) {  // Repeat until no more edge-disjoint paths can be found

		vector<int> parent_vertex(G.n, -1);
		vector<int> parent_edge(G.n, -1);
		queue<int> q;

		for (int v : Tv.vertices) {
			parent_vertex[v] = -2;
			q.push(v);
		}

		int target = -1;


		while (!q.empty() && target == -1) {
			int v = q.front(); q.pop();


			for (int eid : G.vertices[v].adj) {

				// had a problem with it on the start, so I asked ChatGpt and he said that this const Edge is the most effective, so I kept it
				const Edge& e = G.edges[eid];
				int u = (e.u == v) ? e.v : e.u;


				if (flow[eid] == 0 && parent_vertex[u] == -1) {
					parent_vertex[u] = v;
					parent_edge[u] = eid;
					q.push(u);

					if (Tw.vertex_set.count(u)) {
						target = u;
						break;
					}
				}


				if (flow[eid] == 1 && parent_vertex[u] == -1) {
					parent_vertex[u] = v;
					parent_edge[u] = eid;
					q.push(u);
				}
			}
		}

		if (target == -1) break;

		// reconstructing the path (from the end to the start):
		vector<int> newpath;
		int v = target;
		while (parent_vertex[v] != -2) {
			int eid = parent_edge[v];
			newpath.push_back(eid);
			flow[eid] ^= 1;

			v = parent_vertex[v];
		}

		// since path is from the end to the start, reverse is in order
		reverse(newpath.begin(), newpath.end());

		paths.push_back(newpath);
	}
}


// Returns edge IDs of the cut between Tv and Tw, based on paths
vector<int> findcut(const Graph& G, const Tree& Tv, const Tree& Tw, const vector<vector<int>>& paths) {
	set<int> cut_edges;

	for (const auto& p : paths) {
		for (int eid : p) {
			cut_edges.insert(eid);
		}
	}

	for (int v : Tv.vertices) {
		for (int eid : G.vertices[v].adj) {
			const Edge& e = G.edges[eid];
			int u = (e.u == v) ? e.v : e.u;
			if (!Tv.vertex_set.count(u)) {
				cut_edges.insert(eid);
			}
		}
	}

	for (int v : Tw.vertices) {
		for (int eid : G.vertices[v].adj) {
			const Edge& e = G.edges[eid];
			int u = (e.u == v) ? e.v : e.u;
			if (!Tw.vertex_set.count(u)) {
				cut_edges.insert(eid);
			}
		}
	}

	return vector<int>(cut_edges.begin(), cut_edges.end());
}





int run_cubic_n3_algorithm(const Graph& G) {
	CycleCut cc = girth(G);
	int cutsize = cc.size;
	vector<int> cut = cc.edge_ids;

	for (int v = 0; v < G.n; v++) {
		for (int w = v + 1; w < G.n; w++) {
			vector<vector<int>> paths;

			Tree Tv = initTree(G, v);
			Tree Tw = initTree(G, w);
			int d = -1;

			do {
				d++;
				bool grewV = !Tv.last.empty();
				bool grewW = !Tw.last.empty();

				growTree(G, Tv);
				growTree(G, Tw);

				if (!grewV || !grewW)
					break;


				if (!disjoint(Tv, Tw))
					break;

				findpaths(G, Tv, Tw, paths);

				vector<int> current_cut = findcut(G, Tv, Tw, paths);
				int current_cutsize = current_cut.size();

				if (current_cutsize < cutsize &&
					current_cutsize < 3 * (1 << d)) {

					cutsize = current_cutsize;
					cut = current_cut;
				}

			} while (3 * (1 << d) < cutsize);
		}
	}
	return cutsize;
}

int run_cubic_n2_algorithm(const Graph& G) {
	CycleCut gCut = girth(G);
	int g = gCut.size;

	
	vector<vector<int>> A0 = buildA0(G, g); // from Section 5

	int minCutSize = g;
	vector<int> minCutEdges = gCut.edge_ids;

	for (const auto& A : A0) {
		for (int v = 0; v < G.n; ++v) {
			Tree T = buildTree(G, v, 0);
			int d = 0;

			while (3 * (1 << d) < minCutSize) {
				T = buildTree(G, v, d);

				// Skip if disjoint check fails with A
				set<int> treeSet(T.vertices.begin(), T.vertices.end());
				set<int> subSet(A.begin(), A.end());
				vector<int> inter;
				set_intersection(treeSet.begin(), treeSet.end(),
					subSet.begin(), subSet.end(),
					back_inserter(inter));
				if (!inter.empty()) break;

				int S, Tnode;
				Graph FG = buildFlowGraph(G, A, T.vertices, S, Tnode);
				int cutSize = maxflow(FG, S, Tnode);

				if (cutSize < 3 * (1 << d) && cutSize < minCutSize) {
					minCutSize = cutSize;

					minCutEdges.clear();
					for (int u : A) {
						for (int eid : G.vertices[u].adj) {
							const Edge& e = G.edges[eid];
							int nei = (e.u == u) ? e.v : e.u;
							if (treeSet.count(nei)) minCutEdges.push_back(e.id);
						}
					}
				}

				++d;
			}
		}
	}

	return minCutSize;
}




int cyclicCut(const Graph& G) {
	if (G.n % 2 == 1) {
		cout << "not 3 - normal\n";
		return 0;
	}
	if (G.n == 4) {
		cout << "Got to the n3 algorithm, but keep in mind that this is just an exception\n";
		return run_cubic_n3_algorithm(G);
		cout << "4\n";
		//return 2;
	}
	else if (G.n == 6) {
		cout << "6\n";
		return 3;
	}
	if (G.n > 8) {
		if (G.n < 242) {
			cout << "Got to the n3 algorithm\n";
			return run_cubic_n3_algorithm(G);
		}
		else {
			cout << "Got to the n2 algorithm\n";
			return run_cubic_n2_algorithm(G);
		}
	}
	cout << "less than 8 vertices\n";
	return 0;
}

Graph makeEmptyGraph(int n) {
	Graph G;
	G.n = n;
	G.vertices.resize(n);

	for (int i = 0; i < n; i++) {
		G.vertices[i].number = i;
	}
	return G;
}

void addEdge(Graph& G, int u, int v) {
	int id = G.edges.size();

	G.edges.push_back({id, u, v});

	G.vertices[u].adj.push_back(id);
	G.vertices[u].flow.push_back(0);

	G.vertices[v].adj.push_back(id);
	G.vertices[v].flow.push_back(0);
}


Graph buildPetersen() {
	Graph G = makeEmptyGraph(10);

	addEdge(G, 0, 1);
	addEdge(G, 1, 2);
	addEdge(G, 2, 3);
	addEdge(G, 3, 4);
	addEdge(G, 4, 0);

	addEdge(G, 5, 7);
	addEdge(G, 7, 9);
	addEdge(G, 9, 6);
	addEdge(G, 6, 8);
	addEdge(G, 8, 5);

	for (int i = 0; i < 5; i++) {
		addEdge(G, i, i + 5);
	}

	return G;
}

Graph buildHexagonalPrism() {
	Graph G = makeEmptyGraph(12);

	for (int i = 0; i < 6; i++) {
		addEdge(G, i, (i + 1) % 6);
	}

	for (int i = 6; i < 12; i++) {
		addEdge(G, i, 6 + (i + 1 - 6) % 6);
	}

	for (int i = 0; i < 6; i++) {
		addEdge(G, i, i + 6);
	}

	return G;
}

Graph buildMobiusLadder10() {
	Graph G = makeEmptyGraph(10);
	for (int i = 0; i < 10; i++) {
		addEdge(G, i, (i + 1) % 10);
	}

	for (int i = 0; i < 5; i++) {
		addEdge(G, i, i + 5);
	}

	return G;
}

Graph special4() {
	Graph G = makeEmptyGraph(4);

	addEdge(G, 0, 1);
	addEdge(G, 1, 0);

	addEdge(G, 2, 3);
	addEdge(G, 3, 2);

	addEdge(G, 0, 2);
	addEdge(G, 1, 3);
	return G;
}

Graph largeOneGirth3() {
	int blocks = 81;   // 81 * 4 = 324 vertices
	Graph G = makeEmptyGraph(blocks * 4);

	for (int b = 0; b < blocks; b++) {
		int base = 4 * b;

		addEdge(G, base + 0, base + 1);
		addEdge(G, base + 0, base + 2);
		addEdge(G, base + 0, base + 3);

		addEdge(G, base + 1, base + 2);
		addEdge(G, base + 1, base + 3);

		addEdge(G, base + 2, base + 3);
	}

	return G;
}