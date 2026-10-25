#include "journal.hpp"
#include <algorithm>
#include <cmath>

Journal::Journal(std::vector<Trade> trades) : trades_(std::move(trades)) {
    // Sort trades chronologically using Trade::operator<
    std::sort(trades_.begin(), trades_.end());

    // Register all 4 default metrics polymorphically
    metrics_.push_back(std::make_unique<WinRate>());
    metrics_.push_back(std::make_unique<ProfitFactor>());
    metrics_.push_back(std::make_unique<MaxDrawdown>());
    metrics_.push_back(std::make_unique<TotalPnL>());
}

void Journal::addMetric(std::unique_ptr<IMetric> m) {
    if (m) {
        metrics_.push_back(std::move(m));
    }
}

std::vector<std::pair<std::string, double>> Journal::computeAll() const {
    std::vector<std::pair<std::string, double>> out;
    out.reserve(metrics_.size());
    for (const auto& m : metrics_) {
        out.emplace_back(m->name(), m->compute(trades_));
    }
    return out;
}

double Journal::totalPnL() const {
    TotalPnL pnl;
    return pnl.compute(trades_);
}

double Journal::avgWin() const {
    double grossProfit = 0.0;
    int winCount = 0;
    for (const auto& t : trades_) {
        if (t.isWin()) {
            grossProfit += t.profit();
            ++winCount;
        }
    }
    return winCount == 0 ? 0.0 : grossProfit / winCount;
}

double Journal::avgLoss() const {
    double grossLoss = 0.0;
    int lossCount = 0;
    for (const auto& t : trades_) {
        if (t.profit() < 0) {
            grossLoss += (-t.profit());
            ++lossCount;
        }
    }
    return lossCount == 0 ? 0.0 : grossLoss / lossCount;
}

double Journal::winRate() const {
    WinRate wr;
    return wr.compute(trades_);
}

double Journal::profitFactor() const {
    ProfitFactor pf;
    return pf.compute(trades_);
}

double Journal::maxDrawdown() const {
    MaxDrawdown dd;
    return dd.compute(trades_);
}

std::vector<double> Journal::equityCurve() const {
    std::vector<double> curve;
    curve.reserve(trades_.size());
    double running = 0.0;
    for (const auto& t : trades_) {
        running += t.profit();
        curve.push_back(running);
    }
    return curve;
}

std::vector<Trade> Journal::filterByResult(bool wins) const {
    std::vector<Trade> filtered;
    for (const auto& t : trades_) {
        if (t.isWin() == wins) {
            filtered.push_back(t);
        }
    }
    return filtered;
}
