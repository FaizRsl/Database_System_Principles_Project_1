#include "DiskAllocation.h"
/**
 * @brief This constructor initializes the DiskAllocation class with a specified total size and block size.
 * It allocates memory for the disk and sets up a mapping table to manage disk blocks.
 * @param size The total size of the disk in megabytes.
 * @param sizeOfBlock The size of each disk block in bytes.
 */
DiskAllocation::DiskAllocation(int size, int sizeOfBlock)
{
    blockSize = sizeOfBlock;
    disk = malloc(size*1000000);

    // Initialise mapTable - i.e. split disk into blocks
    numOfBlocks = (size*1000000) / sizeOfBlock;
    int i;
    for (i=0; i<numOfBlocks; i++)
    {
        // Add offset to disk's base address to get this block's address
        void* blockAddress = reinterpret_cast<void*>(reinterpret_cast<char*>(disk) + i*sizeOfBlock);
        mapTable[blockAddress] = true;
        emptyBlocks.push_back(blockAddress);
    }
}

/**
 * @brief This function toggles the usage status of a disk block specified by its address in the mapping table.
 * It updates the mapping table to mark the block as used or unused.
 * @param blockAddr The address of the disk block to update in the mapping table.
 */
//Toggles whether block is in use or not
void DiskAllocation::updateMapTable(void* blockAddr)
{
    mapTable[blockAddr] = !mapTable[blockAddr];
    if (mapTable[blockAddr]) { // block is now unused
        emptyBlocks.push_back(blockAddr);
    }
}

/**
 * @brief This function retrieves the address of the first available (unused) block in the list of free blocks and removes it from the list.
 * @return The address of the first unused block in the list.
 */
//Returns the address of the first free block in the list of free blocks remaining and pop it
void* DiskAllocation::getUnusedBlock()
{
    void* blockAddr = emptyBlocks.front();
    emptyBlocks.pop_front();
    return blockAddr;
}