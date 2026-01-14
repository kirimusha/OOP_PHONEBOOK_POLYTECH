#ifndef UI
#define UI

#include "../include/ContactManager.h"
#include "../include/Contact.h"
#include "../include/PhoneNumber.h"
#include "../include/FileRepository.h"
#include "../include/ContactManager.h"
#include <iostream>

using namespace std;

// для красоты
void printColorfulPhoneBook();

// Вспомогательные функции
void clearInput();
void clearScreen();
void waitForEnter();
bool stringToTime(const string& str, time_t& out);
PhoneType selectPhoneType();

// Основные функции UI
void addContact(ContactManager& manager);
void viewContacts(const ContactManager& manager);
void sortContacts(const ContactManager& manager);
void searchContacts(const ContactManager& manager);
void editContact(ContactManager& manager);
void removeContact(const ContactManager& manager);
void editPhones(Contact& contact);

#endif