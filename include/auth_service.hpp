#pragma once
#include "user_repository.hpp"
#include <optional>
#include <string>

// ---------------------------------------------------------------------------
// AuthService – handles authentication, registration and password hashing (Owner: Member C).
// Uses libsodium (Argon2id) for secure password hashing.
// ---------------------------------------------------------------------------

class AuthService {
public:
    AuthService() : userRepo_(nullptr) {}
    explicit AuthService(UserRepository& userRepo);
    virtual ~AuthService() = default;

    // Registers a new user account with hashed password.
    // Returns the newly created user ID, or std::nullopt if username is taken.
    // Throws ValidationError if username or password fail basic validation.
    virtual std::optional<int> registerUser(const std::string& username, const std::string& password);

    // Validates credentials and logs in.
    // Returns the user ID on success, or std::nullopt on failure.
    virtual std::optional<int> login(const std::string& username, const std::string& password);

    // Helper: Computes Argon2id password hash string using libsodium.
    static std::string hashPassword(const std::string& password);

    // Helper: Verifies password against Argon2id hash string using libsodium.
    static bool verifyPassword(const std::string& hash, const std::string& password);

private:
    UserRepository* userRepo_{nullptr};
};
