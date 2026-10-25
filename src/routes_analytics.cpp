#include "routes.hpp"
#include "journal.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace {
    std::string formatMoney(double val) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(2) << std::abs(val);
        return ss.str();
    }

    std::string formatPercent(double val) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << val << "%";
        return ss.str();
    }
}

void registerAnalyticsRoutes(CrowApp& app, Deps& deps) {
    // GET and POST / - Dashboard view (POST supported for 307 redirect follow-through)
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::GET, crow::HTTPMethod::POST).CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        auto userTrades = deps.trades.search(userId, {});
        std::sort(userTrades.begin(), userTrades.end());
        Journal journal(std::move(userTrades));

        auto user = deps.users.findById(userId);
        std::string username = user ? user->username : "Trader";

        const size_t totalTrades = journal.trades().size();
        const double totalPnL = journal.totalPnL();
        const double winRate = journal.winRate();
        const double profitFactor = journal.profitFactor();
        const double maxDrawdown = journal.maxDrawdown();
        const double avgWin = journal.avgWin();
        const double avgLoss = journal.avgLoss();
        const size_t winningTrades = journal.filterByResult(true).size();
        const size_t losingTrades = journal.filterByResult(false).size();

        // If client explicitly requested application/json, return raw metrics object
        std::string acceptHeader = req.get_header_value("Accept");
        if (acceptHeader.find("application/json") != std::string::npos) {
            crow::json::wvalue resJson;
            resJson["total_pnl"] = totalPnL;
            resJson["win_rate"] = winRate;
            resJson["avg_win"] = avgWin;
            resJson["avg_loss"] = avgLoss;
            resJson["trade_count"] = totalTrades;
            resJson["profit_factor"] = profitFactor;
            resJson["max_drawdown"] = maxDrawdown;
            return crow::response(200, resJson);
        }

        // Render dashboard.html template
        crow::mustache::context ctx;
        ctx["username"] = username;
        ctx["has_trades"] = (totalTrades > 0);
        ctx["total_trades"] = totalTrades;
        ctx["winning_trades"] = winningTrades;
        ctx["losing_trades"] = losingTrades;
        ctx["total_pnl"] = formatMoney(totalPnL);
        ctx["is_pnl_positive"] = (totalPnL >= 0.0);
        ctx["win_rate"] = formatPercent(winRate);
        ctx["win_rate_val"] = std::to_string(static_cast<int>(std::round(winRate)));
        ctx["profit_factor"] = (profitFactor < 0.0) ? "N/A" : formatMoney(profitFactor);
        ctx["max_drawdown"] = formatMoney(maxDrawdown);
        ctx["avg_win"] = formatMoney(avgWin);
        ctx["avg_loss"] = formatMoney(avgLoss);

        if (avgLoss > 0.0) {
            ctx["payoff_ratio"] = formatMoney(avgWin / avgLoss);
        } else {
            ctx["payoff_ratio"] = "N/A";
        }

        // Populate recent activity (up to 8 latest trades, newest first)
        const auto& allTrades = journal.trades();
        std::vector<crow::mustache::context> recentList;
        size_t count = 0;
        for (auto it = allTrades.rbegin(); it != allTrades.rend() && count < 8; ++it, ++count) {
            crow::mustache::context item;
            item["id"] = it->id();
            item["date"] = it->tradeDate();
            item["ticker"] = it->ticker();
            item["direction"] = Trade::toString(it->direction());
            item["is_long"] = (it->direction() == Trade::Direction::Long);
            item["entry"] = formatMoney(it->entry());
            item["exit"] = formatMoney(it->exit());
            item["quantity"] = it->quantity();
            item["fees"] = formatMoney(it->fees());
            item["profit"] = formatMoney(it->profit());
            item["is_win"] = it->isWin();
            item["notes"] = it->notes();
            recentList.push_back(item);
        }
        ctx["recent_trades"] = std::move(recentList);

        return crow::response(crow::mustache::load("dashboard.html").render(ctx));
    });

    // GET /api/equity-curve - JSON endpoint for Chart.js
    CROW_ROUTE(app, "/api/equity-curve").CROW_MIDDLEWARES(app, AuthGuard)([&deps](const crow::request& req) {
        int userId = currentUserId(req);
        if (userId <= 0) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        auto userTrades = deps.trades.search(userId, {});
        std::sort(userTrades.begin(), userTrades.end());
        Journal journal(std::move(userTrades));
        auto curve = journal.equityCurve();
        const auto& trades = journal.trades();

        if (trades.empty()) {
            crow::response res(200, "application/json", "[]");
            return res;
        }

        crow::json::wvalue out;
        for (size_t i = 0; i < trades.size(); ++i) {
            out[i]["index"] = static_cast<int>(i + 1);
            out[i]["date"] = trades[i].tradeDate();
            out[i]["ticker"] = trades[i].ticker();
            out[i]["direction"] = Trade::toString(trades[i].direction());
            out[i]["profit"] = trades[i].profit();
            out[i]["equity"] = curve[i];
        }

        return crow::response(200, out);
    });
}
