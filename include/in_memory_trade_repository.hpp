#pragma once
#include "trade_repository.hpp"
#include <map>
#include <mutex>

// ---------------------------------------------------------------------------
// InMemoryTradeRepository – in-memory map-based implementation of ITradeRepository 
// Enables early development and testing for Members A & C without SQLite dependency.
// Thread-safe with internal mutex.
// ---------------------------------------------------------------------------

class InMemoryTradeRepository : public ITradeRepository {
public:
    InMemoryTradeRepository() = default;

    int add(const Trade& trade) override;
    bool update(const Trade& trade) override;
    bool remove(int userId, int tradeId) override;
    std::optional<Trade> findById(int userId, int id) const override;
    std::vector<Trade> search(int userId, const TradeFilter& filter) const override;

    // Helper for testing
    void clear();
    size_t size() const;

private:
    mutable std::mutex mutex_;
    int nextId_{1};
    std::map<int, Trade> trades_;
};
