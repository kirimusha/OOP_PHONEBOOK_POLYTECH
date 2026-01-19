#ifndef CONTACT_H
#define CONTACT_H

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
    list<PhoneNumber> phones;  // Телефоны

    static list<PhoneNumber> prompt_for_phones();

public:
    // Конструкторы

    Contact() = default;

    // Основной конструктор
    Contact(string firstName,
            string lastName,
            string patronymic,
            string address,
            string birthDate,
            string email,
            list<PhoneNumber> phones);

    // Конструкторы копирования и перемещения
    Contact(const Contact& other) = default;
    Contact(Contact&& other) noexcept;

    // Операторы присваивания
    Contact& operator=(const Contact& other);
    Contact& operator=(Contact&& other) noexcept;

    // Геттеры (возвращаем по константной ссылке)
    const string& get_firstName() const;
    const string& get_lastName() const;
    const string& get_patronymic() const;
    const string& get_address() const;
    const string& get_birthDate() const;
    const string& get_email() const;
    const list<PhoneNumber>& get_phones() const;  // Важно: по ссылке!

    // Сеттеры (принимаем по значению и перемещаем)
    bool set_firstName(string newFirstName);
    bool set_lastName(string newLastName);
    bool set_patronymic(string newPatronymic);
    void set_address(string newAddress);
    bool set_birthDate(string newBirthDate);
    bool set_email(string newEmail, const string& firstName);
    bool set_phones(list<PhoneNumber> newPhones);

    // Валидация
    bool is_valid() const;

    // Статические методы валидации
    static bool validate_name(const string& name);
    static bool validate_email(const string& email, const string& firstName);
    static bool validate_birthDate(const string& date);

    // Работа с телефонами
    void addPhone(PhoneNumber phone);  // Принимаем по значению
    void removePhone(int index);
    void clearPhones();
    int phoneCount() const;

    // Для сохранения в файл
    string toJson() const;
    static Contact fromJson(const string& json);
    json toJsonObj() const;

    // Для вывода
    string toString() const;

    // Операторы сравнения
    bool operator==(const Contact& other) const;
    bool operator!=(const Contact& other) const;

    // оператор new
    static void* operator new(size_t size);
    static void operator delete(void* ptr) noexcept;
};

#endif // CONTACT_H