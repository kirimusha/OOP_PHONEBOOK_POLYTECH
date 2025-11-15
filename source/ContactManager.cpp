#include "ContactManager.h"
#include <algorithm>

ContactManager::ContactManager(IContactRepository* repo)
    : m_repository(repo)
{}

bool ContactManager::addContact(const Contact& contact) {
    return m_repository->addContact(contact);
}

bool ContactManager::removeContact(const string& email) {
    return m_repository->removeContact(email);
}

bool ContactManager::updateContact(const Contact& contact) {
    return m_repository->updateContact(contact);
}

Contact ContactManager::getContact(const string& email) const {
    return m_repository->getContact(email);
}

vector<Contact> ContactManager::getAllContacts() const {
    return m_repository->getAllContacts();
}

vector<Contact> ContactManager::searchByName(const string& name) const {
    auto contacts = m_repository->getAllContacts(); // сам понимает какой тип данных
    vector<Contact> result;

    // функция для приведения строки к нижнему регистру
    auto toLower = [](string s) {
        transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char ch){ return tolower(ch); });
        return s;
    };

    string lowerName = toLower(name);

    copy_if(contacts.begin(), contacts.end(), back_inserter(result),
        [&](const Contact& c) {
            string full = c.get_firstName() + " " + c.get_lastName() + " " + c.get_patronymic();
            return full.find(name) != string::npos;
        });

    return result;
}

bool ContactManager::updateAllContacts(const vector<Contact>& contacts) {
    // Получаем текущие контакты
    auto currentContacts = m_repository->getAllContacts();
    
    // Удаляем все контакты
    for (const auto& contact : currentContacts) {
        m_repository->removeContact(contact.get_email());
    }
    
    // Добавляем отсортированные контакты
    for (const auto& contact : contacts) {
        if (!m_repository->addContact(contact)) {
            return false;
        }
    }
    
    return true;
}
