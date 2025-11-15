#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <string>
#include <regex>
#include <ctime>

using namespace std;

class Validators {
public:
    // Передаем строки по ссылке, чтобы не копировать

    /**
    Проверка имени (ФИО)
    
    Допускаются:
    - буквы любых алфавитов
    - пробелы
    - дефис (не в начале и не в конце)

     */
    static bool validate_name(const string& name);    
    /**
    Проверка телефона
    
    Варианты записи телефона:
        +78121234567
        88121234567
        +7(812)1234567
        8(812)1234567
        +7(812)123-45-67
        8(812)123-45-67

     */
    static bool validate_phone(const string& phone);
    static bool validate_email(const string& newEmail);

    static bool validate_birthDate(const string& birthDate);
    static bool parse_date(const string& date_str, int& year, int& month, int& day);
    static int days_in_month(int month, int year);
    static bool is_past_date(int year, int month, int day);
};

#endif // VALIDATORS_H
