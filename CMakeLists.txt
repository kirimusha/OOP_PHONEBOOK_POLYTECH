cmake_minimum_required(VERSION 3.28.3)
project(oopPhoneBook)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(source)
include_directories(third_party)

add_executable(oopPhoneBook
        include/Contact.h
        include/ContactManager.h
        include/FileRepository.h
        include/IContactRepository.h
        include/PhoneNumber.h
        include/UI.h
        include/Validators.h
        source/Contact.cpp
        source/ContactManager.cpp
        source/FileRepository.cpp
        source/main.cpp
        source/PhoneNumber.cpp
        source/UI.cpp
        source/Validators.cpp
        third_party/json.hpp
        source/UI.cpp
        source/main.cpp)
