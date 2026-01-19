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
target_include_directories(PhoneBook_Console PRIVATE include third_party)

# Qt приложение
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(QT_SOURCES
qt/main_qt.cpp
qt/QtPhoneBook.cpp
qt/ContactDialog.cpp
qt/ContactDetailsDialog.cpp
${COMMON_SOURCES}
database/PostgreSQLRepository.cpp
)

set(QT_HEADERS
qt/QtPhoneBook.h
qt/ContactDialog.h
qt/ContactDetailsDialog.h
database/PostgreSQLRepository.h
)

# Поиск PostgreSQL
find_path(POSTGRESQL_INCLUDE_DIR
NAMES libpq-fe.h
PATHS
/usr/include/postgresql
/usr/include
/usr/local/include
/usr/local/include/postgresql
/usr/include/x86_64-linux-gnu
PATH_SUFFIXES postgresql
)

find_library(POSTGRESQL_LIBRARY
NAMES pq libpq
PATHS
/usr/lib
/usr/lib/x86_64-linux-gnu
/usr/local/lib
/usr/lib64
)

if(POSTGRESQL_INCLUDE_DIR AND POSTGRESQL_LIBRARY)
set(POSTGRESQL_FOUND TRUE)
message(STATUS "Found PostgreSQL include: ${POSTGRESQL_INCLUDE_DIR}")
message(STATUS "Found PostgreSQL library: ${POSTGRESQL_LIBRARY}")
add_definitions(-DHAVE_POSTGRESQL)
else()
message(WARNING "PostgreSQL not found. Database features will be disabled.")
# Определяем макрос для отключения PostgreSQL
add_definitions(-DNO_POSTGRESQL)
endif()

add_executable(PhoneBook_Qt
${QT_SOURCES}
${QT_HEADERS}
)

# Добавляем include директории для PhoneBook_Qt
target_include_directories(PhoneBook_Qt PRIVATE
include
qt
database
third_party
${CMAKE_CURRENT_SOURCE_DIR}
)

# Добавляем директорию с PostgreSQL если найдена
if(POSTGRESQL_FOUND)
target_include_directories(PhoneBook_Qt PRIVATE
${POSTGRESQL_INCLUDE_DIR}
)
endif()

# Собираем все библиотеки для link
set(ALL_LIBS Qt6::Widgets)

if(POSTGRESQL_FOUND)
list(APPEND ALL_LIBS ${POSTGRESQL_LIBRARY})
endif()

target_link_libraries(PhoneBook_Qt ${ALL_LIBS})

# Настройка выходных директорий
set_target_properties(PhoneBook_Console PhoneBook_Qt
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Копирование необходимых файлов (только если файл существует)
if(EXISTS ${CMAKE_SOURCE_DIR}/contacts_qt.json)
file(COPY ${CMAKE_SOURCE_DIR}/contacts_qt.json DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()