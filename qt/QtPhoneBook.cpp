#include "../qt/QtPhoneBook.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QApplication>
#include <QActionGroup>
#include <QInputDialog>
#include <QRadioButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QRadioButton>

QtPhoneBook::QtPhoneBook(QWidget *parent)
    : QMainWindow(parent)
    , m_manager(nullptr)
    , m_fileRepository(nullptr)
    , m_dbRepository(nullptr)
    , m_currentRepository(nullptr)
    , m_contactsTable(nullptr)
    , m_searchLineEdit(nullptr)
    , m_useFileAction(nullptr)
    , m_useDatabaseAction(nullptr)
    , m_useDatabaseStorage(false)
{
    setupUi();
    setupConnections();
}

QtPhoneBook::~QtPhoneBook()
{
    delete m_manager;
    delete m_fileRepository;
    delete m_dbRepository;
    // m_currentRepository - это указатель на один из вышеудаленных
}

void QtPhoneBook::setupUi()
{
    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    setupMenuBar();
    setupSearchPanel();

    m_contactsTable = new QTableWidget(this);
    m_contactsTable->setColumnCount(8);
    m_contactsTable->setSortingEnabled(true);

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

    mainLayout->addWidget(m_contactsTable, 1);

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

    // Меню "Хранилище"
    QMenu *storageMenu = menuBar()->addMenu("Хранилище");

    m_useFileAction = new QAction("Использовать файл", this);
    m_useFileAction->setCheckable(true);
    m_useFileAction->setChecked(true);

    m_useDatabaseAction = new QAction("Использовать базу данных", this);
    m_useDatabaseAction->setCheckable(true);

    QActionGroup *storageGroup = new QActionGroup(this);
    storageGroup->addAction(m_useFileAction);
    storageGroup->addAction(m_useDatabaseAction);

    storageMenu->addAction(m_useFileAction);
    storageMenu->addAction(m_useDatabaseAction);
    storageMenu->addSeparator();

    QAction *settingsAction = new QAction("Настройки хранилища...", this);
    storageMenu->addAction(settingsAction);

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
    
    // Подключение новых сигналов для хранилища
    connect(m_useFileAction, &QAction::triggered, this, &QtPhoneBook::switchToFileStorage);
    connect(m_useDatabaseAction, &QAction::triggered, this, &QtPhoneBook::switchToDatabaseStorage);
    connect(settingsAction, &QAction::triggered, this, &QtPhoneBook::showStorageSettings);
    
    connect(aboutAction, &QAction::triggered, this, &QtPhoneBook::showAboutDialog);
    connect(aboutQtAction, &QAction::triggered, []() {
        QApplication::aboutQt();
    });
}

void QtPhoneBook::setupSearchPanel()
{
    auto searchWidget = new QWidget(this);
    auto searchLayout = new QHBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 5);

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("Поиск по имени, email или телефону...");

    auto searchButton = new QPushButton("Поиск", this);
    auto clearButton = new QPushButton("Очистить", this);

    searchLayout->addWidget(new QLabel("Поиск:"));
    searchLayout->addWidget(m_searchLineEdit);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(clearButton);

    connect(searchButton, &QPushButton::clicked, this, &QtPhoneBook::searchContacts);
    connect(clearButton, &QPushButton::clicked, this, &QtPhoneBook::clearSearch);
    connect(m_searchLineEdit, &QLineEdit::returnPressed, this, &QtPhoneBook::searchContacts);

    centralWidget()->layout()->addWidget(searchWidget);
}

void QtPhoneBook::setupConnections()
{
    // Инициализация репозиториев по умолчанию с файлом
    m_fileRepository = new FileRepository("contacts_qt.json");
    m_dbRepository = nullptr;
    m_currentRepository = m_fileRepository;
    m_useDatabaseStorage = false;
    
    m_manager = new ContactManager(m_currentRepository);

    // Подключение двойного клика по таблице
    connect(m_contactsTable, &QTableWidget::itemDoubleClicked,
            this, &QtPhoneBook::onContactDoubleClicked);

    // Подключаем двойной клик по заголовку столбца
    connect(m_contactsTable->horizontalHeader(), &QHeaderView::sectionDoubleClicked,
            this, &QtPhoneBook::onHeaderDoubleClicked);

    // Подключаем сигнал изменения сортировки
    connect(m_contactsTable->horizontalHeader(), &QHeaderView::sortIndicatorChanged,
            this, &QtPhoneBook::onSortIndicatorChanged);

    loadContacts();
    updateStorageStatus();
}

void QtPhoneBook::loadContacts()
{
    if (!m_manager) return;
    
    vector<Contact> contacts = m_manager->getAllContacts();
    displayContacts(contacts);

    statusBar()->showMessage(QString("Загружено %1 контактов").arg(contacts.size()));
}

void QtPhoneBook::displayContacts(const vector<Contact>& contacts) const {
    if (!m_contactsTable) return;
    
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
    if (m_contactsTable->rowCount() > 0) {
        m_contactsTable->resizeColumnsToContents();
    }

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
    if (!m_contactsTable) return;
    
    // Отключаем сортировку
    m_contactsTable->setSortingEnabled(false);

    // Сбрасываем индикатор в заголовке
    m_contactsTable->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

    // Получаем контакты в исходном порядке
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
    if (!m_contactsTable) return;
    
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
    if (!m_contactsTable) return;
    
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
    if (!m_searchLineEdit || !m_manager) return;
    
    QString query = m_searchLineEdit->text().trimmed();

    if (query.isEmpty())
    {
        loadContacts();
        return;
    }

    // Блокируем сортировку на время поиска
    if (m_contactsTable) {
        m_contactsTable->setSortingEnabled(false);
    }

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
    if (m_contactsTable) {
        m_contactsTable->setSortingEnabled(true);
    }

    statusBar()->showMessage(QString("Найдено %1 контактов").arg(results.size()));
}

void QtPhoneBook::clearSearch()
{
    if (m_searchLineEdit) {
        m_searchLineEdit->clear();
    }
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
    if (!m_contactsTable || !m_manager) return;
    
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
    if (m_contactsTable) {
        // При двойном клике по заголовку растягиваем этот столбец по содержимому
        m_contactsTable->resizeColumnToContents(logicalIndex);

        // Но сохраняем растягивание последнего столбца
        if (logicalIndex < m_contactsTable->columnCount() - 1) {
            m_contactsTable->horizontalHeader()->setStretchLastSection(true);
        }
    }
}

void QtPhoneBook::showAboutDialog()
{
    QString storageType = m_useDatabaseStorage ? "База данных PostgreSQL" : "JSON файл";
    
    QMessageBox::about(this, "О программе Phone Book",
        "<h2>Phone Book</h2>"
        "<p>Версия: 1.1.0</p>"
        "<p>Программа для управления контактами с графическим интерфейсом на Qt.</p>"
        "<p><b>Текущее хранилище:</b> " + storageType + "</p>"
        "<p>Основные возможности:</p>"
        "<ul>"
        "<li>Добавление, редактирование и удаление контактов</li>"
        "<li>Хранение в файле или базе данных PostgreSQL</li>"
        "<li>Переключение между хранилищами на лету</li>"
        "<li>Хранение множества телефонов для каждого контакта</li>"
        "<li>Поиск по имени, email или телефону</li>"
        "<li>Сортировка по всем столбцам</li>"
        "<li>Валидация вводимых данных</li>"
        "<li>Автоматическое сохранение</li>"
        "</ul>"
        "<p>Разработано на C++ с использованием Qt framework.</p>");
}

void QtPhoneBook::switchToFileStorage()
{
    if (!m_useDatabaseStorage) return;
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Переключение хранилища",
                                  "Переключиться на хранение в файл?\n"
                                  "Все данные будут загружены из файла.",
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_useDatabaseStorage = false;
        
        // Удаляем старый менеджер
        delete m_manager;
        m_manager = nullptr;
        
        // Переключаемся на файловое хранилище
        m_currentRepository = m_fileRepository;
        m_manager = new ContactManager(m_currentRepository);
        
        loadContacts();
        updateStorageStatus();
        
        QMessageBox::information(this, "Успех", 
            "Хранилище переключено на файл (contacts_qt.json)");
    }
}

void QtPhoneBook::switchToDatabaseStorage()
{
    if (m_useDatabaseStorage) return;
    
    if (!PostgreSQLRepository::isPostgreSQLAvailable()) {
        QMessageBox::warning(this, "Ошибка", 
            "PostgreSQL не доступен. Убедитесь, что:\n"
            "1. Установлен PostgreSQL\n"
            "2. Установлена библиотека libpq\n"
            "3. База данных создана и доступна");
        return;
    }
    
    // Запрашиваем параметры подключения
    bool ok;
    QString connectionString = QInputDialog::getText(this, "Подключение к PostgreSQL",
        "Введите строку подключения (примеры):\n"
        "host=localhost port=5432 dbname=phonebook user=phonebook_user password=password123\n"
        "postgresql://phonebook_user:password123@localhost:5432/phonebook",
        QLineEdit::Normal, 
        "host=localhost port=5432 dbname=phonebook user=phonebook_user password=password123",
        &ok);
    
    if (!ok || connectionString.isEmpty()) {
        return;
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Переключение хранилища",
                                  "Переключиться на хранение в базу данных?\n"
                                  "Все данные будут загружены из БД.",
                                  QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        try {
            // Создаем новое репозиторий для БД
            if (m_dbRepository) {
                delete m_dbRepository;
                m_dbRepository = nullptr;
            }
            
            m_dbRepository = new PostgreSQLRepository(connectionString.toStdString());
            
            if (!m_dbRepository->connect()) {
                delete m_dbRepository;
                m_dbRepository = nullptr;
                QMessageBox::warning(this, "Ошибка", 
                    "Не удалось подключиться к базе данных.\n"
                    "Проверьте параметры подключения.");
                return;
            }
            
            m_useDatabaseStorage = true;
            
            // Удаляем старый менеджер
            delete m_manager;
            m_manager = nullptr;
            
            // Переключаемся на хранилище БД
            m_currentRepository = m_dbRepository;
            m_manager = new ContactManager(m_currentRepository);
            
            loadContacts();
            updateStorageStatus();
            
            QMessageBox::information(this, "Успех", 
                "Хранилище переключено на базу данных PostgreSQL");
                
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка", 
                QString("Ошибка при подключении к БД: %1").arg(e.what()));
            
            // Восстанавливаем файловое хранилище в случае ошибки
            if (m_dbRepository) {
                delete m_dbRepository;
                m_dbRepository = nullptr;
            }
            m_useDatabaseStorage = false;
            m_currentRepository = m_fileRepository;
            m_manager = new ContactManager(m_currentRepository);
            updateStorageStatus();
        }
    }
}

void QtPhoneBook::switchStorage(IContactRepository* newRepository)
{
    if (!newRepository) return;
    
    if (m_manager) {
        delete m_manager;
        m_manager = nullptr;
    }
    
    m_currentRepository = newRepository;
    m_manager = new ContactManager(m_currentRepository);
    
    loadContacts();
    updateStorageStatus();
}

void QtPhoneBook::updateStorageStatus()
{
    QString storageType = m_useDatabaseStorage ? "База данных (PostgreSQL)" : "Файл (contacts_qt.json)";
    statusBar()->showMessage(QString("Хранилище: %1 | Готово").arg(storageType));
    
    // Обновляем состояние меню
    if (m_useFileAction && m_useDatabaseAction) {
        m_useFileAction->setChecked(!m_useDatabaseStorage);
        m_useDatabaseAction->setChecked(m_useDatabaseStorage);
    }
}

void QtPhoneBook::showStorageSettings()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Настройки хранилища");
    dialog.resize(450, 250);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    QGroupBox* storageGroup = new QGroupBox("Выберите тип хранилища", &dialog);
    QVBoxLayout* storageLayout = new QVBoxLayout(storageGroup);
    
    QRadioButton* fileRadio = new QRadioButton("Файл (JSON)", storageGroup);
    QRadioButton* dbRadio = new QRadioButton("База данных (PostgreSQL)", storageGroup);
    
    fileRadio->setChecked(!m_useDatabaseStorage);
    dbRadio->setChecked(m_useDatabaseStorage);
    
    storageLayout->addWidget(fileRadio);
    storageLayout->addWidget(dbRadio);
    
    // Параметры для БД
    QGroupBox* dbGroup = new QGroupBox("Параметры PostgreSQL", &dialog);
    QFormLayout* dbLayout = new QFormLayout(dbGroup);
    
    QLineEdit* hostEdit = new QLineEdit("localhost", dbGroup);
    QLineEdit* portEdit = new QLineEdit("5432", dbGroup);
    QLineEdit* dbNameEdit = new QLineEdit("phonebook", dbGroup);
    QLineEdit* userEdit = new QLineEdit("postgres", dbGroup);
    QLineEdit* passwordEdit = new QLineEdit("", dbGroup);
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    dbLayout->addRow("Хост:", hostEdit);
    dbLayout->addRow("Порт:", portEdit);
    dbLayout->addRow("Имя БД:", dbNameEdit);
    dbLayout->addRow("Пользователь:", userEdit);
    dbLayout->addRow("Пароль:", passwordEdit);
    
    dbGroup->setEnabled(dbRadio->isChecked());
    
    // Кнопки
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    
    layout->addWidget(storageGroup);
    layout->addWidget(dbGroup);
    layout->addWidget(buttonBox);
    
    // Подключения
    connect(fileRadio, &QRadioButton::toggled, dbGroup, &QGroupBox::setDisabled);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (fileRadio->isChecked()) {
            switchToFileStorage();
        } else {
            // Формируем строку подключения
            QString connectionString = QString("host=%1 port=%2 dbname=%3 user=%4 password=%5")
                .arg(hostEdit->text())
                .arg(portEdit->text())
                .arg(dbNameEdit->text())
                .arg(userEdit->text())
                .arg(passwordEdit->text());
            
            try {
                if (m_dbRepository) {
                    delete m_dbRepository;
                    m_dbRepository = nullptr;
                }
                
                m_dbRepository = new PostgreSQLRepository(connectionString.toStdString());
                
                if (!m_dbRepository->connect()) {
                    QMessageBox::warning(&dialog, "Ошибка", 
                        "Не удалось подключиться к базе данных.");
                    delete m_dbRepository;
                    m_dbRepository = nullptr;
                    return;
                }
                
                m_useDatabaseStorage = true;
                switchStorage(m_dbRepository);
                
                QMessageBox::information(&dialog, "Успех", 
                    "Подключение к базе данных установлено успешно.");
                
            } catch (const std::exception& e) {
                QMessageBox::critical(&dialog, "Ошибка", 
                    QString("Ошибка при подключении к БД: %1").arg(e.what()));
            }
        }
    }
}