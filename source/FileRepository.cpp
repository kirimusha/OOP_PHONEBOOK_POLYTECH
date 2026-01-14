#include "../include/FileRepository.h"
#include <algorithm>
#include <iostream>
#include <utility>

using json = nlohmann::json;
using namespace std;

FileRepository::FileRepository(string  filename)
    : m_filename(std::move(filename))
{
    loadFromFile();
}

void FileRepository::loadFromFile() {
    m_contacts.clear();

    ifstream file(m_filename);
    if (!file.is_open()) {
        cerr << "[FileRepository] Could not open file: " << m_filename << "\n";
        return;
    }

    try {
        // JSON массив
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
        saveToFile(m_filename);
    }

    file.close();
}


void FileRepository::saveToFile(const string& filename) const {
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
    const auto it = find_if(m_contacts.begin(), m_contacts.end(),
                           [&](const Contact& c){ return c.get_email() == contact.get_email(); });
    if (it != m_contacts.end()) return false;

    m_contacts.push_back(contact);
    saveToFile(m_filename);
    return true;
}

bool FileRepository::removeContact(const string& email) {
    auto it = remove_if(m_contacts.begin(), m_contacts.end(),
                             [&](const Contact& c){ return c.get_email() == email; });
    if (it == m_contacts.end()) return false;

    m_contacts.erase(it, m_contacts.end());
    saveToFile(m_filename);
    return true;
}

bool FileRepository::updateContact(const string& oldEmail, const Contact& newContact) {
    // Ищем по старому email
    auto it = find_if(m_contacts.begin(), m_contacts.end(),
        [&](const Contact& c){ return c.get_email() == oldEmail; });

    if (it == m_contacts.end()) {
        return false; // Контакт не найден
    }

    // Проверяем, не меняется ли email на уже существующий
    if (oldEmail != newContact.get_email()) {
        // Ищем, нет ли уже контакта с новым email
        auto duplicate = find_if(m_contacts.begin(), m_contacts.end(),
            [&](const Contact& c){
                return c.get_email() == newContact.get_email() &&
                       c.get_email() != oldEmail; // Исключаем себя
            });

        if (duplicate != m_contacts.end()) {
            cerr << "[FileRepository] Email " << newContact.get_email()
                 << " already exists!\n";
            return false;
        }
    }

    *it = newContact;
    saveToFile(m_filename);
    return true;
}

Contact FileRepository::getContact(const string& email) const {
    const auto it = find_if(m_contacts.begin(), m_contacts.end(),
                           [&](const Contact& c){ return c.get_email() == email; });
    if (it != m_contacts.end()) return *it;

    return {}; // пустой контакт
}

vector<Contact> FileRepository::getAllContacts() const {
    return m_contacts;
}

