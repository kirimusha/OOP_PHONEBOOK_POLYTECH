#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <cctype>

using namespace std;

class Validators {
public:
    // вспомогательные функции

    // удаляем пробелы по краям
    static string trim(const string& str) {
        const size_t start = str.find_first_not_of(" \t\n\r");
        if (start == string::npos) return ""; // string::npos значит не найдено

        const size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }

    static string remove_spaces(const string& s) {
        string out;
        for (const char c : s) {
            if (!isspace(static_cast<unsigned char>(c)))
                out += c;
        }
        return out;
    }

    /*
    Варианты записи телефона:
        +78121234567
        88121234567
        +7(812)1234567
        8(812)1234567
        +7(812)123-45-67
        8(812)123-45-67
     */
    static bool validate_phone(const string& phone);
    static bool validate_name(const string& name);
    static bool validate_email(const string& newEmail, const string& username);

    static bool validate_birthDate(const string& birthDate);
    static int days_in_month(int month, int year);
    static bool is_past_date(int day, int month, int year);
};

#endif
