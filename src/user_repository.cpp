#include "user_repository.hpp"
#include "database.hpp"
#include "statement.hpp"
#include "errors.hpp"
#include <sqlite3.h>

UserRepository::UserRepository(Database& db) : db_(&db) {}

std::optional<int> UserRepository::create(const std::string& username, const std::string& passwordHash) {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in UserRepository::create");
    }

    const std::string sql = "INSERT INTO users (username, password_hash) VALUES (?, ?);";
    sqlite3_stmt* rawStmt = nullptr;
    int rc = sqlite3_prepare_v2(db_->handle(), sql.c_str(), -1, &rawStmt, nullptr);
    if (rc != SQLITE_OK) {
        throw DatabaseError(std::string("Failed to prepare user insert: ") + sqlite3_errmsg(db_->handle()));
    }

    sqlite3_bind_text(rawStmt, 1, username.c_str(), static_cast<int>(username.length()), SQLITE_TRANSIENT);
    sqlite3_bind_text(rawStmt, 2, passwordHash.c_str(), static_cast<int>(passwordHash.length()), SQLITE_TRANSIENT);

    rc = sqlite3_step(rawStmt);
    sqlite3_finalize(rawStmt);

    if (rc == SQLITE_DONE) {
        return static_cast<int>(db_->lastInsertRowId());
    }

    // Duplicate username or constraint violation
    if (rc == SQLITE_CONSTRAINT || (rc & 0xFF) == SQLITE_CONSTRAINT) {
        return std::nullopt;
    }

    throw DatabaseError(std::string("Failed to insert user: ") + sqlite3_errmsg(db_->handle()));
}

std::optional<User> UserRepository::findByUsername(const std::string& username) const {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in UserRepository::findByUsername");
    }

    const std::string sql = "SELECT id, username, password_hash FROM users WHERE username = ?;";
    Statement stmt(db_->handle(), sql);
    stmt.bind(1, username);

    if (stmt.step()) {
        int id = stmt.getInt(0);
        std::string uname = stmt.getString(1);
        std::string hash = stmt.getString(2);
        return User(id, std::move(uname), std::move(hash));
    }

    return std::nullopt;
}

std::optional<User> UserRepository::findById(int id) const {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in UserRepository::findById");
    }

    const std::string sql = "SELECT id, username, password_hash FROM users WHERE id = ?;";
    Statement stmt(db_->handle(), sql);
    stmt.bind(1, id);

    if (stmt.step()) {
        int userId = stmt.getInt(0);
        std::string uname = stmt.getString(1);
        std::string hash = stmt.getString(2);
        return User(userId, std::move(uname), std::move(hash));
    }

    return std::nullopt;
}
