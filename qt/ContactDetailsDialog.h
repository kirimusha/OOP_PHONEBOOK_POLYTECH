#ifndef CONTACTDETAILSDIALOG_H
#define CONTACTDETAILSDIALOG_H

#include <QDialog>
#include "../include/Contact.h"

class ContactDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    // Невозможно неявное преобразование из Contact в ContactDetailsDialog
    explicit ContactDetailsDialog(const Contact& contact, QWidget *parent = nullptr);

private:
    void setupUi(const Contact& contact);
};

#endif
