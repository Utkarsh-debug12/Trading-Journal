cmake_minimum_required(VERSION 3.16)
project(trading_journal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Homebrew search path support on macOS
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)
find_library(SODIUM_LIB NAMES sodium REQUIRED)

set(SQLITE_TARGET SQLite::SQLite3)
if(TARGET SQLite3::SQLite3)
    set(SQLITE_TARGET SQLite3::SQLite3)
endif()

# Global include directories for all targets
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/crow
    /opt/homebrew/include
)

# Core library (Domain + Persistence + Auth + Middleware + Validators)
add_library(tj_core
    src/trade.cpp
    src/metrics.cpp
    src/journal.cpp
    src/validators.cpp
    src/database.cpp
    src/statement.cpp
    src/user_repository.cpp
    src/in_memory_trade_repository.cpp
    src/trade_repository.cpp
    src/auth_service.cpp
    src/middleware.cpp
)
target_link_libraries(tj_core PUBLIC ${SQLITE_TARGET} ${SODIUM_LIB} Threads::Threads)

# Main web executable
add_executable(trading_journal
    src/main.cpp
    src/routes_auth.cpp
    src/routes_trades.cpp
    src/routes_analytics.cpp
)
target_link_libraries(trading_journal PRIVATE tj_core)
