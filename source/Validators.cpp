#include "Validators.h"

using namespace std;

// имя, фамилия, отчество
bool Validators::validate_name(const string& name) {
    const string name_trimmed = trim(name);
    if (name_trimmed.empty()) return false;

    static const regex name_regex(R"(^[a-zA-Z][a-zA-Z0-9\s-]*[a-zA-Z0-9]$)");

    return regex_match(name_trimmed, name_regex);
}

bool Validators::validate_email(const string& newEmail, const string& username) {
    string email = remove_spaces(trim(newEmail));

    if (email.empty() || username.empty()) return false;

    // находим где @ находится
    const size_t at_pos = email.find('@');

    // берем часть до @
    const string email_username = email.substr(0, at_pos);

    if (email_username.find(username) == string::npos) { // string::npos значит не найдено
        return false;
    }
    
    static const regex email_regex(
        R"(^[a-zA-Z0-9._]+@[a-zA-Z0-9._]+\.[a-zA-Z]{2,}$)"
    );

    return regex_match(email, email_regex);
}

bool Validators::validate_phone(const string& phone) {
    const string s = trim(phone);
    if (s.empty()) return false;

    static const regex phone_regex(
        R"(^(\+7|8)(?:\(\d{3}\)|\d{3})(?:\d{3}(?:\d{2}){2}|\d{3}-\d{2}-\d{2}|\d{7})$)"
    );

    return regex_match(s, phone_regex);
}

bool Validators::validate_birthDate(const string& birthDate) {
    static const regex date_regex(R"(^(\d{2})\.(\d{2})\.(\d{4})$)");

    if (birthDate.empty()) return true;

    smatch match;
    if (!regex_match(birthDate, match, date_regex)) {
        return false;
    }

    // компоненты даты из групп регулярного выражения
    const int day = stoi(match[1].str()); // stoi() - string to integer
    const int month = stoi(match[2].str());
    const int year = stoi(match[3].str());

    if (month < 1 || month > 12) {
        return false;
    }

    if (day < 1 || day > days_in_month(month, year)) {
        return false;
    }

    if (!is_past_date(day, month, year)) {
        return false;
    }
    
    return true;
}

// с учетом високосного года
int Validators::days_in_month(int month, int year) {
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                return 29; // високосный
            } else {
                return 28; // не високосный
            }
        default:
            return 0;
    }
}

// проверка, что дата в прошлом, не в будущем
bool Validators::is_past_date(int day, int month, int year) {
    const time_t t = time(nullptr);
    const tm* now = localtime(&t);
    const int currentYear = now->tm_year + 1900;
    const int currentMonth = now->tm_mon + 1;
    const int currentDay = now->tm_mday;
    
    if (year > currentYear) return false;
    if (year == currentYear && month > currentMonth) return false;
    if (year == currentYear && month == currentMonth && day > currentDay) return false;
    
    return true;
}
