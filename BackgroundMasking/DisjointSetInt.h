#ifndef _DISJOINTSET_
#define _DISJOINTSET_

#include<iostream>
#include<climits>

using namespace std;

class DisjointSetInt{
private:
	int * head;
	int numNodes;
	int numSets;
public:
	DisjointSetInt(int);
	~DisjointSetInt();

	int getNumSets();
	bool inSameSet(int, int);
	int getSizeOfSet(int);
	void mergeSets(int, int);
	int getHead(int);
};

#endif // #ifndef _DISJOINTSET_