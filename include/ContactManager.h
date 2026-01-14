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

    bool addContact(const Contact& contact) const;
    bool removeContact(const string& email) const;
    bool updateContact(const string& oldEmail, const Contact& contact) const;
    bool updateAllContacts(const vector<Contact>& contacts) const;
    Contact getContact(const string& email) const;
    vector<Contact> getAllContacts() const;

    vector<Contact> searchByName(const string& name) const;
    IContactRepository* getRepository() const { return m_repository; }
};
