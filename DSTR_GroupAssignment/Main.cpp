#include <iostream>
using namespace std;

//void runArrayBasedSystem();
//void runLinkedListBasedSystem();

int main() {
    int choice;

    do {
        cout << "\nFLIGHT RESERVATION SYSTEM\n";
        cout << "1. Array-Based Implementation\n";
        cout << "2. Linked List-Based Implementation\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            /*runArrayBasedSystem();*/
            break;
        case 2:
            /*runLinkedListBasedSystem();*/
            break;
        case 0:
            cout << "Exiting system...\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}

