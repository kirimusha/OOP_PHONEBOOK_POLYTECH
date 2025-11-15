#ifndef UI
#define UI

#include "ContactManager.h"

// Вспомогательные функции
void clearInput();
bool stringToTime(const string& str, time_t& out);
PhoneType selectPhoneType();

// Основные функции UI
void addContact(ContactManager& manager);
void viewContacts(const ContactManager& manager);
void sortContacts(ContactManager& manager);
void searchContacts(const ContactManager& manager);
void editContact(ContactManager& manager);
void removeContact(ContactManager& manager);
void editPhones(Contact& contact);

#endif