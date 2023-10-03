#include "Database.h"
#include "databaseStorage.h"
#include <chrono>

using namespace std;

Database::Database(unsigned int diskSize, unsigned int blockSize)
{
    DISK_SIZE = diskSize; // calculated in MB
    BLOCK_SIZE = blockSize; // calculated in B
    MAX_RECORDS = (BLOCK_SIZE - sizeof(unsigned int))/(sizeof(GameData) + sizeof(indexMapping)); // maximum number of movieRecords for a block

    freeBlocks = {}; // Allows for tracking of blocks that can still accomodate additional records
    disk = new DiskAllocation(DISK_SIZE, BLOCK_SIZE);
    bPlusTree = new BPlusTree(BLOCK_SIZE);
    numBlocks = 0;
}

Database::~Database() {
    free(disk);
    free(bPlusTree);
}

void Database::importData(){

    databaseStorage dbStorage;
    vector<GameData> data = dbStorage.getDatabaseRecord();
    this->numRecords = 0;
    // Loop over the data and insert all the movie records
    for (auto gamedata_address = data.begin(); gamedata_address != data.end(); ++gamedata_address)
    {
        insertRecord(*gamedata_address);
        this->numRecords++;
    } //close for loop
    cout << "Data has been successfully imported" << endl;


} //close function

// Inserts a movieRecord and updates the B+ Tree
void Database::insertRecord(GameData gameData)
{
    // note that checking if record is already inserted should be done in the B+ tree implementation
    void* blockAddress;
    void* blockToInsert;
    unsigned int* numRecords;
    indexMapping* indexMappingTable;

    //Retrieve a block for insertion of record, get new block from disk if all blocks are fully filled
    if (freeBlocks.size() == 0) {
        // no free blocks, get a new one and initialize header information
        blockAddress = disk->getUnusedBlock();
        disk->updateMapTable(blockAddress);
        numBlocks++;
        freeBlocks.push_front(blockAddress);
        blockToInsert = blockAddress;
        numRecords = (unsigned int*)blockToInsert;
        *numRecords = 0; // initialize first 4 bytes to be 0
    }
    else {
        blockAddress = freeBlocks.front();
        blockToInsert = blockAddress;
        numRecords = (unsigned int*)blockToInsert;
    }

    if(initialBlockPtr==nullptr)    initialBlockPtr = blockToInsert;

    indexMappingTable = (indexMapping*)(numRecords + 1); // pointer to start of indexMapping table, starts directly after numRecords
    GameData* tail = (GameData*)((char*)blockToInsert + BLOCK_SIZE - sizeof(GameData)); // pointer to record slot at bottom of the block

    // Setting insertRecordPointer to point to the space for insertion of record
    // Records are inserted starting from the back of the block
    GameData* insertRecordPointer = tail - *numRecords;
    indexMapping* insertindexMappingPointer = indexMappingTable + *numRecords;

    // Search for gravestones which indicate free spaces within the blocks for record insertion
    int numGravestones = 0;
    int index = *numRecords;
    for(unsigned int i = 0; i<*numRecords; i++)
    {
        if ((indexMappingTable + i)->indexOfRecord == -1)
        {
            // Replace last gravestone
            insertRecordPointer = tail - i;
            insertindexMappingPointer = indexMappingTable + i;
            index = i;
            numGravestones++;
        }
    }

    if (numGravestones > 0)
    {
        numGravestones--; // revive a single gravestone
        (*numRecords)--; // to keep numRecords unchanged when it is incremented later
    }

    // Insert record to disk
    *insertRecordPointer = gameData; // insert record data
    *insertindexMappingPointer = {gameData.FG_PCT_home, index}; // insert new indexMapping table entry
    (*numRecords)++;

    // Update B+ Tree with new record inserted
    bPlusTree->insertRecord(gameData.FG_PCT_home, {blockAddress, gameData.FG_PCT_home});

    // Remove block from list of freeblocks if updated block cannot hold any more records
    if (*numRecords == MAX_RECORDS && numGravestones == 0)
    {
        freeBlocks.pop_front();
    }

    return;
}

/*
//Prints recordID values of records within data block, used for reporting statistics for experiments
void Database::printDataBlock(void* block, ofstream &output) {

    int numRecords = *((unsigned int*) block);

    indexMapping* indexMappingTable = (indexMapping*)(((unsigned int*)block) + 1); // Pointer to start of indexMapping table, starts directly after numRecords
    GameData* dataRecords = (GameData*)((char*) block + BLOCK_SIZE - sizeof(GameData)); // Pointer to start of record slot at bottom of the block

    cout << " | ";
    char toPrint[24];
    for (int i=0; i<MAX_RECORDS; i++) {
        if (i < numRecords) {
            snprintf(toPrint, 24, "%12s | ", dataRecords[i].FG_PCT_home);
        } else {
            snprintf(toPrint, 24, "%12s | ", "            ");
        }
        output << toPrint;
        cout << toPrint;
    }
    cout << "\n";
    output << "\n";
}

void Database::printBPlusTreeKeys(void* node, ofstream& output)
{
    if (node == nullptr) {
        return; // Base case: reached a null node
    }

    // Cast the node to NodeHeader to access its header information
    NodeHeader* header = static_cast<NodeHeader*>(node);

    // Check if the node is a leaf node
    if (header->isLeaf) {
        // If it's a leaf node, cast it to access its data
        pointerBlockPair* ptrArr = static_cast<pointerBlockPair*>(node) + 1;
        float* points_homeArr = reinterpret_cast<float*>(ptrArr + header->numKeys);

        // Print all the keys in the leaf node
        for (unsigned int i = 0; i < header->numKeys; i++) {
            output << points_homeArr[i] << " ";
        }
    } else {
        // If it's an internal node, traverse its children
        pointerBlockPair* ptrArr = static_cast<pointerBlockPair*>(node) + 1;
        for (unsigned int i = 0; i < header->numKeys; i++) {
            // Recursively print keys from child nodes
            printBPlusTreeKeys(ptrArr[i].blockAddress, output);
        }
    }
}

// Call this function to print all keys from the root of the B+ tree
void Database::printBPlusTree(ofstream& output)
{
    if (bPlusTree == nullptr || bPlusTree->root == nullptr) {
        output << "B+ Tree is empty." << endl;
        return;
    }

    printBPlusTreeKeys(bPlusTree->root, output);
}
*/

