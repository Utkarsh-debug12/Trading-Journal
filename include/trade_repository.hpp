#pragma once
#include "trade.hpp"
#include <optional>
#include <vector>

class Database;

// ---------------------------------------------------------------------------
// ITradeRepository – abstract storage interface for trade records 
// All methods that access or mutate trades are scoped by userId for isolation.
// ---------------------------------------------------------------------------

class ITradeRepository {
public:
    virtual ~ITradeRepository() = default;

    // Adds a new trade record for a user. Returns generated primary key trade ID.
    virtual int add(const Trade& trade) = 0;

    // Updates an existing trade record scoped by trade.userId().
    virtual bool update(const Trade& trade) = 0;

    // Removes a trade by ID for the specified user.
    virtual bool remove(int userId, int tradeId) = 0;

    // Finds a trade by its ID, ensuring it belongs to userId.
    virtual std::optional<Trade> findById(int userId, int id) const = 0;

    // Searches/filters trades belonging to userId matching criteria in filter.
    virtual std::vector<Trade> search(int userId, const TradeFilter& filter) const = 0;
};

// ---------------------------------------------------------------------------
// SqliteTradeRepository – persistent SQLite implementation 
// ---------------------------------------------------------------------------

class SqliteTradeRepository : public ITradeRepository {
public:
    SqliteTradeRepository() = default;
    explicit SqliteTradeRepository(Database& db);

    int add(const Trade& trade) override;
    bool update(const Trade& trade) override;
    bool remove(int userId, int tradeId) override;
    std::optional<Trade> findById(int userId, int id) const override;
    std::vector<Trade> search(int userId, const TradeFilter& filter) const override;

private:
    Database* db_{nullptr};
};
