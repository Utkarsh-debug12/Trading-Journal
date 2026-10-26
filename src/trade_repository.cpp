#include "trade_repository.hpp"
#include "database.hpp"
#include "statement.hpp"
#include "errors.hpp"

SqliteTradeRepository::SqliteTradeRepository(Database& db)
    : db_(&db) {}

int SqliteTradeRepository::add(const Trade& trade) {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in SqliteTradeRepository::add()");
    }

    const std::string sql =
        "INSERT INTO trades (user_id, ticker, direction, entry_price, exit_price, quantity, fees, trade_date, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

    Statement stmt(db_->handle(), sql);
    stmt.bind(1, trade.userId());
    stmt.bind(2, trade.ticker());
    stmt.bind(3, Trade::toString(trade.direction()));
    stmt.bind(4, trade.entry());
    stmt.bind(5, trade.exit());
    stmt.bind(6, trade.quantity());
    stmt.bind(7, trade.fees());
    stmt.bind(8, trade.tradeDate());
    stmt.bind(9, trade.notes());
    stmt.execute();

    return static_cast<int>(db_->lastInsertRowId());
}

bool SqliteTradeRepository::update(const Trade& trade) {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in SqliteTradeRepository::update()");
    }

    const std::string sql =
        "UPDATE trades SET ticker = ?, direction = ?, entry_price = ?, exit_price = ?, quantity = ?, "
        "fees = ?, trade_date = ?, notes = ? WHERE id = ? AND user_id = ?;";

    Statement stmt(db_->handle(), sql);
    stmt.bind(1, trade.ticker());
    stmt.bind(2, Trade::toString(trade.direction()));
    stmt.bind(3, trade.entry());
    stmt.bind(4, trade.exit());
    stmt.bind(5, trade.quantity());
    stmt.bind(6, trade.fees());
    stmt.bind(7, trade.tradeDate());
    stmt.bind(8, trade.notes());
    stmt.bind(9, trade.id());
    stmt.bind(10, trade.userId());
    stmt.execute();

    return db_->changes() > 0;
}

bool SqliteTradeRepository::remove(int userId, int tradeId) {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in SqliteTradeRepository::remove()");
    }

    const std::string sql = "DELETE FROM trades WHERE id = ? AND user_id = ?;";
    Statement stmt(db_->handle(), sql);
    stmt.bind(1, tradeId);
    stmt.bind(2, userId);
    stmt.execute();

    return db_->changes() > 0;
}

std::optional<Trade> SqliteTradeRepository::findById(int userId, int id) const {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in SqliteTradeRepository::findById()");
    }

    const std::string sql =
        "SELECT id, user_id, ticker, direction, entry_price, exit_price, quantity, fees, trade_date, notes "
        "FROM trades WHERE id = ? AND user_id = ?;";

    Statement stmt(db_->handle(), sql);
    stmt.bind(1, id);
    stmt.bind(2, userId);

    if (stmt.step()) {
        int tid = stmt.getInt(0);
        int uid = stmt.getInt(1);
        std::string ticker = stmt.getString(2);
        Trade::Direction dir = Trade::fromString(stmt.getString(3));
        double entry = stmt.getDouble(4);
        double exit = stmt.getDouble(5);
        int qty = stmt.getInt(6);
        double fees = stmt.getDouble(7);
        std::string date = stmt.getString(8);
        std::string notes = stmt.getString(9);

        return Trade(tid, uid, std::move(ticker), dir, entry, exit, qty, fees, std::move(date), std::move(notes));
    }

    return std::nullopt;
}

std::vector<Trade> SqliteTradeRepository::search(int userId, const TradeFilter& filter) const {
    if (!db_ || !db_->handle()) {
        throw DatabaseError("Database handle is null or closed in SqliteTradeRepository::search()");
    }

    std::string sql =
        "SELECT id, user_id, ticker, direction, entry_price, exit_price, quantity, fees, trade_date, notes "
        "FROM trades WHERE user_id = ?";

    if (filter.ticker.has_value() && !filter.ticker->empty()) {
        sql += " AND ticker = ? COLLATE NOCASE";
    }
    if (filter.direction.has_value()) {
        sql += " AND direction = ?";
    }
    if (filter.startDate.has_value() && !filter.startDate->empty()) {
        sql += " AND trade_date >= ?";
    }
    if (filter.endDate.has_value() && !filter.endDate->empty()) {
        sql += " AND trade_date <= ?";
    }
    sql += " ORDER BY trade_date ASC, id ASC;";

    Statement stmt(db_->handle(), sql);
    int bindIdx = 1;
    stmt.bind(bindIdx++, userId);

    if (filter.ticker.has_value() && !filter.ticker->empty()) {
        stmt.bind(bindIdx++, *filter.ticker);
    }
    if (filter.direction.has_value()) {
        stmt.bind(bindIdx++, Trade::toString(*filter.direction));
    }
    if (filter.startDate.has_value() && !filter.startDate->empty()) {
        stmt.bind(bindIdx++, *filter.startDate);
    }
    if (filter.endDate.has_value() && !filter.endDate->empty()) {
        stmt.bind(bindIdx++, *filter.endDate);
    }

    std::vector<Trade> out;
    while (stmt.step()) {
        int tid = stmt.getInt(0);
        int uid = stmt.getInt(1);
        std::string ticker = stmt.getString(2);
        Trade::Direction dir = Trade::fromString(stmt.getString(3));
        double entry = stmt.getDouble(4);
        double exit = stmt.getDouble(5);
        int qty = stmt.getInt(6);
        double fees = stmt.getDouble(7);
        std::string date = stmt.getString(8);
        std::string notes = stmt.getString(9);

        Trade t(tid, uid, std::move(ticker), dir, entry, exit, qty, fees, std::move(date), std::move(notes));

        if (filter.winOnly.has_value()) {
            if (*filter.winOnly && !t.isWin()) continue;
            if (!*filter.winOnly && t.isWin()) continue;
        }

        out.push_back(std::move(t));
    }

    return out;
}
