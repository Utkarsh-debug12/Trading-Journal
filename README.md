# Trading Journal: Hybrid C++ & Flask Application

## 📖 Project Overview  
The Trading Journal is a web-based application designed for traders to log, track, and analyze their trading history. The core business logic, including complex profit/loss (P&L) calculations and data structuring, is handled by a compiled C++ engine to ensure maximum performance and strict memory management. The user interface and web routing are managed by Python's Flask framework, rendering HTML templates via Jinja2. Data persistence is achieved using a local SQLite database.

## ✨ Core Features & Capabilities
* **Trade Management:** Log, edit, and track historical trades.
* **High-Performance Analytics:** C++ backend computes complex aggregate metrics efficiently (Win Rate, Profit Factor, Maximum Drawdown, Expectancy).
* **Interactive Data Visualization:** View P&L curves and trade distributions via Chart.js integration.
* **Broker CSV Import/Export [NEW]:** Seamlessly import historical trade data from major brokers (Binance, TD Ameritrade, Interactive Brokers) or export for tax purposes.
* **User Authentication [NEW]:** Secure login system enabling multi-user support, keeping individual trading journals private.
* **Live Market Data [NEW]:** Optional integration with financial APIs (e.g., Alpha Vantage, Yahoo Finance) to compare historical trades against live market benchmarks.
* **Risk Management Toolkit [ADDED]:** Pre-trade position-size calculator (based on account risk %, stop-loss distance), automatic risk/reward ratio computation, and Kelly Criterion sizing suggestions.
* **Trade Psychology & Discipline Tracking [ADDED]:** Tag each trade with emotional state (e.g., FOMO, revenge trade, disciplined) and a confidence rating (1–5) to correlate mindset with performance over time.
* **Calendar Heatmap View [ADDED]:** Monthly calendar showing daily net P&L color-coded by gain/loss, similar to a GitHub contribution graph.
* **Watchlist & Price Alerts [ADDED]:** Maintain a watchlist of tickers and set price/percentage-move alerts that trigger email or in-app notifications.
* **Advanced Filtering & Search [ADDED]:** Filter/search the trade log by ticker, date range, strategy tag, direction, win/loss, or emotional tag.
* **Report Export (PDF/CSV) [ADDED]:** Generate polished PDF performance reports (for tax season, mentors, or prop-firm evaluations) in addition to raw CSV export.
* **Real-Time Notifications [ADDED]:** Email/push notifications for drawdown thresholds, streaks, and milestone achievements (e.g., "New equity high").
* **Strategy Backtesting Module [ADDED]:** Replay historical trades or hypothetical rule sets against stored/live price data to estimate strategy performance before risking capital.
* **Trade Replay & Timeline [ADDED]:** Chronological, annotated timeline of a single trade's lifecycle (entry, scale-ins/outs, exit, notes, screenshots).
* **Correlation & Exposure Analysis [ADDED]:** Detect overexposure to correlated tickers/sectors and flag concentration risk.
* **Multi-Currency Support [ADDED]:** Log and convert P&L across different account currencies using live FX rates.
* **Dark Mode / Theming [ADDED]:** User-toggleable light/dark UI themes, persisted per user.
* **Mobile-Responsive / PWA Support [ADDED]:** Installable Progressive Web App with offline trade-entry queuing for logging trades on the go.
* **Role-Based Access Control [ADDED]:** Admin role for managing users, viewing aggregate (anonymized) platform stats, and moderating shared content.
* **Advanced Statistics Suite [ADDED]:** Sharpe Ratio, Sortino Ratio, R-multiple distribution, and win/loss streak tracking alongside the existing Win Rate/Profit Factor/Drawdown/Expectancy metrics.
* **Tax Lot Accounting [ADDED]:** FIFO/LIFO/average-cost lot matching for accurate realized gain/loss calculation, especially for partial fills and scaled entries/exits.
* **Multi-Instrument Support [ADDED]:** First-class handling of options and futures contracts (strike, expiry, contract multiplier, Greeks) in addition to equities and crypto.
* **Pre-Trade Checklists & Templates [ADDED]:** Customizable rule checklists (e.g., "Is this A+ setup? Is risk defined?") that must be acknowledged before logging a new trade, reinforcing discipline.
* **Chart Annotation / TradingView Embed [ADDED]:** Embed a TradingView (or lightweight-charts) widget on each trade's detail page, with the ability to annotate entry/exit points directly on the chart.
* **Full-Text Notes Search [ADDED]:** Search across trade notes/rationale text, not just structured fields like ticker or tag.
* **Data Export & Right-to-be-Forgotten [ADDED]:** Users can export their complete account data (GDPR-style data portability) or request full account deletion.
* **Guided Onboarding [ADDED]:** First-run walkthrough/tutorial and sample seed data to help new users understand the dashboard and metrics.
* **Accessibility (a11y) Support [ADDED]:** Keyboard navigation, ARIA labels, and sufficient color contrast (including in dark mode and the P&L calendar heatmap) to meet WCAG 2.1 AA guidelines.
* **Public REST API [ADDED]:** Token-authenticated API exposing trade CRUD and analytics endpoints so users can build their own integrations or scripts.
* **Webhook Ingestion [ADDED]:** Accept inbound webhooks (e.g., from TradingView alerts) to auto-create draft trade entries pending user confirmation.
* **Duplicate Detection on Import [ADDED]:** CSV/broker imports are checked against existing records (ticker + timestamp + size) to prevent accidental double-entry.
* **Automated Backups [ADDED]:** Scheduled, encrypted database backups (local or to cloud storage) with a one-click restore flow.
* **Localization (i18n) [ADDED]:** UI strings and number/date/currency formatting externalized for translation into additional languages.
* **Subscription Tiers [ADDED]:** Optional Free vs. Pro plan gating (e.g., trade-count limits, advanced analytics, PDF exports reserved for Pro) with Stripe billing integration.
* **Print-Friendly Views [ADDED]:** Clean, ad-free print stylesheet for trade logs and performance reports.

## 🛠️ Technology Stack
* **Core Engine:** C++ (Object-Oriented Principles, `std::vector`)
* **Binding Layer:** `pybind11` (bridges C++ and Python)
* **Web Framework:** Python (Flask, Jinja2)
* **Frontend UI:** HTML5, CSS (Bootstrap recommended), JavaScript (Chart.js)
* **Database:** SQLite
* **Containerization [NEW]:** Docker (Simplifies setup of the hybrid C++/Python environment)
* **Task Queue & Caching [ADDED]:** Celery + Redis for asynchronous jobs (PDF generation, alert checks, broker sync) and API response caching.
* **Real-Time Layer [ADDED]:** Flask-SocketIO (or WebSockets) for live dashboard updates and instant price-alert delivery.
* **PDF Generation [ADDED]:** WeasyPrint or ReportLab for rendering performance reports.
* **Authentication & Security [ADDED]:** Flask-Login for session management, Flask-WTF for CSRF protection, `bcrypt`/`argon2` for password hashing, and TOTP-based 2FA (e.g., `pyotp`).
* **CI/CD [ADDED]:** GitHub Actions pipeline running Python (`pytest`) and C++ (`gtest`) test suites on every push, plus automated Docker image builds.
* **Database Migrations [ADDED]:** Flask-Migrate/Alembic to version-control schema changes safely across environments.
* **Structured Logging & Monitoring [ADDED]:** Python `logging` with JSON formatting, Sentry for error tracking, and a `/health` endpoint for uptime checks (Prometheus-compatible metrics optional).
* **Configuration Management [ADDED]:** `python-dotenv` for environment-specific secrets/config (API keys, DB URLs) kept out of source control via `.env` files.
* **Charting Widget [ADDED]:** TradingView Lightweight Charts (or full TradingView widget) for embedded, annotatable price charts on trade detail pages.
* **API Authentication [ADDED]:** JWT or API-key based auth (`flask-jwt-extended`) for the public REST API, separate from browser session auth.
* **Billing [ADDED]:** Stripe (`stripe-python`) for subscription/payment handling.
* **Internationalization [ADDED]:** `Flask-Babel` for translated strings and locale-aware formatting.
* **Backup Automation [ADDED]:** Scheduled via `cron` or a Celery Beat periodic task, with backups optionally pushed to S3-compatible storage (`boto3`).

## 🏗️ System Architecture
The application follows a distinct three-tier architecture emphasizing a clear separation of concerns:

### 1. The Data Tier (SQLite)
A persistent local database storing trade records. The schema is expanded to meet professional expectations:

**`trades` table**
* `id` (INTEGER PRIMARY KEY)
* `user_id` (INTEGER) - *[NEW] Foreign key for user accounts*
* `ticker` (TEXT)
* `trade_direction` (TEXT) - *[NEW] e.g., Long / Short*
* `entry_price` (REAL)
* `exit_price` (REAL)
* `position_size` (INTEGER)
* `fees` (REAL) - *[NEW] Brokerage and commission fees*
* `strategy_tags` (TEXT) - *[NEW] Tagging for A/B testing different strategies*
* `timestamp` (DATETIME)
* `notes` (TEXT) - *Include trade rationale or attached screenshots*
* `stop_loss` (REAL) - *[ADDED] Used for risk/reward and position-size calculations*
* `take_profit` (REAL) - *[ADDED]*
* `risk_reward_ratio` (REAL) - *[ADDED] Computed at trade entry*
* `emotion_tag` (TEXT) - *[ADDED] e.g., FOMO, Disciplined, Revenge Trade*
* `confidence_rating` (INTEGER) - *[ADDED] 1–5 scale*
* `currency` (TEXT) - *[ADDED] ISO currency code for multi-currency support*
* `instrument_type` (TEXT) - *[ADDED] e.g., Equity, Option, Future, Crypto, Forex*
* `contract_details` (TEXT/JSON) - *[ADDED] Strike, expiry, multiplier, Greeks for options/futures*
* `lot_matching_method` (TEXT) - *[ADDED] FIFO / LIFO / Average-Cost, used for realized P&L on partial fills*

**`checklist_templates` table [ADDED]**
* `id` (INTEGER PRIMARY KEY)
* `user_id` (INTEGER, FK)
* `name` (TEXT) - *e.g., "A+ Setup Checklist"*
* `items` (TEXT/JSON) - *list of checklist questions/criteria*

**`trade_checklist_results` table [ADDED]**
* `id` (INTEGER PRIMARY KEY)
* `trade_id` (INTEGER, FK)
* `template_id` (INTEGER, FK)
* `responses` (TEXT/JSON) - *pass/fail per checklist item at time of entry*

**`users` table [ADDED]**
* `id` (INTEGER PRIMARY KEY)
* `username` (TEXT, UNIQUE)
* `email` (TEXT, UNIQUE)
* `password_hash` (TEXT)
* `totp_secret` (TEXT, NULLABLE) - *for 2FA*
* `role` (TEXT) - *e.g., "user", "admin"*
* `created_at` (DATETIME)

**`watchlist` table [ADDED]**
* `id` (INTEGER PRIMARY KEY)
* `user_id` (INTEGER, FK)
* `ticker` (TEXT)
* `alert_price` (REAL, NULLABLE)
* `alert_direction` (TEXT, NULLABLE) - *"above"/"below"*

**`alerts_log` table [ADDED]**
* `id` (INTEGER PRIMARY KEY)
* `user_id` (INTEGER, FK)
* `message` (TEXT)
* `triggered_at` (DATETIME)
* `read` (BOOLEAN)

### 2. The Application Tier (Python/Flask)
Acts as the controller and routing mechanism.
* Handles HTTP requests (GET/POST) from the browser.
* Manages user sessions and authentication logic.
* Interacts with the SQLite database to fetch or store data.
* Instantiates C++ objects from the fetched data using the compiled `trading_engine` module.
* Passes the C++ objects to Jinja2 templates for rendering.
* **[ADDED]** Enforces CSRF protection on all form submissions and rate-limits sensitive endpoints (login, password reset) via Flask-Limiter.
* **[ADDED]** Dispatches background jobs (PDF report generation, price-alert polling, broker sync) to a Celery worker queue backed by Redis.
* **[ADDED]** Emits real-time events over WebSockets when new trades are logged or alerts trigger.

### 3. The Processing Tier (C++ Engine)
The analytical powerhouse of the application.
* **Trade Class:** A C++ class representing individual trades, utilizing `const` correctness for read-only metric calculations (e.g., `calculateProfit()`), heavily factoring in fees and direction.
* **Journal Class:** A class utilizing `std::vector<Trade>` to process complex aggregate data efficiently, calculating advanced trading metrics like Profit Factor (gross profit / gross loss), Maximum Drawdown, and Expectancy.
* **pybind11 Wrapper:** Exposes the C++ classes and methods so they can be imported as a standard Python module.
* **[ADDED] RiskManager Class:** Computes position sizing from account equity, risk percentage, and stop-loss distance; also derives Kelly Criterion fraction from historical win rate and average win/loss.
* **[ADDED] CorrelationEngine Class:** Calculates pairwise correlation between tickers in the open/historical portfolio to flag concentration risk.
* **[ADDED] Backtester Class:** Iterates historical price series against a defined rule set to simulate strategy performance (returns, drawdown, win rate) prior to live execution.
* **[ADDED] StatsEngine Class:** Computes Sharpe Ratio, Sortino Ratio, R-multiple distributions, and streak statistics from the trade vector.
* **[ADDED] TaxLotMatcher Class:** Applies FIFO/LIFO/average-cost logic to match partial entries and exits for accurate realized gain/loss on scaled trades.

## 🚀 Installation and Setup Guide

### Prerequisites
Ensure the following software is installed on your system:
* A C++ Compiler (GCC, Clang, or MSVC) supporting C++11 or higher.
* Python 3.x
* `pip` (Python package installer)
* *Optional: Docker & Docker Compose (for containerized setup)*
* **[ADDED]** Redis server (for Celery task queue and caching)
* **[ADDED]** A `.env` file for local secrets (DB URL, API keys, `SECRET_KEY`) — never commit this to source control

### Environment Setup

1. **Clone or create the project directory:**
   ```bash
   mkdir trading_journal
   cd trading_journal
   ```

2. **Create a Virtual Environment (Recommended):**
   ```bash
   python3 -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

3. **Install Required Python Packages:**
   ```bash
   pip install flask pybind11 pytest
   # [ADDED] Additional packages for new features:
   pip install flask-login flask-wtf flask-limiter flask-socketio \
               celery redis bcrypt pyotp weasyprint requests
   # [ADDED] Round 2: migrations, config, monitoring, stats
   pip install flask-migrate python-dotenv sentry-sdk numpy pandas
   ```

### Compiling the C++ Engine
You must compile the C++ code into a shared object file that Python can import. Run the following command in the terminal within your project directory:

```bash
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) trading_engine.cpp -o trading_engine$(python3-config --extension-suffix)
```
*Note: This command generates a `.so` (Linux/macOS) or `.pyd` (Windows) file.*

### Database Migrations [ADDED]
Initialize and apply schema migrations before first run (and any time the schema changes):
```bash
flask db init
flask db migrate -m "Initial schema"
flask db upgrade
```

### Running the Application
Once the module is compiled, start the Flask development server:
```bash
python app.py
```
Navigate to `http://127.0.0.1:5000` in your web browser to view the application.

**[ADDED]** To enable background jobs and real-time alerts, also start the Redis server and a Celery worker:
```bash
redis-server &
celery -A app.celery worker --loglevel=info
```

**[ADDED]** Verify the app is healthy via the monitoring endpoint:
```bash
curl http://127.0.0.1:5000/health
```

## 🧪 Testing
* **Python Tier:** Run `pytest tests/` to execute unit tests for Flask routing, authentication, and database logic.
* **C++ Tier:** Include Google Test (`gtest`) to write unit tests for mathematical correctness within the `Trade` and `Journal` classes.
* **[ADDED] Continuous Integration:** GitHub Actions workflow runs `pytest` and `gtest` suites, plus a linter (`flake8`/`cppcheck`), on every pull request.
* **[ADDED] Load Testing:** Use `locust` or `k6` to simulate concurrent users and validate performance of alert polling and report generation under load.

## 🔒 Security Considerations [ADDED]
* **Password Storage:** Passwords are never stored in plaintext; hashed with `bcrypt` or `argon2` and a per-user salt.
* **Two-Factor Authentication (2FA):** Optional TOTP-based 2FA via `pyotp`, compatible with standard authenticator apps.
* **CSRF Protection:** All state-changing forms are protected via Flask-WTF CSRF tokens.
* **Rate Limiting:** Login and password-reset endpoints are rate-limited to mitigate brute-force attacks.
* **Session Security:** Secure, HTTP-only, same-site cookies; session timeout after inactivity.
* **Data Isolation:** Row-level checks ensure a user can only ever query/modify their own `trades`, `watchlist`, and `alerts_log` rows.

## 🔮 Future Expansion Considerations
* **SQLite Integration Logic:** Currently, database retrieval logic resides in Python for simplicity. For maximum performance in a larger app, the SQLite C API could be integrated directly into the C++ `Journal` class.
* **Handling `std::vector`:** Use `pybind11`'s `<pybind11/stl.h>` header to automatically convert C++ `std::vector` to Python lists when passing data between the languages.
* **Advanced Dashboarding:** Expand the UI with a responsive, non-reloading dashboard utilizing AJAX/WebSockets for real-time analytics updates.
* **[ADDED] Social/Community Features:** Optional, privacy-respecting sharing of anonymized trade setups or performance summaries with other users.
* **[ADDED] Migration to PostgreSQL:** For multi-user production deployments at scale, migrate from SQLite to PostgreSQL for better concurrency handling.
* **[ADDED] Direct Broker API Sync:** Move beyond CSV import to live, authenticated sync with broker APIs (e.g., Interactive Brokers TWS API, Alpaca) for automatic trade capture.
* **[ADDED] Native Mobile Companion App:** A lightweight React Native/Flutter app for quick trade logging and push notifications, sharing the same REST API as the web app.
* **[ADDED] AI-Assisted Trade Review:** Optional LLM-generated weekly/monthly summary of trading patterns, mistakes, and strengths based on logged notes and metrics.

## 📄 Project Meta [ADDED]
* **License:** Specify an open-source license (e.g., MIT) in a `LICENSE` file if the project is to be shared publicly.
* **Contributing:** A `CONTRIBUTING.md` outlining coding standards (PEP8 for Python, a C++ style guide), branch naming, and PR review expectations.
* **Changelog:** Maintain a `CHANGELOG.md` following Keep a Changelog conventions to track feature additions like the ones in this document over time.
