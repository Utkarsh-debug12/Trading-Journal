#pragma once
#include "crow_all.h"
#include "trade_repository.hpp"
#include "user_repository.hpp"
#include "auth_service.hpp"
#include "middleware.hpp"

using CrowApp = crow::App<AuthGuard>;

// ---------------------------------------------------------------------------
// Deps – dependency injection container holding references to core services.
// Passed to route registration functions to decouple HTTP layer from persistence.
// ---------------------------------------------------------------------------

struct Deps {
    ITradeRepository& trades;
    UserRepository& users;
    AuthService& auth;

    Deps(ITradeRepository& t, UserRepository& u, AuthService& a)
        : trades(t), users(u), auth(a) {}
};

// Route registration declarations
void registerAuthRoutes(CrowApp& app, Deps& deps);
void registerTradeRoutes(CrowApp& app, Deps& deps);
void registerAnalyticsRoutes(CrowApp& app, Deps& deps);
