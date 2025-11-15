#pragma once
#include "IContactRepository.h"
#include <../third_party/json.hpp>
#include <string>
#include <vector>
#include <fstream>

using namespace std;
using json = nlohmann::json;

class FileRepository : public IContactRepository {
private:
    string m_filename;
    vector<Contact> m_contacts;

    void loadFromFile();
    void saveToFile(const std::string& filename);

public:
    FileRepository(const string& filename);
    ~FileRepository() = default;

    bool addContact(const Contact& contact) override; // Эта функция должна переопределять виртуальную функцию из базового класса
    bool removeContact(const string& email) override;
    bool updateContact(const Contact& contact) override;
    Contact getContact(const string& email) const override;
    vector<Contact> getAllContacts() const override;
};
