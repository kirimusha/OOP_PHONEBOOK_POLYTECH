#include "FileRepository.h"
#include <algorithm>
#include <iostream>

using json = nlohmann::json;
using namespace std;

FileRepository::FileRepository(const string& filename)
    : m_filename(filename)
{
    loadFromFile();
}

void FileRepository::loadFromFile() {
    const string filename = "contacts.json"; 
    m_contacts.clear();

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[FileRepository] Could not open file: " << filename << "\n";
        return;
    }

    try {
        // Читаем весь файл как JSON массив
        json jArray;
        file >> jArray;

        for (const auto& jContact : jArray) {
            // Конвертируем JSON объект в строку для обработки
            string contactStr = jContact.dump();
            Contact contact = Contact::fromJson(contactStr);

            // Если контакт реально пустой — не добавляем
            if (contact.get_firstName().empty() && contact.get_lastName().empty())
                continue;

            m_contacts.push_back(contact);
        }

        // if (m_contacts.empty())
        //     cout << "No contacts found.\n";
        // else
        //     cout << "[FileRepository] Loaded " << m_contacts.size() << " contacts.\n";

    } catch (const json::parse_error& e) {
        cerr << "[FileRepository] JSON parse error: " << e.what() << "\n";
        cerr << "File might be empty or corrupted. Creating new file.\n";
        // Создаем пустой массив
        saveToFile(filename);
    }

    file.close();
}


void FileRepository::saveToFile(const string& filename) {
    json jArray = json::array();
    for (const auto& contact : m_contacts) {
        jArray.push_back(contact.toJsonObj());
    }

    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open file for writing: " << filename << "\n";
        return;
    }

    file << jArray.dump(4); // красиво с отступами
    file.close();
}


bool FileRepository::addContact(const Contact& contact) {
    auto it = find_if(m_contacts.begin(), m_contacts.end(),
                           [&](const Contact& c){ return c.get_email() == contact.get_email(); });
    if (it != m_contacts.end()) return false;

    m_contacts.push_back(contact);
    saveToFile("contacts.json");
    return true;
}

bool FileRepository::removeContact(const string& email) {
    auto it = remove_if(m_contacts.begin(), m_contacts.end(),
                             [&](const Contact& c){ return c.get_email() == email; });
    if (it == m_contacts.end()) return false;

    m_contacts.erase(it, m_contacts.end());
    saveToFile("contacts.json");
    return true;
}

bool FileRepository::updateContact(const Contact& contact) {
    auto it = find_if(m_contacts.begin(), m_contacts.end(),
                           [&](const Contact& c){ return c.get_email() == contact.get_email(); });
    if (it == m_contacts.end()) return false;

    *it = contact;
    saveToFile("contacts.json");
    return true;
}

Contact FileRepository::getContact(const string& email) const {
    auto it = find_if(m_contacts.begin(), m_contacts.end(),
                           [&](const Contact& c){ return c.get_email() == email; });
    if (it != m_contacts.end()) return *it;

    return Contact(); // пустой контакт
}

vector<Contact> FileRepository::getAllContacts() const {
    return m_contacts;
}

