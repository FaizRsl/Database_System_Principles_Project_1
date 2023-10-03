#ifndef PROJECT1_BPLUSTREE_H
#define PROJECT1_BPLUSTREE_H

#include <cstdlib>
#include <list>
#include "ProjectStructure.h"
#include <iostream>
#include <math.h>
#include <fstream>

using namespace std;

class BPlusTree {
public:
    void *root;
    unsigned int height;
    unsigned int maxKeys;
    unsigned int sizeOfNode;

    //For Experiments
    unsigned int numNodes;
    unsigned int numOverflowNodes;
    int numIndexAccessed;
    int numNodesDeleted;
    int numOverflowNodesAccessed;
    int numOverflowNodesDeleted;

    int count = 0;

    //Initialisation and setting functions
    BPlusTree(unsigned int sizeOfNode);
    void* getNewNode(bool isLeaf, bool isOverflow);

    //Retrieval functions
    list<pointerBlockPair> findRecord(float pointsHomeStart, float pointsHomeEnd, ofstream &output);
    void* findNode(float points_home, void* node, unsigned int currentHeight, ofstream &output, bool willPrint);

    //Functions for inserting a record
    void insertRecord(float points_home, pointerBlockPair record);
    void splitLeafNode(float points_home, pointerBlockPair record, void* nodeToSplit, pointerBlockPair* ptrArr, float* ptsHomeArr);
    void splitNonLeafNode(float points_home, pointerBlockPair record, void* nodeToSplit, pointerBlockPair* ptrArr, float* ptsHomeArr);
    void updateParentNodeAfterSplit(void* parentNode, void* rightNode, float newParentKey);

    //Functions for deleting a record
    void deleteKey(float pointsHome, void* nodeToDeleteFrom);
    void mergeNodes(void* leftNode, void* rightNode);
    void shiftElementsForward(float* pointsHomeArr, pointerBlockPair* ptrArr, int start, bool isLeaf);
    void shiftElementsBack(float* pointsHomeArr, pointerBlockPair* ptrArr, int end, bool isLeaf);

    //Functions for Experiments/Visualization
    int printIndexBlock(void* node, ofstream &output);
    void printRoot(ofstream &output);
    void printTree(ofstream &output);
};

#endif //PROJECT1_BPLUSTREE_H
