#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Node
{
    char letter;
    int weight;
    Node* leftChild;
    Node* rightChild;
};

void print(Node* root);
//test print method that prints resulting trie by in-order traversal (left, root, right)
Node* newTrieNode(char letter0,int weight0,Node* leftChild0,Node* rightChild0);
//makes a new trie node and returns a pointer to it
void percolateDown(int index); 
//percolate tree down after root is deleted and replaced with last element in heap,
//takes index of node in heap to be percolated down
void percolateUp(int index); 
//percolate tree up after trie node is inserted into last element, takes index
//of node in heap to be percolated up
void encode(); 
//encode first line input and print
void trieSearch(Node* current,char letter,string encode);
//helper function to recursively navigate trie for encoding
void decode(); 
//decode rest of lines in input file, 0 moves right, 1 moves left

string *linesPointer; //pointer to array of lines read in
Node *arrayPointer; //pointer to min-heap of nodes containing letters and weights
Node *trieRootPointer; //pointer to root of trie
int heapCounter; //number of items in node heap
int counter; //counter to keep track of lines read in

int main(int argc, char** argv) 
{
    string inputLines[150]; //150 limit of lines in input file
    linesPointer = inputLines;
    counter = 0; //counter to keep track of number of lines
    ifstream inStream(argv[1]);
    if (!inStream)
    {
        cout << "File not found. Program exiting." <<endl;
        exit(1);
    }
    while(inStream)
    {
        getline(inStream,inputLines[counter]); //get lines in input until EOF
        counter++; //add one to count number of lines put in
    }
    
    string firstLine = inputLines[0]; //firstLine is first line in input file
    int firstLength = firstLine.length()-1; //length of first line minus the new line character
    Node heapArray[firstLength]; //heap Array for letters of first line, minimum all letters (worst case all letters are unique)
    arrayPointer = heapArray;
    heapCounter = 0; //counter for heap array for insert, also the total number of unique characters 
    bool clear = true; //keeps track of whether or not a letter is inserted, true means letter is not duplicate and can be inserted
    for (int i=0;i<firstLength;i++)
    {
        for (int j=0;j<heapCounter;j++)
        {
            if (firstLine[i]==heapArray[j].letter)//if letter is already in array, add +1 to weight
            {
                heapArray[j].weight++;
                clear = false;
                break;
            }         
            clear = true; //cleared to add letter otherwise
        }
        if (clear==true) //if letter isn't already in array, we are cleared to add it
        {
            heapArray[heapCounter].letter = firstLine[i];
            heapArray[heapCounter].weight = 1;
            heapArray[heapCounter].leftChild = NULL;
            heapArray[heapCounter].rightChild = NULL;
            heapCounter++;
        }
    }
    
    Node temp; //temp node to be used for sorting
    for (int i=0;i<heapCounter-1;i++) //sort array using selection sort
    {
        for (int j=i+1;j<heapCounter;j++)
        {
            if (heapArray[j].weight<heapArray[i].weight)
            {
                temp.letter = heapArray[i].letter;
                temp.weight = heapArray[i].weight;
                heapArray[i].letter = heapArray[j].letter;
                heapArray[i].weight = heapArray[j].weight;
                heapArray[j].letter = temp.letter;
                heapArray[j].weight = temp.weight;
            }
            else
            if (heapArray[j].weight==heapArray[i].weight)
            {
                if (heapArray[j].letter<heapArray[i].letter)
                {
                    temp.letter = heapArray[i].letter;
                    temp.weight = heapArray[i].weight;
                    heapArray[i].letter = heapArray[j].letter;
                    heapArray[i].weight = heapArray[j].weight;
                    heapArray[j].letter = temp.letter;
                    heapArray[j].weight = temp.weight;
                }
            }
        }
    }
    
    while (heapCounter>0)
    {
        trieRootPointer = newTrieNode('R',0,NULL,NULL); //trieRootPointer points to new Trie Root
        Node *trieRightChild = newTrieNode(arrayPointer[0].letter,arrayPointer[0].weight,arrayPointer[0].leftChild,arrayPointer[0].rightChild); //make right child node to put in trie
        trieRootPointer->rightChild = trieRightChild; //trie root now has heap first element as right child
        trieRootPointer->weight = trieRootPointer->weight + trieRightChild->weight; //update trie root weight
        arrayPointer[0].weight = -1; //root of heap is now deleted
        if (arrayPointer[0].weight==-1&heapCounter==1) //check to get out of trie building
        {
            break;
        }
        arrayPointer[0].letter = arrayPointer[heapCounter-1].letter; //first element gets data from last element (swap))
        arrayPointer[0].weight = arrayPointer[heapCounter-1].weight;
        arrayPointer[0].leftChild = arrayPointer[heapCounter-1].leftChild;
        arrayPointer[0].rightChild = arrayPointer[heapCounter-1].rightChild;
        arrayPointer[heapCounter-1].weight = -1; //last element in heap is deleted
        heapCounter--; //heapCounter updates to number of nodes in heap
        percolateDown(0); //percolate down the root
        Node *trieleftChild = newTrieNode(arrayPointer[0].letter,arrayPointer[0].weight,arrayPointer[0].leftChild,arrayPointer[0].rightChild); //make left child node to put in trie
        trieRootPointer->leftChild = trieleftChild; //trie root now has heap first element as left child
        trieRootPointer->weight = trieRootPointer->weight + trieleftChild->weight; //update trie root weight
        arrayPointer[0].weight = -1; //root of heap deleted
        if (arrayPointer[0].weight==-1&heapCounter==1) //check to get out of trie building
        {
            break;
        }
        arrayPointer[0].letter = arrayPointer[heapCounter-1].letter; //first element gets data from last element (swap))
        arrayPointer[0].weight = arrayPointer[heapCounter-1].weight;
        arrayPointer[0].leftChild = arrayPointer[heapCounter-1].leftChild;
        arrayPointer[0].rightChild = arrayPointer[heapCounter-1].rightChild;
        arrayPointer[heapCounter-1].weight = -1; //last element in heap is deleted
        percolateDown(0);
        arrayPointer[heapCounter-1].letter = trieRootPointer->letter; //insert trie root into last element of heap
        arrayPointer[heapCounter-1].weight = trieRootPointer->weight;
        arrayPointer[heapCounter-1].leftChild = trieRootPointer->leftChild;
        arrayPointer[heapCounter-1].rightChild = trieRootPointer->rightChild;
        percolateUp(heapCounter-1); //call percolate up on last element inserted (trie root)
    }
    //print(trieRootPointer);
    encode();
    decode();
    //end program
    return 0;
}

void percolateDown(int index)
{
    int leftChild = (index*2)+1;
    int rightChild = (index*2)+2;
    Node temp;
    if (leftChild<=heapCounter||rightChild<=heapCounter)
    {
        if (leftChild<=heapCounter&&rightChild>=heapCounter) //left child is present, but right child is not. Check to switch with left child
        {
            if ((arrayPointer[leftChild].weight<arrayPointer[index].weight)&&arrayPointer[leftChild].weight!=-1)
            {
                temp.letter = arrayPointer[index].letter; //swap current index with its left child
                temp.weight = arrayPointer[index].weight;
                temp.leftChild = arrayPointer[index].leftChild;
                temp.rightChild = arrayPointer[index].rightChild;
                arrayPointer[index].letter = arrayPointer[leftChild].letter;
                arrayPointer[index].weight = arrayPointer[leftChild].weight;
                arrayPointer[index].leftChild = arrayPointer[leftChild].leftChild;
                arrayPointer[index].rightChild = arrayPointer[leftChild].rightChild;
                arrayPointer[leftChild].letter = temp.letter;
                arrayPointer[leftChild].weight = temp.weight;
                arrayPointer[leftChild].leftChild = temp.leftChild;
                arrayPointer[leftChild].rightChild = temp.rightChild;
                percolateDown(leftChild); //call percolate down on left child
            }
        }
        else //if left child is smaller than right child, leftChild smaller than current index, and it is not deleted, swap
        if((arrayPointer[leftChild].weight<arrayPointer[rightChild].weight)&&(arrayPointer[leftChild].weight<arrayPointer[index].weight)&&arrayPointer[leftChild].weight!=-1)
        {
            temp.letter = arrayPointer[index].letter; //swap current index with its left child
            temp.weight = arrayPointer[index].weight;
            temp.leftChild = arrayPointer[index].leftChild;
            temp.rightChild = arrayPointer[index].rightChild;
            arrayPointer[index].letter = arrayPointer[leftChild].letter;
            arrayPointer[index].weight = arrayPointer[leftChild].weight;
            arrayPointer[index].leftChild = arrayPointer[leftChild].leftChild;
            arrayPointer[index].rightChild = arrayPointer[leftChild].rightChild;
            arrayPointer[leftChild].letter = temp.letter;
            arrayPointer[leftChild].weight = temp.weight;
            arrayPointer[leftChild].leftChild = temp.leftChild;
            arrayPointer[leftChild].rightChild = temp.rightChild;
            percolateDown(leftChild); //call percolate down on left child
        }
        else //if right child is smaller than left child, rightChild smaller than index, and it is not deleted, swap
        if((arrayPointer[rightChild].weight<arrayPointer[leftChild].weight)&&(arrayPointer[rightChild].weight<arrayPointer[index].weight)&&arrayPointer[rightChild].weight!=-1)
        {
            temp.letter = arrayPointer[index].letter; //swap current index with its right child
            temp.weight = arrayPointer[index].weight;
            temp.leftChild = arrayPointer[index].leftChild;
            temp.rightChild = arrayPointer[index].rightChild;
            arrayPointer[index].letter = arrayPointer[rightChild].letter;
            arrayPointer[index].weight = arrayPointer[rightChild].weight;
            arrayPointer[index].leftChild = arrayPointer[rightChild].leftChild;
            arrayPointer[index].rightChild = arrayPointer[rightChild].rightChild;
            arrayPointer[rightChild].letter = temp.letter;
            arrayPointer[rightChild].weight = temp.weight;
            arrayPointer[rightChild].leftChild = temp.leftChild;
            arrayPointer[rightChild].rightChild = temp.rightChild;
            percolateDown(rightChild); //call percolate down on right child
        }
        else //if the left child is equal to right child, the index is smaller than them, and the right child is not deleted, swap
        if((arrayPointer[leftChild].weight==arrayPointer[rightChild].weight)&&(arrayPointer[rightChild].weight<arrayPointer[index].weight)&&arrayPointer[rightChild].weight!=-1)
        {
            temp.letter = arrayPointer[index].letter; //swap current index with its right child
            temp.weight = arrayPointer[index].weight;
            temp.leftChild = arrayPointer[index].leftChild;
            temp.rightChild = arrayPointer[index].rightChild;
            arrayPointer[index].letter = arrayPointer[rightChild].letter;
            arrayPointer[index].weight = arrayPointer[rightChild].weight;
            arrayPointer[index].leftChild = arrayPointer[rightChild].leftChild;
            arrayPointer[index].rightChild = arrayPointer[rightChild].rightChild;
            arrayPointer[rightChild].letter = temp.letter;
            arrayPointer[rightChild].weight = temp.weight;
            arrayPointer[rightChild].leftChild = temp.leftChild;
            arrayPointer[rightChild].rightChild = temp.rightChild;
            percolateDown(rightChild); //call percolate down on right child
        }
    }
}

void percolateUp(int index)
{
    int which = 0; //decides if the current index is a left or right child. 0 is right, 1 is left;
    if (index%2)
    {
        which = 1; //index is odd, so index is a left child
    }
    int left = index/2;
    int right = (index/2)-1;
    Node temp;
    if (left>=0||right>=0)
    {   // if child is the left child and parent is less than child and it is not deleted, swap
        if (which==1&&(arrayPointer[index].weight<arrayPointer[left].weight)&&arrayPointer[left].weight!=-1)
        {
            temp.letter = arrayPointer[left].letter;
            temp.weight = arrayPointer[left].weight;
            temp.leftChild = arrayPointer[left].leftChild;
            temp.rightChild = arrayPointer[left].rightChild;
            arrayPointer[left].letter = arrayPointer[index].letter;
            arrayPointer[left].weight = arrayPointer[index].weight;
            arrayPointer[left].leftChild = arrayPointer[index].leftChild;
            arrayPointer[left].rightChild = arrayPointer[index].rightChild;
            arrayPointer[index].letter = temp.letter;
            arrayPointer[index].weight = temp.weight;
            arrayPointer[index].leftChild = temp.leftChild;
            arrayPointer[index].rightChild = temp.rightChild;
            percolateUp(left); //call percolate up on parent
        }
        else //if child is right child and parent is less than child and it is not deleted, swap
        if(which==0&&(arrayPointer[index].weight<arrayPointer[right].weight)&&arrayPointer[right].weight!=-1)
        {
            temp.letter = arrayPointer[right].letter;
            temp.weight = arrayPointer[right].weight;
            temp.leftChild = arrayPointer[right].leftChild;
            temp.rightChild = arrayPointer[right].rightChild;
            arrayPointer[right].letter = arrayPointer[index].letter;
            arrayPointer[right].weight = arrayPointer[index].weight;
            arrayPointer[right].leftChild = arrayPointer[index].leftChild;
            arrayPointer[right].rightChild = arrayPointer[index].rightChild;
            arrayPointer[index].letter = temp.letter;
            arrayPointer[index].weight = temp.weight;
            arrayPointer[index].leftChild = temp.leftChild;
            arrayPointer[index].rightChild = temp.rightChild;
            percolateUp(right);
        }
    }
}

Node* newTrieNode(char letter0,int weight0,Node* leftChild0,Node* rightChild0)
{
    Node* newNodePointer = new Node;
    newNodePointer->letter = letter0;
    newNodePointer->weight = weight0;
    newNodePointer->leftChild = leftChild0;
    newNodePointer->rightChild = rightChild0;
    return newNodePointer;
}

void print(Node* root)
{
    if (root!=NULL)
    {
        if (root->leftChild!=NULL)
        {
            print(root->leftChild);
        } 
         cout << root->letter << " " << root->weight << endl;
        if (root->rightChild!=NULL)
        {
            print(root->rightChild);
        }        
    }
}

void encode()
{
    string toEncode = linesPointer[0];
    for (int i=0;i<toEncode.length()-1;i++) //take each letter in first line and pass to trieSearch
    {
        trieSearch(trieRootPointer,toEncode[i],"");
    }
    cout << endl;
}

void trieSearch(Node* current,char letter,string encode)
{
    if (current!=NULL) 
    {
        if (current->letter!=letter) //search left and right subtrees, passing on the current encoding being built
        {
            string leftEncode = encode + "1";
            trieSearch(current->leftChild,letter,leftEncode);
            string rightEncode = encode + "0";
            trieSearch(current->rightChild,letter,rightEncode);
        }
        else 
        if(current->letter==letter) //if found, print the encoded string
        {
            cout << encode;
        }
    }
}

void decode()
{
    string decode;
    Node* current = trieRootPointer; //set a pointer to point to root of trie
    for (int i=1;i<counter-1;i++) //for every line in input file after the first
    {
        decode = linesPointer[i];
        for (int j=0;j<decode.length();j++) //go through each letter and navigate trie accordingly
        {
            if (decode[j]=='0') //if code is 0, go left
            {
                current = current->rightChild;
                if (current->leftChild==NULL&&current->rightChild==NULL) //if we are at a leaf, print the letter and reset pointer back to root of trie
                {
                    cout << current->letter;
                    current = trieRootPointer;
                }
            }
            else 
            if (decode[j]=='1') // if code is 1, go right
            {
                current = current->leftChild;
                if (current->leftChild==NULL&&current->rightChild==NULL)
                {
                    cout << current->letter;
                    current = trieRootPointer;
                }
            }
        }
        cout << endl;
    }
}