#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include "Database.h"
#include "ProjectStructure.h"

using namespace std;

int main() {

    Database* db;
    const unsigned int blockSize = 400;
    // Using disk capacity of 100MB
    unsigned int diskSize = 100;
    string resultsDir = filesystem::current_path().parent_path().string() + "\\outputs\\";
    db = new Database(diskSize, blockSize);

    ofstream exp1Output;
    ifstream exp1Input;
    ofstream exp2Output;
    ifstream exp2Input;
    ofstream exp3Output;
    ifstream exp3Input;
    ofstream exp4Output;
    ifstream exp4Input;
    ofstream exp5Output;
    ofstream test;
    ifstream testInput;
    string line; // Open an output file

    int choice;

    db->importData();

    /*vector<duplicateCounter> potato = db->bPlusTree->duplicateCount;
    for (auto& entry : potato) {
        if (entry.keyValue == 0.5) {
            // Value found, increment the count
            cout << "duplicate count is: " << entry.count << endl;
            break;
        }
    }*/

    while(1){
        cout << "1) Run Experiment 1\n"
                "2) Run Experiment 2\n"
                "3) Run Experiment 3\n"
                "4) Run Experiment 4\n"
                "5) Run Experiment 5\n"
                "6) Exit program\n";

        cout << "Enter your choice: ";
        try {
            cin >> choice;
            if (cin.fail() || choice < 1 || choice > 6) {
                throw runtime_error("Invalid choice. Please enter an integer from 1-6.");
            }
        } catch (const exception& e) {
            cout << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch(choice){
            case 1:
                cout << "Experiment 1:" <<endl;
                exp1Output.open(resultsDir + "experiment1output.txt");
                // Write output to file
                exp1Output << "Number of records: " << db->numRecords << endl;
                exp1Output << "Size of a record: " << (sizeof(GameData)) << "-Byte" << endl;
                exp1Output << "Number of records stored in a block: " << ceil((blockSize - sizeof(unsigned int))/ (sizeof(GameData) + sizeof(indexMapping))) << endl;
                exp1Output << "Number of blocks for storing the data: " << db->numBlocks << endl;
                exp1Output.close();

                // to read the data from "experiment1output.txt"
                exp1Input.open(resultsDir + "experiment1output.txt");
                if(exp1Input.is_open()){
                    while (getline(exp1Input, line)) {
                        cout << line << endl;
                    }
                } else {
                    cout << "Unable to read the file" << endl;
                }
                exp1Input.close();
                break;

            case 2:
                cout << "Experiment 2:" <<endl;
                exp2Output.open(resultsDir + "experiment2output.txt");

                exp2Output << "Parameter n of the B+ Tree: " << db->bPlusTree->maxKeys << "\n";
                exp2Output << "Number of nodes (excluding overflow): " << db->bPlusTree->numNodes << "\n";
                exp2Output << "Number of overflow nodes: " << db->bPlusTree->numOverflowNodes << "\n";
                exp2Output << "Total Number of nodes (including overflow): " << (db->bPlusTree->numNodes + db->bPlusTree->numOverflowNodes) << "\n";
                exp2Output << "Number of levels of the B+ tree: " << db->bPlusTree->height+1 << "\n"; // DBMS starts height at 0
                exp2Output << "Root: \n";
                //db->bPlusTree->printIndexBlock(db->bPlusTree->root, exp2Output);
                db->bPlusTree->printTree(exp2Output);
                //db->printBPlusTree(exp2Output);
                exp2Output.close();

                cout << "Experiment 2:" <<endl;
                exp2Input.open(resultsDir + "experiment2output.txt");
                if(exp2Input.is_open()){
                    while (getline(exp2Input, line)) {
                        cout << line << endl;
                    }
                } else {
                    cout << "Unable to read the file" << endl;
                }
                exp2Input.close();
                break;

            case 3:
                cout << "Experiment 3: " << endl;
                // already writing in the file in BPlusTree.cpp in findNode and find record
//                exp3Output.open(resultsDir + "experiment3output.txt");
//                exp3Output << "Retrieve movies with 'FG_PCT_HOME' equal to 0.5";
//                db->bPlusTree->findRecord(0.5, 0.5001, exp3Output);
//                exp3Output.close();
                exp3Input.open(resultsDir + "experiment3output.txt");
                if(exp3Input.is_open()){
                    while (getline(exp3Input, line)) {
                        cout << line << endl;
                    }
                } else {
                    cout << "Unable to read the file" << endl;
                }
                exp3Input.close();
                break;
            case 4:
                cout << "Experiment 4: ";
                // already writing in the file in BPlusTree.cpp in findNode and find record
//                exp4Output.open(resultsDir + "experiment4output.txt");
//                exp4Output << "Retrieve movies with 'FG_PCT_HOME' equal to 0.5";
//                db->bPlusTree->findRecord(0.6, 1.0, exp4Output);
//                exp4Output.close();
                // reading from the txt file for experiment-4
                exp4Input.open(resultsDir + "experiment4output.txt");
                if(exp4Input.is_open()){
                    while (getline(exp4Input, line)) {
                        cout << line << endl;
                    }
                } else {
                    cout << "Unable to read the file" << endl;
                }
                exp4Input.close();
                break;
            case 5: //test case for temporary
                cout << "Experiment5:" << endl;
                exp5Output.open(resultsDir + "experiment5output.txt");
                db->bPlusTree->findKeyToDelete(0.35,db->bPlusTree->root, exp5Output);
                exp5Output.close();
                break;
            case 6:
                cout << "Terminating the project" << endl;
                exit(0);
            default:
                cout << "Invalid input. Please input a value from 1 to 6." << endl;
                break;


        }
    }



    return 0;
}































//#include <fstream>
//#include <iostream>
//#include <cstring>
//#include <filesystem>
//#include "Database.h"
//
//using namespace std;
//
//int main() {
//    Database* db;
//    const unsigned int blockSize = 400;
//    unsigned int diskSize = 100;
//    string resultsDir = filesystem::current_path().parent_path().string() + "/outputs/";
//    db = new Database(diskSize, blockSize);
//

//
//    int choice;
//
//    db->importData();
//
//    while (true) {
//        cout << "1) Run Experiment 1\n"
//                "2) Run Experiment 2\n"
//                "3) Run Experiment 3\n"
//                "4) Run Experiment 4\n"
//                "5) Run Experiment 5\n"
//                "6) Exit program\n";
//
//        cout << "Enter your choice: ";
//        try {
//            cin >> choice;
//            if (cin.fail() || choice < 1 || choice > 6) {
//                throw runtime_error("Invalid choice. Please enter an integer from 1-6.");
//            }
//        } catch (const exception& e) {
//            cout << e.what() << endl;
//            cin.clear();
//            cin.ignore(numeric_limits<streamsize>::max(), '\n');
//            continue;
//        }
//
//        switch (choice) {
//            case 1:
//                cout << "Experiment 1:" <<endl;
//                // To write in the "experiment1output.txt"
//                exp1Output.open(resultsDir + "experiment1output.txt");
//                // Write output to file
//                exp1Output << "Number of records: " << db->numRecords << endl;
//                exp1Output << "Size of a record: " << (sizeof(GameData)) << "-Byte" << endl;
//                exp1Output << "Number of records stored in a block: " << ceil((blockSize - sizeof(unsigned int))/ (sizeof(GameData) + sizeof(indexMapping))) << endl;
//                exp1Output << "Number of blocks for storing the data: " << db->numBlocks << endl;
//                exp1Output.close();
//
//
//
//                // print to screen
////                cout << "Number of records: " <<  db->numRecords << endl;
////                cout << "Size of a record: " << (sizeof(GameData)) << "-Byte" << endl;
////                cout << "Number of records stored in a block: " << ceil((blockSize - sizeof(unsigned int))/ (sizeof(GameData) + sizeof(indexMapping))) << endl;
////                cout << "Number of blocks for storing the data: " << db->numBlocks << endl;
//
//
//            case 2:
//                // To write in the "experiment2output.txt"
//                exp2Output.open(resultsDir + "experiment2output.txt");
//                exp2Output << "Parameter n of the B+ Tree: " << db->bPlusTree->maxKeys << "\n";
//                exp2Output << "Number of nodes (excluding overflow): " << db->bPlusTree->numNodes << "\n";
//                exp2Output << "Number of overflow nodes: " << db->bPlusTree->numOverflowNodes << "\n";
//                exp2Output << "Total Number of nodes (including overflow): " << (db->bPlusTree->numNodes + db->bPlusTree->numOverflowNodes) << "\n";
//                exp2Output << "Number of levels of the B+ tree: " << db->bPlusTree->height+1 << "\n"; // DBMS starts height at 0
//                exp2Output.close();
//                cout << "Experiment 2:" <<endl;
//                exp2Input.open(resultsDir + "experiment2output.txt");
//                if(exp2Input.is_open()){
//                    while (getline(exp2Input, line)) {
//                        cout << line << endl;
//                    }
//                } else {
//                    cout << "Unable to read the file" << endl;
//                }
//                exp2Input.close();
//                break;
//
//
//                // print to screen
////                cout << "Parameter n of the B+ Tree: " << db->bPlusTree->maxKeys << "\n";
////                cout << "Number of nodes (excluding overflow): " << db->bPlusTree->numNodes << "\n";
////                cout << "Number of overflow nodes: " << db->bPlusTree->numOverflowNodes << "\n";
////                cout << "Total Number of nodes (including overflow): " << (db->bPlusTree->numNodes + db->bPlusTree->numOverflowNodes) << "\n";
////                cout << "Height of B+ Tree: " << db->bPlusTree->height+1 << "\n"; // DBMS starts height at 0
////                cout << "\n=====Content of root=====" << endl;
////                cout << "Root: \n";
////                exp2Output << "Root: \n";
////                //db->bPlusTree->printIndexBlock(db->bPlusTree->root, exp2Output);
////                db->bPlusTree->printTree(exp2Output);
////                //db->printBPlusTree(exp2Output);
//
//                //exp2Output.close();
//
//            case 3:
//                cout << "Experiment 3: " << endl;
//                exp3Output.open(resultsDir + "experiment3output.txt");
//                cout << "Retrieve movies with 'FG_PCT_HOME' equal to 0.5" << endl;
//                db->bPlusTree->findRecord(0.5, 0.5, exp3Output);
//                break;
//            case 4:
//                cout << "Experiment 4:" << endl;
//                //Write output to file
//                exp4Output.open(resultsDir + "experiment4output.txt");
//                db->bPlusTree->findRecord(0.6, 0.1, exp4Output);
//                //print to screen
//                cout << "Retrieve movies with 'FG_PCT_HOME' from 0.6 to 1" << "\n";
//                //cout << db->bPlusTree->findRecord(0.6, 0.1, exp4Output) << "\n";
//                //debug this
//                exp4Output.close();
//                cout << endl << endl;
//                test.close();
//                break;
//            case 5:
//                // To write in the "experiment2output.txt"
////                test.open(resultsDir + "test.txt");
////                test.close();
//                cout << "test" << endl;
//                testInput.open(resultsDir + "test.txt");
//                if(testInput.is_open()){
//                    while (getline(testInput, line)) {
//                        cout << line << endl;
//                    }
//                } else {
//                    cout << "Unable to read the file" << endl;
//                }
//                testInput.close();
//                //db->bPlusTree->printTree(test);
//
//                break;
//            case 6:
//                cout << "Terminating the Program" << endl;
//                return 0;
//                break;// Exit the program
//            default:
//                cout << "Invalid input. Please try again." << endl;
//                break;
//        }
//    }
//
//    return 0;
//}
//
