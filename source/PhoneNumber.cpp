#include "PhoneNumber.h"
#include "Validators.h"
#include <iostream>
#include <string>
#include <regex>
#include "../third_party/json.hpp"

using json = nlohmann::json;
using namespace std;


PhoneNumber::PhoneNumber(const string& number, PhoneType type)
    : number(number), type(type) {}

// Геттеры
string PhoneNumber::get_number() const {
    return number;
}

PhoneType PhoneNumber::get_type() const {
    return type;
}

string PhoneNumber::type_to_string() const {
    switch (type) {
        case PhoneType::Work:   return "work";
        case PhoneType::Home:   return "home";
        case PhoneType::Mobile: return "mobile";
        default: return "unknown";
    }
}

bool PhoneNumber::set_number(const string& newNumber) {
    if (validate(number)) {
        number = newNumber;
        return true;
    }
    cerr << "[PhoneNumber] Invalid number: " << number << endl;
    return false;
}

void PhoneNumber::set_type(PhoneType newType) {
    type = newType;
}

// Нормализация — оставить только цифры, привести к 7XXXXXXXXXX
string PhoneNumber::normalized() const {
    string clean;
    // Оставляем только цифры
    for (char c : number) {
        if (c >= '0' && c <= '9')  // Простая проверка на цифру
            clean += c;
    }

    // Приведение к международному формату (только цифры)
    if (clean.size() > 0) {
        if (clean[0] == '8') {
            // Заменяем 8 на 7 для кода России
            clean[0] = '7';
        } else if (clean[0] != '7') {
            // Если номер не начинается с 7 или 8, добавляем код России
            if (clean.size() == 10)
                clean = "7" + clean;
        }
    }
    return clean;
}

bool PhoneNumber::is_valid() const {
    return validate(number);
}

// Простая обёртка над Validators
bool PhoneNumber::validate(const string& number) {
    return Validators::validate_phone(number);
}

// Сериализация в JSON
json PhoneNumber::toJsonObj() const {
    json j;
    j["number"] = number;
    j["type"] = static_cast<int>(type);
    return j;
}

string PhoneNumber::toJson() const {
    return toJsonObj().dump();
}

// Десериализация
PhoneNumber PhoneNumber::fromJson(const string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        // Проверяем обязательные поля
        if (!j.contains("number") || !j.contains("type")) {
            throw invalid_argument("Missing required fields in JSON");
        }
        
        string number = j["number"].get<string>();
        int typeValue = j["type"].get<int>();
        
        // Валидация типа телефона
        PhoneType phoneType;
        switch (typeValue) {
            case 0: phoneType = PhoneType::Work; break;
            case 1: phoneType = PhoneType::Home; break;
            case 2: phoneType = PhoneType::Mobile; break;
            default:
                throw std::invalid_argument(
                    "Invalid phone type: " + std::to_string(typeValue) + 
                    ". Valid values are: 0 (Work), 1 (Home), 2 (Mobile)"
                );
        }
        
        return PhoneNumber(number, static_cast<PhoneType>(typeValue));
        
    } catch (const json::parse_error& e) {
        throw invalid_argument("Invalid JSON format: " + string(e.what()));
    } catch (const json::type_error& e) {
        throw invalid_argument("Invalid data types in JSON: " + string(e.what()));
    }
}

string PhoneNumber::toString() const {
    return type_to_string() + ": " + number;
}

bool PhoneNumber::operator==(const PhoneNumber& other) const {
    return normalized() == other.normalized();
}

bool PhoneNumber::operator!=(const PhoneNumber& other) const {
    return !(*this == other);
}
