#pragma once
#include "errors.hpp"
#include <sqlite3.h>
#include <string>

// ---------------------------------------------------------------------------
// Statement – RAII wrapper over sqlite3_stmt* 
// Non-copyable, movable. Provides parameterized binding, step execution,
// and typed column getters.
// ---------------------------------------------------------------------------

class Statement {
public:
    // Prepare a SQL statement against an open database handle
    Statement(sqlite3* db, const std::string& sql);

    ~Statement() noexcept;

    // Rule of Five: Non-copyable, movable
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement(Statement&& other) noexcept;
    Statement& operator=(Statement&& other) noexcept;

    // Parameter binding (1-based index)
    void bind(int index, int value);
    void bind(int index, int64_t value);
    void bind(int index, double value);
    void bind(int index, const std::string& value);
    void bind(int index, const char* value);
    void bindNull(int index);

    // Step execution: returns true if a row is available, false if done
    bool step();

    // Execute statement expecting completion (e.g. INSERT, UPDATE, DELETE)
    void execute();

    // Reset and clear bindings for reuse
    void reset();
    void clearBindings();

    // Typed column getters (0-based index)
    int getInt(int col) const;
    int64_t getInt64(int col) const;
    double getDouble(int col) const;
    std::string getString(int col) const;
    bool isNull(int col) const;

    // Raw statement pointer accessor
    sqlite3_stmt* handle() const noexcept { return stmt_; }

private:
    sqlite3* db_{nullptr};
    sqlite3_stmt* stmt_{nullptr};
    std::string sql_;
};
