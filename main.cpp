#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include "classes.h"

using namespace std;


int main() {

    // Create the index
    HashIndex hashIndex("EmployeeIndex");
    hashIndex.createFromFile("Employee2.csv");

    // Loop to lookup IDs until user is ready to quit
    string searchID;
    cout << "Enter the employee ID to find or type exit to terminate: ";
    while (cin >> searchID && searchID != "exit") {
        try{
        int64_t id = stoll(searchID);
        string record;
        hashIndex.findAndPrintEmployee(id);
        }
        catch (const invalid_argument& e) {
            cerr << "Invalid input. Please enter a valid employee ID or type exit to terminate." << endl;
        }
    }

    return 0;
}
