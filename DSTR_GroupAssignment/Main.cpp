#include <iostream>
#include <fstream>
#include <cstring>
#include <string>       
#include <random>    
#include <limits>
#include <chrono>

using namespace std;

const int MAX_PASSENGERS = 180;

struct Passenger {
    int passengerID;
    char name[50];
    int seatRow;
    char seatCol;
    char seatClass[15];
};

struct Node {
    Passenger data;
    Node* next;
};

long calculateLinkedListMemory(Node* head);
long calculateArrayMemory(int count);

void benchmarkLinkedListDeletion(Node*& head);
void benchmarkArrayDeletion(Passenger passengers[], int& count);

long calculateLinkedListMemory(Node* head);
long calculateArrayMemory(int count);

long calculateLinkedListMemory(Node* head) {
    int nodes = 0;
    Node* temp = head;
    while (temp) {
        nodes++;
        temp = temp->next;
    }
    return nodes * sizeof(Node);
}

long calculateArrayMemory(int count) {
    return count * sizeof(Passenger);
}

// --- SHARED UTILITIES ---
// 6 Columns: A, B, C | D, E, F
void displaySeatingChart(bool taken[31][6]) {
    cout << "\n      CURRENT SEATING CHART (1-30, A-F)\n";
    cout << "      A B C   D E F\n";
    cout << "     ---------------\n";
    for (int r = 1; r <= 30; r++) {
        printf("%02d | ", r);
        for (int c = 0; c < 6; c++) {
            if (c == 3) cout << "  "; // Aisle between C and D
            cout << (taken[r][c] ? "X " : ". ");
        }
        cout << endl;
    }
    cout << " (X = Occupied, . = Available)\n";
}

void finalizeBooking(Passenger &p, int newID, int row, char col) {
    p.passengerID = newID;
    p.seatRow = row;
    p.seatCol = col;

    cout << "\nSeat available! Assigned ID: " << p.passengerID << endl;
    cout << "Enter Passenger Name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(p.name, 50);

    int classChoice;
    cout << "Select Class (1. Economy, 2. Business): ";
    while (!(cin >> classChoice) || (classChoice < 1 || classChoice > 2)) {
        cout << "Invalid choice. Please enter 1 or 2: ";
        cin.clear();
        cin.ignore(100, '\n');
    }
    
    if (classChoice == 1) strcpy(p.seatClass, "Economy");
    else strcpy(p.seatClass, "Business");

    cout << "Reservation completed successfully.\n";
}

// --- LINKED LIST IMPLEMENTATION ---
void loadFromCSV_Linked(const char* filename, Node*& head) {
    ifstream file(filename);
    if (!file) return;

    string line;
    getline(file, line); // Skip header

    bool taken[31][6] = {false};
    head = nullptr;
    int loaded = 0;

    while (getline(file, line) && loaded < MAX_PASSENGERS) {
        char buf[512] = {0};
        strncpy(buf, line.c_str(), sizeof(buf) - 1);

        char* token = strtok(buf, ",");
        int id = atoi(token);
        token = strtok(nullptr, ",");
        char name[50]; if(token) strncpy(name, token, 49);
        token = strtok(nullptr, ",");
        int row = atoi(token);
        token = strtok(nullptr, ",");
        char col = (token) ? toupper(token[0]) : ' ';
        token = strtok(nullptr, ",");
        char sClass[15]; if(token) strncpy(sClass, token, 14);

        int cIdx = col - 'A';
        // Collision Detection: Only load if seat is A-F and not already filled
        if (row >= 1 && row <= 30 && cIdx >= 0 && cIdx < 6 && !taken[row][cIdx]) {
            taken[row][cIdx] = true;
            Node* newNode = new Node;
            newNode->data.passengerID = id;
            strcpy(newNode->data.name, name);
            newNode->data.seatRow = row;
            newNode->data.seatCol = col;
            strcpy(newNode->data.seatClass, sClass);
            
            newNode->next = head;
            head = newNode;
            loaded++;
        }
    }
    file.close();
    cout << "Initialized: " << loaded << " unique seat records loaded (A-F range).\n";
}

void insertPassenger_Linked(Node*& head) {
    bool taken[31][6] = {false};
    int maxID = 0;
    Node* temp = head;
    while (temp) {
        int cIdx = toupper(temp->data.seatCol) - 'A';
        if (temp->data.seatRow >= 1 && temp->data.seatRow <= 30 && cIdx >= 0 && cIdx < 6)
            taken[temp->data.seatRow][cIdx] = true;
        if (temp->data.passengerID > maxID) maxID = temp->data.passengerID;
        temp = temp->next;
    }
    displaySeatingChart(taken);

    int row; char col;
    cout << "\nEnter Requested Row (1-30): "; cin >> row;
    if (row < 1 || row > 30) { cout << "Invalid row.\n"; return; }
    cout << "Enter Requested Column (A-F): "; cin >> col;
    col = toupper(col);
    int cIdx = col - 'A';

    if (cIdx < 0 || cIdx > 5 || taken[row][cIdx]) {
        cout << "Error: Seat is unavailable or out of A-F range!\n";
        return;
    }

    Node* newNode = new Node;
    finalizeBooking(newNode->data, maxID + 1, row, col);
    newNode->next = head;
    head = newNode;
}

bool deletePassenger(Node*& head, int id) {
    Node* curr = head;
    Node* prev = nullptr;

    while (curr) {
        if (curr->data.passengerID == id) {
            if (!prev) head = curr->next;
            else prev->next = curr->next;
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

// Link-list based Manifest & Seat Report
void printPassengers(Node* head) {
    auto start = chrono::high_resolution_clock::now();
    int nodeCount = 0;

    //seating chart(Linked List-Based)
    cout << "\n=====================\n";
    cout << "    SEATING CHART     \n    A B C   D E F\n";
    for (int r = 1; r <= 30; r++) {
        printf("%02d |", r);
        for (char c = 'A'; c <= 'F'; c++) {
            if (c == 'D') cout << "  "; 
            bool occupied = false;
            Node* temp = head;
            while (temp) {
                if (temp->data.seatRow == r && toupper(temp->data.seatCol) == c) {
                    occupied = true;
                    break;
                }
                temp = temp->next;
            }
            cout << (occupied ? "X " : ". ");
        }
        cout << endl;
    }

    //PASSENGER MANIFEST
    cout << "\n==========================\n";
    cout << "    PASSENGER MANIFEST    \n";
    for (int r = 1; r <= 30; r++) {
        for (char c = 'A'; c <= 'F'; c++) {
            Node* temp = head;
            while (temp) {
                if (temp->data.seatRow == r && toupper(temp->data.seatCol) == c) {
                    cout << temp->data.seatRow << temp->data.seatCol << " | "
                         << temp->data.passengerID << " | "
                         << temp->data.seatClass << " | "
                         << temp->data.name << endl;
                    break; // Move to next seat once passenger is found
                }
                temp = temp->next;
            }
        }
    }

    // Performance/Memory Analysis
    Node* countPtr = head;
    while(countPtr) { nodeCount++; countPtr = countPtr->next; }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "\nTime taken: " << elapsed.count() << " s\n";
    cout << "Memory used: " << (nodeCount * sizeof(Node)) << " bytes\n";
}

// Search function for linked-list implementation

void searchPassenger_Linked(Node* head) {
    int searchID;
    cout << "Enter Passenger ID to search: ";

    while (!(cin >> searchID)) {
        cout << "Invalid input! Please enter a numeric ID: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    Node* temp = head;
    bool found = false;
    while (temp != nullptr) {
        if (temp->data.passengerID == searchID) {
            cout << "\n--- Passenger Found ---\n";
            cout << "ID: " << temp->data.passengerID << endl;
            cout << "Name: " << temp->data.name << endl;
            cout << "Seat: " << temp->data.seatRow << temp->data.seatCol << endl;
            cout << "Class: " << temp->data.seatClass << endl;
            cout << "-----------------------\n";
            found = true;
            break; 
        }
        temp = temp->next;
    }
    if (!found) {
        cout << "Error: Passenger with ID " << searchID << " not found.\n";
    }

    //timer
    cout << "\n[Running Performance Test (100,000 iterations)]\n";

    auto start = chrono::high_resolution_clock::now(); 

    for(int k = 0; k < 100000; k++) { 
        Node* curr = head; 
        while (curr != nullptr) {
            if (curr->data.passengerID == searchID) {
                break; 
            }
            curr = curr->next;
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << ">> Time Efficiency (Linked List): " << elapsed.count() << " seconds.\n";

    cout << "\n----------------------------------------\n";

    cout << "\n[Memory Usage Analysis - Linked List]\n";

    //memory usage analysis
    int nodeCount = 0;
    Node* counterPtr = head;
    while (counterPtr) {
    nodeCount++;
    counterPtr = counterPtr->next;
    }

    long dataSize = nodeCount * sizeof(Passenger);

    long pointerOverhead = nodeCount * sizeof(Node*); 

    long totalDynamic = dataSize + pointerOverhead;

    cout << "   Current Node Count:       " << nodeCount << "\n";
    cout << "   Data Size (Payload):      " << dataSize << " bytes\n";
    cout << "   Pointer Overhead (Next*): " << pointerOverhead << " bytes\n";
    cout << "   Total Dynamic Memory:     " << totalDynamic << " bytes\n";
    cout << "   -> Trade-off: No wasted space for empty seats, but pays extra for pointers.\n";
    cout << "\n----------------------------------------\n";
}

void benchmarkLinkedListDeletion(Node*& head) {
    if (!head) {
        cout << "No passengers to delete.\n";
        return;
    }

    int deleteID;
    cout << "Enter Passenger ID to delete: ";

    // 🔴 Input validation
    if (!(cin >> deleteID)) {
        cout << "Error: Invalid input. Passenger ID must be a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    // --- Normal deletion ---
    if (!deletePassenger(head, deleteID)) {
        cout << "Passenger not found.\n";
        return;
    }

    cout << "Passenger deleted successfully.\n";

    // --- Benchmark ---
    long memBefore = calculateLinkedListMemory(head);
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 100000; i++) {
        deletePassenger(head, deleteID);
    }

    auto end = chrono::high_resolution_clock::now();
    long memAfter = calculateLinkedListMemory(head);

    chrono::duration<double> elapsed = end - start;

    cout << "\n--- LINKED LIST DELETION BENCHMARK ---\n";
    cout << "Time taken (100,000 deletions): " << elapsed.count() << " seconds\n";
    cout << "Memory before deletion: " << memBefore << " bytes\n";
    cout << "Memory after deletion:  " << memAfter << " bytes\n";
    cout << "-----------------------------------\n";
}




void runLinkedListBasedSystem() {
    Node* head = nullptr;
    loadFromCSV_Linked("flight_passenger_data.csv", head);

    int choice;
    do {
        cout << "\n==========================\n";
        cout << "    LINKED-LIST SYSTEM    \n";
        cout << "==========================\n";
        cout << "1. Manifest & Seat Report\n";
        cout << "2. Reservation\n"; //Insertion
        cout << "3. Cancellation\n"; //Deletion
        cout << "4. Seat Lookup\n"; //Search
        cout << "0. Back\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: 
                cout << "\nManifest & Seat Report...\n";
                printPassengers(head);
                break;

            case 2:
                insertPassenger_Linked(head);
                break;

            case 3:
                benchmarkLinkedListDeletion(head);
                break;
                
            case 4:
                searchPassenger_Linked(head);
                break;

            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    // Clean up memory before exiting back to main menu
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

// --- ARRAY IMPLEMENTATION ---
void loadFromCSV_Array(const char* filename, Passenger arr[], int& count) {
    ifstream file(filename);
    if (!file) return;

    string line;
    getline(file, line); 
    
    bool taken[31][6] = {false};
    count = 0;

    while (getline(file, line) && count < MAX_PASSENGERS) {
        char buf[512] = {0};
        strncpy(buf, line.c_str(), sizeof(buf) - 1);

        char* token = strtok(buf, ",");
        int id = atoi(token);
        token = strtok(nullptr, ",");
        char name[50]; if(token) strncpy(name, token, 49);
        token = strtok(nullptr, ",");
        int row = atoi(token);
        token = strtok(nullptr, ",");
        char col = (token) ? toupper(token[0]) : ' ';
        token = strtok(nullptr, ",");
        char sClass[15]; if(token) strncpy(sClass, token, 14);

        int cIdx = col - 'A';
        if (row >= 1 && row <= 30 && cIdx >= 0 && cIdx < 6 && !taken[row][cIdx]) {
            taken[row][cIdx] = true;
            arr[count].passengerID = id;
            strcpy(arr[count].name, name);
            arr[count].seatRow = row;
            arr[count].seatCol = col;
            strcpy(arr[count].seatClass, sClass);
            count++;
        }
    }
    file.close();
    cout << "Initialized: " << count << " unique seat records loaded (A-F range).\n";
}

void insertPassenger_Array(Passenger arr[], int& count) {
    if (count >= MAX_PASSENGERS) { cout << "Flight Full.\n"; return; }

    bool taken[31][6] = {false};
    int maxID = 0;
    for (int i = 0; i < count; i++) {
        int cIdx = toupper(arr[i].seatCol) - 'A';
        if (arr[i].seatRow >= 1 && arr[i].seatRow <= 30 && cIdx >= 0 && cIdx < 6)
            taken[arr[i].seatRow][cIdx] = true;
        if (arr[i].passengerID > maxID) maxID = arr[i].passengerID;
    }
    displaySeatingChart(taken);

    int row; char col;
    cout << "\nEnter Requested Row (1-30): "; cin >> row;
    if (row < 1 || row > 30) return;
    cout << "Enter Requested Column (A-F): "; cin >> col;
    col = toupper(col);
    int cIdx = col - 'A';

    if (cIdx < 0 || cIdx > 5 || taken[row][cIdx]) {
        cout << "Seat unavailable or invalid.\n";
        return;
    }

    finalizeBooking(arr[count], maxID + 1, row, col);
    count++;
}

bool deletePassenger(Passenger arr[], int& count, int id) {
    int idx = -1;
    for (int i = 0; i < count; ++i) {
        if (arr[i].passengerID == id) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return false;

    for (int i = idx; i < count - 1; ++i) {
        arr[i] = arr[i + 1];
    }
    count--;
    return true;
}

// array-based Manifest & Seat Report
void printPassengers(Passenger arr[], int count) {
    auto start = chrono::high_resolution_clock::now();

    // seating chart(Array-Based)
    cout << "\n===================\n";
    cout << "   SEATING CHART     \n    A B C   D E F\n";
    for (int r = 1; r <= 30; r++) {
        printf("%02d |", r);
        for (char c = 'A'; c <= 'F'; c++) {
            if (c == 'D') cout << "  "; 
            bool occupied = false;
            for (int i = 0; i < count; i++) {
                if (arr[i].seatRow == r && toupper(arr[i].seatCol) == c) {
                    occupied = true;
                    break;
                }
            }
            cout << (occupied ? "X " : ". ");
        }
        cout << endl;
    }

    //PASSENGER MANIFEST
    cout << "\n========================\n";
    cout << "   PASSENGER MANIFEST    \n";
    cout << "Seat | ID | Class | Name\n";
    for (int r = 1; r <= 30; r++) {
        for (char c = 'A'; c <= 'F'; c++) {
            for (int i = 0; i < count; i++) {
                if (arr[i].seatRow == r && toupper(arr[i].seatCol) == c) {
                    cout << arr[i].seatRow << arr[i].seatCol << " | "
                         << arr[i].passengerID << " | "
                         << arr[i].seatClass << " | "
                         << arr[i].name << endl;
                    break;
                }
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "\nTime taken: " << elapsed.count() << " s\n";
    cout << "Memory used: " << (MAX_PASSENGERS * sizeof(Passenger)) << " bytes\n";
}

// Search function for array-based implementation

void searchPassenger_Array(Passenger arr[], int count) {
    int searchID;
    cout << "Enter Passenger ID to search: ";
    
    while (!(cin >> searchID)) {
        cout << "Invalid input!\nPlease enter a numeric ID: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    bool found = false;
    for (int i = 0; i < count; ++i) {
        if (arr[i].passengerID == searchID) {
            cout << "\n--- Passenger Found ---\n";
            cout << "ID: " << arr[i].passengerID << endl;
            cout << "Name: " << arr[i].name << endl;
            cout << "Seat: " << arr[i].seatRow << arr[i].seatCol << endl;
            cout << "Class: " << arr[i].seatClass << endl;
            cout << "-----------------------\n";
            found = true;
            break; 
        }
    }
    if (!found) {
        cout << "Error: Passenger with ID " << searchID << " not found.\n";
    }

    //timer
    cout << "\n[Running Time Test (100,000 iterations)]\n";
    
    auto start = chrono::high_resolution_clock::now();

    for(int k = 0; k < 100000; k++) { 
        for (int i = 0; i < count; ++i) {
            if (arr[i].passengerID == searchID) {
                break;
            }
        }
    }
    
    auto end = chrono::high_resolution_clock::now(); 
    chrono::duration<double> elapsed = end - start;

    cout << ">> Time Efficiency (Array): " << elapsed.count() << " seconds.\n";

    cout << "\n----------------------------------------\n";

    //memory usage analysis
    cout << "\n[Memory Usage Analysis - Array]\n";
    long totalAllocated = MAX_PASSENGERS * sizeof(Passenger);
    long actualUsed = count * sizeof(Passenger);
    long wastedSpace = totalAllocated - actualUsed;
    cout << "   Total Static Memory Allocated: " << totalAllocated << " bytes\n";
    cout << "   Actually Used Memory:          " << actualUsed << " bytes\n";
    cout << "   Wasted Space (Empty Seats):    " << wastedSpace << " bytes\n";
    cout << "\n----------------------------------------\n";
}

void benchmarkArrayDeletion(Passenger passengers[], int& count) {
    if (count == 0) {
        cout << "No passengers to delete.\n";
        return;
    }

    int deleteID;
    cout << "Enter Passenger ID to delete: ";

    // 🔴 Input validation
    if (!(cin >> deleteID)) {
        cout << "Error: Invalid input. Passenger ID must be a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    // --- Normal deletion ---
    if (!deletePassenger(passengers, count, deleteID)) {
        cout << "Passenger not found.\n";
        return;
    }

    cout << "Passenger deleted successfully.\n";

    // --- Benchmark ---
    long memBefore = calculateArrayMemory(count);
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 100000 && count > 0; i++) {
        deletePassenger(passengers, count, deleteID);
    }

    auto end = chrono::high_resolution_clock::now();
    long memAfter = calculateArrayMemory(count);

    chrono::duration<double> elapsed = end - start;

    cout << "\n--- ARRAY DELETION BENCHMARK ---\n";
    cout << "Time taken (100,000 deletions): " << elapsed.count() << " seconds\n";
    cout << "Memory before deletion: " << memBefore << " bytes\n";
    cout << "Memory after deletion:  " << memAfter << " bytes\n";
    cout << "-----------------------------------\n";
}



void runArrayBasedSystem() {
    Passenger passengers[MAX_PASSENGERS];
    int count = 0;

    loadFromCSV_Array("flight_passenger_data.csv", passengers, count);

    int choice;
    do {
        cout << "\n==========================\n";
        cout << "    ARRAY-BASED SYSTEM    \n";
        cout << "==========================\n";
        cout << "1. Manifest & Seat Report\n";
        cout << "2. Reservation\n"; //Insertion
        cout << "3. Cancellation\n"; //Deletion
        cout << "4. Seat Lookup\n"; //Search
        cout << "0. Back\nChoice: ";
        cin >> choice;
        
        switch (choice) {
            case 1: //Manifest report
                cout << "\nManifest & Seat Report...";
                printPassengers(passengers, count);
                break;

            case 2: //Insertion
                insertPassenger_Array(passengers, count);
                break;

            case 3://Deletion
                benchmarkArrayDeletion(passengers, count);
                break;

            case 4://Search
                searchPassenger_Array(passengers, count);
                break;

            case 0: break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);
}

// MAIN MENU
int main() {
    int choice;
    do {
        cout << "\n=================================\n";
        cout << "        FLIGHT RESERVATION       \n";
        cout << "                &                \n";
        cout << "    SEATING MANAGEMENT SYSTEM    \n";
        cout << "=================================\n";
        cout << "1. Array-Based Implementation \n";
        cout << "2. Linked List-Based Implementation \n";
        cout << "0. Exit \n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: runArrayBasedSystem(); break;
            case 2: runLinkedListBasedSystem(); break;
            case 0: cout << "Exiting system...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}