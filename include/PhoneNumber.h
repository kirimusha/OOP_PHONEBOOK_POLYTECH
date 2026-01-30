#ifndef PHONENUMBER_H
#define PHONENUMBER_H

#include <iostream>
#include <string>
#include "../third_party/json.hpp"

using json = nlohmann::json;
using namespace std;

enum class PhoneType {
    Work,      // Рабочий
    Home,      // Домашний
    Mobile     // Служебный
};

class PhoneNumber {
private:
    string number;
    PhoneType type;

public:
    // Конструкторы
    // Дефолтный конструктор
    PhoneNumber() = default;
    PhoneNumber(const string& number, PhoneType type = PhoneType::Mobile);
    // конструктор копирования
    // PhoneNumber(const PhoneNumber &copy);
    // деструктор
    // ~PhoneNumber();
    
    // Геттеры
    string get_number() const;
    PhoneType get_type() const;
    string type_to_string() const;
    
    // Сеттеры
    bool set_number(const string& newNumber);
    void set_type(PhoneType newType);
    
    // Нормализация (все форматы -> +7XXXXXXXXXX)
    string normalized() const;
    
    // Валидация
    bool is_valid() const;
    static bool validate(const string& number);
    
    // JSON сериализация
    string toJson() const;
    static PhoneNumber fromJson(const string& json);
    json toJsonObj() const;
    
    // чтобы удобно выводить содержимое объекта в виде строки
    string toString() const;
    
    // одинаковые ли телефоны (для поиска и сортировки)
    bool operator==(const PhoneNumber& other) const;
    bool operator!=(const PhoneNumber& other) const;
};

#endif // PHONENUMBER_H