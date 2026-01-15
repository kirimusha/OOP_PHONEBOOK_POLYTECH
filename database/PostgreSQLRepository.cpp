#include "../database/PostgreSQLRepository.h"
#include <iostream>
#include <sstream>
#include <utility>

using namespace std;

PostgreSQLRepository::PostgreSQLRepository(string  connectionString)
    : m_connection(nullptr), m_connectionString(move(connectionString))
{
    connect();
}

PostgreSQLRepository::~PostgreSQLRepository()
{
    disconnect();
}

bool PostgreSQLRepository::connect()
{
    if (m_connection) {
        PQfinish(m_connection);
    }

    m_connection = PQconnectdb(m_connectionString.c_str());

    if (PQstatus(m_connection) != CONNECTION_OK) {
        cerr << "Connection to database failed: " << PQerrorMessage(m_connection) << endl;
        PQfinish(m_connection);
        m_connection = nullptr;
        return false;
    }

    ensureTablesExist();
    return true;
}

void PostgreSQLRepository::disconnect()
{
    if (m_connection) {
        PQfinish(m_connection);
        m_connection = nullptr;
    }
}

void PostgreSQLRepository::ensureTablesExist() const {
    if (!m_connection) return;

    // Создаем таблицу контактов
    string createContactsTable = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            email VARCHAR(255) PRIMARY KEY,
            first_name VARCHAR(100) NOT NULL,
            last_name VARCHAR(100) NOT NULL,
            patronymic VARCHAR(100),
            address TEXT,
            birth_date VARCHAR(10),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";

    executeQuery(createContactsTable);

    // Создаем таблицу телефонов
    string createPhonesTable = R"(
        CREATE TABLE IF NOT EXISTS phone_numbers (
            id SERIAL PRIMARY KEY,
            contact_email VARCHAR(255) REFERENCES contacts(email) ON DELETE CASCADE,
            number VARCHAR(20) NOT NULL,
            type VARCHAR(20) NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";

    executeQuery(createPhonesTable);
}

bool PostgreSQLRepository::executeQuery(const string& query) const
{
    if (!m_connection) {
        cerr << "ERROR: No database connection!" << endl;
        return false;
    }

    PGresult* res = PQexec(m_connection, query.c_str());
    ExecStatusType status = PQresultStatus(res);

    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        cerr << "=== SQL ERROR ===" << endl;
        cerr << "Error: " << PQerrorMessage(m_connection) << endl;
        cerr << "Query: " << query << endl;
        cerr << "Status: " << PQresStatus(status) << " (" << status << ")" << endl;
        cerr << "=== END ERROR ===" << endl;

        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

PGresult* PostgreSQLRepository::executeQueryWithResult(const string& query) const
{
    if (!m_connection) return nullptr;

    PGresult* res = PQexec(m_connection, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Query failed: " << PQerrorMessage(m_connection) << endl;
        PQclear(res);
        return nullptr;
    }

    return res;
}

PhoneType PostgreSQLRepository::stringToPhoneType(const string& typeStr) const
{
    if (typeStr == "Mobile") return PhoneType::Mobile;
    if (typeStr == "Home") return PhoneType::Home;
    if (typeStr == "Work") return PhoneType::Work;
    return PhoneType::Mobile;
}

string PostgreSQLRepository::phoneTypeToString(PhoneType type) const
{
    switch (type) {
        case PhoneType::Mobile: return "Mobile";
        case PhoneType::Home: return "Home";
        case PhoneType::Work: return "Work";
        default: return "Mobile";
    }
}

bool PostgreSQLRepository::addContact(const Contact& contact)
{
    if (!m_connection) {
        cerr << "No database connection!" << endl;
        return false;
    }

    // Начинаем транзакцию
    if (!executeQuery("BEGIN")) {
        cerr << "Failed to begin transaction" << endl;
        return false;
    }

    try {
        // Вставляем контакт - используем PQescapeLiteral правильно
        stringstream contactQuery;

        // Экранируем все значения
        char* escaped_email = PQescapeLiteral(m_connection, contact.get_email().c_str(), contact.get_email().length());
        char* escaped_first = PQescapeLiteral(m_connection, contact.get_firstName().c_str(), contact.get_firstName().length());
        char* escaped_last = PQescapeLiteral(m_connection, contact.get_lastName().c_str(), contact.get_lastName().length());
        char* escaped_patr = PQescapeLiteral(m_connection, contact.get_patronymic().c_str(), contact.get_patronymic().length());
        char* escaped_addr = PQescapeLiteral(m_connection, contact.get_address().c_str(), contact.get_address().length());
        char* escaped_birth = PQescapeLiteral(m_connection, contact.get_birthDate().c_str(), contact.get_birthDate().length());

        contactQuery << "INSERT INTO contacts (email, first_name, last_name, patronymic, address, birth_date) "
                     << "VALUES ("
                     << escaped_email << ", "
                     << escaped_first << ", "
                     << escaped_last << ", "
                     << escaped_patr << ", "
                     << escaped_addr << ", "
                     << escaped_birth
                     << ") ON CONFLICT (email) DO NOTHING";

        // Освобождаем память
        PQfreemem(escaped_email);
        PQfreemem(escaped_first);
        PQfreemem(escaped_last);
        PQfreemem(escaped_patr);
        PQfreemem(escaped_addr);
        PQfreemem(escaped_birth);

        cout << "DEBUG: Contact query: " << contactQuery.str() << endl;

        if (!executeQuery(contactQuery.str())) {
            cerr << "Failed to insert contact" << endl;
            executeQuery("ROLLBACK");
            return false;
        }

        // Вставляем телефоны
        list<PhoneNumber> phones = contact.get_phones();
        for (const auto& phone : phones) {
            stringstream phoneQuery;

            // Экранируем номер телефона
            char* escaped_phone_num = PQescapeLiteral(m_connection, phone.get_number().c_str(), phone.get_number().length());

            // Для email используем то же значение что и для контакта
            char* escaped_email_for_phone = PQescapeLiteral(m_connection, contact.get_email().c_str(), contact.get_email().length());

            phoneQuery << "INSERT INTO phone_numbers (contact_email, number, type) "
                       << "VALUES ("
                       << escaped_email_for_phone << ", "
                       << escaped_phone_num << ", '"
                       << phoneTypeToString(phone.get_type())
                       << "')";

            // Освобождаем память
            PQfreemem(escaped_phone_num);
            PQfreemem(escaped_email_for_phone);

            cout << "DEBUG: Phone query: " << phoneQuery.str() << endl;

            if (!executeQuery(phoneQuery.str())) {
                cerr << "Failed to insert phone" << endl;
                executeQuery("ROLLBACK");
                return false;
            }
        }

        if (!executeQuery("COMMIT")) {
            cerr << "Failed to commit transaction" << endl;
            executeQuery("ROLLBACK");
            return false;
        }

        cout << "Contact added successfully!" << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "Exception in addContact: " << e.what() << endl;
        executeQuery("ROLLBACK");
        return false;
    }
    catch (...) {
        cerr << "Unknown exception in addContact" << endl;
        executeQuery("ROLLBACK");
        return false;
    }
}

bool PostgreSQLRepository::removeContact(const string& email)
{
    if (!m_connection) return false;

    char* escaped_email = PQescapeLiteral(m_connection, email.c_str(), email.length());

    stringstream query;
    query << "DELETE FROM contacts WHERE email = " << escaped_email;

    cout << "DEBUG removeContact query: " << query.str() << endl;

    bool result = executeQuery(query.str());

    PQfreemem(escaped_email);

    return result;
}

bool PostgreSQLRepository::updateContact(const string& oldEmail, const Contact& newContact)
{
    if (!m_connection) return false;

    // Начинаем транзакцию
    executeQuery("BEGIN");

    try {
        // Экранируем старый email
        char* escaped_old_email = PQescapeLiteral(m_connection, oldEmail.c_str(), oldEmail.length());

        // Удаляем старые телефоны
        stringstream deletePhonesQuery;
        deletePhonesQuery << "DELETE FROM phone_numbers WHERE contact_email = " << escaped_old_email;

        cout << "DEBUG updateContact delete phones: " << deletePhonesQuery.str() << endl;

        if (!executeQuery(deletePhonesQuery.str())) {
            PQfreemem(escaped_old_email);
            executeQuery("ROLLBACK");
            return false;
        }

        // Экранируем новые значения
        char* escaped_new_email = PQescapeLiteral(m_connection, newContact.get_email().c_str(), newContact.get_email().length());
        char* escaped_first = PQescapeLiteral(m_connection, newContact.get_firstName().c_str(), newContact.get_firstName().length());
        char* escaped_last = PQescapeLiteral(m_connection, newContact.get_lastName().c_str(), newContact.get_lastName().length());
        char* escaped_patr = PQescapeLiteral(m_connection, newContact.get_patronymic().c_str(), newContact.get_patronymic().length());
        char* escaped_addr = PQescapeLiteral(m_connection, newContact.get_address().c_str(), newContact.get_address().length());
        char* escaped_birth = PQescapeLiteral(m_connection, newContact.get_birthDate().c_str(), newContact.get_birthDate().length());

        // Обновляем контакт
        stringstream updateContactQuery;
        updateContactQuery << "UPDATE contacts SET "
                          << "email = " << escaped_new_email << ", "
                          << "first_name = " << escaped_first << ", "
                          << "last_name = " << escaped_last << ", "
                          << "patronymic = " << escaped_patr << ", "
                          << "address = " << escaped_addr << ", "
                          << "birth_date = " << escaped_birth << " "
                          << "WHERE email = " << escaped_old_email;

        cout << "DEBUG updateContact update: " << updateContactQuery.str() << endl;

        // Освобождаем память
        PQfreemem(escaped_old_email);
        PQfreemem(escaped_new_email);
        PQfreemem(escaped_first);
        PQfreemem(escaped_last);
        PQfreemem(escaped_patr);
        PQfreemem(escaped_addr);
        PQfreemem(escaped_birth);

        if (!executeQuery(updateContactQuery.str())) {
            executeQuery("ROLLBACK");
            return false;
        }

        // Вставляем новые телефоны
        list<PhoneNumber> phones = newContact.get_phones();
        for (const auto& phone : phones) {
            char* escaped_phone_email = PQescapeLiteral(m_connection, newContact.get_email().c_str(), newContact.get_email().length());
            char* escaped_phone_num = PQescapeLiteral(m_connection, phone.get_number().c_str(), phone.get_number().length());

            stringstream phoneQuery;
            phoneQuery << "INSERT INTO phone_numbers (contact_email, number, type) "
                       << "VALUES ("
                       << escaped_phone_email << ", "
                       << escaped_phone_num << ", '"
                       << phoneTypeToString(phone.get_type())
                       << "')";

            cout << "DEBUG updateContact insert phone: " << phoneQuery.str() << endl;

            PQfreemem(escaped_phone_email);
            PQfreemem(escaped_phone_num);

            if (!executeQuery(phoneQuery.str())) {
                executeQuery("ROLLBACK");
                return false;
            }
        }

        executeQuery("COMMIT");
        return true;
    }
    catch (...) {
        executeQuery("ROLLBACK");
        return false;
    }
}

Contact PostgreSQLRepository::getContact(const string& email) const
{
    if (!m_connection) return Contact();

    // Экранируем email ПРАВИЛЬНО
    char* escaped_email = PQescapeLiteral(m_connection, email.c_str(), email.length());

    // Получаем данные контакта
    stringstream contactQuery;
    contactQuery << "SELECT first_name, last_name, patronymic, address, birth_date, email "
                 << "FROM contacts WHERE email = " << escaped_email;

    cout << "DEBUG getContact query: " << contactQuery.str() << endl;

    PGresult* contactRes = executeQueryWithResult(contactQuery.str());

    // Освобождаем память
    PQfreemem(escaped_email);

    if (!contactRes || PQntuples(contactRes) == 0) {
        if (contactRes) PQclear(contactRes);
        cerr << "Contact not found for email: " << email << endl;
        return Contact();
    }

    // Получаем поля контакта
    string firstName = PQgetvalue(contactRes, 0, 0) ? PQgetvalue(contactRes, 0, 0) : "";
    string lastName = PQgetvalue(contactRes, 0, 1) ? PQgetvalue(contactRes, 0, 1) : "";
    string patronymic = PQgetvalue(contactRes, 0, 2) ? PQgetvalue(contactRes, 0, 2) : "";
    string address = PQgetvalue(contactRes, 0, 3) ? PQgetvalue(contactRes, 0, 3) : "";
    string birthDate = PQgetvalue(contactRes, 0, 4) ? PQgetvalue(contactRes, 0, 4) : "";
    string contactEmail = PQgetvalue(contactRes, 0, 5) ? PQgetvalue(contactRes, 0, 5) : "";

    PQclear(contactRes);

    // Получаем телефоны
    char* escaped_email2 = PQescapeLiteral(m_connection, email.c_str(), email.length());
    stringstream phonesQuery;
    phonesQuery << "SELECT number, type FROM phone_numbers WHERE contact_email = " << escaped_email2;

    cout << "DEBUG getContact phones query: " << phonesQuery.str() << endl;

    PGresult* phonesRes = executeQueryWithResult(phonesQuery.str());
    PQfreemem(escaped_email2);

    list<PhoneNumber> phones;

    if (phonesRes) {
        int rows = PQntuples(phonesRes);
        for (int i = 0; i < rows; i++) {
            string number = PQgetvalue(phonesRes, i, 0) ? PQgetvalue(phonesRes, i, 0) : "";
            string typeStr = PQgetvalue(phonesRes, i, 1) ? PQgetvalue(phonesRes, i, 1) : "";
            PhoneType type = stringToPhoneType(typeStr);
            phones.push_back(PhoneNumber(number, type));
        }
        PQclear(phonesRes);
    }

    return Contact(firstName, lastName, patronymic, address, birthDate, contactEmail, phones);
}

vector<Contact> PostgreSQLRepository::getAllContacts() const
{
    vector<Contact> contacts;

    if (!m_connection) return contacts;

    // Получаем все контакты
    string query = "SELECT email FROM contacts ORDER BY last_name, first_name";
    PGresult* res = executeQueryWithResult(query);

    if (!res) return contacts;

    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        string email = PQgetvalue(res, i, 0);
        Contact contact = getContact(email);
        if (!contact.get_email().empty()) {
            contacts.push_back(contact);
        }
    }

    PQclear(res);
    return contacts;
}

bool PostgreSQLRepository::isPostgreSQLAvailable()
{
    // пока так
    return true;
}