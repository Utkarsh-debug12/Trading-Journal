-- ---------------------------------------------------------------------------
-- Trading Journal Database Schema
-- ---------------------------------------------------------------------------

PRAGMA foreign_keys = ON;

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL
);

-- Trades table (every trade is scoped to a specific user)
CREATE TABLE IF NOT EXISTS trades (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    ticker TEXT NOT NULL,
    direction TEXT NOT NULL CHECK(direction IN ('Long', 'Short')),
    entry_price REAL NOT NULL,
    exit_price REAL NOT NULL,
    quantity INTEGER NOT NULL,
    fees REAL DEFAULT 0.0,
    trade_date TEXT NOT NULL,
    notes TEXT DEFAULT ''
);

-- Index for fast user trade lookups & chronological sorting
CREATE INDEX IF NOT EXISTS idx_trades_user_date ON trades(user_id, trade_date);
CREATE INDEX IF NOT EXISTS idx_trades_user_ticker ON trades(user_id, ticker);
