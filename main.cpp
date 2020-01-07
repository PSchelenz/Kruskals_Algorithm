#include <iostream>
#include <fstream>
#include <time.h>
#define ISTRUE false

using namespace std;

int findCounter = 0;

class Node
{
public:
	float x;
	float y;
};

class Edge
{
public:
	int v1, v2;
	float cost;
	Edge() = default;
	Edge(const Edge *&edges)
	{
		if (edges != this)
		{
			this->v1 = edges->v1;
			this->v2 = edges->v2;
			this->cost = edges->cost;
		}
	}
};

class Graph
{
public:
	int nodes_amount, edges_amount;
	Node** nodes;
	Edge** edges;

	void InitEdges(int edges_amount)
	{
		this->edges_amount = edges_amount;
		edges = new Edge * [edges_amount];
	}

	Graph(int nodes_amount)
	{
		nodes = new Node * [nodes_amount];
		this->nodes_amount = nodes_amount;
	}
	~Graph()
	{
		for(int i = 0 ; i < this->edges_amount; i++)
			delete this->edges[i];
		
		for(int i = 0 ; i < this->nodes_amount; i++)
			delete this->nodes[i];
		
		delete[] this->edges;
		delete[] this->nodes;
	}
};

class Union_Find
{
public:
	int* representants;
	int* ranks;
	int nodes_amount;
	
	Union_Find(int n = 0)
	{
		nodes_amount = n;
		representants = new int[n];
		ranks = new int[n];
	}
	
	int find(int, bool);
	void unite(int, int, bool);
};

int Union_Find::find(int idx, bool pc)
{
	if (pc == true)
	{
		if (representants[idx] != idx)
			representants[idx] = find(representants[idx], pc);

		++findCounter;
		return representants[idx];
	}
	else
	{
		++findCounter;
		return representants[idx];
	}
}

void Union_Find::unite(int idx1, int idx2, bool ubr)
{
	if (ubr == true)
	{
		if (ranks[idx1] < ranks[idx2])
			representants[idx1] = idx2;
		else if (ranks[idx1] > ranks[idx2])
			representants[idx2] = idx1;
		else
		{
			representants[idx2] = idx1;
			++ranks[idx1];
		}
	}
	else
	{
		int hint = representants[idx2];
		for (int i = 0; i < nodes_amount; i++)
			if (representants[i] == hint)
				representants[i] = idx1;
	}
}

Edge **merges(Edge **first, Edge **second, int fSize, int sSize)
{
	Edge** sorted = new Edge * [fSize + sSize];
	for(int fIdx = 0, sIdx = 0, mIdx = 0; mIdx < fSize + sSize; mIdx++)
	{
		if (fIdx >= fSize)
			sorted[mIdx] = new Edge(*second[sIdx++]);

		else if (sIdx >= sSize)
			sorted[mIdx] = new Edge(*first[fIdx++]);

		else if (first[fIdx]->cost <= second[sIdx]->cost)
			sorted[mIdx] = new Edge(*first[fIdx++]);

		else
			sorted[mIdx] = new Edge(*second[sIdx++]);
	}

	return sorted;
}
                                                                                         //http://samouczekprogramisty.pl/podstawy-zlozonosci-obliczeniowej/
Edge **sorts(Edge **edges, int size)
{
	if (size <= 1)
		return edges;

	int firstSize = size / 2;
	Edge** first = new Edge * [firstSize];
	Edge** second = new Edge * [size - firstSize];

	for(int i = 0; i < firstSize; i++)
	{
		first[i] = new Edge(*edges[i]);
	}

	for(int i = 0; i < size - firstSize; i++)
	{
		second[i] = new Edge(*edges[firstSize + i]);
	}

	return merges(sorts(first, firstSize), sorts(second, size - firstSize), firstSize, size - firstSize);
}

void Kruskals_Algorithm(Graph *&graph)
{
	Union_Find magic(graph->nodes_amount);
	Edge **tree = new Edge*[graph->nodes_amount - 1];
	
	clock_t begin = clock();
	graph->edges = sorts(graph->edges, graph->edges_amount);
	clock_t end = clock();

	for(int i = 0; i < graph->nodes_amount; i++)
	{
		magic.representants[i] = i;
		magic.ranks[i] = 0;
	}

	int edges = 0, counter = 0;
	
	clock_t begin2 = clock();
	while(edges < graph->nodes_amount - 1 && counter < graph->edges_amount)
	{
		const int res_1 = magic.find(graph->edges[counter]->v1, ISTRUE);
		const int res_2 = magic.find(graph->edges[counter++]->v2, ISTRUE);
		findCounter += 2;

		if(res_1 != res_2)
		{
			tree[edges++] = graph->edges[counter - 1];
			magic.unite(res_1, res_2, ISTRUE);
		}
	}
	clock_t end2 = clock();
	cout << "Edges: " << edges << endl;
	float cost = 0;
	for (int i = 0; i < edges; i++)
	{
		cost += tree[i]->cost;
		cout << tree[i]->v1 << "--" << tree[i]->v2 << "==" << tree[i]->cost << endl;
	}
	cout << "Cost: " << cost << endl;
	cout << "Czas obliczen kroku sortujacego: " << (double)(end - begin) / CLOCKS_PER_SEC << " sekund" << endl;
	cout << "Czas obliczen glownej petli: " << (double)(end2 - begin2) / CLOCKS_PER_SEC << " sekund" << endl;
	cout << "Liczba wykonan operacji find: " << findCounter << endl;

	tree = nullptr;
	delete[] tree;
}

Graph* Initialization()
{
	int nodes_amount, edges_amount;
	ifstream file;
	file.open("g1.txt");
	file >> nodes_amount;
	Graph* graph = new Graph(nodes_amount);
	
	for (int i = 0; i < nodes_amount; i++)
	{
		graph->nodes[i] = new Node();
		file >> graph->nodes[i]->x >> graph->nodes[i]->y;
	}

	file >> edges_amount;
	graph->InitEdges(edges_amount);

	for (int i = 0; i < edges_amount; i++)
	{
		graph->edges[i] = new Edge();
		file >> graph->edges[i]->v1 >> graph->edges[i]->v2 >> graph->edges[i]->cost;
	}
	
	return graph;
}

int main()
{
	Graph* graph = Initialization();
	Kruskals_Algorithm(graph);
	
	delete graph;
	return 0;
}