#include <bits/stdc++.h>

using namespace std;

int counter = 0;
const int mx = 5e5 + 5; //size limit
vector<int> adj[mx], scc_nodes[mx];
vector<int> comp_graph[mx];
int visited[mx], instack[mx], disc[mx], lowlink[mx], comp[mx];
set<int> comp_nodes;

int comp_visited[mx], comp_instack[mx], comp_disc[mx], comp_lowlink[mx], comp_comp[mx];

int yeah = 0, nah = 0;

void tarjan(int u, stack<int> &st)
{
	disc[u] = lowlink[u] = ++counter;
	st.push(u);
	visited[u] = 1;
	instack[u] = 1;
	
	for(auto v : adj[u])
	{
		if(visited[v] == 0)
		{
			tarjan(v, st);
			lowlink[u] = min(lowlink[u], lowlink[v]);
		}
		else if(instack[v])
			lowlink[u] = min(lowlink[u], disc[v]); //This condition in only specific for articulation points
			// for bridges and SCC we can use lowlink[u]= min(lowlink[u], lowlink[v])
			// But here the definition of lowlink will obviously change
	}
	
	if(disc[u] == lowlink[u])
	{
		int node;
		do{
			node = st.top();
			st.pop();
			//cout<<node<<" ";
			instack[node] = 0;
			comp[node] = u;
		}while(node != u);
		//cout<<"\n";
	}
	
}

//To reset all values to perfrom tarjan again
void reset(int origin)
{
	for(int i : scc_nodes[origin])
	{
		visited[i] = 0;
		instack[i] = 0;
		disc[i] = 0;
		lowlink[i] = 0;
	}
	counter = 0;
}


void super_join(int u, stack<int> &comp_st)
{
	comp_disc[u] = comp_lowlink[u] = ++counter;
	comp_st.push(u);
	comp_visited[u] = 1;
	comp_instack[u] = 1;
	
	for(auto v : comp_graph[u])
	{
		if(comp_visited[comp[v]] == 0)
		{
			super_join(comp[v], comp_st);
			comp_lowlink[u] = min(comp_lowlink[u], comp_lowlink[comp[v]]);
		}
		else if(comp_instack[comp[v]])
			comp_lowlink[u] = min(comp_lowlink[u], comp_disc[comp[v]]); //This condition in only specific for articulation points
			// for bridges and SCC we can use lowlink[u]= min(lowlink[u], lowlink[v])
			// But here the definition of lowlink will obviously change
	}
	
	if(comp_disc[u] == comp_lowlink[u])
	{
		int node;
		do{
			node = comp_st.top();
			comp_st.pop();
			//cout<<node<<" ";
			comp_instack[node] = 0;
			comp_comp[node] = u;
		}while(node != u);
		//cout<<"\n";
	}
}


void reset_comp_graph()
{
	for (int i : comp_nodes)
	{
		comp_visited[i] = 0;
		comp_instack[i] = 0;
		comp_disc[i] = 0;
		comp_lowlink[i] = 0;
		comp_comp[i] = i;
	}
	counter = 0;
}


void insert(int u, int v, int n)
{
	adj[u].push_back(v); // add edge
	
	if(comp[u] == comp[v]) // if edge within component then do nothing
	{
		return;
	}
	else
	{
		comp_graph[comp[u]].push_back(v);
		
		reset_comp_graph();
		
		stack<int> comp_st;
		
		super_join(comp[u], comp_st);
		
		//reconfiguring all data structures after any possible joins
		
		int origin_comp = comp_comp[comp[u]]; // all merged componenets belong here
		
		vector <int> temp_graph;
		//t1 = clock();
		
		for(int i : comp_nodes)
		{	
			if(comp_comp[i] != i && comp_comp[i] == origin_comp)
			{
				for (int vertex : scc_nodes[i])
				{
					comp[vertex] = origin_comp;
					scc_nodes[origin_comp].push_back(vertex);
				}
				scc_nodes[i].clear();
			
			}
		}
		
		
		for (auto j : comp_graph[origin_comp])
		{
			if(origin_comp != comp_comp[comp[j]])
			{
				temp_graph.push_back(j);
			}
		}
		comp_graph[origin_comp].clear();
		
		for (auto j : temp_graph)
		{
			comp_graph[origin_comp].push_back(j);
		}
		temp_graph.clear();
		
		for(int i : comp_nodes)
		{
			if(comp_comp[i] == origin_comp && i != origin_comp)
			{
				for (auto j : comp_graph[i])
				{
					if(comp_comp[i] != comp_comp[comp[j]])
					{
						comp_graph[origin_comp].push_back(j);
					}
				}
				comp_graph[i].clear();
			}
		}
		
		comp_nodes.insert(origin_comp);
		
	}
}

//Runs tarjan for only 1 component
void improved_tarjan(int u, int origin, stack<int> &st)
{
	disc[u] = lowlink[u] = ++counter;
	st.push(u);
	visited[u] = 1;
	instack[u] = 1;
	
	for(auto v : adj[u])
	{
		if(comp[v] == origin && visited[v] == 0)
		{
			improved_tarjan(v, origin, st);
			lowlink[u] = min(lowlink[u], lowlink[v]);
		}
		else if(instack[v])
			lowlink[u] = min(lowlink[u], disc[v]); //This condition in only specific for articulation points
			// for bridges and SCC we can use lowlink[u]= min(lowlink[u], lowlink[v])
			// But here the definition of lowlink will obviously change
	}
	
	if(disc[u] == lowlink[u])
	{
		int node;
		do{
			node = st.top();
			st.pop();
			//cout<<node<<" ";
			instack[node] = 0;
			comp[node] = u;
		}while(node != u);
		//cout<<"\n";
	}
	
}


void remove(int u, int v, int n)
{
	bool happen = false;
	for(int i = 0; i < (int)adj[u].size(); i++) 
	{
        if(adj[u][i] == v) 
        {
            adj[u].erase(adj[u].begin() + i); // removes the edge 
            happen = true;
            break;
        }
    }
    
    if(!happen)
    return;
    
    if(comp[u] != comp[v]) // Deleted from different component no major effect
    {
		int index = comp[u];
		//remove edge from u's component inter component list
		for(int i = 0; i < (int)comp_graph[index].size(); i++)
		{
			if(comp_graph[index][i] == v)
			{
				comp_graph[index].erase(comp_graph[index].begin() + i); //removes the edge
				break;
			}
		}
	}
	else // Deletion from same component could break down SCC
	{
		//remove from intracomponent edges ?
		//run improved tarjan's on only those nodes which belong to this component
		int origin = comp[v];
		reset(origin); // to reset all necessary array values
		stack<int> st;
		vector<int> temp; // to save all nodes in current scc
		
		for(int node : scc_nodes[origin])
		{
			temp.push_back(node);
			if(!visited[node])
				improved_tarjan(node, origin, st);
		}
		
		//reconfigure scc_nodes and inter_comp edges
		comp_graph[origin].clear();
		scc_nodes[origin].clear();
		
		for(auto itr = comp_nodes.begin(); itr != comp_nodes.end(); itr++)
		{
			if(*itr == origin)
			{
				comp_nodes.erase(itr);
				break;
			}
		}
		
		for(int node : temp)
		{
			scc_nodes[comp[node]].push_back(node);
			comp_nodes.insert(comp[node]);
			for(auto j : adj[node])
			{
				if(comp[j] != comp[node])
				{
					comp_graph[comp[node]].push_back(j);
				}
			}
		}
		
	}
	
}

// Check if u and v are in same component
void query(int u, int v)
{
	if(comp[u] == comp[v])
	{
		cout<<"YES\n";
		yeah++;
	}
	else
	{
		cout<<"NO\n";
		nah++;
	}
}


clock_t start;

void getCurrentTime() 
{
	double value = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout<<fixed<<setprecision(6)<<value<<"\n";
}



int main()
{
	freopen("input-graphs/p2p5.txt","r",stdin);
    freopen("p2p5out.txt","w",stdout);
    
	int n, e;
	cin>>n>>e;
	
	for(int i = 0; i < e; i++)
	{
		int a, b;
		cin>>a>>b;
		adj[a].push_back(b);
	}
	
	stack<int> st;
	
	// Run initial Tarjan
	for(int i = 1; i <= n; i++)
	{
		if(!visited[i])
			tarjan(i, st);
	}
	
	// Add inter-component edges based on initial results
	for(int i = 1; i <= n; i++)
	{
		comp_nodes.insert(comp[i]);
		
		scc_nodes[comp[i]].push_back(i);
		
		for(auto j : adj[i])
		{
			if(comp[j] != comp[i])
			{
				comp_graph[comp[i]].push_back(j);
			}
		}
	}
		
	//Precomputation time
	//all same comp values belong to one SCC
	
	//1 - insert(u,v), 2 - remove(u,v), 3 - query(u,v)
	start = clock(); // start measuring time
	
	int queries;
	cin>>queries;
	
	while(queries--) 
	{
		int ch, u, v;
		
		cin>>ch>>u>>v;

		if(ch == 1)
		{
			insert(u, v, n);
		}
		else if(ch == 2)
		{
			remove(u, v, n);
		}
		else if(ch == 3)
		{
			query(u, v);	
		}
	}
	
	getCurrentTime(); // get total time of process
	cout<<"\n"<<yeah<<" "<<nah<<"\n";
	
	return 0;
}
