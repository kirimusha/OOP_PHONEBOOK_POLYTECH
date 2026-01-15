#ifndef CONTACT_H // Защита от повторного подключения.
#define CONTACT_H // Зачем? Если файл подключить дважды, компилятор выдаст ошибку "класс объявлен дважды"

#include "../include/PhoneNumber.h"
#include "../include/Validators.h"
#include <list>
#include <string>
#include <iostream>
#include "../third_party/json.hpp"

using json = nlohmann::json;
using namespace std;

class Contact {
private:
    string firstName;          // Имя
    string lastName;           // Фамилия
    string patronymic;         // Отчество
    string address;            // Адрес
    string birthDate;          // Дата рождения
    string email;              // Email
    list<PhoneNumber> phones;  // Телефоны (рабочий, домашний, служебный)

    static list<PhoneNumber> prompt_for_phones();
public:
    // Конструкторы
    // Дефолтный конструктор
    Contact() = default;
    Contact(const string& firstName,
            const string& lastName, 
            const string& patronymic,
            const string& address,
            const string& birthDate,
            const string& email,
            const list<PhoneNumber>& phones);
    // конструктор копирования
    Contact(const Contact& other);
    // деструктор
    ~Contact();
    
    // Геттеры
    string get_firstName() const;
    string get_lastName() const;
    string get_patronymic() const;
    string get_address() const;
    string get_birthDate() const;
    string get_email() const;
    list<PhoneNumber> get_phones() const;

    
    // Сеттеры
    bool set_firstName(const string& newFirstName); // Почему bool? Чтобы понять, прошла ли валидация
    bool set_lastName(const string& newLastName); 
    bool set_patronymic(const string& newPatronymic);
    void set_address(const string& newAddress);
    bool set_birthDate(const string& newBirthDate);
    bool set_email(const string& newEmail, const string& firstName);
    bool set_phones(const list<PhoneNumber>& newPhones);
    
    // Валидация
    bool is_valid() const;

    // Статические методы валидации
    static bool validate_name(const string& name);   // static можно вызывать без объекта
    static bool validate_email(const string& email, const string& firstName); // проверить данные до создания контакта
    static bool validate_birthDate(const string& date);

    // Работа с телефонами
    void addPhone(const PhoneNumber& phone);
    void removePhone(int index);
    void clearPhones();
    int phoneCount() const;

    // для сохранения в файл
    string toJson() const;                        // Контакт в JSON
    static Contact fromJson(const string& json);  // JSON в Контакт
    json toJsonObj() const;

    // чтобы удобно выводить содержимое объекта в виде строки
    string toString() const;
    
    // одинаковые ли контакты (для поиска и сортировки)
    bool operator==(const Contact& other) const;
    bool operator!=(const Contact& other) const;
};

#endif // CONTACT_H