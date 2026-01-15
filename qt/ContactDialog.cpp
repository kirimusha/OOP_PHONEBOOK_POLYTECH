#include "../qt/ContactDialog.h"
#include <QFormLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>

ContactDialog::ContactDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setupConnections();
}

void ContactDialog::setupUi()
{
    setWindowTitle("Контакт");
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    setupMainFields(mainLayout);
    setupPhoneGroup(mainLayout);
    setupButtons(mainLayout);

    m_phoneTypeCombo->setCurrentIndex(0);
}

void ContactDialog::setupMainFields(QVBoxLayout *mainLayout)
{
    QFormLayout *formLayout = new QFormLayout();

    m_firstNameEdit = new QLineEdit(this);
    m_lastNameEdit = new QLineEdit(this);
    m_patronymicEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_birthDateEdit = new QLineEdit(this);
    m_addressEdit = new QLineEdit(this);

    // Настройка подсказок
    m_birthDateEdit->setPlaceholderText("ДД.ММ.ГГГГ");
    m_emailEdit->setPlaceholderText("Должен содержать имя");

    formLayout->addRow("Имя *:", m_firstNameEdit);
    formLayout->addRow("Фамилия *:", m_lastNameEdit);
    formLayout->addRow("Отчество:", m_patronymicEdit);
    formLayout->addRow("Email *:", m_emailEdit);
    formLayout->addRow("Дата рождения:", m_birthDateEdit);
    formLayout->addRow("Адрес:", m_addressEdit);

    mainLayout->addLayout(formLayout);
}

void ContactDialog::setupPhoneGroup(QVBoxLayout *mainLayout)
{
    auto *phoneGroup = new QGroupBox("Телефоны *", this);
    const auto phoneLayout = new QVBoxLayout(phoneGroup);

    // Панель добавления телефона
    const auto addPhoneLayout = new QHBoxLayout();

    m_phoneNumberEdit = new QLineEdit(this);
    m_phoneTypeCombo = new QComboBox(this);
    m_phoneTypeCombo->addItems({"Мобильный", "Домашний", "Рабочий"});

    auto *addPhoneButton = new QPushButton("Добавить", this);

    addPhoneLayout->addWidget(new QLabel("Номер:"));
    addPhoneLayout->addWidget(m_phoneNumberEdit);
    addPhoneLayout->addWidget(new QLabel("Тип:"));
    addPhoneLayout->addWidget(m_phoneTypeCombo);
    addPhoneLayout->addWidget(addPhoneButton);

    phoneLayout->addLayout(addPhoneLayout);

    // Список телефонов
    m_phonesList = new QListWidget(this);
    phoneLayout->addWidget(m_phonesList);

    // Кнопки управления телефонами
    auto *phoneButtonsLayout = new QHBoxLayout();
    auto *removePhoneButton = new QPushButton("Удалить телефон", this);
    auto *clearPhonesButton = new QPushButton("Очистить все", this);

    phoneButtonsLayout->addWidget(removePhoneButton);
    phoneButtonsLayout->addWidget(clearPhonesButton);
    phoneButtonsLayout->addStretch();

    phoneLayout->addLayout(phoneButtonsLayout);

    mainLayout->addWidget(phoneGroup);

    // Подключение сигналов
    connect(addPhoneButton, &QPushButton::clicked, this, &ContactDialog::addPhone);
    connect(removePhoneButton, &QPushButton::clicked, this, &ContactDialog::removePhone);
    connect(clearPhonesButton, &QPushButton::clicked, m_phonesList, &QListWidget::clear);
}

void ContactDialog::setupButtons(QVBoxLayout *mainLayout)
{
    auto *buttonLayout = new QHBoxLayout();

    auto *okButton = new QPushButton("OK", this);
    auto *cancelButton = new QPushButton("Отмена", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &ContactDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &ContactDialog::reject);
}

void ContactDialog::setupConnections()
{
    // Валидация при изменении имени
    connect(m_firstNameEdit, &QLineEdit::textChanged, this, [this]() {
        const QString firstName = m_firstNameEdit->text();

        if (const QString email = m_emailEdit->text(); !email.isEmpty() && !firstName.isEmpty())
        {
            // Проверяем, содержит ли email имя
            if (!email.contains(firstName, Qt::CaseInsensitive))
            {
                m_emailEdit->setStyleSheet("QLineEdit { background-color: #FFCCCC; }");
                m_emailEdit->setToolTip("Email должен содержать имя контакта");
            }
            else
            {
                m_emailEdit->setStyleSheet("");
                m_emailEdit->setToolTip("");
            }
        }
    });
}

void ContactDialog::setContact(const Contact& contact) const
{
    m_firstNameEdit->setText(QString::fromStdString(contact.get_firstName()));
    m_lastNameEdit->setText(QString::fromStdString(contact.get_lastName()));
    m_patronymicEdit->setText(QString::fromStdString(contact.get_patronymic()));
    m_emailEdit->setText(QString::fromStdString(contact.get_email()));
    m_birthDateEdit->setText(QString::fromStdString(contact.get_birthDate()));
    m_addressEdit->setText(QString::fromStdString(contact.get_address()));

    // Заполняем телефоны
    m_phonesList->clear();
    list<PhoneNumber> phones = contact.get_phones();
    for (const auto& phone : phones)
    {
        // Преобразуем тип телефона в русское название для отображения
        QString typeStr;
        switch(phone.get_type()) {
            case PhoneType::Mobile: typeStr = "Мобильный"; break;
            case PhoneType::Home: typeStr = "Домашний"; break;
            case PhoneType::Work: typeStr = "Рабочий"; break;
            default: typeStr = "Мобильный";
        }

        QString itemText = QString("%1 (%2)")
            .arg(QString::fromStdString(phone.get_number()))
            .arg(typeStr);
        m_phonesList->addItem(itemText);
    }
}

Contact ContactDialog::getContact() const
{
    // Создаем список телефонов
    list<PhoneNumber> phones;
    for (int i = 0; i < m_phonesList->count(); ++i)
    {
        QString itemText = m_phonesList->item(i)->text();

        // Парсим строку "номер (тип)"
        QStringList parts = itemText.split(" (");
        if (parts.size() >= 2)
        {
            QString number = parts[0].trimmed();
            QString typeStr = parts[1].trimmed().remove(")");

            // Сопоставляем русские названия с enum PhoneType
            PhoneType type = PhoneType::Mobile; // по умолчанию

            if (typeStr == "Мобильный") {
                type = PhoneType::Mobile;
            } else if (typeStr == "Домашний") {
                type = PhoneType::Home;
            } else if (typeStr == "Рабочий") {
                type = PhoneType::Work;
            }

            phones.push_back(PhoneNumber(number.toStdString(), type));
        }
    }

    // Создаем контакт
    Contact contact(
        m_firstNameEdit->text().toStdString(),
        m_lastNameEdit->text().toStdString(),
        m_patronymicEdit->text().toStdString(),
        m_addressEdit->text().toStdString(),
        m_birthDateEdit->text().toStdString(),
        m_emailEdit->text().toStdString(),
        phones
    );

    return contact;
}

void ContactDialog::addPhone()
{
    const QString number = m_phoneNumberEdit->text().trimmed();
    if (number.isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите номер телефона");
        return;
    }

    // Создаем временный PhoneNumber для валидации
    const auto type = static_cast<PhoneType>(m_phoneTypeCombo->currentIndex());

    if (const PhoneNumber phone(number.toStdString(), type); !phone.is_valid())
    {
        QMessageBox::warning(this, "Ошибка",
            "Неверный формат номера телефона\n"
            "Формат: +7XXXXXXXXXX или 8XXXXXXXXXX");
        return;
    }

    // Добавляем в список
    const QString typeStr = m_phoneTypeCombo->currentText();
    QString itemText = QString("%1 (%2)").arg(number).arg(typeStr);
    m_phonesList->addItem(itemText);

    // Очищаем поле ввода
    m_phoneNumberEdit->clear();
}

void ContactDialog::removePhone()
{
    int row = m_phonesList->currentRow();
    if (row >= 0)
    {
        delete m_phonesList->takeItem(row);
    }
}

void ContactDialog::accept()
{
    // Валидация обязательных полей
    if (m_firstNameEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Имя обязательно для заполнения");
        return;
    }

    if (m_lastNameEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Фамилия обязательна для заполнения");
        return;
    }

    if (m_emailEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Email обязателен для заполнения");
        return;
    }

    // Проверка, что email содержит имя
    QString firstName = m_firstNameEdit->text().toLower();
    QString email = m_emailEdit->text().toLower();
    QString emailUser = email.split('@').first();

    if (!emailUser.contains(firstName))
    {
        QMessageBox::warning(this, "Ошибка",
            "Email должен содержать имя контакта\n"
            "Пример: если имя 'ivan', то email может быть 'ivan@mail.com'");
        return;
    }

    // Проверка даты рождения
    QString birthDate = m_birthDateEdit->text().trimmed();
    if (!birthDate.isEmpty())
    {
        // Создаем временный контакт для валидации
        Contact temp;
        if (!temp.set_birthDate(birthDate.toStdString()))
        {
            QMessageBox::warning(this, "Ошибка",
                "Неверный формат даты рождения\n"
                "Используйте формат ДД.ММ.ГГГГ");
            return;
        }
    }

    // Проверка наличия хотя бы одного телефона
    if (m_phonesList->count() == 0)
    {
        QMessageBox::warning(this, "Ошибка", "Необходимо добавить хотя бы один телефон");
        return;
    }

    QDialog::accept();
}