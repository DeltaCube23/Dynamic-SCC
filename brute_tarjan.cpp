#include <bits/stdc++.h>

using namespace std;

int counter = 0;
const int mx = 5e5 + 5; //size limit
vector<int> adj[mx];
int visited[mx], instack[mx], disc[mx], lowlink[mx], comp[mx];
//adj - adjacency list of the graph
//visitied - indicates if a node is visited or not
//instack - indicates if a node is currently in the stack or not
//disc - tells discovery time of that node
//lowlink - tells the lowlink value of that node
//comp - indicates which SCC the node belongs to

int yeah = 0, nah = 0;

void dfs(int u, stack<int> &st) //Tarjan's Algorithm DFS
{
	disc[u] = lowlink[u] = ++counter;
	st.push(u);
	visited[u] = 1;
	instack[u] = 1;
	
	for(auto v : adj[u])
	{
		if(visited[v] == 0)
		{
			dfs(v, st);
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

void reset(int n) //To reset all array values and counter for performing Tarjan's again
{
	for(int i=1; i<=n; i++)
	{
		visited[i] = 0;
		instack[i] = 0;
		disc[i] = 0;
		lowlink[i] = 0;
	}
	counter = 0;
}

void insert(int u, int v, int n) // To add an edge
{
	adj[u].push_back(v);
	
	reset(n);
	
	stack<int> st;

	for(int i=1; i<=n; i++)
	{
		if(!visited[i])
			dfs(i, st);
	}
}

void remove(int u, int v, int n) // To remove an edge
{
	for(int i = 0; i < (int)adj[u].size(); i++) 
	{
        if (adj[u][i] == v) 
        {
            adj[u].erase(adj[u].begin() + i);
            break;
        }
    }
	
	reset(n);
	
	stack<int> st;
	
	for(int i=1; i<=n; i++)
	{
		if(!visited[i])
			dfs(i, st);
	}
}

void query(int u, int v) // Query to check if 2 points are in same SCC or not
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
	freopen("input-graphs/wikiin.txt","r",stdin);
    freopen("wikiout.txt","w",stdout);
    
	int n, e;
	cin>>n>>e;
	
	for(int i=0; i<e; i++)
	{
		int a, b;
		cin>>a>>b;
		adj[a].push_back(b);
	}
	
	stack<int> st;
	
	for(int i=1; i<=n; i++)
	{
		if(!visited[i])
			dfs(i, st);
	}
	
	//all same comp values belong to one SCC
	
	// 1 - insert(u,v), 2 - remove(u,v), 3 - query(u,v)
	
	start = clock(); // Start time
	
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
	
	getCurrentTime(); // Get total process time
	cout<<"\n"<<yeah<<" "<<nah;
	
	return 0;
}
			
