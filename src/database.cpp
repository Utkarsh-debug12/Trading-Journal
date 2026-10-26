#include "database.hpp"
#include <fstream>
#include <sstream>
#include <utility>

Database::Database(const std::string& dbPath, bool enableForeignKeys)
    : path_(dbPath) {
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    if (dbPath == ":memory:") {
        flags |= SQLITE_OPEN_MEMORY;
    }
    int rc = sqlite3_open_v2(dbPath.c_str(), &db_, flags, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "Unknown SQLite initialization error";
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        throw DatabaseError("Failed to open database '" + dbPath + "': " + err);
    }

    if (enableForeignKeys) {
        execute("PRAGMA foreign_keys = ON;");
    }
}

Database::~Database() noexcept {
    close();
}

Database::Database(Database&& other) noexcept
    : db_(other.db_), path_(std::move(other.path_)) {
    other.db_ = nullptr;
}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        close();
        db_ = other.db_;
        path_ = std::move(other.path_);
        other.db_ = nullptr;
    }
    return *this;
}

void Database::close() noexcept {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void Database::execute(const std::string& sql) {
    if (!db_) {
        throw DatabaseError("Attempted execute on closed database");
    }

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "Unknown SQL execution error";
        sqlite3_free(errMsg);
        throw DatabaseError("SQL execution failed: " + err + " (SQL: " + sql + ")");
    }
}

void Database::executeFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw DatabaseError("Could not open SQL script file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    execute(buffer.str());
}

int64_t Database::lastInsertRowId() const noexcept {
    return db_ ? sqlite3_last_insert_rowid(db_) : 0;
}

int Database::changes() const noexcept {
    return db_ ? sqlite3_changes(db_) : 0;
}
