#ifndef PROJECT1_DATABASESTORAGE_H
#define PROJECT1_DATABASESTORAGE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ProjectStructure.h"

using namespace std;
/**
 * @brief databaseStorage class represents a storage mechanism for database records.
 *
 * This class provides a method to retrieve database records as a vector of GameData objects.
 *
 */
class databaseStorage{
    public:
        /**
         * @brief Retrieves database records.
         *
         * This function retrieves database records and returns them as a vector of GameData objects.
         *
         * @return A vector of GameData objects representing database records.
         */
        vector<GameData> getDatabaseRecord();
};

#endif //PROJECT1_DATABASESTORAGE_H
