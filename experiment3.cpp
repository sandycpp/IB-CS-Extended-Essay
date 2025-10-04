#include<bits/stdc++.h>
using namespace std;

#define ar array

const int V = 2000, INF = 1e9;

struct DSU {
	vector<int> link, size;

	DSU(int n) {
		link.resize(n);
		size.assign(n, 1);
		for(int i=0; i<n; i++)
			link[i]=i;
	}

	int find(int x) {
		while(link[x]!=x) {
			link[x] = link[link[x]];
			x = link[x];
		}
		return x;
	}

	bool same(int a, int b) {
		return find(a) == find(b);
	}

	void unite(int a, int b) {
		a = find(a);
		b = find(b);
		if(a==b) return;
		if(size[a]<size[b]) swap(a, b);
		size[a]+=size[b];
		link[b]=a;
	}
};

struct Graph {
	vector<ar<int, 3>> edges;
	vector<vector<ar<int, 2>>> adj;
};

int Kruskal(vector<ar<int, 3>>& edges) {
	sort(edges.begin(), edges.end());
	int totalWeight = 0;
	int count = 0;
	DSU dsu(V);
	for(auto& [weight, a, b]: edges) {
		if(dsu.same(a, b)) continue;
		dsu.unite(a, b);
		totalWeight += weight;
		count++;
		if(count==V-1) return totalWeight;
	}
	return totalWeight;
}

int Prim(const vector<vector<ar<int, 2>>>& adjacencyList) {
	vector<int> d(V, INF);
	
	priority_queue<ar<int, 2>, vector<ar<int, 2>>, greater<ar<int, 2>>> pq;
	pq.push({d[0]=0, 0});
	int totalWeight=0;
	int count=0;
	while(pq.size()) {
		int node = pq.top()[1];
		int weight = pq.top()[0];
		pq.pop();
		if(d[node]!=weight) continue; // move on if this is old edge
		totalWeight += weight;
		d[node] = 0;
		count++;
		if(count == V) return totalWeight; 
		for(auto& [w, node2]:adjacencyList[node]) {
			if(w<d[node2]) {
				pq.push({d[node2]=w, node2});
			}
		}
	}
	return totalWeight;
}

Graph generateRandomGraph(double density, int maxWeight = 100000) {
	Graph g;
	g.adj.resize(V);
	set<pair<int, int>> existingEdges;
	random_device seed; // creates a seed
	mt19937 gen(seed()); // generates deterministic random numbers with higher randomness than rand()
	uniform_int_distribution<int> generateWeight(1, maxWeight);
	uniform_int_distribution<int> generateNode(0, V-1);

	int maxEdges = V*(V-1) / 2;
	int targetEdges = (int)(density * maxEdges);
	
	DSU dsu(V);
	int dsuComponents = V;
	while((int)existingEdges.size() < targetEdges) {
		int u = generateNode(gen);
		int v = generateNode(gen);
		if(u==v) continue;
		if(u>v) swap(u, v);
		if(existingEdges.count({u, v})) continue;
		if(dsuComponents>1) {
			if(dsu.same(u, v)) continue;
			dsu.unite(u, v);
			dsuComponents--;
		}
		int w = generateWeight(gen);
		g.edges.push_back({w, u, v});
		g.adj[u].push_back({w, v});
		g.adj[v].push_back({w, u});
		existingEdges.insert({u, v});
	}
	
	return g;
}

int main() {
	vector<double> densities = {0.0015, 0.002, 0.0025, 0.003, 0.0035, 0.004, 0.0045, 0.005, 0.0055, 0.006, 0.0065, 0.007, 0.0075, 0.008, 0.0085, 0.009, 0.0095, 0.01, 0.02, 0.04, 0.08, 0.16};
	int trials = 20;

	ofstream out("results.csv");
	out << "density,kruskal_time,prim_time\n";

	for(double density:densities) {
		double kruskalTotal = 0;
		double primTotal = 0;
		bool sanityCheck = 1;
		int kruskalVal, primVal;
		for(int i=0; i<trials; i++) {
			Graph g = generateRandomGraph(density);

			auto start = chrono::high_resolution_clock::now();
			primVal = Prim(g.adj);
			auto end = chrono::high_resolution_clock::now();
			primTotal += chrono::duration<double>(end-start).count();

			auto start2 = chrono::high_resolution_clock::now();
			kruskalVal = Kruskal(g.edges);
			auto end2 = chrono::high_resolution_clock::now();
			kruskalTotal += chrono::duration<double>(end2-start2).count();

			sanityCheck = kruskalVal == primVal;
		}

		out << density << "," << kruskalTotal/trials << "," << primTotal/trials << "\n";
		cout << "Correctness check " << (sanityCheck? "passed\n":"failed\n");
		cout << "Done density " << density << "\n";
	}
}
