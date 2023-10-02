#ifndef PROJECT1_DATABASE_H
#define PROJECT1_DATABASE_H

#include <iostream>
#include <map>
#include <list>
#include <algorithm>
#include <set>
#include "DiskAllocation.h"
#include "BPlusTree.h"
#include "ProjectStructure.h"
#include <string>
#include <fstream>

using namespace std;

class Database {
public:
    // Define global data available to whole DBMS
    int DISK_SIZE; // calculated in MB
    int BLOCK_SIZE; // calculated in B
    int MAX_RECORDS; // maximum number of movieRecords for a block
    int numRecords; // total number of records
    int numBlocks;

    list<void*> freeBlocks; // Allows for tracking of blocks that can still accomodate additional records
    BPlusTree* bPlusTree;
    DiskAllocation* disk;
    void* initialBlockPtr;

    //Initialisation functions
    Database(unsigned int diskSize, unsigned int blockSize);
    ~Database();

    void importData();
    void insertRecord(GameData gameData);

    //Functions for Experiments/Visualization
    void printDataBlock(void* block, ofstream &output);
};


#endif //PROJECT1_DATABASE_H
