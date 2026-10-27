-- ---------------------------------------------------------------------------
-- Trading Journal Sample Seed Data (20 Trades for Demo)
-- ---------------------------------------------------------------------------

-- 1. Create default demo user (Password: Password123!)
-- Hash generated with Argon2id via libsodium
INSERT OR IGNORE INTO users (id, username, password_hash)
VALUES (
    1,
    'demo',
    '$argon2id$v=19$m=65536,t=2,p=1$ifxo19ZgXLMEfu13LNsyVA$TGbxGAl1ryipjtAmVnhlSozLMc4QnnXaaw8NEad73JU'
);

-- 2. Seed 20 realistic trades spanning Long/Short positions, diverse tickers,
-- and chronological progression across Jan-Feb 2025 for user_id = 1.
DELETE FROM trades WHERE user_id = 1;

INSERT INTO trades (user_id, ticker, direction, entry_price, exit_price, quantity, fees, trade_date, notes) VALUES
(1, 'SPY',  'Long',  470.00, 475.50, 50,  1.50, '2025-01-02', 'Opening breakout over premarket high'),
(1, 'AAPL', 'Long',  185.00, 192.50, 100, 2.00, '2025-01-05', 'Bull flag continuation on strong volume'),
(1, 'TSLA', 'Short', 245.00, 252.00, 60,  2.00, '2025-01-08', 'Failed breakdown, stopped out at VWAP'),
(1, 'NVDA', 'Long',  480.00, 510.00, 30,  2.00, '2025-01-10', 'AI conference catalyst, strong gap and go'),
(1, 'AMD',  'Long',  138.00, 146.00, 80,  2.00, '2025-01-12', 'Semiconductor sector sympathy follow-through'),
(1, 'MSFT', 'Long',  375.00, 388.00, 40,  1.50, '2025-01-15', 'Cloud revenue growth expectations bounce'),
(1, 'QQQ',  'Short', 405.00, 408.50, 100, 2.00, '2025-01-17', 'Overextended daily RSI hedge, stopped out'),
(1, 'AMZN', 'Long',  152.00, 159.00, 75,  1.50, '2025-01-19', 'E-commerce logistics upgrade catalyst'),
(1, 'META', 'Long',  360.00, 385.00, 35,  2.00, '2025-01-22', 'Ad revenue turnaround breakout'),
(1, 'GOOGL','Short', 142.00, 138.00, 100, 2.00, '2025-01-24', 'Antitrust headlines gap down continuation'),
(1, 'TSLA', 'Long',  215.00, 208.00, 50,  1.50, '2025-01-26', 'Support failed after margins guidance'),
(1, 'NVDA', 'Long',  530.00, 565.00, 25,  2.00, '2025-01-29', 'Data center demand surge continuation'),
(1, 'SPY',  'Short', 490.00, 493.00, 80,  2.00, '2025-01-31', 'FOMC rate decision volatility shakeout'),
(1, 'AAPL', 'Short', 195.00, 189.00, 70,  2.00, '2025-02-02', 'Earnings post-announcement sell-off'),
(1, 'AMD',  'Short', 175.00, 182.00, 40,  2.00, '2025-02-05', 'Reversal failure, early exit on momentum'),
(1, 'MSFT', 'Long',  400.00, 415.00, 30,  1.50, '2025-02-07', 'Copilot enterprise adoption acceleration'),
(1, 'COIN', 'Long',  150.00, 172.00, 40,  2.00, '2025-02-09', 'Crypto ETF approval volume expansion'),
(1, 'PLTR', 'Long',  22.00,  20.50,  300, 3.00, '2025-02-11', 'False breakout above multi-month range'),
(1, 'AMZN', 'Long',  168.00, 178.00, 60,  2.00, '2025-02-13', 'Prime delivery efficiency re-rating'),
(1, 'NVDA', 'Long',  680.00, 725.00, 20,  2.00, '2025-02-15', 'All-time highs blue sky breakout');
