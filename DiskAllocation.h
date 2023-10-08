#ifndef PROJECT1_DISKALLOCATION_H
#define PROJECT1_DISKALLOCATION_H

#include <utility>
#include <unordered_map>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <list>

using namespace std;
/**
 * @brief DiskAllocation class represents a virtual disk allocation manager.
 *
 * This class provides functionality to manage a virtual disk, which is divided into blocks.
 * It keeps track of the status (used or unused) of each block on the disk.
 */
class DiskAllocation {
    public:

        void* disk; // holds disk's base address
        int blockSize;
        int numOfBlocks;
        list<void*> emptyBlocks;
        unordered_map<void*, bool> mapTable;    // mapTable keeps track for each block, if it is not in use

        // Constructs a new disk of {size}MB and splits the disk into multiple blocks of {sizeOfBlock}B each
        /**
         * @brief Constructs a new DiskAllocation object.
         * @param size The size of the virtual disk in megabytes (MB).
         * @param sizeOfBlock The size of each block in bytes.
         */
        DiskAllocation(int size, int sizeOfBlock);

        // function to set a block as empty or non-empty
        /**
         * @brief Toggles whether a block is in use or not.
         *
         * This function updates the usage status of a specified block.
         *
         * @param blockAddr The address of the block to update.
         */
        void updateMapTable(void* blockAddr);

        // function to return a block's address
        /**
         * @brief Retrieves the address of a specific block by its ID.
         * @param blockId The ID of the block to retrieve.
         * @return The address of the requested block.
         */
        void* fetchBlockAddress(int blockId);

        // function to get an unused block
        /**
         * @brief Gets an unused block from the list of empty blocks.
         * @return The address of an unused block.
         */
        void* getUnusedBlock();
};


#endif //PROJECT1_DISKALLOCATION_H
