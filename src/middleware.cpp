#include "middleware.hpp"
#include <mutex>
#include <random>
#include <sstream>
#include <unordered_map>

namespace {
    std::mutex g_sessionMutex;
    std::unordered_map<std::string, int> g_sessions; // token -> userId

    std::string generateToken() {
        static constexpr char charset[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static thread_local std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

        std::string token;
        token.reserve(32);
        for (int i = 0; i < 32; ++i) {
            token += charset[dist(rng)];
        }
        return token;
    }
}

namespace auth {
    std::string createSession(int userId) {
        std::lock_guard<std::mutex> lock(g_sessionMutex);
        std::string token = generateToken();
        g_sessions[token] = userId;
        return token;
    }

    void destroySession(const std::string& token) {
        std::lock_guard<std::mutex> lock(g_sessionMutex);
        g_sessions.erase(token);
    }

    int getUserIdFromSession(const std::string& token) {
        if (token.empty()) {
            return -1;
        }
        std::lock_guard<std::mutex> lock(g_sessionMutex);
        auto it = g_sessions.find(token);
        if (it != g_sessions.end()) {
            return it->second;
        }
        return -1;
    }

    std::string extractSessionToken(const crow::request& req) {
        std::string cookieHeader = req.get_header_value("Cookie");
        if (cookieHeader.empty()) {
            return "";
        }

        const std::string key = "session_token=";
        auto pos = cookieHeader.find(key);
        if (pos == std::string::npos) {
            return "";
        }

        size_t start = pos + key.length();
        size_t end = cookieHeader.find(';', start);
        return cookieHeader.substr(start, (end == std::string::npos) ? std::string::npos : end - start);
    }

    void setSessionCookie(crow::response& res, const std::string& token) {
        // Set HTTP-only, Path=/ session cookie
        res.add_header("Set-Cookie", "session_token=" + token + "; Path=/; HttpOnly; SameSite=Lax");
    }

    void clearSessionCookie(crow::response& res) {
        // Invalidate cookie by setting past expiration
        res.add_header("Set-Cookie", "session_token=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT; HttpOnly");
    }
}

int currentUserId(const crow::request& req) {
    std::string token = auth::extractSessionToken(req);
    return auth::getUserIdFromSession(token);
}
