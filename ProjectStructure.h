#ifndef PROJECT1_PROJECTSTRUCTURE_H
#define PROJECT1_PROJECTSTRUCTURE_H

#include <string>
#include <iostream>
#include <ctime>

using namespace std;

#pragma pack(1)
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

struct indexMapping
{
    float key;
    int indexOfRecord; // -1 represents a deleted record in block
};

struct pointerBlockPair // 8 bytes
{
    void* blockAddress;
    float recordID; // -1 indicates an overflow, any positive indicates the a duplicated record
};

// Used to store relevant header information for a node in the B+ tree
struct NodeHeader // 14->16 (padded) bytes, multiple of 4
{
    unsigned int numKeys;
    pointerBlockPair pointerToParent;
    bool isLeaf;
};



#endif //PROJECT1_PROJECTSTRUCTURE_H
