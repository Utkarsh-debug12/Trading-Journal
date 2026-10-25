#pragma once
#include <optional>
#include <string>

class Database;

// ---------------------------------------------------------------------------
// User – represents a user account record in the database.
// ---------------------------------------------------------------------------

struct User {
    int id{0};
    std::string username;
    std::string passwordHash;

    User() = default;
    User(int id, std::string username, std::string passwordHash)
        : id(id), username(std::move(username)), passwordHash(std::move(passwordHash)) {}

    bool operator==(const User& o) const {
        return id == o.id && username == o.username && passwordHash == o.passwordHash;
    }
};

// ---------------------------------------------------------------------------
// UserRepository – handles user persistence in SQLite (Owner: Member B).
// ---------------------------------------------------------------------------

class UserRepository {
public:
    UserRepository() = default;
    explicit UserRepository(Database& db);
    virtual ~UserRepository() = default;

    // Creates a new user record.
    // Returns the newly inserted user ID, or std::nullopt if the username already exists.
    virtual std::optional<int> create(const std::string& username, const std::string& passwordHash);

    // Finds a user by their unique username.
    // Returns std::nullopt if no such user is found.
    virtual std::optional<User> findByUsername(const std::string& username) const;

    // Finds a user by their primary key ID.
    // Returns std::nullopt if no such user is found.
    virtual std::optional<User> findById(int id) const;

private:
    Database* db_{nullptr};
};
