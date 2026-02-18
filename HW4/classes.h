#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <cmath>

using namespace std;

class Record
{
public:
    int id, manager_id; // Employee ID and their manager's ID
    string bio, name;   // Fixed length string to store employee name and biography

    Record(vector<string> &fields)
    {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    // Function to get the size of the record
    int get_size()
    {
        // sizeof(int) is for name/bio size() in serialize function
        return sizeof(id) + sizeof(manager_id) + sizeof(int) + name.size() + sizeof(int) + bio.size();
    }

    // Function to serialize the record for writing to file
    string serialize() const
    {
        ostringstream oss;
        oss.write(reinterpret_cast<const char *>(&id), sizeof(id));
        oss.write(reinterpret_cast<const char *>(&manager_id), sizeof(manager_id));
        int name_len = name.size();
        int bio_len = bio.size();
        oss.write(reinterpret_cast<const char *>(&name_len), sizeof(name_len));
        oss.write(name.c_str(), name.size());
        oss.write(reinterpret_cast<const char *>(&bio_len), sizeof(bio_len));
        oss.write(bio.c_str(), bio.size());
        return oss.str();
    }

    // Function to deserialize the record from a buffer read from file
    static Record deserialize(const char *buffer)
    {
        if (buffer == nullptr)
        {
            cout << "ERROR: buffer is null" << endl;
            exit(1);
        }
        int offset = 0;
        int id, manager_id, name_len, bio_len;
        memcpy(&id, buffer + offset, sizeof(id));
        offset += sizeof(id);
        memcpy(&manager_id, buffer + offset, sizeof(manager_id));
        offset += sizeof(manager_id);
        memcpy(&name_len, buffer + offset, sizeof(name_len));
        offset += sizeof(name_len);
        string name(buffer + offset, name_len);
        offset += name_len;
        memcpy(&bio_len, buffer + offset, sizeof(bio_len));
        offset += sizeof(bio_len);
        string bio(buffer + offset, bio_len);
        vector<string> fields = {
            to_string(id),
            name,
            bio,
            to_string(manager_id)};
        return Record(fields);
    }

    void print()
    {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

class Page
{
public:
    vector<Record> records;                // Data_Area containing the records
    vector<pair<int, int>> slot_directory; // Slot directory containing offset and size of each record
    int cur_size = 0;                      // Current size of the page including the overflow page pointer. if you also write the length of slot directory change it accordingly.
    int overflowPointerIndex;              // Initially set to -1, indicating the page has no overflow page.
                                           // Update it to the position of the overflow page when one is created.

    // Constructor
    Page() : overflowPointerIndex(-1) {}

    Record *search_record_by_id(int id)
    {
        for (const auto &record : records)
        {
            if (record.id == id)
            {
                return const_cast<Record *>(&record); // Return a pointer to the found record
            }
        }
        return nullptr;
    }

    // Function to insert a record into the page
    bool insert_record_into_page(Record r)
    {
        int record_size = r.get_size();
        int slot_size = sizeof(int) * 2;
        int used_records = cur_size;
        int used_slots = slot_directory.size() * slot_size;
        int footer = sizeof(int) * 2;

        int new_used_records = used_records + record_size;
        int new_used_slots = used_slots + slot_size;

        if (new_used_records + new_used_slots + footer > 4096)
        {
            return false;
        }
        else
        {

            // TODO: update slot directory information
            int offset = 0; // Start offset after the overflow pointer index
            if (!slot_directory.empty())
            {
                // cout << "DEBUG: Enters here" << endl;
                offset = slot_directory.back().first + slot_directory.back().second; // Calculate offset based on last record's offset and size
            }
            records.push_back(r);
            slot_directory.push_back(make_pair(offset, record_size));

            // consistent cur size
            cur_size = new_used_records;
            return true;
        }
    }

    // Function to write the page to a binary output stream. You may use
    void write_into_data_file(ostream &out) const
    {
        char page_data[4096] = {0}; // Buffer to hold page data
        int offset = 0;

        // Write records into page_data buffer
        for (const auto &record : records)
        {
            string serialized = record.serialize();
            memcpy(page_data + offset, serialized.c_str(), serialized.size());
            offset += serialized.size();
        }

        // TODO:
        //  - Write slot_directory in reverse order into page_data buffer.
        //  - Write overflowPointerIndex into page_data buffer.
        //  You should write the first entry of the slot_directory, which have the info about the first record at the bottom of the page, before overflowPointerIndex.
        int slot_count = slot_directory.size();
        int overflow_position = 4096 - sizeof(int);
        int slot_count_position = overflow_position - sizeof(int);
        int directory_offset = slot_count_position;

        for (int i = 0; i < slot_count; i++)
        {
            directory_offset -= sizeof(int) * 2;

            int record_offset = slot_directory[i].first;
            int record_size = slot_directory[i].second;

            memcpy(page_data + directory_offset, &record_offset, sizeof(int));
            memcpy(page_data + directory_offset + sizeof(int), &record_size, sizeof(int));
        }

        memcpy(page_data + slot_count_position, &slot_count, sizeof(int));
        memcpy(page_data + overflow_position, &overflowPointerIndex, sizeof(int));

        // Write the page_data buffer to the output stream
        out.write(page_data, sizeof(page_data));
    }

    // Function to read a page from a binary input stream
    bool read_from_data_file(istream &in)
    {
        char page_data[4096] = {0}; // Buffer to hold page data
        in.read(page_data, 4096);   // Read data from input stream

        streamsize bytes_read = in.gcount();
        if (bytes_read == 4096)
        {
            // TODO: Process data to fill the records, slot_directory, and overflowPointerIndex
            // clear to avoid segfault
            records.clear();
            slot_directory.clear();

            int overflow_position = 4096 - sizeof(int);
            int slotCountPosition = overflow_position - sizeof(int);

            memcpy(&overflowPointerIndex, page_data + overflow_position, sizeof(int));
            // initalize EVVERYTHINGG
            int slotCount = 0;
            memcpy(&slotCount, page_data + slotCountPosition, sizeof(int));

            // cout << "DEBUG: slotCount=" << slotCount << " overflow=" << overflowPointerIndex << endl;

            // treat invalid slotcount as empty page for segfault
            if (slotCount <= 0 || slotCount > 4096 / (2 * (int)sizeof(int)))
            {
                // cout << "DEBUG: treating page as empty (bad slotCount)\n";
                return false;
            }

            int directory_position = slotCountPosition;

            for (int i = 0; i < slotCount; i++)
            {
                directory_position -= sizeof(int) * 2;
                // intalizing some more
                int record_offset = 0;
                int record_size = 0;
                memcpy(&record_offset, page_data + directory_position, sizeof(int));
                memcpy(&record_size, page_data + directory_position + sizeof(int), sizeof(int));

                // cout << "DEBUG: slot[" << i << "] offset=" << record_offset << " size=" << record_size << endl;

                slot_directory.push_back(make_pair(record_offset, record_size));
            }

            for (auto &entry : slot_directory)
            {
                int record_offset = entry.first;
                Record r = Record::deserialize(page_data + record_offset);
                records.push_back(r);
            }

            // redo cur size
            cur_size = 0;
            for (auto &entry : slot_directory)
            {
                cur_size += entry.second;
            }
        }
        else
        {
            cerr << "Incomplete read: Expected 4096 bytes, but only read " << bytes_read << " bytes." << endl;
            return false;
        }

        return bytes_read == 4096;
    }
};

class HashIndex
{
private:
    const size_t maxCacheSize = 1; // Maximum number of pages in the buffer
    const int Page_SIZE = 4096;    // Size of each page in bytes
    vector<int> PageDirectory;     // Map h(id) to a bucket location in EmployeeIndex(e.g., the jth bucket)
    // can scan to correct bucket using j*Page_SIZE as offset (using seek function)
    // can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int nextFreePage; // Next place to write a bucket
    string fileName;

    // Function to compute hash value for a given ID
    int compute_hash_value(int id)
    {
        // cout << "Debugging: Computing hash value for ID: " << id << endl;
        int hash_value;

        // TODO: Implement the hash function h = id mod 2^8
        hash_value = id % 256;
        // cout << "Debugging: Computed hash value: " << hash_value << endl;
        return hash_value;
    }

    // Function to add a new record to an existing page in the index file
    void addRecordToIndex(int pageIndex, Page &page, Record &record)
    {
        //       cout << "Opening index file: " << fileName << endl;
        {
            ofstream create(fileName, ios::binary | ios::app);
        }
        fstream indexFile(fileName, ios::binary | ios::in | ios::out);
        if (!indexFile.is_open())
        {
            cerr << "Error: Unable to open index file" << endl;
            return;
        }

        indexFile.seekp(0, ios::end);
        long fileSize = indexFile.tellp();

        long requiredSize = (pageIndex + 1) * Page_SIZE;

        if (fileSize < requiredSize)
        {
            // Extend file with zeros
            indexFile.seekp(requiredSize - 1, ios::beg);
            char zero = 0;
            indexFile.write(&zero, 1);
            indexFile.flush();
        }

        // cout << "Index file opened successfully." << endl;

        // TODO:
        // - Use seekp() to seek to the offset of the correct page in the index file
        // indexFile.seekp(pageIndex * Page_SIZE, ios::beg);
        // - try insert_record_into_page()
        // - if it fails, then you'll need to either...
        // - go to next overflow page and try inserting there (keep doing this until you find a spot for the record)
        // - create an overflow page (if page.overflowPointerIndex == -1) using nextFreePage. update nextFreePage index and pageIndex.
        // Seek to the appropriate position in the index file

        int currentPageIndex = pageIndex;
        /* cout << "Debugging: Writing record " << record.id
                     << " to page " << currentPageIndex
                     << " at file offset "
                     << currentPageIndex * Page_SIZE
                     << endl;
        */
        while (true)
        {
            // seek to current page
            indexFile.seekg(currentPageIndex * Page_SIZE, ios::beg);
            // cout << "DEDBUG: reading page: " << currentPageIndex << endl;

            // read the page from the index file
            if (!page.read_from_data_file(indexFile)) // page is empty, initialize it
            {
                page = Page();
                // cout << "DEBUG: read worked " << endl;
            }
            // cout << "DEBUG: inserting record into page " << endl;
            if (page.insert_record_into_page(record))
            {
                // cout << "DEBUG: insert worked " << endl;
                indexFile.seekp(currentPageIndex * Page_SIZE, ios::beg);
                // cout << "DEBUG: writing page to disk " << endl;
                page.write_into_data_file(indexFile);
                // cout << "DEBUG: writing worked" << endl;

                indexFile.flush();

                // Immediately read back what we just wrote
                indexFile.seekg(currentPageIndex * Page_SIZE, ios::beg);
                /*
                                Page verifyPage;
                                verifyPage.read_from_data_file(indexFile);

                                cout << "Debugging: Page " << currentPageIndex
                                     << " now contains " << verifyPage.records.size()
                                     << " records." << endl;

                                for (auto &r : verifyPage.records)
                                {
                                    cout << "  Found ID in page: " << r.id << endl;
                                }
                */
                break;
            }
            else
            {
                if (page.overflowPointerIndex != -1)
                {
                    currentPageIndex = page.overflowPointerIndex;
                }
                else
                {
                    Page overflowPage;
                    overflowPage.insert_record_into_page(record);
                    int newPageIndex = nextFreePage++;

                    // write overflow page to disk
                    indexFile.seekp(newPageIndex * Page_SIZE, ios::beg);
                    overflowPage.write_into_data_file(indexFile);

                    // TODO: After inserting the record, write the modified page back to the index file.
                    // Remember to use the correct position (i.e., pageIndex) if you are writing out an overflow page!
                    page.overflowPointerIndex = newPageIndex;
                    indexFile.seekp(currentPageIndex * Page_SIZE, ios::beg);
                    page.write_into_data_file(indexFile);
                    indexFile.flush();

                    // Immediately read back what we just wrote
                    indexFile.seekg(currentPageIndex * Page_SIZE, ios::beg);

                    Page verifyPage;
                    verifyPage.read_from_data_file(indexFile);
                    /*
                                        cout << "Debugging: Page " << currentPageIndex
                                             << " now contains " << verifyPage.records.size()
                                             << " records." << endl;

                                        for (auto &r : verifyPage.records)
                                        {
                                            cout << "  Found ID in page: " << r.id << endl;
                                        }
                    */
                    break;
                }
            }
        }
        indexFile.close();
    }

    // Function to search for a record by ID in a given page of the index file
    void searchRecordByIdInPage(int pageIndex, int id)
    {
        // cout << "Debugging: Searching for record with ID: " << id << " in page index: " << pageIndex << endl;
        // Open index file in binary mode for reading
        fstream indexFile(fileName, ios::binary | ios::in);
        if (!indexFile)
        {
            cerr << "Error: Unable to open index file for searching record." << endl;
            return;
        }

        int currentPageIndex = pageIndex;
        /* cout << "Debugging: Reading page " << currentPageIndex
                     << " from file offset "
                     << currentPageIndex * Page_SIZE
                     << endl;
        */
        while (currentPageIndex != -1)
        {
            // Seek to the appropriate position in the index file
            indexFile.seekg(currentPageIndex * Page_SIZE, ios::beg);

            // Read the page from the index file
            Page page;
            page.read_from_data_file(indexFile);
            /*
                        cout << "Debugging: Page has "
                             << page.records.size()
                             << " records after reading." << endl;

                        for (auto &r : page.records)
                        {
                            cout << "  Record ID in page: " << r.id << endl;
                        }
            */
            // TODO:
            //  - Search for the record by ID in the page
            //  - Check for overflow pages and report if record with given ID is not found
            Record *result = page.search_record_by_id(id);

            if (result != nullptr)
            {
                result->print();
                indexFile.close();
                return;
            }
            currentPageIndex = page.overflowPointerIndex; // Move to the next overflow page if it exists
        }
        cout << "Record with ID " << id << " not found." << endl;
        indexFile.close();
    }

public:
    HashIndex(string indexFileName) : nextFreePage(0), fileName(indexFileName)
    {
        // Initialize PageDirectory with -1 to indicate empty buckets
        PageDirectory.resize(256, -1);
    }

    // Function to create hash index from Employee CSV file
    void createFromFile(string csvFileName)
    {
        // Read CSV file and add records to index
        // Open the CSV file for reading
        ifstream csvFile(csvFileName);

        string line;
        // Read each line from the CSV file
        while (getline(csvFile, line))
        {
            // Parse the line and create a Record object
            stringstream ss(line);
            string item;
            vector<string> fields;
            while (getline(ss, item, ','))
            {
                fields.push_back(item);
            }
            Record record(fields);

            // TODO:
            //   - Compute hash value for the record's ID using compute_hash_value() function.
            int hashValue = compute_hash_value(record.id);
            //   - Get the page index from PageDirectory. If it's not in PageDirectory, define a new page using nextFreePage.
            int pageIndex;
            if (PageDirectory[hashValue] == -1)
            {
                pageIndex = nextFreePage++;
                PageDirectory[hashValue] = pageIndex;
            }
            else
            {
                pageIndex = PageDirectory[hashValue];
            }
            //   - Insert the record into the appropriate page in the index file using addRecordToIndex() function.
            Page tempPage;
            addRecordToIndex(pageIndex, tempPage, record);
        }

        // Close the CSV file
        csvFile.close();
    }

    // Function to search for a record by ID in the hash index
    void findAndPrintEmployee(int id)
    {
        // cout << "Debugging: Searching for employee with ID: " << id << endl;
        // Open index file in binary mode for reading
        ifstream indexFile(fileName, ios::binary | ios::in);

        // TODO:
        //  - Compute hash value for the given ID using compute_hash_value() function
        int hashValue = compute_hash_value(id);
        //  - Search for the record in the page corresponding to the hash value using searchRecordByIdInPage() function
        if (PageDirectory[hashValue] == -1)
        {
            cout << "Record with ID " << id << " not found." << endl;
            return;
        }
        int pageIndex = PageDirectory[hashValue];
        searchRecordByIdInPage(pageIndex, id);
        // Close the index file
        indexFile.close();
    }
};
