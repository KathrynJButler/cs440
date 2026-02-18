#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include "classes.h"

using namespace std;


int main(int argc, char* argv[]) {

    // Create the index
    HashIndex hashIndex("EmployeeIndex");
    hashIndex.createFromFile("Employee2.csv");

    // Loop to lookup IDs until user is ready to quit
    // command line arguments
    int CLCount = argc;
    int searchID;
    int i = 1;
    for (i = 1; i < CLCount; i++) {
        cout << "Search for argument" << i << ":" << endl;
        searchID = stoi(argv[i]);
        hashIndex.findAndPrintEmployee(searchID);
        cout << "\n" << endl;
    }
/*
=====================================================================================
TO DO: Fix this portion for the searching feature :) 
we got the command line search done already
=====================================================================================


    string StrSearchID;
    cout << "Enter the employee ID to find or type exit to terminate: ";
    while (cin >> searchID && searchID != "exit") {
        try{
        int64_t id = stoll(StrSearchID);
        string record;
        cout << "Search for" << id << ":" << endl;
        hashIndex.findAndPrintEmployee(id);
        cout << "\n" << endl;
        }
        catch (const invalid_argument& e) {
            cerr << "Invalid input. Please enter a valid employee ID or type exit to terminate." << endl;
        }
    }
*/
    return 0;
}
