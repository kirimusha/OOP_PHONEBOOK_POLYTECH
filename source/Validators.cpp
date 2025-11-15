#include "Validators.h"
#include <iostream>
#include <string>
#include <cctype>
#include <ctime>
#include <vector>
#include <locale>
#include <codecvt>
#include <regex>

using namespace std;
using std::isspace;

// Вспомогательная функция для удаления пробелов по краям
static string trim(const string& str) {
    ::std::size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    
    ::std::size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

// Удаление всех пробелов
static string remove_spaces(const string& s) {
    string out;
    for (char c : s) {
        if (!std::isspace(static_cast<unsigned char>(c)))
            out += c;
    }
    return out;
}

// Проверка буквы
static bool isLetter(unsigned char c) {
    // латиница
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return true;
    // кириллица (базовый диапазон)
    if (c >= 0xC0 && c <= 0xFF) return true; // кириллица в UTF-8
    return false;
}

// Проверка цифры
static bool isDigit(unsigned char c) {
    return c >= '0' && c <= '9';
}

// Проверка буквенно-цифрового символа (аналог isalnum для Unicode)
static bool isAlnum(char32_t c) {
    return isLetter(c) || isDigit(c);
}

// Имя / Фамилия / Отчество
bool Validators::validate_name(const string& name) {
    // Удаляем незначимые пробелы перед и после
    string name_trimmed = trim(name);
    if (name_trimmed.empty()) return false;

    // Не может начинаться или заканчиваться на дефис
    if (name_trimmed.front() == '-' || name_trimmed.back() == '-')
        return false;

    // Должен начинаться на букву
    if (!isLetter(name_trimmed.front()))
        return false;

    // Проверяем все символы
    for (::std::size_t i = 0; i < name_trimmed.size(); i++) {
        unsigned char c = name_trimmed[i];
        
        // Допустимы: буквы, цифры, дефис, пробел
        if (!(isLetter(c) || isDigit(c) || c == '-' || c == ' '))
            return false;
    }

    return true;
}

// Email
bool Validators::validate_email(const string& newEmail) {
    string email = remove_spaces(trim(newEmail));
    if (email.empty()) return false;
    
    static const regex email_regex(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );

    return regex_match(email, email_regex);
}

// Телефон - ИСПРАВЛЕННАЯ ВЕРСИЯ
bool Validators::validate_phone(const string& phone) {
    string s = trim(phone);
    if (s.empty()) return false;

    string cleaned;
    bool had_plus = false;
    
    // Сохраняем информацию о наличии +
    if (!s.empty() && s[0] == '+') {
        had_plus = true;
    }
    
    // Очищаем от нецифровых символов (кроме + который уже учли)
    for (char c : s) {
        if (c == ' ' || c == '-' || c == '(' || c == ')' || c == '+') continue;
        cleaned += c;
    }

    if (cleaned.empty()) return false;

    // Проверяем что все символы - цифры
    for (char c : cleaned) {
        if (!isDigit(c)) return false;
    }

    // Проверяем длину
    if (cleaned.size() != 11) return false;
    
    // Ключевое исправление: проверяем первый символ с учетом +
    char first_digit = cleaned[0];
    
    if (had_plus) {
        // Если был +, то первый символ должен быть 7
        if (first_digit != '7') return false;
    } else {
        // Если не было +, то первый символ может быть 8
        if (first_digit != '8') return false;
    }

    return true;
}

// Дата рождения
bool Validators::validate_birthDate(const string& birthDate) {
    if (birthDate.empty()) return false;
    
    // Парсим дату из строки (ожидаем формат YYYY-MM-DD)
    int day, month, year;
    if (!parse_date(birthDate, day, month, year)) {
        return false;
    }
    
    // Проверяем корректность месяца
    if (month < 1 || month > 12) {
        return false;
    }

    // Проверяем корректность дня для данного месяца и года
    if (day < 1 || day > days_in_month(month, year)) {
        return false;
    }
    
    // Проверяем что дата меньше текущей
    if (!is_past_date(day, month, year)) {
        return false;
    }
    
    return true;
}

// Парсинг даты из строки (формат DD.MM.YYYY)
bool Validators::parse_date(const string& date_str, int& day, int& month, int& year) {
    // Проверяем базовый формат
    if (date_str.length() != 10 || date_str[2] != '.' || date_str[5] != '.') {
        return false;
    }
    
    try {
        // Парсинг дня (первые 2 символа)
        day = stoi(date_str.substr(0, 2));    // "02.03.2006" -> "02" -> 2
        
        // Парсинг месяца (символы 3-4)
        month = stoi(date_str.substr(3, 2));  // "02.03.2006" -> "03" -> 3
        
        // Парсинг года (символы 6-9)  
        year = stoi(date_str.substr(6, 4));   // "02.03.2006" -> "2006" -> 2006
        return true;
    } catch (...) {
        return false;
    }
}

// Количество дней в месяце с учетом високосного года
int Validators::days_in_month(int month, int year) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            // Проверка високосного года внутри функции
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                return 29; // високосный
            } else {
                return 28; // не високосный
            }
        default:
            return 0;
    }
}

// Проверка что дата в прошлом (меньше текущей)
bool Validators::is_past_date(int year, int month, int day) {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    
    if (year > currentYear) return false;
    if (year == currentYear && month > currentMonth) return false;
    if (year == currentYear && month == currentMonth && day > currentDay) return false;
    
    return true;
}
