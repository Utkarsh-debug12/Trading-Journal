#include "routes.hpp"
#include "validators.hpp"
#include "errors.hpp"
#include <iomanip>
#include <sstream>

namespace {
    std::string formatMoney(double val) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << std::abs(val);
        return ss.str();
    }

    std::string getQueryString(const crow::request& req) {
        auto pos = req.raw_url.find('?');
        if (pos != std::string::npos) {
            return req.raw_url.substr(pos + 1);
        }
        pos = req.url.find('?');
        if (pos != std::string::npos) {
            return req.url.substr(pos + 1);
        }
        return "";
    }

    TradeFilter parseTradeFilter(const crow::query_string& qs) {
        TradeFilter filter;

        const char* tickerParam = qs.get("ticker");
        if (tickerParam && *tickerParam) {
            std::string t = tickerParam;
            size_t first = t.find_first_not_of(" \t\r\n");
            if (first != std::string::npos) {
                size_t last = t.find_last_not_of(" \t\r\n");
                t = t.substr(first, last - first + 1);
            }
            if (!t.empty()) {
                filter.ticker = t;
            }
        }

        const char* winOnlyParam = qs.get("winOnly");
        if (winOnlyParam && *winOnlyParam) {
            std::string w(winOnlyParam);
            if (w == "true") {
                filter.winOnly = true;
            } else if (w == "false") {
                filter.winOnly = false;
            }
        }

        const char* dirParam = qs.get("direction");
        if (dirParam && *dirParam) {
            try {
                filter.direction = Trade::fromString(dirParam);
            } catch (...) {}
        }

        const char* startParam = qs.get("startDate");
        if (startParam && *startParam) {
            filter.startDate = startParam;
        }

        const char* endParam = qs.get("endDate");
        if (endParam && *endParam) {
            filter.endDate = endParam;
        }

        return filter;
    }
}

void registerTradeRoutes(CrowApp& app, Deps& deps) {
    // GET & POST /trades - List trades with search filters (POST supported for 307 redirect follow-through)
    CROW_ROUTE(app, "/trades").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        TradeFilter filter = parseTradeFilter(req.url_params);

        std::vector<Trade> tradeList;
        try {
            tradeList = deps.trades.search(userId, filter);
        } catch (const DatabaseError& e) {
            crow::response res(500, std::string("Database error: ") + e.what());
            return res;
        }

        crow::mustache::context ctx;
        ctx["ticker_filter"] = filter.ticker.value_or("");
        ctx["start_date_filter"] = filter.startDate.value_or("");
        ctx["end_date_filter"] = filter.endDate.value_or("");
        ctx["query_string"] = getQueryString(req);

        // Flash message support
        const char* errParam = req.url_params.get("error");
        if (errParam && *errParam) {
            ctx["error"] = errParam;
        }
        const char* succParam = req.url_params.get("success");
        if (succParam && *succParam) {
            ctx["success"] = succParam;
        }

        if (filter.winOnly.has_value()) {
            if (*filter.winOnly) {
                ctx["filter_wins_selected"] = true;
            } else {
                ctx["filter_losses_selected"] = true;
            }
        }

        if (filter.direction.has_value()) {
            if (*filter.direction == Trade::Direction::Long) {
                ctx["filter_long_selected"] = true;
            } else {
                ctx["filter_short_selected"] = true;
            }
        }

        auto user = deps.users.findById(userId);
        ctx["username"] = user ? user->username : "Trader";
        ctx["has_trades"] = !tradeList.empty();
        ctx["trades_count"] = tradeList.size();

        bool hasFilters = filter.ticker.has_value() || filter.winOnly.has_value() ||
                          filter.direction.has_value() || filter.startDate.has_value() || filter.endDate.has_value();
        ctx["has_active_filters"] = hasFilters;

        std::vector<crow::mustache::context> tradesJson;
        for (const auto& t : tradeList) {
            crow::mustache::context item;
            item["id"] = t.id();
            item["ticker"] = t.ticker();
            item["direction"] = Trade::toString(t.direction());
            item["is_long"] = (t.direction() == Trade::Direction::Long);
            item["entry"] = formatMoney(t.entry());
            item["exit"] = formatMoney(t.exit());
            item["quantity"] = t.quantity();
            item["fees"] = formatMoney(t.fees());
            item["profit"] = formatMoney(t.profit());
            item["is_win"] = t.isWin();
            item["date"] = t.tradeDate();
            item["notes"] = t.notes();
            tradesJson.push_back(item);
        }
        ctx["trades"] = std::move(tradesJson);

        return crow::response(crow::mustache::load("trades.html").render(ctx));
    });

    // GET /trades/new - Show new trade form
    CROW_ROUTE(app, "/trades/new").CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        auto user = deps.users.findById(userId);
        crow::mustache::context ctx;
        ctx["page_title"] = "Log New Trade";
        ctx["form_action"] = "/trades/new";
        ctx["username"] = user ? user->username : "Trader";
        ctx["is_long"] = true;
        ctx["quantity"] = "1";
        ctx["fees"] = "0.00";

        return crow::response(crow::mustache::load("trade_form.html").render(ctx));
    });

    // POST /trades/new - Submit new trade form
    CROW_ROUTE(app, "/trades/new").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        crow::query_string params("?" + req.body);
        FormData form;
        form.ticker = params.get("ticker") ? params.get("ticker") : "";
        form.direction = params.get("direction") ? params.get("direction") : "Long";
        form.entryPrice = params.get("entryPrice") ? params.get("entryPrice") : "";
        form.exitPrice = params.get("exitPrice") ? params.get("exitPrice") : "";
        form.quantity = params.get("quantity") ? params.get("quantity") : "";
        form.fees = params.get("fees") ? params.get("fees") : "0.00";
        form.tradeDate = params.get("tradeDate") ? params.get("tradeDate") : "";
        form.notes = params.get("notes") ? params.get("notes") : "";

        try {
            Trade trade = Validators::validateTradeForm(form, userId);
            deps.trades.add(trade);
            crow::response res;
            res.redirect("/trades?success=Trade%20logged%20successfully");
            return res;
        } catch (const ValidationError& e) {
            auto user = deps.users.findById(userId);
            crow::mustache::context ctx;
            ctx["page_title"] = "Log New Trade";
            ctx["form_action"] = "/trades/new";
            ctx["username"] = user ? user->username : "Trader";
            ctx["error"] = e.what();
            ctx["ticker"] = form.ticker;
            ctx["is_long"] = (form.direction == "Long");
            ctx["is_short"] = (form.direction == "Short");
            ctx["entry"] = form.entryPrice;
            ctx["exit"] = form.exitPrice;
            ctx["quantity"] = form.quantity;
            ctx["fees"] = form.fees;
            ctx["tradeDate"] = form.tradeDate;
            ctx["notes"] = form.notes;
            return crow::response(400, crow::mustache::load("trade_form.html").render(ctx));
        } catch (const DatabaseError& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // GET /trades/<id>/edit - Show edit trade form
    CROW_ROUTE(app, "/trades/<int>/edit").CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req, int tradeId) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        std::optional<Trade> trade;
        try {
            trade = deps.trades.findById(userId, tradeId);
        } catch (const DatabaseError& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }

        if (!trade.has_value()) {
            crow::response res;
            res.redirect("/trades?error=Trade%20not%20found%20or%20unauthorized");
            return res;
        }

        auto user = deps.users.findById(userId);
        crow::mustache::context ctx;
        ctx["page_title"] = "Edit Trade #" + std::to_string(tradeId);
        ctx["form_action"] = "/trades/" + std::to_string(tradeId) + "/edit";
        ctx["username"] = user ? user->username : "Trader";
        ctx["ticker"] = trade->ticker();
        ctx["is_long"] = (trade->direction() == Trade::Direction::Long);
        ctx["is_short"] = (trade->direction() == Trade::Direction::Short);
        ctx["entry"] = formatMoney(trade->entry());
        ctx["exit"] = formatMoney(trade->exit());
        ctx["quantity"] = trade->quantity();
        ctx["fees"] = formatMoney(trade->fees());
        ctx["tradeDate"] = trade->tradeDate();
        ctx["notes"] = trade->notes();

        return crow::response(crow::mustache::load("trade_form.html").render(ctx));
    });

    // POST /trades/<id>/edit - Submit edit trade form
    CROW_ROUTE(app, "/trades/<int>/edit").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req, int tradeId) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        crow::query_string params("?" + req.body);
        FormData form;
        form.ticker = params.get("ticker") ? params.get("ticker") : "";
        form.direction = params.get("direction") ? params.get("direction") : "Long";
        form.entryPrice = params.get("entryPrice") ? params.get("entryPrice") : "";
        form.exitPrice = params.get("exitPrice") ? params.get("exitPrice") : "";
        form.quantity = params.get("quantity") ? params.get("quantity") : "";
        form.fees = params.get("fees") ? params.get("fees") : "";
        form.tradeDate = params.get("tradeDate") ? params.get("tradeDate") : "";
        form.notes = params.get("notes") ? params.get("notes") : "";

        try {
            Trade trade = Validators::validateTradeForm(form, userId, tradeId);
            bool updated = deps.trades.update(trade);
            crow::response res;
            if (updated) {
                res.redirect("/trades?success=Trade%20updated%20successfully");
            } else {
                res.redirect("/trades?error=Trade%20not%20found%20or%20unauthorized");
            }
            return res;
        } catch (const ValidationError& e) {
            auto user = deps.users.findById(userId);
            crow::mustache::context ctx;
            ctx["page_title"] = "Edit Trade #" + std::to_string(tradeId);
            ctx["form_action"] = "/trades/" + std::to_string(tradeId) + "/edit";
            ctx["username"] = user ? user->username : "Trader";
            ctx["error"] = e.what();
            ctx["ticker"] = form.ticker;
            ctx["is_long"] = (form.direction == "Long");
            ctx["is_short"] = (form.direction == "Short");
            ctx["entry"] = form.entryPrice;
            ctx["exit"] = form.exitPrice;
            ctx["quantity"] = form.quantity;
            ctx["fees"] = form.fees;
            ctx["tradeDate"] = form.tradeDate;
            ctx["notes"] = form.notes;
            return crow::response(400, crow::mustache::load("trade_form.html").render(ctx));
        } catch (const DatabaseError& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // POST /trades/<id>/delete - Delete trade (scoped by userId)
    CROW_ROUTE(app, "/trades/<int>/delete").methods(crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req, int tradeId) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        try {
            bool removed = deps.trades.remove(userId, tradeId);
            crow::response res;
            if (removed) {
                res.redirect("/trades?success=Trade%20deleted%20successfully");
            } else {
                res.redirect("/trades?error=Trade%20not%20found%20or%20unauthorized");
            }
            return res;
        } catch (const DatabaseError& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // Helper lambda for CSV export (RFC 4180 compliant)
    auto handleExport = [&deps](const crow::request& req) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        TradeFilter filter = parseTradeFilter(req.url_params);
        std::vector<Trade> trades = deps.trades.search(userId, filter);

        std::ostringstream csv;
        csv << "id,ticker,direction,entry,exit,quantity,fees,date,profit,notes\n";
        for (const auto& t : trades) {
            std::string notes = t.notes();
            std::string escapedNotes;
            for (char c : notes) {
                if (c == '"') {
                    escapedNotes += "\"\"";
                } else {
                    escapedNotes += c;
                }
            }
            csv << t.id() << ","
                << t.ticker() << ","
                << Trade::toString(t.direction()) << ","
                << std::fixed << std::setprecision(2)
                << t.entry() << ","
                << t.exit() << ","
                << t.quantity() << ","
                << t.fees() << ","
                << t.tradeDate() << ","
                << t.profit() << ",\""
                << escapedNotes << "\"\n";
        }

        crow::response res(csv.str());
        res.add_header("Content-Type", "text/csv");
        res.add_header("Content-Disposition", "attachment; filename=\"trades_export.csv\"");
        return res;
    };

    // GET /export - Download CSV (supports filters in query string)
    CROW_ROUTE(app, "/export").CROW_MIDDLEWARES(app, AuthGuard)(handleExport);

    // GET /trades/export - Download CSV alias
    CROW_ROUTE(app, "/trades/export").CROW_MIDDLEWARES(app, AuthGuard)(handleExport);
}
