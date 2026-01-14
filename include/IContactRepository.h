#ifndef ICONTACTREPOSITORY_H
#define ICONTACTREPOSITORY_H

#include <vector>
#include <string>
#include "Contact.h"

using namespace std;

class IContactRepository {
public:
    virtual ~IContactRepository() = default; 

    virtual bool addContact(const Contact& contact) = 0; // говорит, что метод можно переопределять
    virtual bool removeContact(const string& email) = 0;
    virtual bool updateContact(const string& oldEmail, const Contact& newContact) = 0;
    virtual Contact getContact(const string& email) const = 0;
    virtual vector<Contact> getAllContacts() const = 0;
};


#endif
