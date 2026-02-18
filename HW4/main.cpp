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
#include <sstream>
#include "classes.h"

using namespace std;

int main(int argc, char *argv[])
{
    // Create the index
    cout << "Creating hash index from Employee.csv (This may take around 3 minutes)... " << endl;
    HashIndex hashIndex("EmployeeIndex");
    hashIndex.createFromFile("Employee.csv");
    cout << "Done!" << endl << endl;

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
        cout << endl << endl;
    }

    string line;
    cout << "Enter the employee ID to find or type exit to terminate: ";
    while (getline(cin, line))
    {
        stringstream ss(line);
        string StrSearchID;
        bool exitFlag = false;
        while (ss >> StrSearchID)
        {
            if (StrSearchID == "exit")
            {
                exitFlag = true;
                break;
            }
            try
            {
                int64_t id = stoll(StrSearchID);
                hashIndex.findAndPrintEmployee(id);
            }
            catch (const invalid_argument &e)
            {
                cerr << "Input '" << StrSearchID << "' is not a valid integer." << endl;
            }
            cout << endl << endl;
        }
        if (exitFlag)
            break;
        cout << "Enter the employee ID to find or type exit to terminate: ";
    }

    // remove the EmployeeIndex file
    remove("EmployeeIndex");
    return 0;
}
