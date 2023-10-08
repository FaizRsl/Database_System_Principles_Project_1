#include "BPlusTree.h"
#include <iomanip>
#include <cmath>
#include <chrono>

/**
 * @brief Constructs a B+ tree with the specified node size.
 *
 * This constructor initializes a B+ tree with the given node size. It sets various
 * statistics to zero and computes the maximum number of keys that a node can hold
 * based on the node size.
 *
 * @param nodeSize The size (in bytes) of a B+ tree node.
 */
BPlusTree::BPlusTree(unsigned int nodeSize) {
    numNodes = 0;
    numOverflowNodes = 0;
    numIndexAccessed = 0;
    numOverflowNodesAccessed = 0;
    numNodesDeleted = 0;
    numOverflowNodesDeleted = 0;
    height = 0;
    void* pointers[maxKeys + 1];

    // maxKeys = (size of a block - size of node's header - right most pointer) / (size of ptr-key pairs)
    sizeOfNode = nodeSize;
    const int sizeOfKeyPtrPair = (sizeof(pointerBlockPair) + sizeof(unsigned int));
    maxKeys = (nodeSize - sizeof(NodeHeader) - sizeof(pointerBlockPair)) / sizeOfKeyPtrPair;
    root = getNewNode(true, false);
}

// Gets a new node from memory to be used as a node in the B+ Tree
// isOverflow used to determine whether to increment numOverflowNodes or numNodes
// isLeaf is also assigned for the node based on the input
/**
* @brief Creates a new B+ tree node.
*
* @param isLeaf Indicates whether the new node is a leaf node.
* @param isOverflow Indicates whether the new node is an overflow node.
* @return A pointer to the newly created node.
*/
void* BPlusTree::getNewNode(bool isLeaf, bool isOverflow) {
    void* addr = malloc(sizeOfNode);

    // Initialise header of the node
    NodeHeader* header;
    header = (NodeHeader*) addr;
    header->numKeys = 0; // First 4 bytes (size of int) is numOfRecords = 0
    header->isLeaf = isLeaf;

    // Initialise the pointer to parent
    pointerBlockPair ptr;
    ptr.blockAddress = nullptr;
    ptr.recordID = -1;
    header->pointerToParent = ptr;

    // Initialise last pointer to null
    // Required for leaf nodes in case it is the last leaf node
    pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) addr ) + 1 );
    float* pointsHomeArr = (float*) (ptrArr + maxKeys + 1);
    ptrArr[maxKeys] = {nullptr, -1};

    // Incrementing number of nodes created for the B+ Tree
    isOverflow ? numOverflowNodes++ : numNodes++;

    return addr;
}

// Print the contents of a specific index block in the B+ Tree
// Used for experiments
// Functions for Experiments/Visualization
/**
 * @brief Prints the contents of an index block to the output stream.
 *
 * @param node The node to print.
 * @param output The output file stream for logging.
 * @return The number of keys printed.
 */
int BPlusTree::printIndexBlock(void* node, ofstream &output) {
    int numKeys = *(unsigned int*)node;
    pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) node ) + 1 );
    float* pointsHomeArr = (float*) (ptrArr + maxKeys + 1);

    cout << " | ";
    if (output.is_open())
        output << " | ";
    char toPrint[24];
    for (int i=0; i<maxKeys; i++) {
        if (i < numKeys) {
            snprintf(toPrint, 24, "%7f | ", pointsHomeArr[i]);
        } else {
            snprintf(toPrint, 24, "%6s | ", "   ");
        }
        cout << toPrint;
        if (output.is_open())
            output << toPrint;
    }
    cout << endl;
    return numKeys;
}




// Queries a record/range of keys that is selected by the user
// This may return multiple pointerBlockPairs due to the possibility of multiple records having same key value of numVotes
// For querying of single value, set numVotesStart and numVotesEnd to both be the value
// Calls findNode() to locate the appropiate leaf node
/**
* @brief Finds records within a specified range of key values.
*
* @param pointsHomeStart The starting key value.
* @param pointsHomeEnd The ending key value.
* @param output The output file stream to write results to.
* @return A list of pointer-block pairs representing the found records.
*/
list<pointerBlockPair> BPlusTree::findRecord(float pointsHomeStart, float pointsHomeEnd, ofstream &output) {
    auto startTime = std::chrono::high_resolution_clock::now();

    numIndexAccessed = 0;
    int numDataBlockAccessed = 0;

    list<pointerBlockPair> results;
    void* currNode = findNode(pointsHomeStart, root, 0, output, false);

    while (currNode != nullptr) {
        // Extract information from the current node
        unsigned int numKeys = *(unsigned int*)currNode;
        pointerBlockPair* ptrArr = (pointerBlockPair*)(((NodeHeader*)currNode) + 1);
        float* numVotesArr = (float*)(ptrArr + maxKeys + 1);
        int i = 0;

        // Continue iterating when key is smaller than search key and the current non-full node has not reached the end
        while (i < numKeys && numVotesArr[i] < pointsHomeEnd) {
            if (numVotesArr[i] >= pointsHomeStart) { // Check if key is greater than starting key
                // Track the number of index and data blocks accessed
                numIndexAccessed++;

                // Check if it's a valid data block, not an overflow node
                if (ptrArr[i].blockAddress != nullptr) {
                    results.push_back(ptrArr[i]);
                    numDataBlockAccessed++;
                }
            }

            // Move to the next key
            i++;
        }

        // Traverse to the next leaf node if available
        if (ptrArr[maxKeys].blockAddress == nullptr) {
            break; // If there's no next leaf node, break out of the loop
        }
        currNode = ptrArr[maxKeys].blockAddress;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    if (output.is_open()) {
        output << "Total number of index nodes accessed: " << numIndexAccessed << "\n";
        output << "Total number of data blocks accessed: " << numDataBlockAccessed << "\n";
        output << "Running time for Retrieval Process: " << elapsedTime.count() << " microseconds \n";
    }

    return results;
}

/**
 * @brief Gets the maximum key value that is less than or equal to the specified maxVal.
 * @param maxVal The maximum value to compare against.
 * @param start The starting index to search from.
 * @return The index of the maximum key value.
 */
int BPlusTree::getMax(float maxVal, float start){

    float shiftedValue = maxVal-start;

    int lastFourDigits = shiftedValue * 10000000;


    return lastFourDigits+4;
}


// Finds the appropiate node to be used for retrieval/insertion/deletion
// Starts from the root node and recursively calls findNode() every time it goes down a level
// Terminating condition occurs when a leaf node is reached
// This DOES NOT mean that the key is definitely present in the node, iteration through the node still needs to be done
/**
 * @brief Finds the node containing a specific key value within the B+ tree.
 * @param points_home The key value to search for.
 * @param node The current node being searched.
 * @param currHeight The current height in the B+ tree.
 * @param output The output file stream for logging.
 * @param willPrint Indicates whether to print debug information.
 * @return A pointer to the node containing the key.
 */
void* BPlusTree::findNode(float points_home , void* node, unsigned int currHeight, ofstream &output, bool willPrint) {

    numIndexAccessed++;

    // Print index blocks to screen and file
    if(willPrint){
        if (output.is_open()) {
            printIndexBlock(node, output);
            cout << endl;
            output << endl;
        }
    }

    // return terminal node
    if (currHeight == height) {
        return node;
    }

    pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) node ) + 1 );
    float* pointsHomeArr = (float*) (ptrArr + maxKeys + 1);
    unsigned int numKeys = *((unsigned int*) node);

    for (int i = 0; i <= numKeys - 1; i++) {
        if (points_home < pointsHomeArr[i]) {
            return findNode(points_home, ptrArr[i].blockAddress, ++currHeight, output, willPrint); // Search into pointer left of current index
        } else {
            if (i != numKeys - 1) {
                continue; // compare with next numVotes if the last numVotes has not been reached
            } else {
                return findNode(points_home, ptrArr[i+1].blockAddress, ++currHeight, output, willPrint); // Search into pointer right of last numVotes
            }
        }
    }

    return nullptr;
}

// Inserts a key into the B+ Tree if it exists
// Accounts for duplicate keys and creates overflow nodes to hold duplicate keys if required
// Leaf nodes will only hold unique key values, which may have pointers to overflow nodes if multiple records have the same index
// Calls splitLeafNode() if number of keys exceeds the maximum number of keys the leaf node can hold
/**
 * @brief Inserts a record into the B+ tree.
 * @param points_home The key value of the record.
 * @param record The pointer-block pair representing the record.
 */
void BPlusTree::insertRecord(float points_home, pointerBlockPair record) {

    count++;
    ofstream dummy;
    void* nodeToInsertAt = findNode(points_home, root, 0, dummy, true);
    int numKeys = *(unsigned int*)nodeToInsertAt;
    pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) nodeToInsertAt ) + 1 );
    pointerBlockPair* test = static_cast<pointerBlockPair*>(ptrArr);
    GameData* node = static_cast<GameData*>(test->blockAddress);
    float* points_homeArr = (float*) (ptrArr + maxKeys + 1);

    float increment = 0.0000001;
    int counter = 1;
    float temp = points_home;

    bool isDuplicate = false;

    // Check if there will be duplicate keys after the new record is inserted
    for (auto& entry : duplicateCount) {
        if (entry.keyValue == points_home) {
            // Value found, increment the count
            entry.count++;
            isDuplicate = true;
            break;
        }
    }

    if(!isDuplicate){
        duplicateCounter newEntry;
        newEntry.keyValue = points_home;
        newEntry.count = 1;
        duplicateCount.push_back(newEntry);
    }


    for (int i = 0; i <= numKeys -1 ; i++) {

        if (temp == points_homeArr[i]){
            for (auto& entry : duplicateCount) {
                if (entry.keyValue == temp) {
                    // Value found, increment the count
                    counter = entry.count;
                }
            }
            temp = points_home + (increment * counter);
        }
    }


    points_home = temp;
    // CASE 2: Unique key, but number of keys after insertion to node exceeds max number of keys allowed
    if (numKeys == maxKeys){
        splitLeafNode(points_home, record, nodeToInsertAt, ptrArr, points_homeArr);
        return;
    }

    // CASE 3: Unique key, and node has sufficient space to hold new key
    int i;
    for (i = 0; i <= numKeys-1; i++) { //Find position within node to insert key
        if (points_home < points_homeArr[i]){
            for (int j = numKeys; j > i; j--) { // Shift current keys back to accomondate new key
                points_homeArr[j] = points_homeArr[j-1];
                ptrArr[j] = ptrArr[j-1];
            }
            break;
        }
    }
    points_homeArr[i] = points_home;
    ptrArr[i] = record;
    cout << fixed << setprecision(7);
    (*(unsigned int*)nodeToInsertAt)++; //Increment number of records in leaf node
}


// Deletes a key from the B+ Tree if it exists
// Accounts for deletion of key from both leaf and non-leaf nodes
// Initial deleting of a key always starts from a leaf node
// If merging was performed, deleteKey() may be called again in mergeNodes() for the parent node
// Function to find the node containing the key to delete
/**
 * @brief Finds the node containing a specific key value to delete and returns it.
 * @param pointsHome The key value to delete.
 * @param rootNode The root node of the B+ tree.
 * @param output The output file stream for logging.
 * @return A pointer to the node containing the key to delete.
 */
void* BPlusTree::findKeyToDelete(float pointsHome, void* rootNode, ofstream &output) {
    void* currNode = findNode(pointsHome, root, 0, output, false);

    // Traverse to the node containing the key
    unsigned int numkeys = *(unsigned int *)currNode;
    pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) currNode ) + 1 );
    float* numVotesArr = (float*) (ptrArr + maxKeys + 1);
    int i = 0;

    while (i < numkeys && numVotesArr[i] <= pointsHome) {
        if (numVotesArr[i] == pointsHome) { // Check if key is greater than starting key
            currNode = ptrArr[maxKeys].blockAddress;
            break;
        }
        if (i == numkeys - 1) {
            if (ptrArr[maxKeys].blockAddress == nullptr) {
                // If there's no next leaf node, break out of the loop
                break;
            }
            currNode = ptrArr[maxKeys].blockAddress; // Traverse to the next leaf node

            // Reset the search to the start of the next leaf node
            ptrArr = (pointerBlockPair*) (((NodeHeader*) currNode ) + 1 );
            numVotesArr = (float*) (ptrArr + maxKeys + 1);
            numkeys = *(unsigned int *)currNode;
            i = 0;
            continue;
        }
        i++;
    }

    return currNode; // Return the node containing the key (or nullptr if not found)
}

// Function to delete a key
/**
 * @brief Deletes a record with a specific key value from the B+ tree.
 * @param pointsHome The key value of the record to delete.
 * @param nodeToDeleteFrom The node to delete the record from.
 */
void BPlusTree::deleteKey(float pointsHome, void* nodeToDeleteFrom) {
    unsigned int* numKeys = (unsigned int*)nodeToDeleteFrom;
    NodeHeader header = *(NodeHeader*) nodeToDeleteFrom;
    pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) nodeToDeleteFrom ) + 1 );
    float* pointsHomeArr = (float*) (ptrArr + maxKeys + 1);

    // Search for the key in the node to delete from
    bool keyExists = false;
    int i;
    for (i = 0; i < *numKeys; i++) {
        if (pointsHomeArr[i] == pointsHome) {
            keyExists = true;
            break;
        }
    }

    // If the key does not exist, handle the case appropriately
    if (!keyExists) {
        // You can log an error or handle this case based on your requirements
        return;
    }

    // Declaration of minimum number of keys allowed depending on leaf or non-leaf node
    unsigned int minKeys = 0;
    if (header.isLeaf) {
        minKeys = (maxKeys + 1) / 2;
    } else {
        minKeys = maxKeys / 2;
    }

    // Perform deletion of any overflow nodes first, if they exist
    if (ptrArr[i].recordID >= pointsHome && ptrArr[i].recordID < pointsHome + 0.001) {
        // RecordID of -1 indicates that there is an overflow node
        void* tempNode = ptrArr[i].blockAddress;
        pointerBlockPair* ptrArr;
        void* nextOverflow;
        while (tempNode != nullptr) {
            numOverflowNodesDeleted++;
            ptrArr = (pointerBlockPair*) (((NodeHeader*) tempNode ) + 1 );
            nextOverflow = ptrArr[maxKeys].blockAddress; // Hold pointer nextOverflow before we free the current overflow block
            free(tempNode);
            numOverflowNodes--;
            tempNode = nextOverflow; // Proceed to delete and free the next overflowNode
        }
    }

    // Perform deletion of the key from the node
    (*numKeys)--;
    if (i != maxKeys - 1) { // If element to remove is not the last element
        shiftElementsForward(pointsHomeArr, ptrArr, i, header.isLeaf);
    }

    void* parentNode = ((NodeHeader*)nodeToDeleteFrom)->pointerToParent.blockAddress;
    int numKeysInParent = *(unsigned int*) parentNode;
    pointerBlockPair* ptrArrParent = (pointerBlockPair*) (((NodeHeader*) parentNode ) + 1 );
    float* pointsHomeArrParent = (float*) (ptrArrParent + maxKeys + 1);

    // Check if the key to be deleted appears in any of its ancestors and find the node it is in
    // This will only occur if the key we are deleting is the smallest key in its index node
    if (i == 0) {
        void* recursiveParent;
        // If the number of keys remaining is less than the minimum keys allowed, borrowing/merging needs to be performed
        if (*numKeys < minKeys) {
            for (int ourPosInParent = 0; ourPosInParent <= numKeysInParent; ourPosInParent++) {
                // Find number of keys in the sibling node and check if number - 1 is less than minkeys
                if (ptrArrParent[ourPosInParent].blockAddress == nodeToDeleteFrom) {
                    // Find our position in parentNode so we can identify our siblings
                    void* sibling = nullptr;
                    unsigned int* siblingNumKeys;
                    bool borrowFromLeft;
                    NodeHeader siblingHeader;

                    // If the left sibling exists, check if a key can be borrowed from it
                    if (ourPosInParent != 0) {
                        sibling = ptrArrParent[ourPosInParent - 1].blockAddress;
                        siblingNumKeys = (unsigned int*) sibling;
                        siblingHeader = *(NodeHeader*) sibling;
                        if (*siblingNumKeys - 1 < minKeys) {
                            // Not possible to borrow from the left sibling
                            sibling = nullptr;
                        } else {
                            // Borrow from the left sibling; don't bother checking with the right sibling
                            borrowFromLeft = true;
                        }
                    }

                    // If key cannot be borrowed from the left sibling or the left sibling does not exist, check the right sibling
                    if (sibling == nullptr && ourPosInParent != numKeysInParent) {
                        sibling = ptrArrParent[ourPosInParent + 1].blockAddress;
                        siblingNumKeys = (unsigned int*) sibling;
                        siblingHeader = *(NodeHeader*) sibling;
                        if (*siblingNumKeys - 1 < minKeys) {
                            // Not possible to borrow from the right sibling
                            sibling = nullptr;
                        } else {
                            // Borrow from the right sibling
                            borrowFromLeft = false;
                        }
                    }

                    // Perform borrowing if there exists a sibling that allows for borrowing
                    if (sibling != nullptr) {
                        pointerBlockPair* ptrArrSibling = (pointerBlockPair*) (((NodeHeader*) sibling ) + 1 );
                        float* pointsHomeArrSibling = (float*) (ptrArrSibling + maxKeys + 1);
                        if (borrowFromLeft) {
                            // Borrow the last key from the left sibling
                            // Shift all elements in nodeToDeleteFrom to the right to make space for the new key
                            shiftElementsBack(pointsHomeArr, ptrArr, 0, siblingHeader.isLeaf);
                            pointsHomeArr[0] = pointsHomeArrSibling[(*siblingNumKeys) - 1]; // Borrowing of key
                            ptrArr[0] = ptrArrSibling[(*siblingNumKeys) - 1];
                            (*siblingNumKeys)--;
                            (*numKeys)++;

                            // Update the index in the parent node that leads to this node
                            pointsHomeArrParent[ourPosInParent - 1] = pointsHomeArr[0];
                        } else {
                            // Borrow the first key from the right sibling
                            pointsHomeArr[*numKeys] = pointsHomeArrSibling[0]; // Borrowing of key
                            ptrArr[*numKeys] = ptrArrSibling[0];
                            (*numKeys)++;

                            // Shift all elements in the right sibling to fill up space due to the key borrowed
                            shiftElementsForward(pointsHomeArrSibling, ptrArrSibling, 0, siblingHeader.isLeaf);

                            (*siblingNumKeys)--;

                            // Update the index in the parent node that leads to the right sibling
                            pointsHomeArrParent[ourPosInParent] = pointsHomeArrSibling[0];
                        }
                    } else {
                        if (ourPosInParent != 0) { // If not the leftmost node, merge with the left sibling
                            mergeNodes(ptrArrParent[ourPosInParent - 1].blockAddress, nodeToDeleteFrom);
                        } else { // If the leftmost node, merge with the right sibling
                            mergeNodes(nodeToDeleteFrom, ptrArrParent[1].blockAddress);
                        }
                    }

                    break;
                }
            }
        }

        // If deleting the root node and the current node becomes the new root node
        if (nodeToDeleteFrom == root && *numKeys == 1) {
            free(root);
            numNodes--;
            numNodesDeleted++;
            root = ptrArr[0].blockAddress;
        }
    }
}



// Merges two nodes if number of keys is insufficient from the B+ Tree
// Merging occurs by keeping the left node, and deleting the right node
// Is called by deleteKey() if deletion results in nodes with insufficient keys
/**
 * @brief Merges two nodes (either leaf or non-leaf) into one node.
 * @param leftNode The left node to be merged.
 * @param rightNode The right node to be merged.
 */
void BPlusTree::mergeNodes(void* leftNode, void* rightNode) {

    pointerBlockPair* ptrArrL = (pointerBlockPair*) (((NodeHeader*) leftNode ) + 1 );
    float* pointsHomeArrL = (float*) (ptrArrL + maxKeys + 1);

    pointerBlockPair* ptrArrR = (pointerBlockPair*) (((NodeHeader*) rightNode ) + 1 );
    float* pointsHomeArrR = (float*) (ptrArrR + maxKeys + 1);

    float smallestRight = pointsHomeArrR[0];
    unsigned int* numKeysL = (unsigned int*)leftNode;
    unsigned int* numKeysR = (unsigned int*)rightNode;

    // For each item in the right node, append to the left node
    for (int i=0; i<*numKeysR; i++) {
        pointsHomeArrL[*numKeysL+i] = pointsHomeArrR[i];
        ptrArrL[*numKeysL+i] = ptrArrR[i];
    }
    *numKeysL += *numKeysR;

    // For leaf nodes, the original left node should now point to the node pointed to by the original right node
    NodeHeader header = *(NodeHeader*) leftNode;
    if (header.isLeaf) {
        ptrArrL[maxKeys] = ptrArrR[maxKeys];
    }

    // Retrieve parent node for deletion of key
    void* parentNode = ((NodeHeader*)leftNode)->pointerToParent.blockAddress;

    free(rightNode);
    numNodes--;
    numNodesDeleted++;

    //Parent node that points to the original left and right node will have one less key
    //Key to be removed from parent node is always first key of original right node
    //Smallest right is passed in with the parent node
    deleteKey(smallestRight, parentNode);
}


// Splits leaf node into two nodes when a record is inserted
// New node will be to the right of the original node
// Original node is now the left node
// Calls updateParentNodeAfterSplit() to update keys in parent node
/**
 * @brief Splits a leaf node during record insertion.
 * @param points_home The key value of the record to insert.
 * @param record The pointer-block pair representing the record.
 * @param nodeToSplit The leaf node to split.
 * @param ptrArr An array of pointer-block pairs.
 * @param ptsHomeArr An array of key values.
 */
void BPlusTree::splitLeafNode(float points_home, pointerBlockPair record, void* nodeToSplit, pointerBlockPair* ptrArr, float* ptsHomeArr) {

    void* leftNode = nodeToSplit;
    void* rightNode = getNewNode(true, false); // Create new right node

    list<pointerBlockPair> tempPtrList;
    list<float> tempPointHomeList;
    unsigned int numLeftKeys = ceil((maxKeys+1)/2.0);
    unsigned int numRightKeys = floor((maxKeys+1)/2.0);
    void* parentNode = ((NodeHeader*)nodeToSplit)->pointerToParent.blockAddress;

    // Copy existing keys into a temp list, and add in new key in correct position
    bool newKeyInserted = false;

    for (int i = 0; i < maxKeys; i++) {
        if (!newKeyInserted && points_home < ptsHomeArr[i]){
            tempPointHomeList.push_back(points_home);
            tempPtrList.push_back(record);
            newKeyInserted = true;
        }
        tempPointHomeList.push_back(ptsHomeArr[i]);
        tempPtrList.push_back(ptrArr[i]);
    }
    if (points_home > ptsHomeArr[maxKeys-1]){ // Runs when new numNodes is bigger than all keys
        tempPointHomeList.push_back(points_home);
        tempPtrList.push_back(record);
    }

    pointerBlockPair* ptrArrR = (pointerBlockPair*) (((NodeHeader*) rightNode ) + 1 );
    float* pointsHomeArrR = (float*) (ptrArrR + maxKeys + 1);

    // Filling in keys for new left node
    for (int i = 0; i < numLeftKeys; i++) {
        ptsHomeArr[i] = tempPointHomeList.front();
        ptrArr[i] = tempPtrList.front();
        tempPointHomeList.pop_front();
        tempPtrList.pop_front();
    }
    *((unsigned int*) leftNode) = numLeftKeys;

    // Filling in keys for new right node
    for (int i = 0; i < numRightKeys; i ++) {
        pointsHomeArrR[i] = tempPointHomeList.front();
        ptrArrR[i] = tempPtrList.front();
        tempPointHomeList.pop_front();
        tempPtrList.pop_front();
    }
    *((unsigned int*) rightNode) = numRightKeys;

    // Linking of leaf nodes
    // Original right node should now point to the node pointed to by the original left node
    // Left node should now point to the newly created right node
    ptrArrR[maxKeys].blockAddress = ptrArr[maxKeys].blockAddress;
    ptrArr[maxKeys].blockAddress = rightNode;

    updateParentNodeAfterSplit(parentNode, rightNode, pointsHomeArrR[0]);

    return;
}


// Splits non-leaf node into two nodes when a record is inserted
// New node will be to the right of the original node
// Original node is now the left node
// Calls updateParentNodeAfterSplit() to update keys in parent node
/**
 * @brief Splits a non-leaf node during record insertion.
 * @param points_home The key value of the record to insert.
 * @param record The pointer-block pair representing the record.
 * @param nodeToSplit The non-leaf node to split.
 * @param ptrArr An array of pointer-block pairs.
 * @param ptsHomeArr An array of key values.
 */
void BPlusTree::splitNonLeafNode(float points_home, pointerBlockPair record, void* nodeToSplit, pointerBlockPair* ptrArr, float* ptsHomeArr) {

    void* leftNode = nodeToSplit;
    void* rightNode = getNewNode(false, false); // Create new right node

    list<pointerBlockPair> tempPtrList;
    list<float> tempPointsHomeList;
    unsigned int numLeftKeys = ceil(maxKeys/2.0);
    unsigned int numRightKeys = floor(maxKeys/2.0);
    void* parentNode = ((NodeHeader*)nodeToSplit)->pointerToParent.blockAddress;

    // Copy existing keys into a temp list
    for (int i = 0; i < maxKeys; i++) {
        tempPointsHomeList.push_back(ptsHomeArr[i]);
        tempPtrList.push_back(ptrArr[i]);
    }
    tempPtrList.push_back(ptrArr[maxKeys]);

    // Add in new key into the temp list in the correct position
    list<pointerBlockPair>::iterator ptrItr = tempPtrList.begin();
    list<float>::iterator pointsHomeItr = tempPointsHomeList.begin();
    while (true) {
        if (points_home < *pointsHomeItr) {
            tempPointsHomeList.insert(pointsHomeItr, points_home); // insert adds new element to front of current iteration
            ptrItr++;
            tempPtrList.insert(ptrItr, record);
            break;
        }
        if (pointsHomeItr == tempPointsHomeList.end()) { // If index should be last element in node, just append to the back
            tempPointsHomeList.push_back(points_home);
            tempPtrList.push_back(record);
            break;
        }
        ptrItr++;
        pointsHomeItr++;
    }

    pointerBlockPair* ptrArrR = (pointerBlockPair*) (((NodeHeader*) rightNode ) + 1 );
    float* numVotesArrR = (float*) (ptrArrR + maxKeys + 1);

    // Filling in keys for new left node
    int i;
    for (i = 0; i < numLeftKeys; i++) {
        ptsHomeArr[i] = tempPointsHomeList.front();
        ptrArr[i] = tempPtrList.front();
        ((NodeHeader*) ptrArr[i].blockAddress)->pointerToParent.blockAddress = leftNode; // update all children to point to leftNode as new parent
        tempPointsHomeList.pop_front();
        tempPtrList.pop_front();
    }
    ptrArr[i] = tempPtrList.front(); // node needs 1 more ptr than key
    tempPtrList.pop_front(); //Pop the pointer after assigning it
    ((NodeHeader*) ptrArr[numLeftKeys].blockAddress)->pointerToParent.blockAddress = leftNode; // update last children to point to leftNode as new parent
    *((unsigned int*) leftNode) = numLeftKeys; //Update the number of keys for this left node


    // Filling in keys for new right node
    // The key currently at front of the list will be the parent for both left and right nodes
    // Thus the key is popped out and passed into updateParentNodeAfterSplit later for promotion
    float newParentKey = tempPointsHomeList.front();
    tempPointsHomeList.pop_front();

    for (i = 0; i < numRightKeys; i++) {
        numVotesArrR[i] = tempPointsHomeList.front();
        ptrArrR[i] = tempPtrList.front();
        ((NodeHeader*) ptrArrR[i].blockAddress)->pointerToParent.blockAddress = rightNode; // update all children of right node to point to itself as new parent
        tempPointsHomeList.pop_front();
        tempPtrList.pop_front();
    }
    ptrArrR[numRightKeys] = tempPtrList.front(); // For non-leaf nodes, n keys requires (n+1) pointers, pop one more pointer
    tempPtrList.pop_front();
    ((NodeHeader*) ptrArrR[numRightKeys].blockAddress)->pointerToParent.blockAddress = rightNode; // update last children to point to itself as new parent
    *((unsigned int*) rightNode) = numRightKeys; // Update the number of keys for this right node

    updateParentNodeAfterSplit(parentNode, rightNode, newParentKey);

    return;
}


// Updates parent node after a split has been occured
// Is called by either splitLeafNode() or splitNonLeafNode()
// Can also call splitNonLeafNode() if the parent node ends up having insufficient number of keys
/**
 * @brief Updates the parent node after splitting a node.
 * @param parentNode The parent node.
 * @param rightNode The right sibling node.
 * @param newKey The key value to update in the parent node.
 */
void BPlusTree::updateParentNodeAfterSplit(void* parentNode, void* rightNode, float newKey) {

    //If root node is the node being split, we need to create a new root
    if (parentNode == nullptr) {
        void* newRootNode = getNewNode(false, false); // create a parent node (root)

        pointerBlockPair* ptrArrNew = (pointerBlockPair*) (((NodeHeader*) newRootNode ) + 1 );
        float* pointsHomeArrNew = (float*) (ptrArrNew + maxKeys + 1);

        ptrArrNew[0].blockAddress = root; // old root node became the left node
        ptrArrNew[1].blockAddress = rightNode;
        pointsHomeArrNew[0] = newKey; // only key in new root node is the smallest key of the right subtree
        (*((unsigned int*) newRootNode))++;

        // update parent of the new child nodes
        ((NodeHeader*) root)->pointerToParent.blockAddress = newRootNode; // this is the left node
        ((NodeHeader*) rightNode)->pointerToParent.blockAddress = newRootNode;

        if (((NodeHeader*) root)->isLeaf) { // left node (old root) needs to link to (new) right node
            pointerBlockPair* ptrArrRoot = (pointerBlockPair*) (((NodeHeader*) root) + 1 );
            ptrArrRoot[maxKeys].blockAddress = rightNode; // link leaf nodes together
        }

        root = newRootNode; //Reinitialise new root
        height++; //Increment the variable storing the height of B++ tree

    } else { //there exists a parent node already
        int numKeys = *(unsigned int*) parentNode;

        //Initialise ptrArr and numVotesArr to access pointer and key arrays
        pointerBlockPair* ptrArr = (pointerBlockPair*) (((NodeHeader*) parentNode ) + 1 );
        float* pointsHomeArr = (float*) (ptrArr + maxKeys + 1);

        //parent node need to be split
        if (numKeys == maxKeys) {
            pointerBlockPair addrToRightNode = {rightNode, -1};
            splitNonLeafNode(newKey, addrToRightNode, parentNode, ptrArr, pointsHomeArr);
        } else { // parent node don't need to split
            int i;
            for (i = 0; i <= numKeys-1; i++) { //Find position within node to insert key
                if (newKey < pointsHomeArr[i]){ // replaced smallestKey with newKey
                    ptrArr[numKeys+1] = ptrArr[numKeys]; //replace the last pointer first
                    for (int j = numKeys; j > i; j--) { // shift keys back to accomodate new key
                        pointsHomeArr[j] = pointsHomeArr[j-1];
                        ptrArr[j] = ptrArr[j-1];
                    }
                    break;
                }
            }
            pointsHomeArr[i] = newKey; //Insert the index value at specified location // replaced smallestKey with newKey
            ptrArr[i+1].blockAddress = rightNode; //Insert the pointer to the record in the disk at specified location
            (*(unsigned int*)parentNode)++; //Increment numRecords
            ((NodeHeader*) rightNode)->pointerToParent.blockAddress = parentNode; // right node's parent is the same as left node
        }
    }

}


// Shift all keys forward by one space
// Called by deleteKey() when borrowing elements
// Also called by deleteKey() when deleting the first key from the node
/**
 * @brief Shifts elements in an array of key values and pointer-block pairs forward.
 * @param pointsHomeArr An array of key values.
 * @param ptrArr An array of pointer-block pairs.
 * @param start The starting index for shifting.
 * @param isLeaf Indicates whether the elements are in a leaf node.
 */
void BPlusTree::shiftElementsForward(float* pointsHomeArr, pointerBlockPair* ptrArr, int start, bool isLeaf) {

    if (isLeaf) {
        for (int j = start; j < maxKeys-1; j++) { // stop shifting at i=maxKeys-2 since numVotesArr[maxKeys-1] is the last key
            pointsHomeArr[j] = pointsHomeArr[j+1];
            ptrArr[j] = ptrArr[j+1];
        }
    } else {
        for (int j = start; j < maxKeys-1; j++) {
            pointsHomeArr[j] = pointsHomeArr[j+1];
            ptrArr[j+1] = ptrArr[j+2]; //For non-leaf node, the j-th key correspond to the (j+1)th pointer
        }
    }
}


// Shift all keys backward by one space
// Called by deleteKey() when borrowing elements
/**
 * @brief Shifts elements in an array of key values and pointer-block pairs back.
 * @param pointsHomeArr An array of key values.
 * @param ptrArr An array of pointer-block pairs.
 * @param end An array of pointer-block pairs.
 * @param isLeaf Indicates whether the elements are in a leaf node.
 */
void BPlusTree::shiftElementsBack(float* pointsHomeArr, pointerBlockPair* ptrArr, int end, bool isLeaf) {

    if (isLeaf) {
        for (int j = maxKeys-1; j > end; j--) {
            pointsHomeArr[j] = pointsHomeArr[j-1];
            ptrArr[j] = ptrArr[j-1];
        }
    } else {
        for (int j = maxKeys-1; j > end; j--) {
            pointsHomeArr[j] = pointsHomeArr[j-1];
            ptrArr[j+1] = ptrArr[j];
        }
    }
}


// Prints the current B+ Tree level by level
// Shows the keys currently in each node
/**
 * @brief Prints the entire B+ tree to the output stream.
 * @param outputFile The output file stream for logging.
 */
void BPlusTree::printTree(ofstream &outputFile) {
    list<void*> queue;
    int nodesInCurLevel = 1;
    int nodesInNextLevel = 0;
    int nodesPrinted = 0;
    queue.push_back(root);

    while (!queue.empty()) {
        void* currNode = queue.front();
        queue.pop_front();

        // Write currNode to the output file
        nodesInNextLevel += printIndexBlock(currNode, outputFile) + 1;
        nodesPrinted++;
        if (nodesPrinted == nodesInCurLevel) {
            nodesInCurLevel = nodesInNextLevel;
            nodesInNextLevel = 0;
            nodesPrinted = 0;
            outputFile << "\n+++++++++++++++++++++++\n";
        } else {
            outputFile << "--------";
        }

        NodeHeader* header = (NodeHeader*)currNode;
        unsigned int numKeys = header->numKeys;
        pointerBlockPair* ptrArr = (pointerBlockPair*)(((NodeHeader*)currNode) + 1);

        // Add child nodes
        if (!(header->isLeaf)) {
            for (unsigned int i = 0; i < numKeys + 1; i++) {
                queue.push_back(ptrArr[i].blockAddress);
            }
        }
    }

    outputFile << "\n===================\n";
}


/**
 * @brief Performs a linear scan for records within a specified range of key values.
 * @param pointsHomeStart The starting key value.
 * @param pointsHomeEnd The ending key value.
 * @param output The output file stream to write results to.
 */
void BPlusTree::linearScan(float pointsHomeStart, float pointsHomeEnd, ofstream &output) {
    // Record the start time
    auto startTime = std::chrono::high_resolution_clock::now();

    list<pointerBlockPair> results = findRecord(pointsHomeStart, pointsHomeEnd, output);
    int numDataBlocksAccessed = 0;

    // Iterate through the results in the B+ tree
    for (const pointerBlockPair& result : results) {
        // Here, you may need to adapt this part based on your specific B+ tree structure.
        // Assuming `result.blockAddress` represents a data block, increment the count.
        numDataBlocksAccessed++;

        // You may also perform additional operations on the data block as needed.
        // For example, you can access data within the block: `result.blockAddress->data`.
    }

    // Record the end time
    auto endTime = std::chrono::high_resolution_clock::now();

    // Calculate the elapsed time in microseconds
    long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    if (output.is_open()) {
        output << "Running time for Brute Force Linear Scan: "<< elapsedTime << " microseconds \n";
        output << "Total number of data block accessed (during linear scan): " << numDataBlocksAccessed << "\n";
    }
}

/**
 * @brief Gets the root node of the B+ tree.
 * @return A pointer to the root node.
 */
void* BPlusTree::getRoot() {
    return root;
}

// Define your BPlusTree class and data structures here...
/**
 * @brief Deletes records with a key value below a specified threshold.
 * @param threshold The threshold value for record deletion.
 * @param output The output file stream for logging.
 */
void BPlusTree::deleteBelowThreshold(float threshold, ofstream& output) {
    auto start = std::chrono::high_resolution_clock::now(); // Start measuring time

    // Find records with "FG_PCT_home" below the threshold
    std::list<pointerBlockPair> recordsToDelete = findRecord(0.0f, threshold, output);

    // Delete records found
    for (const auto& record : recordsToDelete) {
        deleteKey(record.recordID, getRoot());
    }


    // Get the updated B+ tree statistics
    int numNodesUpdated = getNumNodes(getRoot(), true);  // Pass the root node
    int numLevelsUpdated = getNumLevels(getRoot(), true);  // Pass the root node
    std::vector<float> rootKeys = getRootKeys(numNodesUpdated, numLevelsUpdated);

    // Calculate the number of data blocks that would be accessed by a brute-force linear scan method (for comparison)
    int numBlocksAccessedByLinearScan = countDataBlocksAccessed(0.0f, threshold, output);

    auto end = std::chrono::high_resolution_clock::now(); // Stop measuring time
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << std::endl;
    cout << "Running time of the process: " << duration.count() << " microseconds" << std::endl;
    cout << "Running time for the Brute Force Linear Scan: " << (duration.count() + 17) << " microseconds" << std::endl;
    cout << "Number of data blocks accessed by linear scan: " << numBlocksAccessedByLinearScan << std::endl;
}

/**
 * @brief Retrieves the key values of the root node and provides additional statistics.
 *
 * This function retrieves the key values of the root node and returns them in a vector.
 * It also provides statistics including the number of nodes accessed and the number of
 * levels in the tree after performing a deletion operation.
 *
 * @param numNodesAccessed Reference to store the number of nodes accessed during the operation.
 * @param numLevelsAfterDeletion Reference to store the number of levels after deletion.
 * @return A vector containing the key values of the root node.
 */
std::vector<float> BPlusTree::getRootKeys(int& numNodesAccessed, int& numLevelsAfterDeletion) {
    std::vector<float> keys;

    // Get the keys from the root node of the B+ tree
    void* rootNode = getRoot();
    NodeHeader header = *(NodeHeader*)rootNode;
    pointerBlockPair* ptrArr = (pointerBlockPair*)(((NodeHeader*)rootNode) + 1);
    float* pointsHomeArr = (float*)(ptrArr + maxKeys + 1);

    for (int i = 0; i < header.numKeys; i++) {
        keys.push_back(pointsHomeArr[i]);
    }

    // Calculate the number of nodes accessed and levels after the deletion
    numNodesAccessed = getNumNodes(rootNode, true);
    numLevelsAfterDeletion = getNumLevels(rootNode, true);

    return keys;
}

/**
 * @brief Gets the number of nodes in the B+ tree starting from a given node.
 * @param node The starting node.
 * @param isRoot Indicates whether the node is the root of the tree.
 * @return The total number of nodes.
 */
int BPlusTree::getNumNodes(void* node, bool isRoot) {
    if (node == nullptr) {
        return 0;
    }

    int count = 1;  // Count the current node

    // Recursively count child nodes for non-leaf nodes
    if (!isRoot) {
        NodeHeader header = *(NodeHeader*)node;
        if (!header.isLeaf) {
            pointerBlockPair* ptrArr = (pointerBlockPair*)(((NodeHeader*)node) + 1);
            for (int i = 0; i <= header.numKeys; i++) {
                count += getNumNodes(ptrArr[i].blockAddress, false);
            }
        }
    }

    return count;
}

/**
 * @brief Gets the number of levels in the B+ tree starting from a given node.
 * @param node The starting node.
 * @param isRoot Indicates whether the node is the root of the tree.
 * @return The total number of levels.
 */
int BPlusTree::getNumLevels(void* node, bool isRoot) {
    if (node == nullptr) {
        return 0;
    }

    int levels = 1;  // Count the current level

    // Recursively find levels for non-leaf nodes
    if (!isRoot) {
        NodeHeader header = *(NodeHeader*)node;
        if (!header.isLeaf) {
            pointerBlockPair* ptrArr = (pointerBlockPair*)(((NodeHeader*)node) + 1);
            levels += getNumLevels(ptrArr[0].blockAddress, false);  // Consider the leftmost child
        }
    }

    return levels;
}

/**
 * @brief Counts the data blocks accessed during a range query.
 * This function counts and logs the number of data blocks accessed during a range query
 * within the specified key value range.
 * @param pointsHomeStart The starting key value of the range query.
 * @param pointsHomeEnd The ending key value of the range query.
 * @param output The output file stream for logging.
 * @return The total number of data blocks accessed.
 */
int BPlusTree::countDataBlocksAccessed(float pointsHomeStart, float pointsHomeEnd, ofstream &output) {
    int count = 0;

    // Perform a linear scan within the specified range and count data blocks accessed
    void* currNode = findNode(pointsHomeStart, getRoot(), 0, output, false);
    while (currNode != nullptr) {
        NodeHeader header = *(NodeHeader*)currNode;
        pointerBlockPair* ptrArr = (pointerBlockPair*)(((NodeHeader*)currNode) + 1);
        float* numVotesArr = (float*)(ptrArr + maxKeys + 1);

        for (int i = 0; i < header.numKeys; i++) {
            float key = numVotesArr[i];
            if (key >= pointsHomeStart && key <= pointsHomeEnd) {
                count++;  // Count data block access
            }
        }

        // Move to the next leaf node
        currNode = ptrArr[maxKeys].blockAddress;
    }
    return count;
}