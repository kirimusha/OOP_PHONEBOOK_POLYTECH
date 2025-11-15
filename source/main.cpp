#include <iostream>
#include <limits>
#include <io.h>
#include <fcntl.h>
#include "Contact.h"
#include "PhoneNumber.h"
#include "FileRepository.h"
#include "ContactManager.h"
#include "UI.h"

int main() {
    FileRepository repo("contacts.json");
    ContactManager manager(&repo);

    while (true) {
        cout << "\n     PhoneBook    \n";
        cout << "\n";
        cout << "       MENU       \n";
        cout << "1. View phonebook\n";
        cout << "2. Sort phonebook\n";
        cout << "3. Find contact\n";
        cout << "4. Add contact\n";   
        cout << "5. Edit contact\n";
        cout << "6. Delete contact\n";
        cout << "0. Exit\n";
        cout << "> ";

        int choice;
        cin >> choice;
        clearInput();

        switch (choice) {
            case 1: viewContacts(manager); break;
            case 2: sortContacts(manager); break;
            case 3: searchContacts(manager); break;
            case 4: addContact(manager); break;
            case 5: editContact(manager); break;
            case 6: removeContact(manager); break;
            case 0: return 0;
            default: cout << "Wrong choise\n"; break;
        }
    }

    return 0;
}
