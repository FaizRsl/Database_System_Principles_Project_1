#ifndef PROJECT1_PROJECTSTRUCTURE_H
#define PROJECT1_PROJECTSTRUCTURE_H

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

#pragma pack(1)
/**
 * @brief Struct to represent game data.
 */
struct GameData {
    time_t GAME_DATE_EST; //8
    unsigned int TEAM_ID_home; //4
    unsigned short PTS_home; //2
    float FG_PCT_home; //4
    float FT_PCT_home; //4
    float FG3_PCT_home; //4
    unsigned short AST_home; //2
    unsigned short REB_home; //2
    unsigned short HOME_TEAM_WINS; //2
};

/**
 * @brief Struct to represent index mapping.
 */
struct indexMapping
{
    float key;
    int indexOfRecord; // -1 represents a deleted record in block
};

/**
 * @brief Struct to represent a pointer-block pair.
 */
struct pointerBlockPair // 8 bytes
{
    void* blockAddress;
    float recordID; // -1 indicates an overflow, any positive indicates the a duplicated record
};

// Used to store relevant header information for a node in the B+ tree
/**
 * @brief Struct to store header information for a node in the B+ tree.
 */
struct NodeHeader // 14->16 (padded) bytes, multiple of 4
{
    unsigned int numKeys;
    pointerBlockPair pointerToParent;
    bool isLeaf;
};

/**
 * @brief Struct to store duplicate counts.
 */
struct duplicateCounter{
    float keyValue;
    int count;
};



#endif //PROJECT1_PROJECTSTRUCTURE_H