#include <iostream>
#include <fstream>
#include <cstring>
#include <filesystem>
#include "Database.h"

using namespace std;

int main() {

    Database* db;
    const unsigned int blockSize = 400;
    // Using disk capacity of 100MB
    unsigned int diskSize = 100;
    string resultsDir = filesystem::current_path().parent_path().string() + "//outputs//";
    db = new Database(diskSize, blockSize);

    ofstream exp1Output;
    ofstream exp2Output;
    ofstream exp3Output;
    ofstream exp4Output;
    ofstream exp5Output;
    ofstream test; // Open an output file

    int choice;

    db->importData();

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


        switch(choice){
            case 1:
                cout << "Experiment 1:" <<endl;
                exp1Output.open(resultsDir + "experiment1output.txt");
                // Write output to file
                exp1Output << "Number of records: " << db->numRecords << endl;
                exp1Output << "Size of a record: " << (sizeof(GameData)) << "-Byte" << endl;
                exp1Output << "Number of records stored in a block: " << ceil((blockSize - sizeof(unsigned int))/ (sizeof(GameData) + sizeof(indexMapping))) << endl;
                exp1Output << "Number of blocks for storing the data: " << db->numBlocks << endl;

                // print to screen
                cout << "Number of records: " <<  db->numRecords << endl;
                cout << "Size of a record: " << (sizeof(GameData)) << "-Byte" << endl;
                cout << "Number of records stored in a block: " << ceil((blockSize - sizeof(unsigned int))/ (sizeof(GameData) + sizeof(indexMapping))) << endl;
                cout << "Number of blocks for storing the data: " << db->numBlocks << endl;
                exp1Output.close();
                break;

            case 2:
                cout << "Experiment 2:" <<endl;
                exp2Output.open(resultsDir + "experiment2output.txt");

                exp2Output << "Parameter n of the B+ Tree: " << db->bPlusTree->maxKeys << "\n";
                exp2Output << "Number of nodes (excluding overflow): " << db->bPlusTree->numNodes << "\n";
                exp2Output << "Number of overflow nodes: " << db->bPlusTree->numOverflowNodes << "\n";
                exp2Output << "Total Number of nodes (including overflow): " << (db->bPlusTree->numNodes + db->bPlusTree->numOverflowNodes) << "\n";
                exp2Output << "Number of levels of the B+ tree: " << db->bPlusTree->height+1 << "\n"; // DBMS starts height at 0

                // print to screen
                cout << "Parameter n of the B+ Tree: " << db->bPlusTree->maxKeys << "\n";
                cout << "Number of nodes (excluding overflow): " << db->bPlusTree->numNodes << "\n";
                cout << "Number of overflow nodes: " << db->bPlusTree->numOverflowNodes << "\n";
                cout << "Total Number of nodes (including overflow): " << (db->bPlusTree->numNodes + db->bPlusTree->numOverflowNodes) << "\n";
                cout << "Height of B+ Tree: " << db->bPlusTree->height+1 << "\n"; // DBMS starts height at 0
                cout << "\n=====Content of root=====" << endl;
                cout << "Root: \n";
                exp2Output << "Root: \n";
                //db->bPlusTree->printIndexBlock(db->bPlusTree->root, exp2Output);
                db->bPlusTree->printTree(exp2Output);
                //db->printBPlusTree(exp2Output);
                exp2Output.close();
                break;

            case 3:
                cout << "Experiment 3: " << endl;
                exp3Output.open(resultsDir + "experiment3output.txt");
                cout << "Retrieve movies with 'FG_PCT_HOME' equal to 0.5" << endl;
                db->bPlusTree->findRecord(0.5, 0.5, exp3Output);
                break;

            case 4:
                cout << "Experiment 4:" << endl;
                //Write output to file
                exp4Output.open(resultsDir + "experiment4output.txt");
                db->bPlusTree->findRecord(0.6, 0.1, exp4Output);
                //print to screen
                cout << "Retrieve movies with 'FG_PCT_HOME' from 0.6 to 1" << "\n";
                //cout << db->bPlusTree->findRecord(0.6, 0.1, exp4Output) << "\n";
                //debug this
                exp4Output.close();
                cout << endl << endl;
                test.close();
                break;

            case 5: //test case for temporary
                cout << "test" << endl;
                test.open(resultsDir + "test.txt");
                db->bPlusTree->printTree(test);
                test.close();
                break;

            case 6:
                cout << "Terminating the program." << endl;
                exit(0);

            default:
                cout << "Invalid input. Please input a value between 1 to 6." << endl;
                break;

                
        }
    }



    return 0;
}
