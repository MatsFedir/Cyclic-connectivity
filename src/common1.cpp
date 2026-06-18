#pragma once

#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <queue>

#include "common.h"
using namespace std;


// Method for girth. Simple O(n^2), just as the papers recommend
int find_girth(const Graph& G) {
	int bestGirth = INT_MAX;
	for (int i = 0; i < G.n; i++) {

		vector<int> dist(G.n, -1);
		vector<int> parent(G.n, -1);

		queue<int> q;
		q.push(i);
		dist[i] = 0;

		while (!q.empty()) {
			int v = q.front();
			q.pop();
			for (int j = 0; j < 3; j++) {
				int w = G.vertices[v].adj[j];
				int edge_id = G.vertices[v].edge_id[j];

				if (dist[w] == -1) {
					dist[w] = dist[v] + 1;
					parent[w] = edge_id;
					q.push(w);
				}
				else if (parent[v] != edge_id) {
					bestGirth = min(bestGirth, dist[v] + dist[w] + 1);
				}
			}
		}
		return bestGirth;
	}
	return bestGirth;
}


// This method follows the papers. Instead of rebuilding the tree again, we grow them with paths.
void treeGrow(const Graph& G, vector<bool>& Tv, vector<vector<pair<int, int>>> paths, vector<int>& TvLeaves, bool addLeaves) {
	for (auto& path : paths) {
		// It is easier to start from the edges that are already inside the 
		Edge edge = G.edges[path[0].first];
		int v = edge.from;
		int w = edge.to;
		if (!Tv[v] || !Tv[w]) {
			reverse(path.begin(), path.end());
		}
		for (int i = 0; i < path.size(); i++) {
			Edge edge = G.edges[path[i].first];
			int a = edge.from;
			int b = edge.to;

			if (!Tv[a]) {
				TvLeaves.push_back(a);
				if (addLeaves) Tv[a] = true;
				break;
			}
			if (!Tv[b]) {
				TvLeaves.push_back(b);
				if (addLeaves) Tv[b] = true;
				break;
			}
		}
	}
}


// This is a flow algorithm for finding paths inspired by Ford-Fulkerson's flow algorithm, but it is in O(n), because we don't need a special structure like his. The main focus is that as we travel the Graph in DFS manner, we only follow the edges that have been walked 0 times, walked from the other end of the edge to this end of the edge, and those that were walked both ways;
bool findpaths(int v, Graph& G, vector<bool>& visited /* also includes Tv */, vector<bool> Tw, vector<pair<int, int>>& path) {
	visited[v] = true;

	if (Tw[v]) return true;

	for (int i = 0; i < 3; i++) {
		int u = G.vertices[v].adj[i];
		int edge_idx = G.vertices[v].edge_id[i];
		Edge& edge = G.edges[edge_idx];

		if (visited[u]) continue;

		int flow = edge.flow;
		int id = edge.id;
		bool correct_edge_direction = (u == edge.to);

		// flow == 0    means that the edge was not walked yet, or was walked both ways
		if (flow == 0) {
			if (correct_edge_direction) edge.flow = 1;
			else edge.flow = 2;

			if (findpaths(u, G, visited, Tw, path)) {
				path.push_back({ edge.id, edge.flow });
				return true;
			}

			edge.flow = 0;
		}
		// flow = 1        means that the edge was only walked forward
		else if (flow == 1) {
			if (!correct_edge_direction) {
				edge.flow = 0;

				if (findpaths(u, G, visited, Tw, path)) {
					path.push_back({ edge.id, edge.flow });
					return true;
				}

				edge.flow = 1;
			}
		}
		// flow = 2        means that the edge was only walked backwards
		else if (flow == 2) {
			if (correct_edge_direction) {
				edge.flow = 0;

				if (findpaths(u, G, visited, Tw, path)) {
					path.push_back({ edge.id, edge.flow });
					return true;
				}

				edge.flow = 2;
			}
		}
	}
	return false;
}


// helper function for the findcut
bool has_cycle(int u, int p, const Graph& G, const vector<bool>& subset, vector<bool>& vis) {
	vis[u] = true;
	for (int i = 0; i < 3; i++) {
		int v = G.vertices[u].adj[i];

		if (!subset[v]) continue;

		if (!vis[v]) {
			if (has_cycle(v, u, G, subset, vis)) return true;
		}
		else if (v != p) {
			return true;
		}
	}
	return false;
}



// Here we do a greedy Max-flow algorithm that may or may not find the cyclic edge cut
// This, however, is not a problem, because he will definitely find it if the trees grow enough
vector<int> findcut(const Graph& G, const vector<bool>& Tv, const vector<bool>& Tw, const vector<vector<pair<int, int>>>& paths) {
	vector<bool> S_reachable(G.n, false);
	vector<int> Q;

	// Seed the BFS with the source set Tv
	for (int i = 0; i < G.n; i++) {
		if (Tv[i]) {
			S_reachable[i] = true;
			Q.push_back(i);
		}
	}

	// Traverse the residual graph
	int head = 0;
	while (head < Q.size()) {
		int u = Q[head++];
		for (int i = 0; i < 3; i++) {
			int v = G.vertices[u].adj[i];
			int edge_idx = G.vertices[u].edge_id[i];
			const Edge& edge = G.edges[edge_idx];

			bool can_go = false;
			if (u == edge.from && edge.flow != 1) can_go = true;
			if (u == edge.to && edge.flow != 2) can_go = true;

			if (can_go && !S_reachable[v]) {
				S_reachable[v] = true;
				Q.push_back(v);
			}
		}
	}

	// Verify that both sides of the partition contain a cycle
	vector<bool> other_side(G.n, false);
	for (int i = 0; i < G.n; i++) other_side[i] = !S_reachable[i];

	bool cycle_S = false;
	vector<bool> visS(G.n, false);
	for (int i = 0; i < G.n; i++) {
		if (S_reachable[i] && !visS[i]) {
			if (has_cycle(i, -1, G, S_reachable, visS)) { cycle_S = true; break; }
		}
	}

	bool cycle_Other = false;
	vector<bool> visO(G.n, false);
	for (int i = 0; i < G.n; i++) {
		if (other_side[i] && !visO[i]) {
			if (has_cycle(i, -1, G, other_side, visO)) { cycle_Other = true; break; }
		}
	}

	// Reject the cut if it is not cyclic edge cut
	if (!cycle_S || !cycle_Other) {
		return vector<int>();
	}

	// Extract the valid cyclic cut edges
	vector<int> result;
	for (const Edge& edge : G.edges) {
		if (S_reachable[edge.from] != S_reachable[edge.to]) {
			result.push_back(edge.id);
		}
	}

	return result;
}


// This method mostly follows the papers, with the exception of trying to actively find the edge cut if we haven't found one yet. We do it by trying to assign the edge cut of size g to the answer.
vector<int> run_n3_algorithm(Graph& G, int girth) {
	int cutsize = girth;
	vector<int> cyclic_cut;

	for (int v = 0; v < G.n; v++) {
		for (int w = v + 1; w < G.n; w++) {


			vector<vector<pair<int, int>>> paths;
			vector<int> leavesV({ v });
			vector<bool> Tv(G.n, false), Tw(G.n, false);
			Tv[v] = true;
			Tw[w] = true;

			int d = -1;

			do {
				for (vector<pair<int, int>> path : paths) {
					for (pair<int, int> edge : path) {
						G.edges[edge.first].flow = edge.second;
					}
				}


				d++;
				treeGrow(G, Tv, paths, leavesV, true);
				treeGrow(G, Tw, paths, leavesV, false);

				bool intersected = false;
				for (int i = 0; i < G.n; i++) {
					if (Tv[i] && Tw[i]) {
						intersected = true;
						break;
					}
				}
				if (intersected) break;


				for (int v : leavesV) {
					while (true) {
						vector<pair<int, int>> path;
						vector<bool> visited = Tv;
						if (!findpaths(v, G, visited, Tw, path)) break;


						paths.push_back(path);
						path.clear();
					}
				}

				int current_cutsize = paths.size();

				if (current_cutsize < cutsize && current_cutsize < 3 * (1 << d)) {

					vector<int> potential_cut = findcut(G, Tv, Tw, paths);

					// Only accept it if it's a valid cyclic cut
					if (!potential_cut.empty()) {
						cutsize = current_cutsize;
						cyclic_cut = potential_cut;

						for (Edge& edge : G.edges) edge.flow = 0;
						break;
					}
					// If it's empty, it was a trivial cut. Do NOT break. 
					// Let the do-while loop continue to grow Tv and Tw!
				}


				if (current_cutsize == cutsize && cyclic_cut.empty()) {

					vector<int> potential_cut = findcut(G, Tv, Tw, paths);

					if (!potential_cut.empty()) {
						cyclic_cut = findcut(G, Tv, Tw, paths);
						break;
					}
				}



				for (Edge& edge : G.edges) {
					edge.flow = 0;
				}

				leavesV.clear();
			} while (3 * (1 << d) <= cutsize);
		}
	}
	return cyclic_cut;
}


// This method follows the papers
bool build_A0(const Graph& G, vector<vector<bool>>& A0, int g) {
	if (g >= 2 && g <= 4) {
		int k = 0;
		for (int i = 0; i < G.n && k != g; i++) {
			vector<bool> A(G.n, false);
			A[G.edges[i].from] = true;
			A[G.edges[i].to] = true;

			A0[i] = (A);
		}
	}


	if (g >= 5 && g <= 12) {
		vector<bool> marked(G.n, false);

		queue<pair<int, int>> q;
		int v = 0;


		for (int i = 0; i < g; i++) {
			while (true) {
				if (marked[v]) v++;
				else break;
				if (v >= G.n - 1) {
					return false;
				}
			}

			vector<bool> A(G.n, false);
			q.push({ v, 0 });

			while (!q.empty()) {
				pair<int, int> p = q.front();
				q.pop();
				int a = p.first;
				int d = p.second;

				marked[a] = true;

				if (d <= 2) {
					A[a] = true;
				}
				if (d == 3) {
					continue;
				}

				for (int b : G.vertices[a].adj) {
					if (marked[b]) continue;

					q.push({ b , d + 1 });
				}
			}
			A0[i] = A;
		}
	}


	if (g >= 13) {

		int D = (g - 1) / 2;
		int td = ceil(log2((g - 1) / 2.0));
		int l = D / td;

		int root = 0;

		int v = G.vertices[root].number;
		int u = G.vertices[root].adj[root];

		vector<int> roots = { u, v };


		for (int i = 0; i < g; i++) {
			queue<pair<int, int>> q;
			vector<int> dist(G.n, 0);
			vector<bool> visited(G.n, false);
			visited[v] = true;
			visited[u] = true;


			q.push({ roots[i], 0 });

			vector<bool> A(G.n, false);

			while (!q.empty()) {
				pair<int, int> p = q.front();
				q.pop();
				int a = p.first;
				int d = p.second;

				visited[a] = true;

				if (d <= td - 1) {
					A[a] = true;
				}
				if (dist[a] == D) {
					continue;
				}
				if (d == td) {
					roots.push_back(a);
					continue;
				}

				for (int b : G.vertices[a].adj) {
					if (visited[b]) continue;

					dist[b] = dist[a] + 1;
					q.push({ b , d + 1 });
				}
			}
			A0[i] = A;
		}
	}
	return true;
}



// This method mostly follows the papers, with the exception of trying to actively find the edge cut if we haven't found one yet. We do it by trying to assign the edge cut of size g to the answer.
vector<int> run_n2_algorithm(Graph& G, int girth) {
	vector<int> cyclic_cut;
	int cutsize = girth;

	vector<vector<bool>> A0(girth);
	build_A0(G, A0, girth);


	for (vector<bool> A : A0) {
		for (int v = 0; v < G.n; ++v) {
			vector<vector<pair<int, int>>> paths;
			vector<bool> Tv(G.n, 0);
			vector<int> leavesV({ v });

			int d = -1;


			do {
				for (vector<pair<int, int>> path : paths) {
					for (pair<int, int> edge : path) {
						G.edges[edge.first].flow = edge.second;
					}
				}
				++d;

				treeGrow(G, Tv, paths, leavesV, true);

				bool intersected = false;
				for (int i = 0; i < G.n; i++) {
					if (Tv[i] && A[i]) {
						intersected = true;
						break;
					}
				}
				if (intersected) break;



				for (int v : leavesV) {
					while (true) {
						vector<pair<int, int>> path;
						vector<bool> visited = Tv;
						if (!findpaths(v, G, visited, A, path)) break;


						paths.push_back(path);
						path.clear();
					}
				}


				int current_cutsize = paths.size();

				if (current_cutsize < cutsize && current_cutsize < 3 * (1 << d)) {

					vector<int> potential_cut = findcut(G, Tv, A, paths);


					if (!potential_cut.empty()) {
						cutsize = current_cutsize;
						cyclic_cut = potential_cut;

						for (Edge& edge : G.edges) edge.flow = 0;
						break;
					}
				}


				
				if (current_cutsize == cutsize && cyclic_cut.empty()) {

					vector<int> potential_cut = findcut(G, Tv, A, paths);

					if (!potential_cut.empty()) {
						cyclic_cut = potential_cut;
						break;
					}
				}



				for (Edge& edge : G.edges) {
					edge.flow = 0;
				}



				Tv.assign(G.n, false);
				leavesV.clear();
			} while (3 * (1 << d) < cutsize);
		}
	}

	return cyclic_cut;
}




// graph contains vectors of the neighbours of a vertices
// This is the starting point of the algorithm.
// We get a graph (vector of neighbours of each edge) and we transform it into a more suitable Graph G
// The output is True or False, depending on whether we have or don't have a cyclic cut. The "cycle_cut" consists of pairs of vertices, between which we must remove an edge if the cycle cut exists.
bool cyclicCut(const vector<vector<int>>& graph, vector<pair<int, int>>& cycle_cut) {
	int n = graph.size();
	if (n < 2) return false;

	vector<Edge> edges;
	vector<Vertex> vertices(n);

	// Transform the graph
	int curr_edge_id = 0;
	for (int v = 0; v < n; v++) {
		vertices[v].number = v;


		// While transforming it, we also check if it's cubic
		if (graph[v].size() != 3) {
			return false;
		}


		for (int w : graph[v]) {
			if (v >= w) {
				Edge e;
				e.flow = 0;
				e.from = v;
				e.to = w;
				e.id = curr_edge_id;

				edges.push_back(e);
				curr_edge_id++;

				vertices[v].adj.push_back(w);
				vertices[v].edge_id.push_back(e.id);

				if (v > w) {
					vertices[w].adj.push_back(v);
					vertices[w].edge_id.push_back(e.id);
				}
			}
		}
	}


	Graph G;
	G.n = n;
	G.edges = edges;
	G.vertices = vertices;


	// This will be the set of edge ids that must be removed to achieve cycle cut. In the end of this method we translate it to pairs of vertices
	vector<int> edges_cut;



	// Before we do the algorithm from the study, we must make sure that the graph is 'normal'. By normal I mean that:
	//		The graph does not have multiple edges between the same vertices
	//		The graph does not have edges that go from v to v
	//		The graph does not already have 2 components that have a cycle each (in which case the cycle cut is empty)
	//
	// If the graph is normal, then it has one component and all of it's edges have unique v and u

	// Here we check if the graph has more that 1 component already
	vector<bool> visited(G.n, false);
	int currV = 0;
	vector<pair<int, int>> components;
	int k = 0;
	while (currV < G.n) {
		if (visited[currV]) {
			currV++;
			continue;
		}


		queue<int> q;
		q.push(currV);

		while (!q.empty()) {
			int v = q.front();
			q.pop();
			visited[v] = true;

			for (int w : G.vertices[v].adj) {
				if (visited[w]) continue;
				q.push(w);
			}
		}
		
		components.push_back({ k, currV });
		k++;
		currV++;
	}

	if (components.size() != 1) {
		// The graph has multiple components
		// Every cubic component has to have a cycle
		return true;
	}


	// Looking for the edges that go from v to v
	// if such edge exists, then the vertex has either:
	//		1 or 2 such edges on one vertex, then we cut off his neighbours and get an independent component with a cycle. Then we need only one more vertex (it's neighbour) and we have a second component. since the graph is cubic, then it's neighbour has to have a cycle within it's own component
	//		3 edges on one vertex. Then the graph already consists of 2 components, so it got handled before

	pair<int, int> s = { 0,-1 };
	for (int v = 0; v < n; v++) {
		int k = 0;
		for (int w : G.vertices[v].adj) {
			if (v == w) {
				k++;
			}
		}
		if (k > s.first) {
			s = { k,v };
		}
	}
	if (s.first != 0) {
		int v = s.second;
		if (s.first >= 1 && n >= 2) {
			for (int w : G.vertices[v].adj) {
				if (w != v) {
					cycle_cut.push_back({ v, w });
				}
			}
			return true;
		}
		else return false;
	}


	// Looking for the pairs of edges that go from the same v to the same u
	// if such edges exists, then:
	//		it's a double edge between v and u, then we cut off their neighbours and get an independent component with a cycle. Then we need only one more vertex (it's neighbours) and we have a second component. since the graph is cubic, then it's neighbour has to have a cycle within it's own component
	//		it's a tripple edge. Then the graph already consists of 2 components, so it got handled before
	for (int v = 0; v < n; v++) {
		vector<int> adj = G.vertices[v].adj;
		int third_wheel = -1;
		int w = -1;
		if (adj[0] == adj[1]) { third_wheel = adj[2];  w = adj[0]; }
		if (adj[0] == adj[2]) { third_wheel = adj[1];  w = adj[0]; }
		if (adj[1] == adj[2]) { third_wheel = adj[0];  w = adj[1]; }

		if (third_wheel != -1) {
			if (n >= 3) {
				cycle_cut.push_back({ v, third_wheel });
				for (int u : G.vertices[w].adj) {
					if (u != v) cycle_cut.push_back({ w, u });
				}

				return true;
			}
			else return false;
		}
	}


	// Up to here we have handled the case where the graph has:
	//		More than one component
	//		Edges that go from v to v
	//		Double and tripple edges

	// Therefore we know that the graph is 'normal' 
	// Run the [n^3 * log(n)] or [n^2 * log^2(n)] algorithms


	if (n < 8) {
		if (n == 4) return false;
		if (n == 6) {
			// A girth of 3 on a cubic 6-vertex graph guarantees it's a Prism.
			// the algorithm here checks what kind of prism it is, knowing that the graph is 'normal'
			if (find_girth(G) == 3) {
				for (int v = 0; v < n; v++) {

					vector<bool> visited(n, false);
					visited[v] = true;

					for (int w : G.vertices[v].adj) {
						visited[w] = true;
					}

					for (int w : G.vertices[v].adj) {
						bool bridge = true;
						for (int u : G.vertices[w].adj) {
							if (visited[u] && u != v) {
								bridge = false;
							}
						}
						if (bridge && v > w) {
							cycle_cut.push_back({ v, w });
						}
					}
				}
				return true;
			}
			else return false;
		}
	}

	int girth = find_girth(G);

	if (G.n >= 8) {
		vector<bool> allowed(G.n, true);
		if (n < 243) {
			edges_cut = run_n3_algorithm(G, girth);
		}
		else {
			edges_cut = run_n2_algorithm(G, girth);
		}
	}

	// Translate the edge ids to pairs of vertices
	for (int edge_id : edges_cut) {
		Edge edge = G.edges[edge_id];
		int v = edge.from;
		int w = edge.to;
		cycle_cut.push_back({ v, w });
	}

	return true;
}
