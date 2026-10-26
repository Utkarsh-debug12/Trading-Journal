#include "statement.hpp"
#include <utility>

Statement::Statement(sqlite3* db, const std::string& sql)
    : db_(db), sql_(sql) {
    if (!db_) {
        throw DatabaseError("Attempted to prepare statement on null database handle");
    }

    int rc = sqlite3_prepare_v2(db_, sql_.c_str(), -1, &stmt_, nullptr);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        throw DatabaseError("Failed to prepare statement: " + err + " (SQL: " + sql_ + ")");
    }
}

Statement::~Statement() noexcept {
    if (stmt_) {
        sqlite3_finalize(stmt_);
        stmt_ = nullptr;
    }
}

Statement::Statement(Statement&& other) noexcept
    : db_(other.db_), stmt_(other.stmt_), sql_(std::move(other.sql_)) {
    other.db_ = nullptr;
    other.stmt_ = nullptr;
}

Statement& Statement::operator=(Statement&& other) noexcept {
    if (this != &other) {
        if (stmt_) {
            sqlite3_finalize(stmt_);
        }
        db_ = other.db_;
        stmt_ = other.stmt_;
        sql_ = std::move(other.sql_);
        other.db_ = nullptr;
        other.stmt_ = nullptr;
    }
    return *this;
}

void Statement::bind(int index, int value) {
    int rc = sqlite3_bind_int(stmt_, index, value);
    if (rc != SQLITE_OK) {
        throw DatabaseError("Failed to bind int parameter at index " + std::to_string(index));
    }
}

void Statement::bind(int index, int64_t value) {
    int rc = sqlite3_bind_int64(stmt_, index, value);
    if (rc != SQLITE_OK) {
        throw DatabaseError("Failed to bind int64 parameter at index " + std::to_string(index));
    }
}

void Statement::bind(int index, double value) {
    int rc = sqlite3_bind_double(stmt_, index, value);
    if (rc != SQLITE_OK) {
        throw DatabaseError("Failed to bind double parameter at index " + std::to_string(index));
    }
}

void Statement::bind(int index, const std::string& value) {
    int rc = sqlite3_bind_text(stmt_, index, value.c_str(), static_cast<int>(value.length()), SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw DatabaseError("Failed to bind text parameter at index " + std::to_string(index));
    }
}

void Statement::bind(int index, const char* value) {
    if (!value) {
        bindNull(index);
        return;
    }
    int rc = sqlite3_bind_text(stmt_, index, value, -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        throw DatabaseError("Failed to bind c-string parameter at index " + std::to_string(index));
    }
}

void Statement::bindNull(int index) {
    int rc = sqlite3_bind_null(stmt_, index);
    if (rc != SQLITE_OK) {
        throw DatabaseError("Failed to bind null parameter at index " + std::to_string(index));
    }
}

bool Statement::step() {
    int rc = sqlite3_step(stmt_);
    if (rc == SQLITE_ROW) {
        return true;
    }
    if (rc == SQLITE_DONE) {
        return false;
    }
    std::string err = db_ ? sqlite3_errmsg(db_) : "Unknown step error";
    throw DatabaseError("Failed to step statement: " + err + " (SQL: " + sql_ + ")");
}

void Statement::execute() {
    int rc = sqlite3_step(stmt_);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        std::string err = db_ ? sqlite3_errmsg(db_) : "Unknown execute error";
        throw DatabaseError("Execution failed: " + err + " (SQL: " + sql_ + ")");
    }
}

void Statement::reset() {
    if (stmt_) {
        sqlite3_reset(stmt_);
    }
}

void Statement::clearBindings() {
    if (stmt_) {
        sqlite3_clear_bindings(stmt_);
    }
}

int Statement::getInt(int col) const {
    return sqlite3_column_int(stmt_, col);
}

int64_t Statement::getInt64(int col) const {
    return sqlite3_column_int64(stmt_, col);
}

double Statement::getDouble(int col) const {
    return sqlite3_column_double(stmt_, col);
}

std::string Statement::getString(int col) const {
    const unsigned char* text = sqlite3_column_text(stmt_, col);
    return text ? reinterpret_cast<const char*>(text) : "";
}

bool Statement::isNull(int col) const {
    return sqlite3_column_type(stmt_, col) == SQLITE_NULL;
}
