#include <iostream>

#include "common.h" 

int main()
{
    int n;
    cin >> n;
    vector<vector<int>> graph(n);
    for (int i = 0; i < n; i++) {
        int s1, s2, s3;
        cin >> s1 >> s2 >> s3;
        vector<int> neighbours = { s1,s2,s3 };
        graph[i] = neighbours;
    }
    vector<pair<int, int>> cycle_cut;

    if (cyclicCut(graph, cycle_cut)) {
        cout << "success\n";
    }
    else {
        cout << "fail\n";
    }

    for (int i = 0; i < cycle_cut.size(); i++) {
        cout << cycle_cut[i].first << " " << cycle_cut[i].second << "\n";
    }

    return 0;
}
