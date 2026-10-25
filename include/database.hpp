#pragma once
#include "errors.hpp"
#include <sqlite3.h>
#include <string>

// ---------------------------------------------------------------------------
// Database – RAII wrapper over sqlite3* 
// ---------------------------------------------------------------------------

class Database {
public:
    // Open a database file (or in-memory with ":memory:").
    // If initSchema is true and schemaPath is provided, executes DDL schema.
    explicit Database(const std::string& dbPath = ":memory:",
                      bool enableForeignKeys = true);

    ~Database() noexcept;

    // Rule of Five: Non-copyable, movable
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    // Raw handle accessor
    sqlite3* handle() const noexcept { return db_; }

    // Execute arbitrary SQL statement string
    void execute(const std::string& sql);

    // Execute SQL script from a file (e.g. sql/schema.sql)
    void executeFile(const std::string& filepath);

    // SQLite helper accessors
    int64_t lastInsertRowId() const noexcept;
    int changes() const noexcept;
    bool isOpen() const noexcept { return db_ != nullptr; }

    // Explicit close
    void close() noexcept;

private:
    sqlite3* db_{nullptr};
    std::string path_;
};
