#include "crow_all.h"
#include "database.hpp"
#include "user_repository.hpp"
#include "trade_repository.hpp"
#include "in_memory_trade_repository.hpp"
#include "auth_service.hpp"
#include "routes.hpp"
#include "middleware.hpp"
#include <iostream>
#include <cstring>

// ---------------------------------------------------------------------------
// Main server entrypoint for Trading Journal Web Application (Owner: Member C)
// ---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    std::cout << "==================================================" << std::endl;
    std::cout << " Trading Journal C++ Web Server (Member C)" << std::endl;
    std::cout << "==================================================" << std::endl;

    // 1. Configure template and static directories
    crow::mustache::set_base("templates");

    // 2. Initialize Database and execute schema
    std::string dbPath = "trading_journal.db";
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--memory") == 0) {
            dbPath = ":memory:";
        }
    }

    std::cout << "Opening database: " << dbPath << std::endl;
    Database db(dbPath);
    try {
        db.executeFile("sql/schema.sql");
        std::cout << "Database schema verified." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Note on schema: " << e.what() << std::endl;
    }

    // 3. Build Dependency Injection Container
    UserRepository userRepo(db);
    AuthService authService(userRepo);
    SqliteTradeRepository tradeRepo(db); // Phase 2: Switched to SqliteTradeRepository

    Deps deps(tradeRepo, userRepo, authService);

    // 4. Instantiate Crow Application with AuthGuard middleware
    CrowApp app;

    // Static asset serving route (/static/<string>)
    CROW_ROUTE(app, "/static/<path>")
    ([](const crow::request& /*req*/, crow::response& res, std::string path) {
        if (path.find("..") != std::string::npos) {
            res.code = 403;
            res.write("Access denied");
            res.end();
            return;
        }
        std::string fullPath = "static/" + path;
        res.set_static_file_info(fullPath);
        res.end();
    });

    // 5. Register all route modules
    registerAuthRoutes(app, deps);
    registerTradeRoutes(app, deps);
    registerAnalyticsRoutes(app, deps);

    // Validate route graph
    app.validate();

    // If --validate or --test is passed, exit cleanly after validation
    if (argc > 1 && (std::strcmp(argv[1], "--test") == 0 || std::strcmp(argv[1], "--validate") == 0)) {
        std::cout << "[SUCCESS] Route validation and dependency wiring passed successfully." << std::endl;
        return 0;
    }

    // 6. Launch web server on port 18080
    std::cout << "Starting Trading Journal at http://localhost:18080" << std::endl;
    std::cout << "Press Ctrl+C to stop the server." << std::endl;
    app.port(18080).multithreaded().run();

    return 0;
}
