#include "../include/UI.h"

int main() {
    FileRepository repo("../../oopPhoneBook/contacts.json");
    ContactManager manager(&repo);

    while (true) {
        printColorfulPhoneBook();
        cout << "                              " << "       MENU       \n";
        cout << "                              " << "1. View phonebook\n";
        cout << "                              " << "2. Sort phonebook\n";
        cout << "                              " << "3. Find contact\n";
        cout << "                              " << "4. Add contact\n";
        cout << "                              " << "5. Edit contact\n";
        cout << "                              " << "6. Delete contact\n";
        cout << "                              " << "0. Exit\n";
        cout << "                              " << "> ";

        int choice;
        cin >> choice;

        clearInput();

        switch (choice) {
            case 1: 
                viewContacts(manager); 
                waitForEnter();
                break;
            case 2: 
                sortContacts(manager); 
                waitForEnter();
                break;
            case 3: 
                searchContacts(manager); 
                waitForEnter();
                break;
            case 4: 
                addContact(manager); 
                waitForEnter();
                break;
            case 5: 
                editContact(manager); 
                waitForEnter();
                break;
            case 6: 
                removeContact(manager); 
                waitForEnter();
                break;
            case 0: 
                return 0;
            default: 
                cout << "Wrong choice\n";
                waitForEnter();
                break;
        }
    }
    return 0;
}
