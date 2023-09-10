#ifndef PROJECT1_PROJECTSTRUCTURE_H
#define PROJECT1_PROJECTSTRUCTURE_H


#include <string>

struct GameData {
    unsigned int recordID;
    std::string GAME_DATE_EST;
    int TEAM_ID_home;
    int PTS_home;
    double FG_PCT_home;
    double FT_PCT_home;
    double FG3_PCT_home;
    int AST_home;
    int REB_home;
    int HOME_TEAM_WINS;
};

struct indexMapping
{
    unsigned int recordID;
    int indexOfRecord; // -1 represents a deleted record in block
};

struct pointerBlockPair // 8 bytes
{
    void* blockAddress;
    int recordID; // -1 indicates an overflow, any positive indicates the a duplicated record
};

// Used to store relevant header information for a node in the B+ tree
struct NodeHeader // 14->16 (padded) bytes, multiple of 4
{
    unsigned int numKeys;
    pointerBlockPair pointerToParent;
    bool isLeaf;
};


#endif //PROJECT1_PROJECTSTRUCTURE_H
