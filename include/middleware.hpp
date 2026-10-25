#pragma once
#include "crow_all.h"
#include <string>

// ---------------------------------------------------------------------------
// Session & Auth Middleware (Owner: Member C)
// ---------------------------------------------------------------------------

namespace auth {
    // Session token lifecycle
    std::string createSession(int userId);
    void destroySession(const std::string& token);
    int getUserIdFromSession(const std::string& token);
    std::string extractSessionToken(const crow::request& req);

    void setSessionCookie(crow::response& res, const std::string& token);
    void clearSessionCookie(crow::response& res);
}

// Contracted helper: returns current logged in userId or -1
int currentUserId(const crow::request& req);

// AuthGuard: redirects unauthenticated requests to /login
struct AuthGuard : public crow::ILocalMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& /*ctx*/) {
        int uid = currentUserId(req);
        if (uid <= 0) {
            res.redirect("/login");
            res.end();
        }
    }

    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) {}
};
