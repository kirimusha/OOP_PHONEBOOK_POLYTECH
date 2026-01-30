#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include "../qt/QtPhoneBook.h"

/*
#include <QApplication>
#include "../qt/QtPhoneBook.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QtPhoneBook phoneBook;
    phoneBook.show();

    return app.exec();
}
*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Устанавливаем нежный розовый стиль
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    const QString styleSheet = R"(
        /* Основные цвета */
        QMainWindow, QDialog, QWidget {
            background-color: #fff5f7;
            color: #5d5d5d;
            font-family: "Segoe UI", Arial, sans-serif;
        }

        /* Меню и тулбар */
        QMenuBar {
            background-color: #ffd6e7;
            color: #8b475d;
            border-bottom: 1px solid #ffb6c1;
            font-weight: bold;
        }
        QMenuBar::item {
            padding: 5px 10px;
            border-radius: 3px;
        }
        QMenuBar::item:selected {
            background-color: #ffb6c1;
            color: #ffffff;
        }
        QMenu {
            background-color: #ffeef2;
            border: 1px solid #ffb6c1;
            color: #8b475d;
        }
        QMenu::item:selected {
            background-color: #ffb6c1;
            color: #ffffff;
        }

        /* Кнопки */
        QPushButton {
            background-color: #ffb6c1;
            border: 1px solid #ff91a4;
            border-radius: 5px;
            padding: 8px 15px;
            color: #8b475d;
            font-weight: bold;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #ff91a4;
            color: white;
        }
        QPushButton:pressed {
            background-color: #ff6b8b;
            color: white;
        }
        QPushButton:disabled {
            background-color: #ffd6e7;
            color: #c8a2b5;
        }

        /* Поля ввода */
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: white;
            border: 1px solid #ffb6c1;
            border-radius: 4px;
            padding: 6px;
            color: #5d5d5d;
            selection-background-color: #ffb6c1;
            selection-color: white;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border: 2px solid #ff91a4;
            background-color: #fffafc;
        }
        QLineEdit[readOnly="true"] {
            background-color: #fffafc;
            color: #8b8b8b;
        }

        /* Таблица */
        QTableWidget {
            background-color: white;
            alternate-background-color: #fffafc;
            gridline-color: #ffd6e7;
            border: 1px solid #ffb6c1;
            border-radius: 4px;
        }
        QTableWidget::item {
            padding: 5px;
            border-right: 1px solid #ffd6e7;
            border-bottom: 1px solid #ffd6e7;
        }
        QTableWidget::item:selected {
            background-color: #ffb6c1;
            color: white;
        }
        QHeaderView::section {
            background-color: #ffd6e7;
            color: #8b475d;
            padding: 8px;
            border: 1px solid #ffb6c1;
            font-weight: bold;
            border-radius: 2px;
        }
        QHeaderView::section:checked {
            background-color: #ff91a4;
            color: white;
        }

        /* Групбоксы */
        QGroupBox {
            background-color: #fffafc;
            border: 2px solid #ffb6c1;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
            color: #8b475d;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            background-color: #ffd6e7;
            border-radius: 3px;
        }

        /* Выпадающие списки */
        QComboBox {
            background-color: white;
            border: 1px solid #ffb6c1;
            border-radius: 4px;
            padding: 6px;
            min-width: 100px;
            color: #5d5d5d;
        }
        QComboBox:hover {
            border: 1px solid #ff91a4;
        }
        QComboBox::drop-down {
            border-left: 1px solid #ffb6c1;
            width: 20px;
        }
        QComboBox::down-arrow {
            image: url(:/icons/down_arrow.png);
            width: 12px;
            height: 12px;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            border: 1px solid #ffb6c1;
            selection-background-color: #ffb6c1;
            selection-color: white;
        }

        /* Список */
        QListWidget {
            background-color: white;
            border: 1px solid #ffb6c1;
            border-radius: 4px;
            color: #5d5d5d;
        }
        QListWidget::item {
            padding: 5px;
            border-bottom: 1px solid #ffd6e7;
        }
        QListWidget::item:selected {
            background-color: #ffb6c1;
            color: white;
        }
        QListWidget::item:hover {
            background-color: #ffd6e7;
        }

        /* Радио кнопки и чекбоксы */
        QRadioButton, QCheckBox {
            color: #8b475d;
            spacing: 8px;
        }
        QRadioButton::indicator, QCheckBox::indicator {
            width: 16px;
            height: 16px;
        }
        QRadioButton::indicator:checked {
            background-color: #ffb6c1;
            border: 3px solid white;
            border-radius: 8px;
        }
        QCheckBox::indicator:checked {
            background-color: #ffb6c1;
            border: 1px solid #ff91a4;
        }

        /* Статус бар */
        QStatusBar {
            background-color: #ffd6e7;
            color: #8b475d;
            border-top: 1px solid #ffb6c1;
        }

        /* Ползунки */
        QScrollBar:vertical {
            background-color: #fffafc;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #ffb6c1;
            border-radius: 6px;
            min-height: 20px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #ff91a4;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            background: none;
        }

        /* Вкладки */
        QTabWidget::pane {
            border: 1px solid #ffb6c1;
            background-color: white;
            border-radius: 4px;
        }
        QTabBar::tab {
            background-color: #ffd6e7;
            color: #8b475d;
            padding: 8px 16px;
            margin-right: 2px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background-color: #ffb6c1;
            color: white;
            font-weight: bold;
        }
        QTabBar::tab:hover:!selected {
            background-color: #ffc0cb;
        }

        /* Разделители */
        QSplitter::handle {
            background-color: #ffb6c1;
        }
        QSplitter::handle:hover {
            background-color: #ff91a4;
        }

        /* Диалоговые окна */
        QDialog {
            background-color: #fff5f7;
            border: 2px solid #ffb6c1;
            border-radius: 8px;
        }

        /* Сообщения */
        QMessageBox {
            background-color: #fff5f7;
        }
        QMessageBox QLabel {
            color: #8b475d;
        }

        /* Подсказки */
        QToolTip {
            background-color: #ffd6e7;
            color: #8b475d;
            border: 1px solid #ffb6c1;
            border-radius: 4px;
            padding: 5px;
        }

        /* Прогресс бар */
        QProgressBar {
            border: 1px solid #ffb6c1;
            border-radius: 4px;
            text-align: center;
            color: #8b475d;
        }
        QProgressBar::chunk {
            background-color: #ffb6c1;
            border-radius: 3px;
        }
    )";

    app.setStyleSheet(styleSheet);

    // Настройка палитры для улучшения внешнего вида
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(255, 245, 247)); // #fff5f7
    palette.setColor(QPalette::WindowText, QColor(139, 71, 93)); // #8b475d
    palette.setColor(QPalette::Base, QColor(255, 250, 252)); // #fffafc
    palette.setColor(QPalette::AlternateBase, QColor(255, 246, 248));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 214, 231)); // #ffd6e7
    palette.setColor(QPalette::ToolTipText, QColor(139, 71, 93)); // #8b475d
    palette.setColor(QPalette::Text, QColor(93, 93, 93)); // #5d5d5d
    palette.setColor(QPalette::Button, QColor(255, 182, 193)); // #ffb6c1
    palette.setColor(QPalette::ButtonText, QColor(139, 71, 93)); // #8b475d
    palette.setColor(QPalette::BrightText, Qt::white);
    palette.setColor(QPalette::Link, QColor(255, 145, 164)); // #ff91a4
    palette.setColor(QPalette::Highlight, QColor(255, 182, 193)); // #ffb6c1
    palette.setColor(QPalette::HighlightedText, Qt::white);

    palette.setColor(QPalette::Disabled, QPalette::Text, QColor(200, 162, 181));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(200, 162, 181));

    QApplication::setPalette(palette);

    QtPhoneBook phoneBook;

    phoneBook.show();
    return QApplication::exec();
}