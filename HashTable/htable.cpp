// htable.cpp by Derek Wong (5516117), 2/4/14

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sstream>
using namespace std;

struct KeyValue
{
    int phoneNum;
    string name;
};

void insert(int key,string value);
//inserts into table using key, value and prints results
void lookup(int key);
//looks up key-value pair using key
void keyDelete(int key);
//marks value as deleted in pair for given key
void printTable();
//prints table in format: (key,value)...
KeyValue* rehashTable();
//rehashes table by doubling its size and rehashing all old values in order
//returns a pointer to the new array
int throughArray(int key);
//helper method to go through array, returns index of found key, -1 otherwise
void insertHelper(int key,string value);
//helper method to insert key-value pairs  using has function 
//and resolve collisions through linear probing

int tableSize = 5;
double numOfValues = 0;
KeyValue *tablePointer;

int main(int argc, char** argv) 
{
    string inputLine;
    string inputTokens[3];
    
    string command = "";
    int key = 0;
    string value = "";    
    
    KeyValue currentTable[tableSize];
    tablePointer = currentTable;
    for (int i=0;i<tableSize;i++) //initialize table to blanks
    {
        currentTable[i].phoneNum = -1;
        currentTable[i].name = "";
    }
    
    while(true)
    {
        getline(cin, inputLine);
        int i = 0;
        stringstream ssin(inputLine);
        while (ssin.good() && i<3)
        {
            ssin >> inputTokens[i];
            i++;
        }
        command = inputTokens[0];
        key = atoi(inputTokens[1].c_str());
        value = inputTokens[2];
        if (command == "insert")
        {
            insert(key,value); 
        }
        else
        if (command == "lookup")
        {
            lookup(key);
        }
        else
        if (command == "delete")
        {
            keyDelete(key);
        }
        else
        if (command == "print")
        {
            printTable();
        }
    }
    return 0;
}

void insert(int key,string value)
{
    int index = throughArray(key);
    if (index != -1)
    {
        cout << "item already present" << endl;
    }
    else
    {
        insertHelper(key,value);
        cout << "item successfully inserted" << endl;
        numOfValues++;
        double loadFactor = numOfValues/tableSize;
        if (loadFactor > 0.7)
        {
            tablePointer = rehashTable();
        }
    }
}

void lookup(int key)
{
    int index = throughArray(key);
    if (index != -1)
    {
        cout << "item found; " << tablePointer[index].name << " " << index << endl;  
    }
    else
    {
        cout << "item not found" << endl;
    }
}

void keyDelete(int key)
{
     int index = throughArray(key);
     if (index != -1)
     {
         tablePointer[index].name = "deleted";
         tablePointer[index].phoneNum = -1;
         cout << "item successfully deleted" << endl;
     }
     else
     {
         cout << "item not present in the table" << endl;
     }
}

void printTable()
{
    for (int i=0;i<tableSize;i++)
    {
        int key = tablePointer[i].phoneNum;
        string value = tablePointer[i].name;
        if(key!=-1 && value!="deleted")
        {
            cout << "(" << key << "," << value << ")";
        }
    }
    cout << endl;
}

KeyValue* rehashTable()
{
    int newTableSize = tableSize*2;
    KeyValue *newTablePointer = new KeyValue[newTableSize];
    for (int i=0;i<newTableSize;i++)
    {
        newTablePointer[i].phoneNum = -1;
        newTablePointer[i].name = "";
    }
    //rehash new stuff
    int key;
    int testIndex;
    string value;
    for (int j=0;j<tableSize;j++)
    {
        key = tablePointer[j].phoneNum;
        value = tablePointer[j].name;
        testIndex = ((key%443)%newTableSize);        
        if (newTablePointer[testIndex].phoneNum==-1)
        {
            newTablePointer[testIndex].phoneNum = key;
            newTablePointer[testIndex].name = value;
        }
        else
        {
            if (testIndex < newTableSize-1)
            {
                for (int i=1;(testIndex+i)<newTableSize;i++)
                {
                    if (newTablePointer[testIndex+i].phoneNum==-1)
                    {
                        newTablePointer[testIndex+i].phoneNum = key;
                        newTablePointer[testIndex+i].name = value;
                        break;
                    }
                }
            }
            else
            {
                for (int j=0;j<testIndex;j++)
                {
                    if (newTablePointer[j].phoneNum==-1)
                    {
                        newTablePointer[j].phoneNum = key;
                        newTablePointer[j].name = value;
                        break;
                    }
                }
            }
        }
    }
    tableSize = newTableSize;
    cout << "table doubled" <<endl;
    return newTablePointer;
}

int throughArray(int key)
{
    for (int i=0;i<tableSize;i++)
    {
        if (tablePointer[i].phoneNum == key)
        {
            return i;
        }
    }
    return -1;
}

void insertHelper(int key,string value)
{
    if (throughArray(key) != -1)
    {
        cout << "item already present" << endl;
    }
    else
    {
        int testIndex = ((key%443)%tableSize);        
        if (tablePointer[testIndex].phoneNum==-1)
        {
            tablePointer[testIndex].phoneNum = key;
            tablePointer[testIndex].name = value;
        }
        else
        {
            if (testIndex < tableSize-1)
            {
                for (int i=1;(testIndex+i)<tableSize;i++)
                {
                    if (tablePointer[testIndex+i].phoneNum==-1)
                    {
                        tablePointer[testIndex+i].phoneNum = key;
                        tablePointer[testIndex+i].name = value;
                        break;
                    }
                }
            }
            else
            {
                for (int j=0;j<testIndex;j++)
                {
                    if (tablePointer[j].phoneNum==-1)
                    {
                        tablePointer[j].phoneNum = key;
                        tablePointer[j].name = value;
                        break;
                    }
                }
            }
        }
        
    }
}