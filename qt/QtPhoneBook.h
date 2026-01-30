#ifndef QTPHONEBOOK_H
#define QTPHONEBOOK_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QDate>
#include <QActionGroup>

#include "../include/ContactManager.h"
#include "../include/Contact.h"
#include "../include/PhoneNumber.h"
#include "../qt/ContactDialog.h"
#include "../qt/ContactDetailsDialog.h"
#include "../include/FileRepository.h"
#include "../database/PostgreSQLRepository.h"

class QtPhoneBook : public QMainWindow
{
    Q_OBJECT

public:
    QtPhoneBook(QWidget *parent = nullptr);
    ~QtPhoneBook();

private slots:
    void addContact();
    void editContact();
    void deleteContact();
    void searchContacts();
    void clearSearch();
    void refreshContacts();
    void saveContacts();
    void viewContactDetails();
    void onContactDoubleClicked(QTableWidgetItem *item);
    void showContextMenu(const QPoint &pos);
    void onHeaderDoubleClicked(int logicalIndex) const;
    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);
    void resetSorting();
    void showAboutDialog();
    void switchToFileStorage();
    void switchToDatabaseStorage();
    void showStorageSettings();

private:
    void setupUi();
    void setupMenuBar();
    void setupSearchPanel();
    void setupConnections();
    void loadContacts();
    void displayContacts(const std::vector<Contact>& contacts) const;
    void switchStorage(IContactRepository* newRepository);
    void updateStorageStatus();

private:
    ContactManager* m_manager{};
    FileRepository* m_fileRepository{};
    PostgreSQLRepository* m_dbRepository{};
    IContactRepository* m_currentRepository{};
    
    bool m_useDatabaseStorage{false};

    QTableWidget* m_contactsTable{};
    QLineEdit* m_searchLineEdit{};
    QAction* m_useFileAction{};
    QAction* m_useDatabaseAction{};
};
#endif