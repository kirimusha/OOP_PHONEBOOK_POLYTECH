#include "../include/UI.h"
#include <iostream>
#include <limits>
#include <algorithm>
#include <cstdlib> // для system()

using namespace std;

void printColorfulPhoneBook() {
    cout << "\033[1;34m ██████╗ ██╗  ██╗ ██████╗ ███╗   ██╗███████╗ ███████╗   ██████╗  ██████╗ ██╗  ██╗  \033[0m\n";
    cout << "\033[1;35m ██╔══██╗██║  ██║██╔═══██╗████╗  ██║██╔════╝ ██╔═══██╗ ██╔═══██╗██╔═══██╗██║ ██╔╝  \033[0m\n";
    cout << "\033[1;33m ██████╔╝███████║██║   ██║██╔██╗ ██║█████╗   ██████║   ██║   ██║██║   ██║█████╔╝   \033[0m\n";
    cout << "\033[1;32m ██╔═══╝ ██╔══██║██║   ██║██║╚██╗██║██╔══╝   ██║   ██║ ██║   ██║██║   ██║██╔═██╗   \033[0m\n";
    cout << "\033[1;31m ██║     ██║  ██║╚██████╔╝██║ ╚████║███████╗ ███████╔╝ ╚██████╔╝╚██████╔╝██║  ██╗  \033[0m\n";
    cout << "\033[1;35m ╚═╝     ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝ ╚══════╝   ╚═════╝  ╚═════╝ ╚═╝  ╚═╝  \033[0m\n";
}

void clearScreen() {
    (void)system("clear");
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    clearScreen();
}

// для конвертации строки в time_t
bool stringToTime(const string& str, time_t& out) {
    int d, m, y;
    if (sscanf(str.c_str(), "%d.%d.%d", &d, &m, &y) != 3) return false;
    tm tm_date = {};
    tm_date.tm_mday = d;
    tm_date.tm_mon  = m - 1;  // 0..11
    tm_date.tm_year = y - 1900;
    out = mktime(&tm_date);
    return true;
}

PhoneType selectPhoneType() {
    int choice;
    cout << "Select the phone type:\n1. Mobile\n2. Home\n3. Work\n>";
    cin >> choice;
    clearInput();
    switch (choice) {
        case 2: return PhoneType::Home;
        case 3: return PhoneType::Work;
        default: return PhoneType::Mobile;
    }
}

void sortContacts(const ContactManager& manager) {

    clearScreen();

    cout << "\n___ Sort Contacts ___\n";
    cout << "Sort by:\n";
    cout << "1. First Name\n";
    cout << "2. Last Name\n";
    cout << "3. Email\n";
    cout << "4. Birth Date\n";
    cout << "0. Back to Menu\n";
    cout << "> ";

    int field;
    cin >> field;
    clearInput();

    if (field == 0) return;
    if (field < 1 || field > 4) {
        cout << "Invalid choice!\n";
        return;
    }

    cout << "\nSort order:\n";
    cout << "1. Ascending (A-Z, Oldest first)\n";
    cout << "2. Descending (Z-A, Newest first)\n";
    cout << "> ";

    int direction;
    cin >> direction;
    clearInput();

    if (direction != 1 && direction != 2) {
        cout << "Invalid order choice!\n";
        return;
    }

    bool ascending = (direction == 1);
    auto contacts = manager.getAllContacts();
    
    string fieldName;
    
    switch (field) {
        case 1:
            sort(contacts.begin(), contacts.end(), 
                 [ascending](const Contact& a, const Contact& b) {
                     return ascending ? (a.get_firstName() < b.get_firstName()) 
                                     : (a.get_firstName() > b.get_firstName());
                 });
            fieldName = "First Name";
            break;
        case 2:
            sort(contacts.begin(), contacts.end(), 
                 [ascending](const Contact& a, const Contact& b) {
                     return ascending ? (a.get_lastName() < b.get_lastName()) 
                                     : (a.get_lastName() > b.get_lastName());
                 });
            fieldName = "Last Name";
            break;
        case 3:
            sort(contacts.begin(), contacts.end(), 
                 [ascending](const Contact& a, const Contact& b) {
                     return ascending ? (a.get_email() < b.get_email()) 
                                     : (a.get_email() > b.get_email());
                 });
            fieldName = "Email";
            break;
        case 4:
            sort(contacts.begin(), contacts.end(), 
                 [ascending](const Contact& a, const Contact& b) {
                     return ascending ? (a.get_birthDate() < b.get_birthDate()) 
                                     : (a.get_birthDate() > b.get_birthDate());
                 });
            fieldName = "Birth Date";
            break;
    }

    const string orderName = ascending ? "Ascending" : "Descending";

    // сохраняем в файл контакты уже отсортированные
    if (manager.updateAllContacts(contacts)) {
        cout << "Contacts sorted and saved to file!\n";
    } else {
        cout << "Error: Failed to save sorted contacts!\n";
    }

    cout << "\n___ Contacts Sorted by " << fieldName << " (" << orderName << ") ___\n";
    for (size_t i = 0; i < contacts.size(); ++i) {
        const auto& c = contacts[i];
        cout << i+1 << ". " << c.get_firstName() << " " << c.get_lastName()
             << " | Email: " << c.get_email() 
             << " | Birth: " << (c.get_birthDate().empty() ? "Not set" : c.get_birthDate()) << "\n";
    }
    
    cout << "\nTotal contacts: " << contacts.size() << "\n";
}


void editContact(ContactManager& manager) {
    clearScreen();

    string email;
    cout << "Enter email of contact to edit: ";
    getline(cin, email);

    Contact contact = manager.getContact(email);
    if (contact.get_email().empty()) {
        cout << "Contact not found!\n";
        return;
    }

    cout << "Editing contact: " << contact.get_firstName() << " " << contact.get_lastName() << "\n";

    while (true) {
        cout << "\n___ Edit Contact ___\n";
        cout << "1. Edit First Name\n";
        cout << "2. Edit Last Name\n";
        cout << "3. Edit Patronymic\n";
        cout << "4. Edit Address\n";
        cout << "5. Edit Birth Date\n";
        cout << "6. Edit Email\n";
        cout << "7. Edit Phones\n";
        cout << "8. Save Changes\n";
        cout << "0. Cancel\n";
        cout << "> ";

        int choice;
        cin >> choice;
        clearInput();

        string input;
        switch (choice) {
            case 1: {
                cout << "Current first name: " << contact.get_firstName() << "\n";
                cout << "New first name: ";
                getline(cin, input);

                // сохраняем старое имя на случай отката
                string oldName = contact.get_firstName();

                // ставим новое имя
                if (contact.set_firstName(input)) {
                    // содержит ли текущий email новое имя
                    string currentEmail = contact.get_email();
                    string newName = contact.get_firstName();

                    if (!currentEmail.empty() && !newName.empty()) {
                        // Приводим к нижнему регистру
                        string emailLower = currentEmail;
                        string nameLower = newName;
                        transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);
                        transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

                        // Извлекаем часть email до @
                        size_t atPos = emailLower.find('@');
                        string emailUser = (atPos != string::npos) ? emailLower.substr(0, atPos) : emailLower;

                        // Проверяем, есть ли новое имя в email
                        if (emailUser.find(nameLower) == string::npos) {
                            cout << "\nEmail '" << currentEmail
                                 << "' does not contain the new name '" << newName << "'\n";
                            cout << "Email must contain the first name. You need to update your email.\n";

                            bool emailUpdated = false;
                            while (!emailUpdated) {
                                cout << "\nEnter new email that contains '" << newName << "': ";
                                string newEmail;
                                getline(cin, newEmail);

                                // Пробуем установить новый email
                                // set_email проверит, содержит ли он имя
                                if (contact.set_email(newEmail, contact.get_firstName())) {
                                    // Проверяем уникальность
                                    Contact existing = manager.getContact(newEmail);
                                    if (existing.get_email().empty() || existing.get_email() == email) {
                                        emailUpdated = true;
                                        cout << "Email updated successfully!\n";
                                    } else {
                                        cout << "Error: This email is already used by another contact!\n";
                                        // Продолжаем цикл
                                    }
                                }
                                // Если set_email вернул false, он сам покажет ошибку
                            }
                        } else {
                            // Email содержит новое имя - все ок
                            cout << "Email contains the new name. No update needed.\n";
                        }
                    }
                }
                break;
            }
            case 2: {
                cout << "Current last name: " << contact.get_lastName() << "\n";
                cout << "New last name: ";
                getline(cin, input);
                contact.set_lastName(input);
                break;
            }
            case 3: {
                cout << "Current patronymic: " << contact.get_patronymic() << "\n";
                cout << "New patronymic: ";
                getline(cin, input);
                contact.set_patronymic(input);
                break;
            }
            case 4: {
                cout << "Current address: " << contact.get_address() << "\n";
                cout << "New address: ";
                getline(cin, input);
                contact.set_address(input);
                break;
            }
            case 5: {
                cout << "Current birth date: " << contact.get_birthDate() << "\n";
                cout << "New birth date (DD.MM.YYYY): ";
                getline(cin, input);
                contact.set_birthDate(input);
                break;
            }
            case 6: {
                cout << "Current email: " << contact.get_email() << "\n";
                cout << "Current first name: " << contact.get_firstName() << "\n";

                string newEmail;
                bool emailUpdated = false;

                while (!emailUpdated) {
                    cout << "New email: ";
                    getline(cin, newEmail);

                    // Пробуем установить email - set_email проверит валидность
                    // и наличие имени в email
                    if (contact.set_email(newEmail, contact.get_firstName())) {
                        // Проверяем уникальность email
                        Contact existing = manager.getContact(newEmail);
                        if (existing.get_email().empty() || existing.get_email() == email) {
                            emailUpdated = true;
                            cout << "Email updated successfully!\n";
                        } else {
                            cout << "Error: This email is already used by another contact!\n";
                            // Восстанавливаем старый email
                            contact.set_email(contact.get_email(), contact.get_firstName());
                        }
                    }
                    // Если set_email вернул false, он сам покажет сообщение об ошибке
                    // и пользователь будет вынужден ввести email, содержащий имя
                }
                break;
            }

            case 7: {
                editPhones(contact);
                break;
            }

            case 8: {  // Save Changes
                if (contact.is_valid()) {
                    // Дополнительная строгая проверка перед сохранением
                    string currentEmail = contact.get_email();
                    string currentName = contact.get_firstName();

                    if (!currentEmail.empty() && !currentName.empty()) {
                        // Приводим к нижнему регистру
                        string emailLower = currentEmail;
                        string nameLower = currentName;
                        transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);
                        transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

                        // Извлекаем часть email до @
                        size_t atPos = emailLower.find('@');
                        string emailUser = (atPos != string::npos) ? emailLower.substr(0, atPos) : emailLower;

                        // Проверяем, есть ли имя в email
                        if (emailUser.find(nameLower) == string::npos) {
                            cout << "\nERROR: Cannot save!\n";
                            cout << "Email '" << currentEmail
                                 << "' does not contain the first name '" << currentName << "'\n";
                            cout << "Email MUST contain the first name. Please edit email first.\n";
                            break; // Не сохраняем
                        }
                    }

                    // Все проверки пройдены - сохраняем
                    if (manager.updateContact(email, contact)) {
                        cout << "Contact updated successfully!\n";
                    } else {
                        cout << "Failed to update contact!\n";
                    }
                    return;
                } else {
                    cout << "Contact data is invalid! Please fix errors before saving.\n";
                }
                break;
            }
            case 0:
                cout << "Edit cancelled.\n";
                return;

            default:
                cout << "Invalid choice!\n";
                break;
        }
    }
}

void editPhones(Contact& contact) {
    auto phones = contact.get_phones();
    
    while (true) {
        cout << "\n___ Phone Numbers ___\n";
        int i = 1;
        for (const auto& phone : phones) {
            cout << i++ << ". " << phone.type_to_string() << ": " << phone.get_number() << "\n";
        }
        
        cout << "\n1. Add Phone\n";
        cout << "2. Remove Phone\n";
        cout << "3. Edit Phone\n";
        cout << "0. Back\n";
        cout << "> ";

        int choice;
        cin >> choice;
        clearInput();

        if (choice == 0) break;

        switch (choice) {
            case 1: {
                string number;
                cout << "Enter phone number: ";
                getline(cin, number);
                
                int typeChoice;
                cout << "Phone type (0=Mobile, 1=Home, 2=Work): ";
                cin >> typeChoice;
                clearInput();

                const auto type = static_cast<PhoneType>(typeChoice);
                PhoneNumber newPhone(number, type);
                
                if (newPhone.is_valid()) {
                    phones.push_back(newPhone);
                    cout << "Phone added!\n";
                } else {
                    cout << "Invalid phone number!\n";
                }
                break;
            }
            case 2: {
                if (phones.empty()) {
                    cout << "No phones to remove!\n";
                    break;
                }
                
                int index;
                cout << "Enter phone number to remove (1-" << phones.size() << "): ";
                cin >> index;
                clearInput();
                
                if (index >= 1 && static_cast<size_t>(index) <= phones.size()) {
                    auto it = phones.begin();
                    advance(it, index - 1);
                    phones.erase(it);
                    cout << "Phone removed!\n";
                } else {
                    cout << "Invalid index!\n";
                }
                break;
            }
            case 3: {
                if (phones.empty()) {
                    cout << "No phones to edit!\n";
                    break;
                }
                
                int index;
                cout << "Enter phone number to edit (1-" << phones.size() << "): ";
                cin >> index;
                clearInput();
                
                if (index >= 1 && static_cast<size_t>(index) <= phones.size()) {
                    auto it = phones.begin();
                    advance(it, index - 1);
                    
                    string newNumber;
                    cout << "Current: " << it->get_number() << "\n";
                    cout << "New number: ";
                    getline(cin, newNumber);
                    
                    int typeChoice;
                    cout << "New type (0=Mobile, 1=Home, 2=Work): ";
                    cin >> typeChoice;
                    clearInput();

                    const auto newType = static_cast<PhoneType>(typeChoice);
                    
                    PhoneNumber updatedPhone(newNumber, newType);
                    if (updatedPhone.is_valid()) {
                        *it = updatedPhone;
                        cout << "Phone updated!\n";
                    } else {
                        cout << "Invalid phone number!\n";
                    }
                } else {
                    cout << "Invalid index!\n";
                }
                break;
            }
            default:
                cout << "Invalid choice!\n";
                break;
        }
    }
    
    contact.set_phones(phones);
}

// Добавление контакта через консоль
void addContact(ContactManager& manager) {
    clearScreen(); // Очистка экрана перед каждым выводом меню
    Contact c;
    string input;

    cout << "Name: ";
    getline(cin, input);
    c.set_firstName(input);

    cout << "Lastname: ";
    getline(cin, input);
    c.set_lastName(input);

    cout << "Patronymic (can be left blank): ";
    getline(cin, input);
    c.set_patronymic(input);

    cout << "Address: ";
    getline(cin, input);
    c.set_address(input);

    // ввод email
    while (true) {
        cout << "Email: ";
        getline(cin, input);
        if (c.set_email(input, c.get_firstName())) {
            // Проверяем, не существует ли уже контакт с таким email
            Contact existing = manager.getContact(c.get_email());
            if (existing.get_email().empty()) {
                break; // Email валидный и уникальный
            } else {
                cout << "Error: Contact with this email already exists!\n";
            }
        } else {
            cout << "Invalid email format. Please try again.\n";
        }
    }

    cout << "Date of birth (DD.MM.YYYY, can be left blank): ";
    getline(cin, input);
    if (!input.empty()) {
        if (!c.set_birthDate(input)) {
            cerr << "Invalid date format or date is in the future\n";
        }
    }

    // Ввод телефонов зацикленный
    cout << "\n___ Phone Numbers ___\n";

    bool hasPhone = false;
    while (!hasPhone) {
        // Очищаем текущие телефоны перед новой попыткой
        c.clearPhones();
        cout << "You must enter at least one phone number.\n";
        cout << "Press Enter on empty line to finish:\n";

        while (true) {
            cout << "Phone number: ";
            getline(cin, input);

            if (input.empty()) {
                // Проверяем, есть ли уже введенные телефоны
                if (c.phoneCount() > 0) {
                    hasPhone = true;
                    cout << "Phone numbers entered successfully!\n";
                    break; // Есть хотя бы один телефон
                } else {
                    cout << "\nERROR: At least one phone number is required!\n";
                    cout << "Please enter a phone number:\n";
                    continue; // Заставляем ввести хотя бы один номер
                }
            }

            // Выбор типа телефона
            int t = 0;
            while (true) {
                cout << "Phone type (0=Mobile, 1=Home, 2=Work): ";
                string typeInput;
                getline(cin, typeInput);

                if (typeInput.empty()) {
                    // По умолчанию мобильный
                    t = 0;
                    break;
                }

                try {
                    t = stoi(typeInput);
                    if (t >= 0 && t <= 2) {
                        break;
                    } else {
                        cout << "Please enter 0, 1, or 2: ";
                    }
                } catch (...) {
                    cout << "Invalid input. Please enter 0, 1, or 2: ";
                }
            }

            auto type = static_cast<PhoneType>(t);
            PhoneNumber newPhone(input, type);

            if (newPhone.is_valid()) {
                c.addPhone(newPhone);
                cout << "Phone number added: " << input << " ("
                     << newPhone.type_to_string() << ")\n";
                cout << "You can add another phone or press Enter to finish.\n";
            } else {
                cout << "Invalid phone number! Please enter a valid phone number.\n";
                cout << "Format: +7XXXXXXXXXX or 8XXXXXXXXXX\n";
            }
        }
    }

    if (c.is_valid()) {
        manager.addContact(c);
        cout << "Contact added!\n";
    } else {
        cerr << "The contact was not deleted or added.\n";
    }
}

void viewContacts(const ContactManager& manager) {
    clearScreen();

    const auto contacts = manager.getAllContacts();
    if (contacts.empty()) {
        cout << "                                " << "Phonebook is empty.\n";
        return;
    }

    printColorfulPhoneBook();

    for (size_t i = 0; i < contacts.size(); ++i) {
        const auto& c = contacts[i];
        cout << "                                " << "___ Contact " << i+1 << " ___\n";
        cout << "                           " << "Name: " << c.get_firstName();
        if (!c.get_patronymic().empty()) {
            cout << " " << c.get_patronymic();
        }
        cout << " " << c.get_lastName();
        cout << "\n";
        
        if (!c.get_birthDate().empty()) {
            cout << "                           " << "Birth Date: " << c.get_birthDate() << "\n";
        }
        
        cout << "                           " << "Email: " << c.get_email() << "\n";
        
        if (!c.get_address().empty()) {
            cout << "                           " << "Address: " << c.get_address() << "\n";
        }
        
        auto phones = c.get_phones();
        if (!phones.empty()) {
            phones.sort([](const PhoneNumber& a, const PhoneNumber& b) {
                return a.get_type() < b.get_type();
            });

            cout << "                           " << "Phones:\n";

            PhoneType currentType = phones.front().get_type();
            bool typePrinted = false;

            for (const auto& phone : phones) {
                if (phone.get_type() != currentType) {
                    currentType = phone.get_type();
                    typePrinted = false;
                }

                if (!typePrinted) {
                    cout << "                           " << "  " << phone.type_to_string() << ":\n";
                    cout << "                           " << "    " << phone.get_number() << "\n";
                    typePrinted = true;
                } else {
                    cout << "                           " << "    " << phone.get_number() << "\n";
                }
            }
        }
        cout << "                           " << "\n";
    }
    cout << "                           " << "Total contacts: " << contacts.size() << "\n";
}

void searchContacts(const ContactManager& manager) {
    clearScreen();
    cout << "\n___ Search Contacts ___\n";
    cout << "1. Search by Name\n";
    cout << "2. Search by Email\n";
    cout << "3. Search by Phone\n";
    cout << "0. Back\n";
    cout << "> ";

    int choice;
    cin >> choice;
    clearInput();

    string query;
    vector<Contact> results;

    switch (choice) {
        case 1: {
            cout << "Enter name to search: ";
            getline(cin, query);
            results = manager.searchByName(query);
            break;
        }
        case 2: {
            cout << "Enter email to search: ";
            getline(cin, query);
            auto allContacts = manager.getAllContacts();
            copy_if(allContacts.begin(), allContacts.end(), back_inserter(results),
                   [&](const Contact& c) {
                       return c.get_email().find(query) != string::npos;
                   });
            break;
        }
        case 3: {
            cout << "Enter phone to search: ";
            getline(cin, query);
            auto allContacts = manager.getAllContacts();
            copy_if(allContacts.begin(), allContacts.end(), back_inserter(results),
                   [&](const Contact& c) {
                       const auto phones = c.get_phones();
                       for (const auto& phone : phones) {
                           if (phone.get_number().find(query) != string::npos) {
                               return true;
                           }
                       }
                       return false;
                   });
            break;
        }
        case 0:
            return;
        default:
            cout << "Invalid choice!\n";
            return;
    }

    if (results.empty()) {
        cout << "No contacts found.\n";
    } else {
        cout << "\n___ Search Results (" << results.size() << " found) ___\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& c = results[i];
            cout << i+1 << ". " << c.get_firstName() << " " << c.get_lastName()
                 << " | Email: " << c.get_email() << " | Phones: ";
            for (const auto& pn : c.get_phones()) {
                cout << pn.get_number() << " ";
            }
            cout << "\n";
        }
    }
}

void removeContact(const ContactManager& manager) {
    clearScreen();

    string email;
    cout << "Enter the contact's email address to delete: ";
    getline(cin, email);

    if (manager.removeContact(email)) {
        cout << "Contact deleted.\n";
    } else {
        cout << "The contact was not found.\n";
    }
}