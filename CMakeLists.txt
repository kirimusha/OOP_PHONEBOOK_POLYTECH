cmake_minimum_required(VERSION 3.16)
project(PhoneBook)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(COMMON_SOURCES
        source/Contact.cpp
        source/PhoneNumber.cpp
        source/Validators.cpp
        source/ContactManager.cpp
        source/FileRepository.cpp
        source/UI.cpp
)

set(COMMON_HEADERS
        include/Contact.h
        include/PhoneNumber.h
        include/Validators.h
        include/ContactManager.h
        include/FileRepository.h
        include/UI.h
        include/IContactRepository.h
)

# Консольное приложение
add_executable(PhoneBook_Console
        source/main.cpp
        ${COMMON_SOURCES}
)
target_include_directories(PhoneBook_Console PRIVATE include)

# Qt приложение
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(QT_SOURCES
        qt/main_qt.cpp
        qt/QtPhoneBook.cpp
        qt/ContactDialog.cpp
        qt/ContactDetailsDialog.cpp
        ${COMMON_SOURCES}
)

add_executable(PhoneBook_Qt
        ${QT_SOURCES}
)

target_include_directories(PhoneBook_Qt PRIVATE include qt)
target_link_libraries(PhoneBook_Qt Qt6::Widgets)

# Для работы с JSON
include_directories(third_party)