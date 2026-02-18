/*  Name: Kathryn Butler
    Email: butlekat@oregonstate.edu
    ONID: butlekat*/

/*  Name: Sarah Pilon
    Email: pilons@oregonstate.edu
    ONID: pilons*/

/* This is a skeleton code for two-pass multi-way sorting, you can make modifications as long as you meet
   all question requirements*/
/* This record_class.h contains the class Records, which can be used to store tuples form Employee.csv */

// #include <bits/stdc++.h>

/*  found that this ^ includes all libraries and isn't supported on my system.
    If we run into errors about libraries, we can add them below.
    I've added the ones we need for now.
*/

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Records
{
public:
    struct EmpRecord
    {
        int id;
        string name;
        string bio;
        int manager_id;
    } emp_record;

    /*** You can add more variables if you want below ***/
    int no_values = 0; // You can use this to check if there are any more tuples
};

// Grab a single block from the Employee.csv file and put it inside the EmpRecord structure of the Records Class
Records Grab_Emp_Record(fstream &empin)
{
    string line, word;
    Records emp;
    if (getline(empin, line, '\n'))
    {                         // grab entire line
        stringstream s(line); // turn line into a stream

        getline(s, word, ',');
        emp.emp_record.id = stoi(word);
        getline(s, word, ',');
        emp.emp_record.name = word;
        getline(s, word, ',');
        emp.emp_record.bio = word;
        getline(s, word, ',');
        emp.emp_record.manager_id = stoi(word);

        return emp;
    }
    else
    {
        emp.no_values = -1;
        return emp;
    }
}