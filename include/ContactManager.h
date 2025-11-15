#pragma once
#include "Contact.h"
#include "IContactRepository.h"
#include <string>
#include <vector>

using namespace std;

class ContactManager {
private:
    IContactRepository* m_repository;

public:
    explicit ContactManager(IContactRepository* repo);

    bool addContact(const Contact& contact);
    bool removeContact(const string& email);
    bool updateContact(const Contact& contact);
    bool updateAllContacts(const vector<Contact>& contacts);
    Contact getContact(const string& email) const;
    vector<Contact> getAllContacts() const;

    vector<Contact> searchByName(const string& name) const;
    IContactRepository* getRepository() const { return m_repository; }
};
