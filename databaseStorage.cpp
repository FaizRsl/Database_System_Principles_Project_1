#include "databaseStorage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "ProjectStructure.h"

std::vector<GameData> databaseStorage::getDatabaseRecord(){
    //need to change to your own absolute path cos im not sure how to set it to work with every1 pc
    std::ifstream inputFile("D:\\Desktop Folder\\NTU\\Year3 Sem1\\Database System Principles\\Projects\\Project1\\games.txt");
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

        gameData.recordID = count;
        count++;

        if (!(iss >> gameData.GAME_DATE_EST >> gameData.TEAM_ID_home >> gameData.PTS_home >>
                  gameData.FG_PCT_home >> gameData.FT_PCT_home >> gameData.FG3_PCT_home >>
                  gameData.AST_home >> gameData.REB_home >> gameData.HOME_TEAM_WINS)) {
            //std::cerr << "Error parsing line: " << line << std::endl;
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


    // For example, you can access the data for the first game as follows:
    GameData firstGame = gameDataList[0];
    std::size_t dataSize = sizeof(firstGame);
    std::cout << "Size of GameData object: " << dataSize << " bytes" << std::endl;

    //std::cout << "GAME_DATE_EST: " << firstGame.GAME_DATE_EST << std::endl;
    //std::cout << "TEAM_ID_home: " << firstGame.TEAM_ID_home << std::endl;

    return gameDataList;
}

