#include <bits/stdc++.h>

using namespace std;

int counter = 0;
const int mx = 2e5 + 5;
vector<int> adj[mx], scc_nodes[mx];
vector<pair <int, int> > inter_comp[mx];
int visited[mx], instack[mx], disc[mx], lowlink[mx], comp[mx];


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

// To merge 2 component together
void join(int u, int v)
{
	int origin = comp[u];
	int transfer = comp[v];
	for(int vertex : scc_nodes[transfer])
	{
		scc_nodes[origin].push_back(vertex);
		comp[vertex] = origin;
	}
	scc_nodes[transfer].clear();
	
	vector<pair <int, int> >temp;
	//add inter-comp edges of both SCCs to temp
	for(auto edge : inter_comp[origin])
		temp.push_back(edge);
	
	for(auto edge : inter_comp[transfer])
		temp.push_back(edge);
	
	inter_comp[origin].clear();
	inter_comp[transfer].clear();
		
	//Now add only the relevant edges back to inter-comp
	int a, b;
	for(auto edge : temp)
	{
		a = edge.first;
		b = edge.second;
		if(comp[a] != comp[b])
		{
			inter_comp[origin].push_back({a,b});
		}
	}	
	
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
		//add to intercomponent edges and check if there is reverse
		//intercomponent edge from the other one
		//if so then merge SCC's
		//else leave it as it is
		inter_comp[comp[u]].push_back({u,v});
		
		bool flag = false;
		for(auto edge : inter_comp[comp[v]])
		{
			int to_node = edge.second;
			if(comp[to_node] == comp[u])
			{
				flag = true;
				break;
			}
		}
		
		if(flag)
			join(u, v);
		
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
	for(int i = 0; i < (int)adj[u].size(); i++) 
	{
        if(adj[u][i] == v) 
        {
            adj[u].erase(adj[u].begin() + i); // removes the edge 
            break;
        }
    }
    
    if(comp[u] != comp[v]) // Deleted from different component no major effect
    {
		int index = comp[u];
		//remove edge from u's component inter component list
		for(int i = 0; i < (int)inter_comp[index].size(); i++)
		{
			if(inter_comp[index][i].first == u && inter_comp[index][i].second == v)
			{
				inter_comp[index].erase(inter_comp[index].begin() + i); //removes the edge
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
		inter_comp[origin].clear();
		scc_nodes[origin].clear();
		
		for(int node : temp)
		{
			scc_nodes[comp[node]].push_back(node);
			for(auto j : adj[node])
			{
				if(comp[j] != comp[node])
				{
					inter_comp[comp[node]].push_back({node,j});
				}
			}
		}
		
	}
	
}

// Check if u and v are in same component
void query(int u, int v)
{
	if(comp[u] == comp[v])
		cout<<"YES\n";
	else
		cout<<"NO\n";
}


clock_t start;

void getCurrentTime() 
{
	double value = (double)(clock() - start) / CLOCKS_PER_SEC;
	cout<<fixed<<setprecision(6)<<value<<"\n";
}


int main()
{
	freopen("p2p.txt","r",stdin);
    freopen("p2pout.txt","w",stdout);
    
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
		scc_nodes[comp[i]].push_back(i);
		for(auto j : adj[i])
		{
			if(comp[j] != comp[i])
			{
				inter_comp[comp[i]].push_back({i,j});
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
			insert(u, v, n);
		else if(ch == 2)
			remove(u, v, n);
		else if(ch == 3)
			query(u, v);	
	}
	
	getCurrentTime(); // get total time of process
	
	return 0;
}
