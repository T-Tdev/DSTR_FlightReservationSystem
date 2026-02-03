#include <iostream>
#include <fstream>
#include <cstring>
#include <string>       
#include <random>       

using namespace std;

struct Passenger {
    int passengerID;
    char name[50];
    int seatRow;
    char seatCol;
    char seatClass[15];
};

// LINKED-LIST IMPLEMENTATION
struct Node {
    Passenger data;
    Node* next;
};

void loadFromCSV_Linked(const char* filename, Node*& head) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return;
    }

    string line;
    getline(file, line); 

    int total = 0;
    while (getline(file, line)) total++;
    file.close();

    const int LOAD = 180;
    int to_load = (total < LOAD) ? total : LOAD;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, total - to_load);

    int skip = dist(gen);  

  
    file.open(filename);
    getline(file, line);          
    for (int i = 0; i < skip; ++i) {
        getline(file, line);
    }

    head = nullptr;
    int loaded = 0;

    while (getline(file, line) && loaded < to_load) {
        Node* node = new Node;
        char buf[512] = {0};          
        strncpy(buf, line.c_str(), sizeof(buf) - 1);

        char* token = strtok(buf, ",");
        if (!token) continue;
        node->data.passengerID = atoi(token);

        token = strtok(nullptr, ",");
        if (token) strncpy(node->data.name, token, sizeof(node->data.name) - 1);

        token = strtok(nullptr, ",");
        if (token) node->data.seatRow = atoi(token);

        token = strtok(nullptr, ",");
        if (token) node->data.seatCol = token[0];

        token = strtok(nullptr, ",");
        if (token) strncpy(node->data.seatClass, token, sizeof(node->data.seatClass) - 1);

        node->next = head;
        head = node;
        loaded++;
    }
    file.close();
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

void printPassengers(Node* head) {
    Node* temp = head;
    while (temp) {
        cout << temp->data.passengerID << " | "
             << temp->data.name << " | "
             << temp->data.seatRow << temp->data.seatCol << " | "
             << temp->data.seatClass << endl;
        temp = temp->next;
    }
}

void runLinkedListBasedSystem() {
    Node* head = nullptr;
    loadFromCSV_Linked("flight_passenger_data.csv", head);

    int choice;
    do {
        cout << "\n=== LINKED-LIST SYSTEM ===\n";
        cout << "1. Cancellation\n";
        cout << "2. Print Manifest\n";
        cout << "0. Back\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            int id;
            cout << "Enter Passenger ID: ";
            cin >> id;
            if (deletePassenger(head, id))
                cout << "Successfully cancelled.\n";
            else
                cout << "Passenger not found.\n";
        }
        else if (choice == 2) {
            cout << "\nPassenger Manifest:\n";
            printPassengers(head);
        }
    } while (choice != 0);

    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

// ARRAY-BASED IMPLEMENTATION
const int MAX_PASSENGERS = 180;  

int loadFromCSV_Array(const char* filename, Passenger arr[], int& count) {
    ifstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file " << filename << endl;
        return 0;
    }

    string line;
    getline(file, line);

    int total = 0;
    while (getline(file, line)) total++;
    file.close();

    const int LOAD = 180;
    int to_load = (total < LOAD) ? total : LOAD;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, total - to_load);

    int skip = dist(gen);

    file.open(filename);
    getline(file, line); 
    for (int i = 0; i < skip; ++i) {
        getline(file, line);
    }

    count = 0;
    while (getline(file, line) && count < to_load && count < MAX_PASSENGERS) {
        char buf[512] = {0};
        strncpy(buf, line.c_str(), sizeof(buf) - 1);

        char* token = strtok(buf, ",");
        if (!token) continue;
        arr[count].passengerID = atoi(token);

        token = strtok(nullptr, ",");
        if (token) strncpy(arr[count].name, token, sizeof(arr[count].name) - 1);

        token = strtok(nullptr, ",");
        if (token) arr[count].seatRow = atoi(token);

        token = strtok(nullptr, ",");
        if (token) arr[count].seatCol = token[0];

        token = strtok(nullptr, ",");
        if (token) strncpy(arr[count].seatClass, token, sizeof(arr[count].seatClass) - 1);

        count++;
    }
    file.close();
    return count;
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

void printPassengers(Passenger arr[], int count) {
    for (int i = 0; i < count; ++i) {
        cout << arr[i].passengerID << " | "
             << arr[i].name << " | "
             << arr[i].seatRow << arr[i].seatCol << " | "
             << arr[i].seatClass << endl;
    }
}

void runArrayBasedSystem() {
    Passenger passengers[MAX_PASSENGERS];
    int count = 0;

    loadFromCSV_Array("flight_passenger_data.csv", passengers, count);

    int choice;
    do {
        cout << "\n=== ARRAY-BASED SYSTEM ===\n";
        cout << "1. Cancellation\n";
        cout << "2. Print Manifest\n";
        cout << "0. Back\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            int id;
            cout << "Enter Passenger ID: ";
            cin >> id;
            if (deletePassenger(passengers, count, id))
                cout << "Successfully cancelled.\n";
            else
                cout << "Passenger not found.\n";
        }
        else if (choice == 2) {
            cout << "\nPassenger Manifest:\n";
            printPassengers(passengers, count);
        }
    } while (choice != 0);
}

// MAIN MENU
int main() {
    int choice;
    do {
        cout << "\n=== FLIGHT RESERVATION SYSTEM ===\n";
        cout << "1. Array-Based Implementation\n";
        cout << "2. Linked List-Based Implementation\n";
        cout << "0. Exit\n";
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