# B-tree
While one or two levels of index are often very helpful in speeding up queries on indexed files or blocks,
there is a more general structure that is commonly used in commercial systems.
The general family of data structures is called a B-tree, and the particular
variant that is most often used and has been implemented here is known as a B+ tree.

However here we only deal with integers rather than pointers to blocks for educative purpose.

### Implementation Specifications
- INSERT X : insert X into the B+ tree
- FIND X : print YES if X is already inserted, else NO
- COUNT X : print number of occurrences of X in B+tree
- RANGE X Y : print number of elements in range x to y (both x and y included)

### Compile Instruction
```sh
g++ B+tree.cpp
```

### Run Instruction
```sh
./a.out
```
Note that the program is set to use stdin and stdout for input and output purposes respectively.

### Implementation details
It is better to look at code and understand the details, I have tried to add comments wherever suitable.

Implementation has 2 structures:
- pair<int,int> key : to store an element and the corresponding count of the element to handle duplicates 
- node :
(i) vector<node*> nxt : contains the pointers to the children
(ii) vector<pair<int,int> > keys : contains the {key,count} pairs
(iii) bool is_leaf : indicative of leaf or not
Incase of a leaf, if it contains k keys, nxt[k] has the pointer to next adjacent node

Implementation has 4 functions:
- node* newnode() : returns a new node with declared elements
- int lookup(node* cur,int key) : looks for a given key within the node cur and returns the index
- int range_query(node* root,int lower_bound,int upper_bound) : count all the occurences of elements in
the range (lower_bound,upper_bound)
iterative function has the following steps:
->descends down from the root to corresponding leaf to get lower_bound.
->Further moves from leaf to leaf using the pointers untill a leaf node is reached that is either NULL
or contains a key >= upper_bound.
Note that this function itself helps answer the queries : 
FIND X : check range_query(root,x,x) > 0
COUNT X : range_query(root,x,x)
RANGE X Y : range_query(root,x,y)
- node* copy(node* tmp,node* cur) : copies node cur by value to the node tmp
- pair<node*,int> insert(node* root,int key) : inserts key into the Btree,
returns the pair {cur,key} : cur is the root of the subtree for a recursive stage, while key is the 
integer key to be inserted in the parent node, key -INF denotes that no key is to be inserted in the parent 
node
recursive function has the following steps :
->descends down from the root to corresponding leaf tracing the key to be inserted.
->If the key is already present, raise the count
->if the leaf is not FULL, insert the key with count 1, change next pointer as well
->Otherwise we need to split the leaf node, for our case, the split occurs as (1,2)
->For an internal non leaf node, check if a key has to be inserted or not
(i) If not to be inserted, no further effort in insertion
(ii) Else, if the current node is not FULL, insert the key and return key as -INF
(iii) If FULL, a spliting is required, there are 3 keys and 4 children pointers, we split it as
1 key, 2 pointers and 1 key, 2 pointers between 2 nodes, we pass the middle key (in sorted order) to the 
parent node.
