#include "../qt/ContactDetailsDialog.h"
#include <QFormLayout>
#include <QTextEdit>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

ContactDetailsDialog::ContactDetailsDialog(const Contact& contact, QWidget *parent)
    : QDialog(parent)
{
    setupUi(contact);
}

void ContactDetailsDialog::setupUi(const Contact& contact)
{
    setWindowTitle("Детали контакта");
    setModal(true);
    resize(350, 200);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(8);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Информация о контакте
    auto *formLayout = new QFormLayout();

    // Имя
    auto *nameLabel = new QLabel(
        QString("%1 %2 %3")
            .arg(QString::fromStdString(contact.get_lastName()),
                 QString::fromStdString(contact.get_firstName()),
                 QString::fromStdString(contact.get_patronymic())),
        this);
    nameLabel->setStyleSheet("font-weight: bold; font-size: 12pt; margin-bottom: 8px;");
    nameLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(nameLabel);

    // Основные поля
    formLayout->setSpacing(6);
    formLayout->addRow("Email:", new QLabel(
        QString::fromStdString(contact.get_email()), this));

    if (!contact.get_birthDate().empty())
    {
        formLayout->addRow("Дата рождения:", new QLabel(
            QString::fromStdString(contact.get_birthDate()), this));
    }

    if (!contact.get_address().empty())
    {
        formLayout->addRow("Адрес:", new QLabel(
            QString::fromStdString(contact.get_address()), this));
    }

    mainLayout->addLayout(formLayout);

    // Телефоны
    list<PhoneNumber> phones = contact.get_phones();
    if (!phones.empty())
    {
        auto *phoneGroup = new QGroupBox("Телефоны", this);
        auto *phoneLayout = new QVBoxLayout(phoneGroup);
        phoneLayout->setSpacing(4);

        for (const auto& phone : phones)
        {
            auto *phoneLabel = new QLabel(
                QString("%1 (%2)")
                    .arg(QString::fromStdString(phone.get_number()),
                         QString::fromStdString(phone.type_to_string())),
                this);
            phoneLayout->addWidget(phoneLabel);
        }

        mainLayout->addWidget(phoneGroup);
    }

    // растягивающий элемент
    mainLayout->addStretch();
}