/*  Name: Kathryn Butler
    Email: butlekat@oregonstate.edu
    ONID: butlekat*/

/*  Name: Sarah Pilon
    Email: pilons@oregonstate.edu
    ONID: pilons*/

#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include "classes.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Create the index
    HashIndex hashIndex("EmployeeIndex");
    hashIndex.createFromFile("Employee.csv");
    cout << "Done!" << endl;

    // Loop to lookup IDs until user is ready to quit
    // command line arguments
    int CLCount = argc;
    int searchID;
    int i = 1;
    for (i = 1; i < CLCount; i++)
    {
        // cout << "Debugging: Search for argument" << i << ":" << endl;
        searchID = stoi(argv[i]);
        hashIndex.findAndPrintEmployee(searchID);
        cout << "\n"
             << endl;
    }
    string StrSearchID;
    cout << "Enter the employee ID to find or type exit to terminate: ";
    while (cin >> StrSearchID && StrSearchID != "exit")
    {
        try
        {
            int64_t id = stoll(StrSearchID);
            string record;
            // cout << "Debugging: Search for " << id << ":" << endl;
            hashIndex.findAndPrintEmployee(id);
        }
        catch (const invalid_argument &e)
        {
            cerr << "Invalid input. Please enter a valid employee ID or type exit to terminate." << endl;
        }
        cout << endl
             << endl
             << "Enter the employee ID to find or type exit to terminate: ";
    }

    // remove the EmployeeIndex file
    remove("EmployeeIndex");
    return 0;
}
