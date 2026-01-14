#include "Contact.h"

using namespace std;
using json = nlohmann::json;

// конструкторы
Contact::Contact(const string& firstName,
                 const string& lastName,
                 const string& patronymic,
                 const string& address,
                 const string& birthDate,
                 const string& email,
                 const list<PhoneNumber>& phones)
    : firstName(firstName),
      lastName(lastName),
      patronymic(patronymic),
      address(address),
      birthDate(birthDate),
      email(email),
      phones(phones)
{
    cout << "[Contact] A contact has been created:" << firstName << lastName;
}

// Деструктор
Contact::~Contact() = default;

// Конструктор копирования
Contact::Contact(const Contact& other)
    : firstName(other.firstName),
      lastName(other.lastName),
      patronymic(other.patronymic),
      address(other.address),
      birthDate(other.birthDate),
      email(other.email),
      phones(other.phones) {}

// геттеры

string Contact::get_firstName() const {
    return firstName;
}

string Contact::get_lastName() const {
    return lastName;
}

string Contact::get_patronymic() const {
    return patronymic;
}

string Contact::get_address() const {
    return address;
}

string Contact::get_birthDate() const {
    return birthDate;
}

string Contact::get_email() const {
    return email;
}

list<PhoneNumber> Contact::get_phones() const {
    return phones;
}

// сеттеры с валидацией

bool Contact::set_firstName(const string& newFirstName) {
    string input = Validators::trim(newFirstName);
    
    while (!validate_name(input)) {
        cerr << "[Contact] Invalid name: " << input << endl;
        cout << "Please enter a valid name: ";
        getline(cin, input);
        input = Validators::trim(input);
    }
    
    firstName = input;
    return true;
}

bool Contact::set_lastName(const string& newLastName) {
    string input = Validators::trim(newLastName);
    
    while (!validate_name(input)) {
        cerr << "[Contact] Invalid last name: " << input << endl;
        cout << "Please enter a valid last name: ";
        getline(cin, input);
        input = Validators::trim(input);
    }
    
    lastName = input;
    return true;
}

bool Contact::set_patronymic(const string& newPatronymic) {
    string input = Validators::trim(newPatronymic);
    
    // Отчество может быть пустым
    while (!input.empty() && !validate_name(input)) {
        cerr << "[Contact] Invalid patronymic: " << input << endl;
        cout << "Please enter a valid patronymic (or leave empty): ";
        getline(cin, input);
        input = Validators::trim(input);
    }
    
    patronymic = input;
    return true;
}

void Contact::set_address(const string& newAddress) {
    address = Validators::trim(newAddress);
}

bool Contact::set_birthDate(const string& newBirthDate) {
    string input = Validators::trim(newBirthDate);
    
    while (!validate_birthDate(input)) {
        cerr << "[Contact] Invalid date of birth: " << input << endl;
        cout << "Please enter a valid date of birth: ";
        getline(cin, input);
        input = Validators::trim(input);
    }
    
    birthDate = input;
    return true;
}

bool Contact::set_email(const string& newEmail, const string& firstName) {
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
    
    email = input;
    return true;
}

bool Contact::set_phones(const list<PhoneNumber>& newPhones) {
    list<PhoneNumber> input = newPhones;
    
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
            phones = input;
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
        
        PhoneNumber phone(input);
        if (phone.is_valid()) {
            phoneList.push_back(phone);
            cout << "Phone number added.\n";
        } else {
            cout << "Invalid phone number. Please enter a valid one.\n";
        }
    }
    
    return phoneList;
}

// работа с телефонами

void Contact::addPhone(const PhoneNumber& phone) {
    if (phone.is_valid()) {
        phones.push_back(phone);
    } else {
        cerr << "[Contact] An attempt to add an invalid phone number" << endl;
    }
}

void Contact::removePhone(int index) {
    if (index >= 0 && index < static_cast<int>(phones.size())) {
        auto it = phones.begin();
        for (int i = 0; i < index; ++i) {
            ++it;
        }
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

// валидация

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

// сериализация — это процесс превращения объекта класса в формат,
// который можно сохранить или передать, например в файл, базу данных или через сеть

// сериализация (JSON)
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
        j["phones"].push_back(phone.toJsonObj()); // нужно, чтобы PhoneNumber тоже возвращал json
    }

    return j;
}

string Contact::toJson() const {
    return toJsonObj().dump(); // возвращает корректную JSON-строку
}


// десериализация
Contact Contact::fromJson(const string& jsonStr) {
    Contact contact;

    try {
        json data = nlohmann::json::parse(jsonStr);

        // Если это массив — достаём первый элемент
        if (data.is_array() && !data.empty()) {
            if (data[0].is_string()) {
                data = nlohmann::json::parse(data[0].get<string>()); // строка внутри массива
            } else {
                data = data[0]; // уже объект
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
            for (const auto& p : data["phones"]) {
                string number = p.value("number", "");
                int typeInt   = p.value("type", 0);
                contact.phones.push_back(PhoneNumber(number, static_cast<PhoneType>(typeInt)));
            }
        }
    }
    catch (const std::exception& e) {
        cerr << "[Contact::fromJson] JSON parse error: " << e.what() << endl;
    }

    return contact;
}

// вспомогательные методы
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
// возвращает что-то типа "Иванов Иван Иванович, birthDate: 15.03.1990, email: ivan@example.com, phones: 89887448548, ..."


bool Contact::operator==(const Contact& other) const {
    return firstName == other.firstName
        && lastName == other.lastName
        && patronymic == other.patronymic
        && email == other.email;
}

// просто логическое отрицание предыдущего ==
bool Contact::operator!=(const Contact& other) const {
    return !(*this == other);
}