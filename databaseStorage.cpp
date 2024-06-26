#include "databaseStorage.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include "ProjectStructure.h"
#include <filesystem>
#include <ctime>
#include <cmath>

using namespace std;
/**
 * @brief Retrieves database records from a file and returns them as a vector of GameData objects.
 *
 * This method reads a file containing database records, parses each line, and constructs
 * GameData objects representing the records. It returns these objects as a vector.
 *
 * @return A vector of GameData objects representing database records.
 */
std::vector<GameData> databaseStorage::getDatabaseRecord(){
    string pathtofile = std::filesystem::current_path().parent_path().string() + "/games.txt";
    std::ifstream inputFile(pathtofile);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
    }

    std::vector<GameData> gameDataList;

    std::string line;

    int count = 1;

    // Skip the first line (column titles)
    std::getline(inputFile, line);


    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        GameData gameData;
        count++;

        string dateString;
        iss >> dateString;

        std::tm tm = {}; // Initialize a std::tm structure with zeros
        std::istringstream dateStream(dateString);
        dateStream >> tm.tm_mday;   // Day of the month
        dateStream.ignore(1);       // Ignore the '/'
        dateStream >> tm.tm_mon;    // Month (0-based, so subtract 1)
        dateStream.ignore(1);       // Ignore the '/'
        dateStream >> tm.tm_year;   // Year (since 1900)

        // Adjust month to be 0-based (January is 0)
        tm.tm_mon--;

        // Adjust year to be years since 1900
        tm.tm_year -= 1900;

        // Fill in other fields with reasonable defaults (e.g., time is set to midnight)
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;

        // Use std::mktime to convert std::tm to a time representation (time_t)
        time_t timestamp = std::mktime(&tm);
        gameData.GAME_DATE_EST = timestamp;

        if (!(iss >> gameData.TEAM_ID_home >> gameData.PTS_home >>
                  gameData.FG_PCT_home >> gameData.FT_PCT_home >> gameData.FG3_PCT_home >>
                  gameData.AST_home >> gameData.REB_home >> gameData.HOME_TEAM_WINS)) {
            continue;
        }
        // Handle missing values (nulls) by setting them to 0
        if (iss.fail()) {
            gameData.FG_PCT_home = 0.0;
            gameData.FT_PCT_home = 0.0;
            gameData.FG3_PCT_home = 0.0;
            gameData.AST_home = 0;
            gameData.REB_home = 0;
            gameData.HOME_TEAM_WINS = 0;
        }

        // Store the parsed data in the vector
        gameDataList.push_back(gameData);
        //cout << count << endl;
    }

    return gameDataList;
}

