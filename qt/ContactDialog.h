#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QListWidget>
#include <QVBoxLayout>


#include "../include/Contact.h"
#include "../include/PhoneNumber.h"

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = nullptr);

    void setContact(const Contact& contact) const;
    Contact getContact() const;

private slots:
    void addPhone();
    void removePhone();
    void accept() override;

private:
    void setupUi();
    void setupMainFields(QVBoxLayout *mainLayout);
    void setupPhoneGroup(QVBoxLayout *mainLayout);
    void setupButtons(QVBoxLayout *mainLayout);
    void setupConnections();

private:
    QLineEdit* m_firstNameEdit;
    QLineEdit* m_lastNameEdit;
    QLineEdit* m_patronymicEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_birthDateEdit;
    QLineEdit* m_addressEdit;

    QLineEdit* m_phoneNumberEdit;
    QComboBox* m_phoneTypeCombo;
    QListWidget* m_phonesList;
};

#endif