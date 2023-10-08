#ifndef PROJECT1_BPLUSTREE_H
#define PROJECT1_BPLUSTREE_H

#include <cstdlib>
#include <list>
#include "ProjectStructure.h"
#include <iostream>
#include <math.h>
#include <fstream>
#include "vector"

using namespace std;

/**
 * @brief Represents a B+ tree data structure for indexing game data.
 */
class BPlusTree {
public:
    void *root; ///< Pointer to the root node of the B+ tree.
    unsigned int height; ///< The height of the B+ tree.
    unsigned int maxKeys; ///< The maximum number of keys that a node can hold.
    unsigned int sizeOfNode; ///< The size (in bytes) of a B+ tree node.

    vector<duplicateCounter> duplicateCount; ///< Vector to store duplicate key-value counts.

    // For Experiments
    unsigned int numNodes; ///< The total number of nodes in the B+ tree.
    unsigned int numOverflowNodes; ///< The total number of overflow nodes in the B+ tree.
    int numIndexAccessed; ///< The number of index nodes accessed during operations.
    int numNodesDeleted; ///< The number of nodes deleted during operations.
    int numOverflowNodesAccessed; ///< The number of overflow nodes accessed during operations.
    int numOverflowNodesDeleted; ///< The number of overflow nodes deleted during operations.

    int count = 0; ///< A counter used for various purposes.

    //Initialisation and setting functions
    /**
     * @brief Constructs a new BPlusTree object.
     * @param sizeOfNode The size (in bytes) of a B+ tree node.
     */
    BPlusTree(unsigned int sizeOfNode);

    /**
     * @brief Creates a new B+ tree node.
     *
     * @param isLeaf Indicates whether the new node is a leaf node.
     * @param isOverflow Indicates whether the new node is an overflow node.
     * @return A pointer to the newly created node.
     */
    void* getNewNode(bool isLeaf, bool isOverflow);

    //Retrieval functions
    /**
     * @brief Finds records within a specified range of key values.
     * @param pointsHomeStart The starting key value.
     * @param pointsHomeEnd The ending key value.
     * @param output The output file stream to write results to.
     * @return A list of pointer-block pairs representing the found records.
     */
    list<pointerBlockPair> findRecord(float pointsHomeStart, float pointsHomeEnd, ofstream &output);

    /**
     * @brief Finds the node containing a specific key value within the B+ tree.
     * @param points_home points_home The key value to search for.
     * @param node The current node being searched.
     * @param currentHeight The current height in the B+ tree.
     * @param output The output file stream for logging.
     * @param willPrint Indicates whether to print debug information.
     * @return A pointer to the node containing the key.
     */
    void* findNode(float points_home, void* node, unsigned int currentHeight, ofstream &output, bool willPrint);

    /**
     * @brief Gets the number of nodes in the B+ tree starting from a given node.
     *
     * @param node The starting node.
     * @param isRoot Indicates whether the node is the root of the tree.
     * @return The total number of nodes.
     */
    int getNumNodes(void* node, bool isRoot);

    /**
     * @brief Gets the number of levels in the B+ tree starting from a given node.
     *
     * @param node The starting node.
     * @param isRoot Indicates whether the node is the root of the tree.
     * @return The total number of levels.
     */
    int getNumLevels(void* node, bool isRoot);

    /**
     * @brief Counts the data blocks accessed during a range query.
     *
     * This function counts and logs the number of data blocks accessed during a range query
     * within the specified key value range.
     *
     * @param pointsHomeStart The starting key value of the range query.
     * @param pointsHomeEnd The ending key value of the range query.
     * @param output The output file stream for logging.
     * @return The total number of data blocks accessed.
     */
    int countDataBlocksAccessed(float pointsHomeStart, float pointsHomeEnd, ofstream &output);

    /**
     * @brief Gets the root node of the B+ tree.
     *
     * @return A pointer to the root node.
     */
    void* getRoot();

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
    std::vector<float> getRootKeys(int& numNodesAccessed, int& numLevelsAfterDeletion);

    //Functions for inserting a record
    /**
     * @brief Inserts a record into the B+ tree.
     * @param points_home The key value of the record.
     * @param record The pointer-block pair representing the record.
     */
    void insertRecord(float points_home, pointerBlockPair record);

    /**
     * @brief Splits a leaf node during record insertion.
     * @param points_home The key value of the record to insert.
     * @param record The pointer-block pair representing the record.
     * @param nodeToSplit The leaf node to split.
     * @param ptrArr An array of pointer-block pairs.
     * @param ptsHomeArr An array of key values.
     */
    void splitLeafNode(float points_home, pointerBlockPair record, void* nodeToSplit, pointerBlockPair* ptrArr, float* ptsHomeArr);

    /**
     * @brief Splits a non-leaf node during record insertion.
     *
     * @param points_home The key value of the record to insert.
     * @param record The pointer-block pair representing the record.
     * @param nodeToSplit The non-leaf node to split.
     * @param ptrArr An array of pointer-block pairs.
     * @param ptsHomeArr An array of key values.
     */
    void splitNonLeafNode(float points_home, pointerBlockPair record, void* nodeToSplit, pointerBlockPair* ptrArr, float* ptsHomeArr);

    /**
     * @brief Updates the parent node after splitting a node.
     *
     * @param parentNode The parent node.
     * @param rightNode The right sibling node.
     * @param newParentKey The key value to update in the parent node.
     */
    void updateParentNodeAfterSplit(void* parentNode, void* rightNode, float newParentKey);

    //Functions for deleting a record
    /**
     * @brief Deletes a record with a specific key value from the B+ tree.
     *
     * @param pointsHome The key value of the record to delete.
     * @param nodeToDeleteFrom The node to delete the record from.
     */
    void deleteKey(float pointsHome, void* nodeToDeleteFrom);

    /**
     * @brief Merges two nodes (either leaf or non-leaf) into one node.
     *
     * @param leftNode The left node to be merged.
     * @param rightNode The right node to be merged.
     */
    void mergeNodes(void* leftNode, void* rightNode);

    /**
     * @brief Shifts elements in an array of key values and pointer-block pairs forward.
     *
     * @param pointsHomeArr An array of key values.
     * @param ptrArr An array of pointer-block pairs.
     * @param start The starting index for shifting.
     * @param isLeaf Indicates whether the elements are in a leaf node.
     */
    void shiftElementsForward(float* pointsHomeArr, pointerBlockPair* ptrArr, int start, bool isLeaf);

    /**
     * @brief Shifts elements in an array of key values and pointer-block pairs back.
     *
     * @param pointsHomeArr An array of key values.
     * @param ptrArr An array of pointer-block pairs.
     * @param end The ending index for shifting.
     * @param isLeaf Indicates whether the elements are in a leaf node.
     */
    void shiftElementsBack(float* pointsHomeArr, pointerBlockPair* ptrArr, int end, bool isLeaf);

    /**
     * @brief Deletes records with a key value below a specified threshold.
     *
     * @param threshold The threshold value for record deletion.
     * @param output The output file stream for logging.
     */
    void deleteBelowThreshold(float threshold, ofstream &output);

    //Functions for Experiments/Visualization
    /**
     * @brief Prints the contents of an index block to the output stream.
     *
     * @param node The node to print.
     * @param output The output file stream for logging.
     * @return The number of keys printed.
     */
    int printIndexBlock(void* node, ofstream &output);
    /**
     * @brief Prints the root node of the B+ tree to the output stream.
     *
     * @param output The output file stream for logging.
     */
    void printRoot(ofstream &output);

    /**
     * @brief Prints the entire B+ tree to the output stream.
     *
     * @param output The output file stream for logging.
     */
    void printTree(ofstream &output);

    /**
     * @brief Gets the maximum key value that is less than or equal to the specified maxVal.
     *
     * @param maxVal The maximum value to compare against.
     * @param start The starting index to search from.
     * @return The index of the maximum key value.
     */
    int getMax(float maxVal, float start);

    /**
     * @brief Finds the node containing a specific key value to delete and returns it.
     *
     * @param pointsHome The key value to delete.
     * @param rootNode The root node of the B+ tree.
     * @param output The output file stream for logging.
     * @return A pointer to the node containing the key to delete.
     */
    void* findKeyToDelete(float pointsHome, void* rootNode, ofstream &output);

    /**
     * @brief Performs a linear scan for records within a specified range of key values.
     *
     * @param pointsHomeStart The starting key value.
     * @param pointsHomeEnd The ending key value.
     * @param output The output file stream to write results to.
     */
    void linearScan(float pointsHomeStart, float pointsHomeEnd, ofstream &output);
    //string printTree(ofstream &outputFile);

    void avgFG3(float pointsHomeStart, float pointsHomeEnd, ofstream &output);

};

#endif //PROJECT1_BPLUSTREE_H