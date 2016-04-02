#include <cstdlib>
#include <iostream>

using namespace std;

struct Edge
{
    int node1;
    int node2;
    int weight;
};

int find(int node); //finds the root of the given node in the adjacency list
void unionSets(int root1, int root2); //union the sets containing the two nodes in the adjacency list
void kruskalAlg(int node1, int node2); //implements Kruskal's algorithm given two nodes
void percolateUp(int index); //percolates a node up given its index
void percolateDown(int index); //percolates a node down given its index
void swapNodes(int a, int b); //helper method to swap two nodes in heap given their indexes

int numUsers; //number of users
int numEdges; //number of edges (relationships)
int size; //size of min heap
int *aList; //pointer to adjacency list
Edge *heapPtr; //pointer to min heap array

int main(int argc, char** argv) 
{
    cin >> numUsers; //Read in number of users
    cin >> numEdges; //read in number of edges
    Edge minHeap[numEdges+1]; //array of nodes and edges
    heapPtr = minHeap;
    for (int i=1;i<numEdges+1;i++) //read in all edges
    {
        cin >> minHeap[i].node1 >> minHeap[i].node2 >> minHeap[i].weight;
        percolateUp(i); //call percolate up on inserted edge to maintain invariant 
    }    

    int adjList[numUsers]; //make adjacency list
    aList = adjList;
    for (int i=0;i<numUsers;i++) //initialize roots to -1 in adjacency list
    {
        adjList[i] = -1;
    }
    
    size = numEdges;
    for (int i=1;i<numEdges+1;i++) //go through edges and implement Kruskal's algorithm
    {
        kruskalAlg(minHeap[1].node1,minHeap[1].node2); //call algorithm on smallest edge, which is root of minHeap
        swapNodes(1,size); //swap the root and last node
        size--; //last node is deleted, heap is now smaller
        percolateDown(1); //call percolate down on root
    } 
    return 0;
}

void percolateUp(int index)
{
    int parent = index/2;  //set parent index
    if (parent==0) //if the parent is beyond the root, return 
    {
        return;
    }
    else
    {
        Edge temp;
        if (heapPtr[index].weight<heapPtr[parent].weight) //if the current edge is less than its parent, swap
        {
            swapNodes(index,parent);
            percolateUp(parent); //recursively call percolate up 
        }
    }
}

void percolateDown(int index)
{
    int leftChild = index*2; //set indices for left and right child
    int rightChild = (index*2)+1;
    if (leftChild<=size||rightChild<=size)
    {
        if (leftChild==size) //left child is present, right child is not, check to switch with left child
        {
            if (heapPtr[leftChild].weight<heapPtr[index].weight) //left child smaller than current node, swap
            {
                swapNodes(leftChild,index);
                percolateDown(leftChild); //recursively call on left child
            }
        }
        else //left child smaller than right child, and left child is smaller than index
        if ((heapPtr[leftChild].weight<heapPtr[rightChild].weight)&&(heapPtr[leftChild].weight<heapPtr[index].weight))
        {
            swapNodes(leftChild,index);
            percolateDown(leftChild);
        }
        else //right child smaller than left child, and right child is smaller than index
        if((heapPtr[rightChild].weight<heapPtr[leftChild].weight)&&(heapPtr[rightChild].weight<heapPtr[index].weight))
        {
            swapNodes(rightChild,index);
            percolateDown(rightChild);
        }
    }
}

void swapNodes(int a, int b)
{
    if (a==b) //don't swap something in the same index
    {
        return;
    }
    Edge temp;
    temp.node1 = heapPtr[a].node1; 
    temp.node2 = heapPtr[a].node2;
    temp.weight = heapPtr[a].weight;
    heapPtr[a].node1 = heapPtr[b].node1;
    heapPtr[a].node2 = heapPtr[b].node2;
    heapPtr[a].weight = heapPtr[b].weight;
    heapPtr[b].node1 = temp.node1; 
    heapPtr[b].node2 = temp.node2;
    heapPtr[b].weight = temp.weight;
}

void kruskalAlg(int node1, int node2)
{
    int root1 = find(node1); //find the roots for each node 
    int root2 = find(node2);
    if (root1!=root2) //if roots are not the same, they are in different sets, so unionize and print
    {
        unionSets(root1,root2);
        cout << node1 << " " << node2 << endl; 
    }   
}

int find(int node)
{
    if (aList[node-1]<0)
    {
        return node;
    }
    else
    {
        return find(aList[node-1]);
    }
}

void unionSets(int root1, int root2)
{
    if (aList[root1-1]==aList[root2-1]) //if both are roots
    {
        if (root1<root2) //if root1 has the smaller index, it becomes the root
        {
            aList[root1-1] = aList[root1-1] + aList[root2-1];
            aList[root2-1] = root1;
        }
        else
        if (root2<root1) //if root2 has the smaller index, it becomes the root
        {
            aList[root2-1] = aList[root2-1] + aList[root1-1];
            aList[root1-1] = root2;
        }
    }
    else
    if (aList[root1-1]<aList[root2-1]) //if root1 has greater weight, it becomes new root
    {
        aList[root1-1] = aList[root1-1] + aList[root2-1];
        aList[root2-1] = root1;
    }
    else
    if (aList[root1-1]>aList[root2-1]) //if root2 has greater weight, it becomes new root
    {
        aList[root2-1] = aList[root2-1] + aList[root1-1];
        aList[root1-1] = root2;
    }
}