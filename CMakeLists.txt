cmake_minimum_required(VERSION 3.16)
project(ptit_apex CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

set(APEX_SOURCES
    src/Currency.cpp
    src/Money.cpp
    src/IdGenerator.cpp
    src/InputUtils.cpp
    src/CurrencyConverter.cpp
    src/Account.cpp
    src/SavingsAccount.cpp
    src/CheckingAccount.cpp
    src/Transaction.cpp
    src/TransactionLog.cpp
    src/Bank.cpp
    src/SampleData.cpp
    src/DemoScenarios.cpp
    src/Menu.cpp
)

add_library(apex_lib STATIC ${APEX_SOURCES})
target_include_directories(apex_lib PUBLIC include)
target_link_libraries(apex_lib Threads::Threads)

add_executable(apex src/main.cpp)
target_link_libraries(apex apex_lib)
