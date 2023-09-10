#ifndef PROJECT1_DATABASESTORAGE_H
#define PROJECT1_DATABASESTORAGE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ProjectStructure.h"

using namespace std;

class databaseStorage{
    public:
        vector<GameData> getDatabaseRecord();
};

#endif //PROJECT1_DATABASESTORAGE_H
