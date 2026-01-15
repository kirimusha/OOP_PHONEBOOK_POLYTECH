#include "../include/Contact.h"

using namespace std;
using json = nlohmann::json;

// Основной конструктор (принимаем по значению и перемещаем)
Contact::Contact(string firstName,
                 string lastName,
                 string patronymic,
                 string address,
                 string birthDate,
                 string email,
                 list<PhoneNumber> phones)
    : firstName(move(firstName)),
      lastName(move(lastName)),
      patronymic(move(patronymic)),
      address(move(address)),
      birthDate(move(birthDate)),
      email(move(email)),
      phones(move(phones))
{
    cout << "[Contact] Contact created: " << this->firstName << " " << this->lastName << "\n";
}

// Геттеры
const string& Contact::get_firstName() const {
    return firstName;
}

const string& Contact::get_lastName() const {
    return lastName;
}

const string& Contact::get_patronymic() const {
    return patronymic;
}

const string& Contact::get_address() const {
    return address;
}

const string& Contact::get_birthDate() const {
    return birthDate;
}

const string& Contact::get_email() const {
    return email;
}

const list<PhoneNumber>& Contact::get_phones() const {
    return phones;
}

// Сеттеры с перемещением
bool Contact::set_firstName(string newFirstName) {
    string input = Validators::trim(newFirstName);

    while (!validate_name(input)) {
        cerr << "[Contact] Invalid name: " << input << endl;
        cout << "Please enter a valid name: ";
        getline(cin, input);
        input = Validators::trim(input);
    }

    firstName = move(input);
    return true;
}

bool Contact::set_lastName(string newLastName) {
    string input = Validators::trim(newLastName);

    while (!validate_name(input)) {
        cerr << "[Contact] Invalid last name: " << input << endl;
        cout << "Please enter a valid last name: ";
        getline(cin, input);
        input = Validators::trim(input);
    }

    lastName = move(input);
    return true;
}

bool Contact::set_patronymic(string newPatronymic) {
    string input = Validators::trim(newPatronymic);

    // Отчество может быть пустым
    while (!input.empty() && !validate_name(input)) {
        cerr << "[Contact] Invalid patronymic: " << input << endl;
        cout << "Please enter a valid patronymic (or leave empty): ";
        getline(cin, input);
        input = Validators::trim(input);
    }

    patronymic = move(input);
    return true;
}

void Contact::set_address(string newAddress) {
    address = Validators::trim(move(newAddress));
}

bool Contact::set_birthDate(string newBirthDate) {
    string input = Validators::trim(newBirthDate);

    while (!validate_birthDate(input)) {
        cerr << "[Contact] Invalid date of birth: " << input << endl;
        cout << "Please enter a valid date of birth: ";
        getline(cin, input);
        input = Validators::trim(input);
    }

    birthDate = move(input);
    return true;
}

bool Contact::set_email(string newEmail, const string& firstName) {
    string input = Validators::trim(newEmail);
    input = Validators::remove_spaces(input);

    while (!Validators::validate_email(input, firstName)) {
        cerr << "[Contact] Invalid email address: " << input << endl;
        cerr << "Email must contain the first name '" << firstName << "' and be in valid format.\n";

        cout << "Please enter a valid email address: ";
        getline(cin, input);

        // повторно обрабатываем ввод
        input = Validators::trim(input);
        input = Validators::remove_spaces(input);
    }

    email = move(input);
    return true;
}

bool Contact::set_phones(list<PhoneNumber> newPhones) {
    list<PhoneNumber> input = move(newPhones);

    while (true) {
        // Контакт должен иметь хотя бы один номер телефона
        if (input.empty()) {
            cerr << "[Contact] Phone list cannot be empty\n";
            input = prompt_for_phones();
            continue;
        }

        // Проверяем, что все номера в списке валидны
        bool allValid = true;
        for (const auto& phone : input) {
            if (!phone.is_valid()) {
                cerr << "[Contact] Invalid phone number in list: " << phone.get_number() << "\n";
                allValid = false;
                break;
            }
        }

        if (allValid) {
            phones = move(input);  // Перемещаем, а не копируем
            return true;
        }

        cout << "Please enter valid phone numbers:\n";
        input = prompt_for_phones();
    }
}

list<PhoneNumber> Contact::prompt_for_phones() {
    list<PhoneNumber> phoneList;
    string input;

    cout << "Enter phone numbers (one per line, empty line to finish):\n";
    while (true) {
        cout << "Phone: ";
        getline(cin, input);
        input = Validators::trim(input);

        if (input.empty()) {
            if (phoneList.empty()) {
                cout << "At least one phone number is required. Continue entering...\n";
                continue;
            }
            break;
        }

        PhoneNumber phone(input);  // Создаем временный объект
        if (phone.is_valid()) {
            phoneList.push_back(move(phone));  // Перемещаем в список
            cout << "Phone number added.\n";
        } else {
            cout << "Invalid phone number. Please enter a valid one.\n";
        }
    }

    return phoneList;
}

// Работа с телефонами
void Contact::addPhone(PhoneNumber phone) {
    if (phone.is_valid()) {
        phones.push_back(move(phone));  // Перемещаем
        cout << "[Contact] Phone number added\n";
    } else {
        cerr << "[Contact] An attempt to add an invalid phone number" << endl;
    }
}

void Contact::removePhone(int index) {
    if (index >= 0 && index < static_cast<int>(phones.size())) {
        auto it = phones.begin();
        advance(it, index);
        phones.erase(it);
        cout << "[Contact] Phone number at index " << index << " removed successfully\n";
    } else {
        cerr << "[Contact] Incorrect phone number index: " << index
             << ". Total phones: " << phones.size() << "\n";
    }
}

void Contact::clearPhones() {
    phones.clear();
}

int Contact::phoneCount() const {
    return static_cast<int>(phones.size());
}

// Валидация
bool Contact::is_valid() const {
    // Проверка обязательных полей
    if (firstName.empty() || lastName.empty() || email.empty()) {
        cerr << "[Contact] Required fields (First name, Last name or Email) are missing\n";
        return false;
    }

    // Проверка валидации имени и фамилии
    if (!validate_name(firstName) || !validate_name(lastName)) {
        cerr << "[Contact] Invalid first or last name\n";
        return false;
    }

    // Проверка отчества (может быть пустым, но если не пустое — должно быть валидным)
    if (!patronymic.empty() && !validate_name(patronymic)) {
        cerr << "[Contact] Invalid patronymic\n";
        return false;
    }

    // Email обязателен и должен быть валидным
    if (!validate_email(email, firstName)) {
        cerr << "[Contact] Invalid email address\n";
        return false;
    }

    // Должен быть хотя бы один телефон
    if (phones.empty()) {
        cerr << "[Contact] There is not a single phone number\n";
        return false;
    }

    // Проверка корректности всех телефонов
    for (const auto& phone : phones) {
        if (!phone.is_valid()) {
            cerr << "[Contact] One of the phones is invalid\n";
            return false;
        }
    }

    // Проверка даты рождения (необязательная, но если задана — валидируем)
    if (!validate_birthDate(birthDate)) {
        cerr << "[Contact] Invalid birth date\n";
        return false;
    }

    return true;
}

bool Contact::validate_name(const string& name) {
    return Validators::validate_name(name);
}

bool Contact::validate_email(const string& email, const string& firstName) {
    return Validators::validate_email(email, firstName);
}

bool Contact::validate_birthDate(const string& date) {
    return Validators::validate_birthDate(date);
}

// Сериализация в JSON
json Contact::toJsonObj() const {
    json j;
    j["firstName"]  = firstName;
    j["lastName"]   = lastName;
    j["patronymic"] = patronymic;
    j["address"]    = address;
    j["birthDate"]  = birthDate;
    j["email"]      = email;

    j["phones"] = nlohmann::json::array();
    for (const auto& phone : phones) {
        j["phones"].push_back(phone.toJsonObj());
    }

    return j;
}

string Contact::toJson() const {
    return toJsonObj().dump();
}

// Десериализация из JSON
Contact Contact::fromJson(const string& jsonStr) {
    Contact contact;

    try {
        json data = nlohmann::json::parse(jsonStr);

        // Если это массив — достаём первый элемент
        if (data.is_array() && !data.empty()) {
            if (data[0].is_string()) {
                data = nlohmann::json::parse(data[0].get<string>());
            } else {
                data = data[0];
            }
        }

        // Простые поля
        contact.firstName  = data.value("firstName", "");
        contact.lastName   = data.value("lastName", "");
        contact.patronymic = data.value("patronymic", "");
        contact.address    = data.value("address", "");
        contact.email      = data.value("email", "");
        contact.birthDate  = data.value("birthDate", "");

        // телефоны
        if (data.contains("phones") && data["phones"].is_array()) {
            list<PhoneNumber> phones;
            for (const auto& p : data["phones"]) {
                string number = p.value("number", "");
                int typeInt   = p.value("type", 0);
                phones.emplace_back(move(number), static_cast<PhoneType>(typeInt));
            }
            contact.phones = move(phones);
        }
    }
    catch (const std::exception& e) {
        cerr << "[Contact::fromJson] JSON parse error: " << e.what() << endl;
    }

    return contact;
}

// Вспомогательные методы
string Contact::toString() const {
    string result = lastName + " " + firstName + " " + patronymic;
    if (!birthDate.empty()) {
        result += ", birthDate: " + birthDate;
    }

    if (!email.empty()) {
        result += ", email: " + email;
    }

    if (!phones.empty()) {
        result += ", phones: " + to_string(phones.size());
    }

    return result;
}

bool Contact::operator==(const Contact& other) const {
    return firstName == other.firstName
        && lastName == other.lastName
        && patronymic == other.patronymic
        && email == other.email;
}

bool Contact::operator!=(const Contact& other) const {
    return !(*this == other);
}