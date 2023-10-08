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

/**
 * @brief Represents a Database management system (DBMS) for game data.
 */
class Database {
public:
    int DISK_SIZE; ///< The size of the virtual disk (in MB).
    int BLOCK_SIZE; ///< The size of each block (in bytes).
    int MAX_RECORDS; ///< The maximum number of records for a block.
    int numRecords; ///< The total number of records.
    int numBlocks; ///< The total number of blocks.

    list<void*> freeBlocks; ///< List of blocks that can still accommodate additional records.
    BPlusTree* bPlusTree; ///< Pointer to the B+ tree used for indexing.
    DiskAllocation* disk; ///< Pointer to disk allocation manager.
    void* initialBlockPtr; ///< Pointer to the initial block.

    /**
     * @brief Constructs a new Database object.
     *
     * @param diskSize The size of the virtual disk in megabytes (MB).
     * @param blockSize The size of each block in bytes.
     */
    Database(unsigned int diskSize, unsigned int blockSize);

    /**
     * @brief Destroys the Database object and frees allocated memory.
     */
    ~Database();

    /**
     * @brief Imports data from an external source and populates the database.
     */
    void importData();

    /**
     * @brief Inserts a game data record into the database.
     *
     * @param gameData The game data record to insert.
     */
    void insertRecord(GameData gameData);

    /**
     * @brief Prints the content of a data block to an output file.
     *
     * @param block The data block to print.
     * @param output The output file stream.
     */
    void printDataBlock(void* block, ofstream &output);

    /**
     * @brief Prints the structure of the B+ tree to an output file.
     *
     * @param output The output file stream.
     */
    void printBPlusTree(ofstream& output);

    /**
     * @brief Prints the keys of a B+ tree node to an output file.
     *
     * @param node The B+ tree node.
     * @param output The output file stream.
     */
    void printBPlusTreeKeys(void* node, ofstream& output);
};

#endif //PROJECT1_DATABASE_H
