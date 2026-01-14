#include "../qt/QtPhoneBook.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QApplication>

QtPhoneBook::QtPhoneBook(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi();
    setupConnections();
    loadContacts();
}

QtPhoneBook::~QtPhoneBook()
{
    delete m_repository;
    delete m_manager;
}

void QtPhoneBook::setupUi()
{
    const auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    const auto mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    setupMenuBar();

    setupSearchPanel();

    m_contactsTable = new QTableWidget(this);
    m_contactsTable->setColumnCount(8);
    m_contactsTable->setSortingEnabled(true);  // Включаем сортировку

    QStringList headers;
    headers << "Имя" << "Фамилия" << "Отчество" << "Email"
            << "Дата рождения" << "Адрес" << "Телефоны" << "Тип";

    m_contactsTable->setHorizontalHeaderLabels(headers);
    m_contactsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_contactsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_contactsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_contactsTable->horizontalHeader()->setSectionsMovable(true);
    m_contactsTable->horizontalHeader()->setSectionsClickable(true);
    m_contactsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_contactsTable->horizontalHeader()->setStretchLastSection(true);


    m_contactsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Настройка контекстного меню
    m_contactsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_contactsTable, &QTableWidget::customContextMenuRequested,
            this, &QtPhoneBook::showContextMenu);

    // Добавляем таблицу в layout с растягиванием
    mainLayout->addWidget(m_contactsTable, 1); // stretch factor = 1

    statusBar()->showMessage("Готово");

    setWindowTitle("Phone Book - Qt");
    resize(900, 600);
}

void QtPhoneBook::setupMenuBar()
{
    // Меню "Файл"
    QMenu *fileMenu = menuBar()->addMenu("Файл");

    QAction *addAction = new QAction("Добавить контакт", this);
    addAction->setShortcut(QKeySequence::New);
    fileMenu->addAction(addAction);

    QAction *editAction = new QAction("Редактировать контакт", this);
    editAction->setShortcut(QKeySequence("Ctrl+E"));
    fileMenu->addAction(editAction);

    QAction *deleteAction = new QAction("Удалить контакт", this);
    deleteAction->setShortcut(QKeySequence::Delete);
    fileMenu->addAction(deleteAction);

    fileMenu->addSeparator();

    QAction *searchAction = new QAction("Поиск...", this);
    searchAction->setShortcut(QKeySequence::Find);
    fileMenu->addAction(searchAction);

    QAction *clearSearchAction = new QAction("Очистить поиск", this);
    clearSearchAction->setShortcut(QKeySequence("Ctrl+Shift+F"));
    fileMenu->addAction(clearSearchAction);

    fileMenu->addSeparator();

    QAction *refreshAction = new QAction("Обновить", this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    fileMenu->addAction(refreshAction);

    QAction *saveAction = new QAction("Сохранить", this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);

    QAction *resetSortAction = new QAction("Сбросить сортировку", this);
    resetSortAction->setShortcut(QKeySequence("Ctrl+R"));
    fileMenu->addAction(resetSortAction);

    fileMenu->addSeparator();

    QAction *viewDetailsAction = new QAction("Просмотреть детали", this);
    viewDetailsAction->setShortcut(QKeySequence("Ctrl+D"));
    fileMenu->addAction(viewDetailsAction);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("Выход", this);
    exitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(exitAction);

    // Меню "Справка"
    QMenu *helpMenu = menuBar()->addMenu("Справка");

    QAction *aboutAction = new QAction("О программе", this);
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction("О Qt", this);
    helpMenu->addAction(aboutQtAction);

    // Подключение сигналов
    connect(addAction, &QAction::triggered, this, &QtPhoneBook::addContact);
    connect(editAction, &QAction::triggered, this, &QtPhoneBook::editContact);
    connect(deleteAction, &QAction::triggered, this, &QtPhoneBook::deleteContact);
    connect(searchAction, &QAction::triggered, [this]() {
        m_searchLineEdit->setFocus();
        m_searchLineEdit->selectAll();
    });
    connect(clearSearchAction, &QAction::triggered, this, &QtPhoneBook::clearSearch);
    connect(saveAction, &QAction::triggered, this, &QtPhoneBook::saveContacts);
    connect(refreshAction, &QAction::triggered, this, &QtPhoneBook::refreshContacts);
    connect(resetSortAction, &QAction::triggered, this, &QtPhoneBook::resetSorting);
    connect(viewDetailsAction, &QAction::triggered, this, &QtPhoneBook::viewContactDetails);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    connect(aboutAction, &QAction::triggered, this, &QtPhoneBook::showAboutDialog);
    connect(aboutQtAction, &QAction::triggered, []() {
        QApplication::aboutQt();
    });
}

void QtPhoneBook::setupSearchPanel()
{
    const auto searchWidget = new QWidget(this);
    const auto searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 5); // Отступ снизу

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("Поиск по имени, email или телефону...");

    const auto searchButton = new QPushButton("Поиск", this);
    const auto clearButton = new QPushButton("Очистить", this);

    searchLayout->addWidget(new QLabel("Поиск:"));
    searchLayout->addWidget(m_searchLineEdit);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(clearButton);

    connect(searchButton, &QPushButton::clicked, this, &QtPhoneBook::searchContacts);
    connect(clearButton, &QPushButton::clicked, this, &QtPhoneBook::clearSearch);
    connect(m_searchLineEdit, &QLineEdit::returnPressed, this, &QtPhoneBook::searchContacts);

    // Добавляем панель поиска сверху
    centralWidget()->layout()->addWidget(searchWidget);
}

void QtPhoneBook::setupConnections()
{
    // Инициализация репозитория и менеджера
    m_repository = new FileRepository("contacts_qt.json");
    m_manager = new ContactManager(m_repository);

    // Подключение двойного клика по таблице (для редактирования)
    connect(m_contactsTable, &QTableWidget::itemDoubleClicked,
            this, &QtPhoneBook::onContactDoubleClicked);

    // Подключаем двойной клик по заголовку столбца
    connect(m_contactsTable->horizontalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &QtPhoneBook::onHeaderDoubleClicked);

    // Подключаем сигнал изменения сортировки
    connect(m_contactsTable->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &QtPhoneBook::onSortIndicatorChanged);
}

void QtPhoneBook::loadContacts()
{
    vector<Contact> contacts = m_manager->getAllContacts();
    displayContacts(contacts);

    statusBar()->showMessage(QString("Загружено %1 контактов").arg(contacts.size()));
}

void QtPhoneBook::displayContacts(const vector<Contact>& contacts) const {
    // Блокируем сортировку на время обновления данных
    m_contactsTable->setSortingEnabled(false);

    m_contactsTable->setRowCount(contacts.size());

    for (size_t i = 0; i < contacts.size(); ++i)
    {
        const Contact& contact = contacts[i];

        // Основные поля
        m_contactsTable->setItem(i, 0, new QTableWidgetItem(
            QString::fromStdString(contact.get_firstName())));
        m_contactsTable->setItem(i, 1, new QTableWidgetItem(
            QString::fromStdString(contact.get_lastName())));
        m_contactsTable->setItem(i, 2, new QTableWidgetItem(
            QString::fromStdString(contact.get_patronymic())));
        m_contactsTable->setItem(i, 3, new QTableWidgetItem(
            QString::fromStdString(contact.get_email())));

        // Для даты рождения создаем специальный элемент для правильной сортировки
        QString birthDateStr = QString::fromStdString(contact.get_birthDate());
        QTableWidgetItem* birthDateItem = new QTableWidgetItem(birthDateStr);

        // Преобразуем дату в QDate для сортировки
        if (!birthDateStr.isEmpty()) {
            QDate date = QDate::fromString(birthDateStr, "dd.MM.yyyy");
            if (date.isValid()) {
                // Сохраняем дату в UserRole для правильной сортировки
                birthDateItem->setData(Qt::UserRole, date);
            }
        }
        m_contactsTable->setItem(i, 4, birthDateItem);

        m_contactsTable->setItem(i, 5, new QTableWidgetItem(
            QString::fromStdString(contact.get_address())));

        // Телефоны
        list<PhoneNumber> phones = contact.get_phones();
        QString phonesStr;
        QString typesStr;

        for (const auto& phone : phones)
        {
            phonesStr += QString::fromStdString(phone.get_number()) + "\n";
            typesStr += QString::fromStdString(phone.type_to_string()) + "\n";
        }

        m_contactsTable->setItem(i, 6, new QTableWidgetItem(phonesStr));
        m_contactsTable->setItem(i, 7, new QTableWidgetItem(typesStr));
    }

    // Включаем сортировку обратно
    m_contactsTable->setSortingEnabled(true);

    // Автоподгонка ширины столбцов при первой загрузке
    m_contactsTable->resizeColumnsToContents();

    // Устанавливаем минимальные ширины
    m_contactsTable->horizontalHeader()->setMinimumSectionSize(80);

    // Устанавливаем начальные ширины для важных столбцов
    m_contactsTable->setColumnWidth(0, 120); // Имя
    m_contactsTable->setColumnWidth(1, 120); // Фамилия
    m_contactsTable->setColumnWidth(3, 180); // Email

    // Восстанавливаем растягивание последнего столбца
    m_contactsTable->horizontalHeader()->setStretchLastSection(true);
}

void QtPhoneBook::resetSorting()
{
    // Отключаем сортировку
    m_contactsTable->setSortingEnabled(false);

    // Сбрасываем индикатор в заголовке
    m_contactsTable->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

    // Получаем контакты в исходном порядке (из файла/базы)
    vector<Contact> contacts = m_manager->getAllContacts();

    // Отображаем их
    displayContacts(contacts);

    // Включаем сортировку обратно
    m_contactsTable->setSortingEnabled(true);

    statusBar()->showMessage("Сортировка сброшена");
}

void QtPhoneBook::onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order)
{
    QString columnName;
    QString sortOrder = (order == Qt::AscendingOrder) ? "по возрастанию" : "по убыванию";

    switch(logicalIndex) {
        case 0: columnName = "Имя"; break;
        case 1: columnName = "Фамилия"; break;
        case 2: columnName = "Отчество"; break;
        case 3: columnName = "Email"; break;
        case 4: columnName = "Дата рождения"; break;
        case 5: columnName = "Адрес"; break;
        case 6: columnName = "Телефоны"; break;
        case 7: columnName = "Тип телефонов"; break;
        default: columnName = "Неизвестный столбец"; break;
    }

    if (logicalIndex >= 0) {
        statusBar()->showMessage(QString("Сортировка по столбцу '%1' (%2)").arg(columnName).arg(sortOrder));
    }
}

void QtPhoneBook::addContact()
{
    ContactDialog dialog(this);
    dialog.setWindowTitle("Добавить контакт");

    if (dialog.exec() == QDialog::Accepted)
    {
        Contact contact = dialog.getContact();

        if (m_manager->addContact(contact))
        {
            loadContacts();
            QMessageBox::information(this, "Успех", "Контакт успешно добавлен");
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось добавить контакт. Возможно, email уже существует.");
        }
    }
}

void QtPhoneBook::editContact()
{
    int row = m_contactsTable->currentRow();
    if (row < 0)
    {
        QMessageBox::warning(this, "Предупреждение", "Выберите контакт для редактирования");
        return;
    }

    QString email = m_contactsTable->item(row, 3)->text();
    Contact oldContact = m_manager->getContact(email.toStdString());

    ContactDialog dialog(this);
    dialog.setContact(oldContact);
    dialog.setWindowTitle("Редактировать контакт");

    if (dialog.exec() == QDialog::Accepted)
    {
        Contact newContact = dialog.getContact();

        if (m_manager->updateContact(email.toStdString(), newContact))
        {
            loadContacts();
            QMessageBox::information(this, "Успех", "Контакт успешно обновлен");
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось обновить контакт");
        }
    }
}

void QtPhoneBook::deleteContact()
{
    int row = m_contactsTable->currentRow();
    if (row < 0)
    {
        QMessageBox::warning(this, "Предупреждение", "Выберите контакт для удаления");
        return;
    }

    QString email = m_contactsTable->item(row, 3)->text();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Подтверждение",
                                  "Вы уверены, что хотите удалить этот контакт?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        if (m_manager->removeContact(email.toStdString()))
        {
            loadContacts();
            QMessageBox::information(this, "Успех", "Контакт успешно удален");
        }
        else
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось удалить контакт");
        }
    }
}

void QtPhoneBook::searchContacts()
{
    QString query = m_searchLineEdit->text().trimmed();

    if (query.isEmpty())
    {
        loadContacts();
        return;
    }

    // Блокируем сортировку на время поиска
    m_contactsTable->setSortingEnabled(false);

    // Ищем по имени
    vector<Contact> results = m_manager->searchByName(query.toStdString());

    // Если не нашли по имени, ищем по email и телефону
    if (results.empty())
    {
        vector<Contact> allContacts = m_manager->getAllContacts();

        for (const auto& contact : allContacts)
        {
            // Поиск по email
            if (contact.get_email().find(query.toStdString()) != string::npos)
            {
                results.push_back(contact);
                continue;
            }

            // Поиск по телефону
            list<PhoneNumber> phones = contact.get_phones();
            for (const auto& phone : phones)
            {
                if (phone.get_number().find(query.toStdString()) != string::npos)
                {
                    results.push_back(contact);
                    break;
                }
            }
        }
    }

    displayContacts(results);

    // Включаем сортировку обратно
    m_contactsTable->setSortingEnabled(true);

    statusBar()->showMessage(QString("Найдено %1 контактов").arg(results.size()));
}

void QtPhoneBook::clearSearch()
{
    m_searchLineEdit->clear();
    loadContacts();
}

void QtPhoneBook::refreshContacts()
{
    loadContacts();
}

void QtPhoneBook::saveContacts()
{
    // Контакты автоматически сохраняются при изменении
    QMessageBox::information(this, "Информация", "Контакты сохраняются автоматически при каждом изменении");
}

void QtPhoneBook::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);

    QAction *addAction = contextMenu.addAction("Добавить");
    QAction *editAction = contextMenu.addAction("Редактировать");
    QAction *deleteAction = contextMenu.addAction("Удалить");
    contextMenu.addSeparator();
    QAction *viewAction = contextMenu.addAction("Просмотреть детали");

    connect(addAction, &QAction::triggered, this, &QtPhoneBook::addContact);
    connect(editAction, &QAction::triggered, this, &QtPhoneBook::editContact);
    connect(deleteAction, &QAction::triggered, this, &QtPhoneBook::deleteContact);
    connect(viewAction, &QAction::triggered, this, &QtPhoneBook::viewContactDetails);

    contextMenu.exec(m_contactsTable->viewport()->mapToGlobal(pos));
}

void QtPhoneBook::viewContactDetails()
{
    int row = m_contactsTable->currentRow();
    if (row < 0) return;

    QString email = m_contactsTable->item(row, 3)->text();
    Contact contact = m_manager->getContact(email.toStdString());

    ContactDetailsDialog dialog(contact, this);
    dialog.exec();
}

void QtPhoneBook::onContactDoubleClicked(QTableWidgetItem *item)
{
    Q_UNUSED(item);
    editContact();
}

void QtPhoneBook::onHeaderDoubleClicked(int logicalIndex) const {
    // При двойном клике по заголовку растягиваем этот столбец по содержимому
    m_contactsTable->resizeColumnToContents(logicalIndex);

    // Но сохраняем растягивание последнего столбца
    if (logicalIndex < m_contactsTable->columnCount() - 1) {
        m_contactsTable->horizontalHeader()->setStretchLastSection(true);
    }
}

void QtPhoneBook::showAboutDialog()
{
    QMessageBox::about(this, "О программе Phone Book",
        "<h2>Phone Book</h2>"
        "<p>Версия: 1.0.0</p>"
        "<p>Программа для управления контактами с графическим интерфейсом на Qt.</p>"
        "<p>Основные возможности:</p>"
        "<ul>"
        "<li>Добавление, редактирование и удаление контактов</li>"
        "<li>Хранение множества телефонов для каждого контакта</li>"
        "<li>Поиск по имени, email или телефону</li>"
        "<li>Сортировка по всем столбцам</li>"
        "<li>Валидация вводимых данных</li>"
        "<li>Автоматическое сохранение в файл</li>"
        "</ul>"
        "<p>Разработано на C++ с использованием Qt framework.</p>");
}