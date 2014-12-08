#include "DisjointSetInt.h"

DisjointSetInt::DisjointSetInt(int initNumNodes){
	head = new int[initNumNodes];
	numNodes = numSets = initNumNodes;
	for (int idSet = 0; idSet < numSets; ++idSet){
		head[idSet] = -1;
	}

}

DisjointSetInt::~DisjointSetInt(){
	if (head != NULL){
		delete[] head;
		head = NULL;
	}
}

int DisjointSetInt::getNumSets(){
	return numSets;
}

bool DisjointSetInt::inSameSet(int idNode1, int idNode2){
	int head1 = getHead(idNode1);
	int head2 = getHead(idNode2);
	if (head1 == -1 || head2 == -1){
		cerr << "access invalid node in disjointSet" << endl;
		return false;
	}
	return (head1 == head2);
}

int DisjointSetInt::getSizeOfSet(int idNode){
	int headNode = getHead(idNode);
	return -head[headNode];
}

void DisjointSetInt::mergeSets(int idNode1, int idNode2){
	int head1 = getHead(idNode1);
	int head2 = getHead(idNode2);
	if (head1 == -1 || head2 == -1){
		cerr << "access invalid node in disjointSet" << endl;
	}
	else if (head1 != head2){
		if (head[head1] < head[head2]){
			head[head1] += head[head2];
			head[head2] = head1;
		}
		else {
			head[head2] += head[head1];
			head[head1] = head2;
		}
		--numSets;
	}
}

int DisjointSetInt::getHead(int idNode){
	if (idNode < 0 || idNode >= numNodes){
		return -1;
	}
	if (head[idNode] < 0){
		return idNode;
	}
	head[idNode] = getHead(head[idNode]);
	return head[idNode];
}