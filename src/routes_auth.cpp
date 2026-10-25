#include "routes.hpp"
#include "errors.hpp"
#include <iostream>

void registerAuthRoutes(CrowApp& app, Deps& deps) {
    // GET /login
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::GET)([](const crow::request& req) {
        if (currentUserId(req) > 0) {
            crow::response res;
            res.redirect("/");
            return res;
        }
        crow::mustache::context ctx;
        return crow::response(crow::mustache::load("login.html").render(ctx));
    });

    // POST /login
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)([&deps](const crow::request& req) {
        crow::query_string params("?" + req.body);
        const char* userRaw = params.get("username");
        const char* passRaw = params.get("password");

        std::string username = userRaw ? userRaw : "";
        std::string password = passRaw ? passRaw : "";

        auto userId = deps.auth.login(username, password);
        if (userId.has_value()) {
            std::string token = auth::createSession(*userId);
            crow::response res;
            res.redirect("/");
            auth::setSessionCookie(res, token);
            return res;
        }

        crow::mustache::context ctx;
        ctx["username"] = username;
        ctx["error"] = "Invalid username or password. Please try again.";
        return crow::response(401, crow::mustache::load("login.html").render(ctx));
    });

    // GET /register
    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::GET)([](const crow::request& req) {
        if (currentUserId(req) > 0) {
            crow::response res;
            res.redirect("/");
            return res;
        }
        crow::mustache::context ctx;
        return crow::response(crow::mustache::load("register.html").render(ctx));
    });

    // POST /register
    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)([&deps](const crow::request& req) {
        crow::query_string params("?" + req.body);
        const char* userRaw = params.get("username");
        const char* passRaw = params.get("password");
        const char* confRaw = params.get("confirm_password");

        std::string username = userRaw ? userRaw : "";
        std::string password = passRaw ? passRaw : "";
        std::string confirm = confRaw ? confRaw : "";

        crow::mustache::context ctx;
        ctx["username"] = username;

        if (password != confirm) {
            ctx["error"] = "Passwords do not match.";
            return crow::response(400, crow::mustache::load("register.html").render(ctx));
        }

        try {
            auto userId = deps.auth.registerUser(username, password);
            if (!userId.has_value()) {
                ctx["error"] = "Username '" + username + "' is already taken. Please choose another.";
                return crow::response(400, crow::mustache::load("register.html").render(ctx));
            }

            std::string token = auth::createSession(*userId);
            crow::response res;
            res.redirect("/");
            auth::setSessionCookie(res, token);
            return res;
        } catch (const ValidationError& e) {
            ctx["error"] = e.what();
            return crow::response(400, crow::mustache::load("register.html").render(ctx));
        } catch (const std::exception& e) {
            ctx["error"] = std::string("Registration failed: ") + e.what();
            return crow::response(500, crow::mustache::load("register.html").render(ctx));
        }
    });

    // GET & POST /logout
    CROW_ROUTE(app, "/logout").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST)([](const crow::request& req) {
        std::string token = auth::extractSessionToken(req);
        if (!token.empty()) {
            auth::destroySession(token);
        }
        crow::response res;
        res.redirect("/login");
        auth::clearSessionCookie(res);
        return res;
    });
}
