#include<bits/stdc++.h>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
 
// for pair comparison function(ascending order) use return (i1.ff < i2.ff);
 
const int ORDER = 3;
const int INF = 1e9+9;

typedef struct node
{
	vector<node*> nxt; // contains the pointers to the children
	vector<pair<int,int> > keys; // contains the {key,count} pairs
	bool is_leaf; // indicative of leaf or not
	// incase of a leaf, if it contains k keys, nxt[k] has the pointer to next adjacent node
}node;

node* newnode()
{
	node* tmp = new node;
	for(int i=0;i<ORDER;++i)
		tmp->nxt.push_back(NULL);
	tmp->is_leaf = false;
	return tmp;
}

int lookup(node* cur,int key)
{
	if((cur->keys).front().first>key)
		return 0;
	else if((cur->keys).back().first<=key)
		return (int)(cur->keys).size();
	for(int i=0;i<(int)cur->keys.size()-1;++i)
		if(cur->keys[i].first<=key && cur->keys[i+1].first>key)
			return i+1;
}

int range_query(node* root,int lower_bound,int upper_bound)
{
	node* cur = root;
	// descending from the root to leaf
	while(!cur->is_leaf)
		cur = cur->nxt[lookup(cur,lower_bound)];
	int index = -1;
	int count = 0;

	// finding the index at which key >= lower_bound
	for(int i=0;i<(int)cur->keys.size();++i)
	{
		if(cur->keys[i].first >= lower_bound)
		{ 
			index = i;
			break;
		} 
	}

	// checking if there is a key at this leaf node itself whose key > upper_bound
	for(int i=index;index >= 0 && i<(int)cur->keys.size();++i)
	{
		if(cur->keys[i].first > upper_bound)
			return count;
		else
			count += cur->keys[i].second;
	}
	
	// changing pointers until a leaf is reached which is NULL or has a key > upper_bound
	cur = cur->nxt[(int)(cur->keys).size()];
	while(cur != NULL && (cur->keys).back().first <= upper_bound)
	{
		for(int i=0;i<(int)cur->keys.size();++i)
			count += cur->keys[i].second;
		cur = cur->nxt[(int)(cur->keys).size()];
	}

	// if a NULL leaf is reached
	if(cur == NULL)
		return count;

	// if the leaf reached has a key > upper_bound
	for(int i=0;i<(int)cur->keys.size();++i)
		if(cur->keys[i].first <= upper_bound)
			count += cur->keys[i].second;

	return count;
}

node* copy(node* tmp,node* cur)
{
	for(int i=0;i<ORDER;++i)
		tmp->nxt[i] = cur->nxt[i];
	tmp->is_leaf = cur->is_leaf;
	tmp->keys = cur->keys;
	return tmp;
}

pair<node*,int> insert(node* root,int key)
{
	node* cur = root;
	if(cur->is_leaf)
	{
		// if key is already present, if so just raise count
		for(int i=0;i<(int)cur->keys.size();++i)
		{
			if(cur->keys[i].first == key)
			{
				cur->keys[i].second += 1;
				return make_pair(root,-INF);
			}
		}

		// if the leaf is not FULL, insert the key with count 1, change next pointer as well
		if((int)cur->keys.size()<ORDER-1)
		{
			cur->keys.push_back(make_pair(key,1));
			sort(cur->keys.begin(),cur->keys.end());
			cur->nxt[(int)(cur->keys).size()] = cur->nxt[(int)(cur->keys).size()-1];
			cur->nxt[(int)(cur->keys).size()-1] = NULL;
			return make_pair(root,-INF);
		}

		// split leaf node
		vector<pair<int,int> > tmp = cur->keys;
		tmp.push_back(make_pair(key,1));
		sort(tmp.begin(),tmp.end());
		cur->keys.clear();
		int j = (int)tmp.size()/2;
		for(int i=0;i<j;++i)
			cur->keys.push_back(tmp[i]);
		node* adj_node = newnode();
		for(int i=j;i<(int)tmp.size();++i)
			adj_node->keys.push_back(tmp[i]);
		adj_node->nxt[(int)adj_node->keys.size()] = cur->nxt[ORDER-1];
		adj_node->is_leaf = true;
		cur->nxt[ORDER-1] = NULL;
		cur->nxt[(int)(cur->keys).size()] = adj_node;
		return make_pair(adj_node,adj_node->keys[0].first);
	}

	// descending from the root to leaf 
	pair<node*,int> connect = insert(cur->nxt[lookup(cur,key)],key);
	node* child_node = connect.first;
	key = connect.second;
	// if there is no splitting of nodes at the immediate level beneath
	if(key == -INF)
		return make_pair(cur,-INF);

	// if the current node is not FULL
	if((int)cur->keys.size() != ORDER-1)
	{
		int insert_index = lookup(cur,key);
		cur->keys.push_back({0,0});
		for(int i=(int)cur->keys.size();i>=insert_index+2;i--)
		{
			cur->nxt[i] = cur->nxt[i-1];
			cur->keys[i-1] = cur->keys[i-2];
		}
		cur->nxt[insert_index+1] = child_node;
		cur->keys[insert_index] = {key,1};
		return make_pair(cur,-INF);
	}

	// if splitting of current node is required
	int j = (ORDER+1)/2;
	node* tmp = newnode();
	tmp = copy(tmp,cur);
	int insert_index = lookup(cur,key);
	tmp->keys.push_back({0,0});
	tmp->nxt.push_back(tmp->nxt[ORDER-1]);
	for(int i=(int)tmp->keys.size();i>=insert_index+2;i--)
	{
		tmp->nxt[i] = tmp->nxt[i-1];
		tmp->keys[i-1] = tmp->keys[i-2];
	}
	tmp->nxt[insert_index+1] = child_node;
	tmp->keys[insert_index] = {key,1};

	// modifying the current node
	cur->keys.clear();
	for(int i=0;i<=j-2;++i)
		cur->keys.push_back(tmp->keys[i]);
	for(int i=0;i<=j-1;++i)
		cur->nxt[i] = tmp->nxt[i];
	cur->is_leaf = false;

	// creating the new neighbouring node
	node* adj_node = newnode();
	for(int i=j;i<(int)tmp->keys.size();++i)
		adj_node->keys.push_back(tmp->keys[i]);
	int pt_cnt = 0;
	for(int i=j;i<=ORDER;++i)
		adj_node->nxt[pt_cnt++] = tmp->nxt[i];
	cur->is_leaf = false;

	return make_pair(adj_node,tmp->keys[j-1].first);
}

node* insert_util(node* root, int key)
{
	pair<node*,int> connect = insert(root,key);
	node* child_node = connect.first;
	key = connect.second;
	if(key == -INF)
		return root;
	node* new_root = newnode();
	new_root->keys.push_back(make_pair(key,1));
	new_root->nxt[0] = root;
	new_root->nxt[1] = child_node;
	return new_root;
}

int main()
{
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);

	node* root = newnode();
	root->is_leaf = true;
	string query;
	while(cin >> query)
	{
		int x,y;
		if(query == "INSERT")
		{
			cin >> x;
			root = insert_util(root,x);
		}
		else if(query == "FIND")
		{
			cin >> x;
			if(range_query(root,x,x))
				cout << "YES";
			else
				cout << "NO";
		}
		else if(query == "COUNT")
		{
			cin >> x;
			cout << range_query(root,x,x);
		}
		else if(query == "RANGE")
		{
			cin >> x >> y;
			cout << range_query(root,x,y);
		}
		if(query != "INSERT")
			cout << "\n";
	}
	return 0;
}