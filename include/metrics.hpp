#pragma once
#include "trade.hpp"
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// IMetric – abstract strategy interface for trade analytics (Owner: Member A).
// ---------------------------------------------------------------------------

class IMetric {
public:
    virtual ~IMetric() = default;                       // virtual destructor: safe delete via base pointer
    virtual std::string name() const = 0;               // pure virtual: IMetric is abstract
    virtual double compute(const std::vector<Trade>& trades) const = 0;
};

// ---------------------------------------------------------------------------
// WinRate – computes percentage of winning trades (profit > 0).
// ---------------------------------------------------------------------------

class WinRate final : public IMetric {
public:
    std::string name() const override { return "Win Rate"; }
    double compute(const std::vector<Trade>& trades) const override;
};

// ---------------------------------------------------------------------------
// ProfitFactor – computes ratio of gross profit to gross loss.
// Returns -1.0 if there are no losses (representing N/A).
// ---------------------------------------------------------------------------

class ProfitFactor final : public IMetric {
public:
    std::string name() const override { return "Profit Factor"; }
    double compute(const std::vector<Trade>& trades) const override;
};

// ---------------------------------------------------------------------------
// MaxDrawdown – computes largest drop from a peak in the cumulative equity curve.
// ---------------------------------------------------------------------------

class MaxDrawdown final : public IMetric {
public:
    std::string name() const override { return "Max Drawdown"; }
    double compute(const std::vector<Trade>& trades) const override;
};

// ---------------------------------------------------------------------------
// TotalPnL – computes cumulative net profit/loss across all trades.
// ---------------------------------------------------------------------------

class TotalPnL final : public IMetric {
public:
    std::string name() const override { return "Total PnL"; }
    double compute(const std::vector<Trade>& trades) const override;
};
