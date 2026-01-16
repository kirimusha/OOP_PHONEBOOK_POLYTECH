#pragma once
#include "../include/IContactRepository.h"
#include "../third_party/json.hpp"
#include <string>
#include <vector>
#include <memory>
#include <libpq-fe.h>

using namespace std;
using json = nlohmann::json;

class PostgreSQLRepository : public IContactRepository {
private:
    PGconn* m_connection;
    string m_connectionString;

    void ensureTablesExist() const;
    bool executeQuery(const string& query) const;
    PGresult* executeQueryWithResult(const string& query) const;
    PhoneType stringToPhoneType(const string& typeStr) const;
    string phoneTypeToString(PhoneType type) const;

public:
    explicit PostgreSQLRepository(string  connectionString);
    ~PostgreSQLRepository() override;

    bool connect();
    void disconnect();

    bool addContact(const Contact& contact) override;
    bool removeContact(const string& email) override;
    bool updateContact(const string& oldEmail, const Contact& newContact) override;
    Contact getContact(const string& email) const override;
    vector<Contact> getAllContacts() const override;

    static bool isPostgreSQLAvailable();
};