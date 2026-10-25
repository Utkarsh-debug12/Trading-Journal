#include "auth_service.hpp"
#include "errors.hpp"
#include <sodium.h>

namespace {
    // Ensures sodium_init is executed safely
    bool ensureSodiumInit() {
        static const bool initialized = ([]() {
            return sodium_init() >= 0;
        })();
        return initialized;
    }
}

AuthService::AuthService(UserRepository& userRepo)
    : userRepo_(&userRepo) {
    ensureSodiumInit();
}

std::string AuthService::hashPassword(const std::string& password) {
    if (!ensureSodiumInit()) {
        throw AppError("libsodium initialization failed");
    }
    if (password.empty()) {
        throw ValidationError("password", "Password cannot be empty");
    }

    char hashed[crypto_pwhash_STRBYTES];
    int rc = crypto_pwhash_str(
        hashed,
        password.c_str(),
        password.length(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    );

    if (rc != 0) {
        throw AppError("crypto_pwhash_str failed: out of memory or internal error");
    }

    return std::string(hashed);
}

bool AuthService::verifyPassword(const std::string& hash, const std::string& password) {
    if (!ensureSodiumInit()) {
        return false;
    }
    if (hash.empty() || password.empty()) {
        return false;
    }

    return crypto_pwhash_str_verify(hash.c_str(), password.c_str(), password.length()) == 0;
}

std::optional<int> AuthService::registerUser(const std::string& username, const std::string& password) {
    if (!userRepo_) {
        return std::nullopt;
    }

    if (username.length() < 3) {
        throw ValidationError("username", "Username must be at least 3 characters long");
    }
    if (password.length() < 6) {
        throw ValidationError("password", "Password must be at least 6 characters long");
    }

    std::string hashed = hashPassword(password);
    return userRepo_->create(username, hashed);
}

std::optional<int> AuthService::login(const std::string& username, const std::string& password) {
    if (!userRepo_ || username.empty() || password.empty()) {
        return std::nullopt;
    }

    auto user = userRepo_->findByUsername(username);
    if (!user.has_value()) {
        return std::nullopt;
    }

    if (verifyPassword(user->passwordHash, password)) {
        return user->id;
    }

    return std::nullopt;
}
