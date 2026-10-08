- [ ]

  # Trading Journal: A C++ Web Application (Simplified Mini Project)

* [ ] *Simplified edition: same core idea, about 40% less code, 3 weeks of work plus 1 spare week.*

## Project Overview

```
```

The Trading Journal is a small web application where a trader can **log trades, review them, and see basic performance numbers**. The backend (web server, routing, login, database access, analytics) is written in **C++17**. The browser side is plain HTML, Bootstrap, and one small Chart.js chart.

It is built by a team of 3 students. The goal is to show good C++ and OOP practice (classes, inheritance, polymorphism, RAII, STL, smart pointers, exceptions), not to build a commercial product.

**Language split:** about 90% C++, 10% HTML/CSS/JS. Each member writes roughly **550-700 lines of C++ (A writes the most because A carries the most OOP design)**.

## Objectives

1. Apply object-oriented C++ (classes, inheritance, polymorphism, RAII, STL) to a real problem.
2. Build a working web application in C++ backed by a database.
3. Implement correct, unit-tested analytics (P&L, win rate, profit factor, drawdown).
4. Practise secure basics: password hashing, prepared statements, per-user data isolation.

## Features

| # | Feature | Description |
| --- | --- | --- |
| 1 | **User Accounts** | Register, log in, log out. Passwords are hashed. Each user sees only their own trades. |
| 2 | **Trade Management** | Add, view, edit, delete trades (ticker, direction, entry/exit price, quantity, fees, date, notes). |
| 3 | **Search & Filter** | Filter the trade list by ticker and by win/loss. |
| 4 | **Dashboard** | Total P&L, number of trades, Win Rate, Average Win, Average Loss, Profit Factor, Maximum Drawdown. |
| 5 | **Equity Curve Chart** | One Chart.js line chart fed by a JSON endpoint. |
| 6 | **CSV Export** | Download all your trades as a CSV file. |

**Out of scope:** tags, calculator, CSV import, live data, alerts, 2FA, payments, backtesting, PDF reports, REST tokens, admin panel (see Future Scope).

## Technology Stack

| Layer | Technology |
| --- | --- |
| Language | C++17 |
| Build | CMake |
| Web framework | Crow (single header `crow_all.h`) with Asio |
| Database | SQLite 3 (C API wrapped in your own RAII classes) |
| JSON | `crow::json` (built into Crow) |
| Password hashing | libsodium (`crypto_pwhash_str`, Argon2id) |
| Templates | Crow Mustache (`.html` files) |
| Frontend | Bootstrap 5 + Chart.js from CDN |
| Testing | GoogleTest |

**Dependencies to install: 4** (Crow header, SQLite, libsodium, GoogleTest) plus Asio.

## System Architecture

```
 Browser (HTML + Bootstrap + Chart.js)
        |  HTTP
        v
 +----------------------------------------+
 | Web Layer (Crow)                       |  routes, sessions, templates
 +----------------------------------------+
 | Domain / Service Layer                 |  Trade, Journal, Validators,
 |                                        |  AuthService, CSV export
 +----------------------------------------+
 | Data Layer                             |  Database (RAII), Statement,
 |                                        |  UserRepository, ITradeRepository
 +----------------------------------------+
        |
   trading_journal.db  (SQLite file)
```

`Trade`, `Journal` and the validators know nothing about Crow or SQLite, so they can be unit-tested alone.

### Key Classes

| Class | Owner | Responsibility |
| --- | --- | --- |
| `AppError`, `ValidationError`, `DatabaseError` | A | Exception hierarchy. `ValidationError` also stores the name of the bad field. |
| `Trade` | A | One trade. `const` methods `profit()` and `isWin()`; operators `<`, `==` and `<<`. |
| `TradeFilter` | A | Struct of `std::optional` fields: `ticker`, `winOnly`. |
| `IMetric` | A | Abstract strategy interface: `name()`, `compute(trades)`, virtual destructor. |
| `WinRate`, `ProfitFactor`, `MaxDrawdown`, `TotalPnL` | A | Concrete metrics derived from `IMetric` (`final`, `override`); each holds one formula. |
| `Journal` | A | Owns `std::vector<Trade>` and `std::vector<std::unique_ptr<IMetric>>`; move-only; runs every metric polymorphically; computes averages and the equity curve. |
| `Validators` | A | Server-side form rules; throw `ValidationError`. |
| `Database` | B | RAII wrapper over `sqlite3*`; non-copyable, movable. |
| `Statement` | B | RAII wrapper over `sqlite3_stmt*`; bind, step, typed getters. |
| `UserRepository` | B | `create()`, `findByUsername()`. |
| `ITradeRepository` | B | Abstract storage interface. |
| `SqliteTradeRepository` | B | Real implementation, every query scoped by `user_id`. |
| `InMemoryTradeRepository` | B | `std::map`-based implementation for early development and tests. |
| `AuthService` | C | Hash and verify passwords with libsodium; username/password rules. |
| `AuthGuard` (middleware) | C | Redirects to `/login` when there is no session; `currentUserId(req)`. |

### Database Schema (2 tables)

**`users`**: `id` INTEGER PRIMARY KEY, `username` TEXT UNIQUE NOT NULL, `password_hash` TEXT NOT NULL.

**`trades`**: `id` INTEGER PRIMARY KEY, `user_id` INTEGER NOT NULL (FK to users), `ticker` TEXT NOT NULL, `direction` TEXT NOT NULL (`Long` / `Short`), `entry_price` REAL NOT NULL, `exit_price` REAL NOT NULL, `quantity` INTEGER NOT NULL, `fees` REAL DEFAULT 0, `trade_date` TEXT NOT NULL (`YYYY-MM-DD`), `notes` TEXT.

### Analytics Formulas

- **Profit (Long):** `(exit - entry) x quantity - fees`
- **Profit (Short):** `(entry - exit) x quantity - fees`
- **Win Rate:** `winning trades / total trades x 100`
- **Profit Factor:** `gross profit / gross loss` (show "N/A" if there are no losses)
- **Equity Curve:** running cumulative sum of profits ordered by date
- **Maximum Drawdown:** largest drop from a running peak of the equity curve to a later low

### Web Routes

| Method | Route | Purpose | Owner |
| --- | --- | --- | --- |
| GET/POST | `/register` | Create account | C |
| GET/POST | `/login` | Log in | C |
| GET | `/logout` | Log out | C |
| GET | `/trades` | List with filters | C |
| GET/POST | `/trades/new` | Add trade | C |
| GET/POST | `/trades/<id>/edit` | Edit trade | C |
| POST | `/trades/<id>/delete` | Delete trade | C |
| GET | `/export` | Download CSV | C |
| GET | `/` | Dashboard (metrics + chart) | A |
| GET | `/api/equity-curve` | JSON for Chart.js | A |

Each route file exposes one registration function declared in `routes.hpp`, for example `registerAuthRoutes(app, deps)`. `main.cpp` calls all three.

## Project File Structure

```
trading_journal/
+-- CMakeLists.txt                       [C]
+-- README.md
+-- third_party/crow/crow_all.h
+-- include/
|   +-- errors.hpp                       [A]  AppError, DatabaseError, ValidationError
|   +-- trade.hpp                        [A]  Trade, TradeFilter
|   +-- metrics.hpp                      [A]  IMetric + WinRate, ProfitFactor, MaxDrawdown, TotalPnL
|   +-- journal.hpp                      [A]
|   +-- validators.hpp                   [A]
|   +-- database.hpp                     [B]
|   +-- statement.hpp                    [B]
|   +-- user_repository.hpp              [B]
|   +-- trade_repository.hpp             [B]  ITradeRepository + SqliteTradeRepository
|   +-- in_memory_trade_repository.hpp   [B]
|   +-- auth_service.hpp                 [C]
|   +-- middleware.hpp                   [C]
|   +-- routes.hpp                       [C]
+-- src/
|   +-- trade.cpp  metrics.cpp  journal.cpp  validators.cpp  routes_analytics.cpp      [A]
|   +-- database.cpp  statement.cpp  user_repository.cpp                  [B]
|   +-- trade_repository.cpp  in_memory_trade_repository.cpp              [B]
|   +-- auth_service.cpp  middleware.cpp  routes_auth.cpp                 [C]
|   +-- routes_trades.cpp  (includes /export)  main.cpp                   [C]
+-- templates/
|   +-- dashboard.html                                                    [A]
|   +-- base.html  login.html  register.html  trades.html  trade_form.html [C]
+-- static/
|   +-- js/charts.js                                                      [A]
|   +-- css/style.css                                                     [C]
+-- sql/
|   +-- schema.sql                                                        [B]
|   +-- seed.sql   (20 sample trades for the demo)                        [A]
+-- tests/
    +-- CMakeLists.txt
    +-- test_trade.cpp  test_journal.cpp  test_metrics.cpp  test_validators.cpp             [A]
    +-- test_repository.cpp                                               [B]
    +-- test_auth.cpp                                                     [C]
```

## Installation and Setup

### Ubuntu / Debian / WSL

```bash
sudo apt update
sudo apt install build-essential cmake libsqlite3-dev libsodium-dev \
                 libasio-dev libgtest-dev
```

Download `crow_all.h` from the Crow releases page and place it in `third_party/crow/`.

### Windows / macOS (vcpkg)

```bash
vcpkg install sqlite3 libsodium asio gtest
```

Pass `-DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake` to CMake.

### Build and Run

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/trading_journal
```

Open **http://localhost:18080**. The SQLite file is created on first run from `sql/schema.sql`. To see demo data, register a user and run `sqlite3 trading_journal.db < sql/seed.sql` (edit the `user_id` in the file if needed).

### Run Tests

```bash
cd build && ctest --output-on-failure
```

### Minimal `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)
project(trading_journal LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)
find_library(SODIUM_LIB sodium REQUIRED)

add_library(tj_core
    src/trade.cpp src/metrics.cpp src/journal.cpp src/validators.cpp           # A
    src/database.cpp src/statement.cpp src/user_repository.cpp # B
    src/trade_repository.cpp src/in_memory_trade_repository.cpp
    src/auth_service.cpp)                                      # C
target_include_directories(tj_core PUBLIC include third_party/crow)
target_link_libraries(tj_core PUBLIC SQLite::SQLite3 ${SODIUM_LIB} Threads::Threads)

add_executable(trading_journal
    src/main.cpp src/middleware.cpp src/routes_auth.cpp
    src/routes_trades.cpp src/routes_analytics.cpp)
target_link_libraries(trading_journal PRIVATE tj_core)

enable_testing()
add_subdirectory(tests)
```

## Code Samples

**`trade.hpp`**

```cpp
#pragma once
#include <optional>
#include <ostream>
#include <string>

class Trade {
public:
    enum class Direction { Long, Short };

    Trade(int id, int userId, std::string ticker, Direction dir,
          double entry, double exit, int qty, double fees,
          std::string tradeDate, std::string notes = "");

    double profit() const;                     // const-correct: does not modify the object
    bool   isWin() const { return profit() > 0; }
    const std::string& ticker() const { return ticker_; }
    const std::string& tradeDate() const { return tradeDate_; }
    // other getters ...

    // operator overloading
    bool operator<(const Trade& o) const { return tradeDate_ < o.tradeDate_; }  // std::sort by date
    bool operator==(const Trade& o) const;
    friend std::ostream& operator<<(std::ostream& os, const Trade& t);          // logs and test output

    // static helpers for the enum
    static std::string toString(Direction d);
    static Direction   fromString(const std::string& s);   // throws ValidationError

private:
    int id_, userId_, quantity_;
    std::string ticker_, tradeDate_, notes_;
    Direction direction_;
    double entry_, exit_, fees_;
};

struct TradeFilter {
    std::optional<std::string> ticker;
    std::optional<bool> winOnly;   // true = wins, false = losses, empty = all
};
```

**`metrics.hpp` / `metrics.cpp` (abstract interface + one derived metric)**

```cpp
// metrics.hpp
class IMetric {
public:
    virtual ~IMetric() = default;                       // virtual destructor: safe delete via base pointer
    virtual std::string name() const = 0;               // pure virtual: IMetric is abstract
    virtual double compute(const std::vector<Trade>& trades) const = 0;
};

class MaxDrawdown final : public IMetric {
public:
    std::string name() const override { return "Max Drawdown"; }
    double compute(const std::vector<Trade>& trades) const override;
};
// WinRate, ProfitFactor and TotalPnL are written the same way

// metrics.cpp
double MaxDrawdown::compute(const std::vector<Trade>& trades) const {
    double equity = 0.0, peak = 0.0, maxDd = 0.0;
    for (const auto& t : trades) {            // trades sorted by date
        equity += t.profit();
        peak = std::max(peak, equity);
        maxDd = std::max(maxDd, peak - equity);
    }
    return maxDd;
}
```

**`journal.hpp` / `journal.cpp` (owns the metrics through smart pointers)**

```cpp
// journal.hpp
class Journal {
public:
    explicit Journal(std::vector<Trade> trades);    // sorts by date, registers the 4 default metrics

    Journal(const Journal&) = delete;               // owns unique_ptrs, so no copying
    Journal& operator=(const Journal&) = delete;
    Journal(Journal&&) = default;                   // move-only
    Journal& operator=(Journal&&) = default;

    void addMetric(std::unique_ptr<IMetric> m) { metrics_.push_back(std::move(m)); }
    std::vector<std::pair<std::string, double>> computeAll() const;   // runtime polymorphism

    double winRate() const;          // convenience getters, each delegates to one metric class
    double profitFactor() const;     // returns -1 when there are no losses
    double maxDrawdown() const;
    double totalPnL() const;
    double avgWin() const;
    double avgLoss() const;
    std::vector<double> equityCurve() const;
    std::vector<Trade> filterByResult(bool wins) const;

private:
    std::vector<Trade> trades_;
    std::vector<std::unique_ptr<IMetric>> metrics_;
};

// journal.cpp
Journal::Journal(std::vector<Trade> trades) : trades_(std::move(trades)) {
    std::sort(trades_.begin(), trades_.end());      // uses Trade::operator<
    metrics_.push_back(std::make_unique<WinRate>());
    metrics_.push_back(std::make_unique<ProfitFactor>());
    metrics_.push_back(std::make_unique<MaxDrawdown>());
    metrics_.push_back(std::make_unique<TotalPnL>());
}

std::vector<std::pair<std::string, double>> Journal::computeAll() const {
    std::vector<std::pair<std::string, double>> out;
    for (const auto& m : metrics_)                  // one call, many different metric classes
        out.emplace_back(m->name(), m->compute(trades_));
    return out;
}
```

**A function template with lambdas (`journal.hpp`)**

```cpp
template <typename Pred>
double sumProfit(const std::vector<Trade>& trades, Pred keep) {
    double total = 0.0;
    for (const auto& t : trades)
        if (keep(t)) total += t.profit();
    return total;
}

// used for profit factor: the same template, two different lambdas
double grossProfit = sumProfit(trades_, [](const Trade& t) { return t.profit() > 0; });
double grossLoss   = -sumProfit(trades_, [](const Trade& t) { return t.profit() < 0; });
```

**Exception hierarchy (`errors.hpp`)**

```cpp
class AppError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class ValidationError : public AppError {
public:
    ValidationError(std::string field, const std::string& msg)
        : AppError(field + ": " + msg), field_(std::move(field)) {}
    const std::string& field() const noexcept { return field_; }   // lets the form highlight the bad field
private:
    std::string field_;
};

class DatabaseError : public AppError {
public:
    using AppError::AppError;
};
```

**A Crow route (`routes_analytics.cpp`)**

```cpp
CROW_ROUTE(app, "/api/equity-curve")([&](const crow::request& req) {
    int userId = currentUserId(req);               // C's middleware
    Journal journal(deps.trades.search(userId, {}));   // B's interface
    auto curve = journal.equityCurve();

    crow::json::wvalue out;
    for (size_t i = 0; i < curve.size(); ++i) out[i] = curve[i];
    return crow::response(200, out);
});
```

**A unit test**

```cpp
TEST(JournalTest, ProfitFactorAndDrawdown) {
    Journal j({ Trade(1,1,"AAPL",Trade::Direction::Long, 100,110,10,0,"2025-01-01"),   // +100
                Trade(2,1,"TSLA",Trade::Direction::Long, 200,190,5, 0,"2025-01-02"),   // -50
                Trade(3,1,"INFY",Trade::Direction::Short,50, 40, 10,0,"2025-01-03") });// +100
    EXPECT_DOUBLE_EQ(j.profitFactor(), 4.0);   // 200 / 50
    EXPECT_DOUBLE_EQ(j.maxDrawdown(), 50.0);
}

TEST(MetricTest, CustomMetricRunsThroughJournal) {   // polymorphism check
    struct TradeCount final : IMetric {
        std::string name() const override { return "Trades"; }
        double compute(const std::vector<Trade>& t) const override {
            return static_cast<double>(t.size());
        }
    };
    Journal j({ Trade(1,1,"AAPL",Trade::Direction::Long,100,110,10,0,"2025-01-01") });
    j.addMetric(std::make_unique<TradeCount>());
    EXPECT_DOUBLE_EQ(j.computeAll().back().second, 1.0);
}
```

## Testing Plan

- **Unit tests (GoogleTest):**
  - **A:** `Trade::profit()` for long/short with and without fees; `Trade` operators (`<` sorts by date, `==`, `<<`); each `IMetric` subclass on hand-calculated data, called directly and through `Journal::computeAll()` (polymorphism check); a custom metric added with `addMetric()`; `sumProfit` with different lambdas; edge cases (no trades, no losses, all losses); `Journal` is movable but not copyable; validators throw `ValidationError` with the right field name.
  - **B:** repository CRUD on in-memory SQLite (`:memory:`); "User A cannot read User B's trade"; `search()` filters; the same test suite runs on both repository implementations.
  - **C:** `AuthService` (hash verifies, wrong password fails, duplicate username rejected); a few route tests for login and add-trade.
- **Manual checklist:**
  - Register with a duplicate username shows an error
  - Wrong password fails to log in
  - User A cannot open User B's trade by changing the URL ID
  - Add / edit / delete a trade updates the dashboard
  - Filters return the correct rows
  - Exported CSV opens correctly in Excel
- **Memory check:** build once with `-fsanitize=address,undefined` and click through every page.

## Security Basics

- Passwords hashed with **Argon2id** (libsodium); plaintext is never stored.
- **Prepared statements** everywhere (no SQL injection).
- Every trade query includes `WHERE user_id = ?` so users are isolated.
- Session cookies are HTTP-only; protected routes redirect to `/login`.
- Mustache `{{ }}` escapes user text to prevent XSS.

## C++ / OOP Concepts Demonstrated (for report and viva)

| Concept | Where | Owner |
| --- | --- | --- |
| Classes & encapsulation | `Trade`, `Journal`, repositories | A, B |
| `const` correctness | `Trade::profit() const`, `IMetric::compute() const`, `Journal` getters | A |
| `enum class` and `static` helpers | `Trade::Direction`, `Trade::toString()` / `fromString()` | A |
| Abstract class and pure virtual functions | `IMetric` | A |
| Inheritance & polymorphism | `IMetric` -> `WinRate`, `ProfitFactor`, `MaxDrawdown`, `TotalPnL` (strategy pattern, virtual destructor, `override`, `final`) | A |
| Smart pointers and ownership | `Journal` owns `std::vector<std::unique_ptr<IMetric>>` | A |
| Rule of five, move-only class | `Journal`: copy deleted, move defaulted | A |
| Operator overloading | `Trade::operator<`, `operator==`, `operator<<` | A |
| Templates | `sumProfit()` generic helper | A |
| Lambdas | Predicates passed to `sumProfit`, sort and filter | A |
| STL containers & algorithms | `std::vector`, `std::sort`, `std::accumulate`, `std::count_if` | A |
| Custom exception hierarchy | `AppError` -> `ValidationError` (stores field name), `DatabaseError` (all derive from `std::runtime_error`) | A (defined), all (used) |
| RAII | `Database`, `Statement` | B |
| Move semantics | `Database` and `Statement` are non-copyable, movable | B |
| Inheritance & polymorphism | `ITradeRepository` -> `SqliteTradeRepository`, `InMemoryTradeRepository` | B |
| `std::optional` | `findById()`, `findByUsername()`, `login()` | B, C |
| Smart pointers | `std::unique_ptr<ITradeRepository>`, `std::shared_ptr<Database>`; no raw `new`/`delete` | B, C |
| C-library interop | sqlite3 API (B), libsodium (C) | B, C |
| Middleware & dependency injection | `AuthGuard`, `Deps` struct built in `main.cpp` | C |

## Team Structure (3 Members)

The project is split by layer. Each member owns about 550-700 lines of C++ (A the most), a distinct set of C++ concepts, their own tests, and a small web-facing slice.

|  | **A: Domain & Analytics** | **B: Persistence** | **C: Auth & Web** |
| --- | --- | --- | --- |
| **Theme** | The brain: trade math, metrics, validation, dashboard | The memory: SQLite and repositories | The face: login, HTTP server, trade pages |
| **C++ files** | `errors.hpp`, `trade.*`, `metrics.*`, `journal.*`, `validators.*`, `routes_analytics.cpp` | `database.*`, `statement.*`, `user_repository.*`, `trade_repository.*`, `in_memory_trade_repository.*`, `schema.sql` | `auth_service.*`, `middleware.*`, `routes.hpp`, `main.cpp`, `routes_auth.cpp`, `routes_trades.cpp` |
| **Core concepts** | Inheritance & polymorphism (`IMetric`), abstract classes, `unique_ptr` ownership, move-only class (rule of five), operator overloading, templates, lambdas, `const`, `enum class`, exception hierarchy | RAII, move semantics, polymorphism, `optional`, smart pointers | Dependency injection, middleware, libsodium, string and file output |
| **Frontend** | `dashboard.html`, `charts.js` | None | `base`, `login`, `register`, `trades`, `trade_form`, `style.css` |
| **Tests** | `test_trade`, `test_journal`, `test_metrics`, `test_validators` | `test_repository` | `test_auth`, basic route tests |

### Member A: Domain & Analytics

Responsibilities:

1. `errors.hpp`: exception hierarchy with `AppError` (derived from `std::runtime_error`), `ValidationError` (stores the bad field name) and `DatabaseError`.
2. `Trade` and `TradeFilter`: `profit()`, `isWin()`, getters, `operator<` (sort by date), `operator==`, `operator<<`, and static `toString()` / `fromString()` for `Direction`.
3. `metrics.*`: abstract `IMetric` (`name()`, `compute(trades)`, virtual destructor) with `final` derived classes `WinRate`, `ProfitFactor`, `MaxDrawdown`, `TotalPnL`, all marked `override`.
4. `Journal`: owns `std::vector<Trade>` and `std::vector<std::unique_ptr<IMetric>>`; copy deleted, move defaulted. Provides `computeAll()`, `addMetric()`, `winRate()`, `profitFactor()`, `maxDrawdown()`, `totalPnL()`, `avgWin()`, `avgLoss()`, `equityCurve()`, `filterByResult()`.
5. A generic `sumProfit(trades, predicate)` function template, used with lambdas for gross profit and gross loss.
6. `validators.*`: prices > 0, quantity > 0, valid `YYYY-MM-DD` date, direction is Long or Short; throw `ValidationError` with the field name.
7. `routes_analytics.cpp`: `/` and `/api/equity-curve`.
8. `dashboard.html`, `charts.js`, and `sql/seed.sql`.
9. Unit tests with hand-calculated expected values.

Viva topics: why `profit()` is `const`; how drawdown is computed; why `IMetric` needs a virtual destructor and how `Journal` calls metrics polymorphically; why `Journal` is move-only and what the rule of five says; how `sumProfit` is instantiated with different lambdas; how `std::sort` uses `Trade::operator<`; how `ValidationError` extends the exception hierarchy and where it is caught.

### Member B: Persistence

Responsibilities:

1. `Database`: RAII wrapper over `sqlite3*`; runs `schema.sql` on first start; enables `PRAGMA foreign_keys = ON`.
2. `Statement`: RAII wrapper over `sqlite3_stmt*` with `bind`, `step`, typed column getters.
3. `UserRepository`: `create()`, `findByUsername()` returning `std::optional`.
4. `ITradeRepository` with two implementations: `SqliteTradeRepository` (`add`, `update`, `remove`, `findById`, `search`, all scoped by `user_id`) and `InMemoryTradeRepository` (delivered in Week 1 so A and C are never blocked).
5. Wrap all SQLite failures in `DatabaseError`.
6. Tests on `:memory:`; the same suite runs against both implementations.

Viva topics: RAII and why copy is deleted but move is defined; how prepared statements stop SQL injection; why two repository classes share one interface.

### Member C: Auth & Web

Responsibilities:

1. `AuthService`: `registerUser()`, `login()` using Argon2id (`crypto_pwhash_str` / `crypto_pwhash_str_verify`).
2. `main.cpp`: Crow app with `CookieParser` and `SessionMiddleware`, open the database, build repositories and services, register all route files.
3. Middleware: `AuthGuard` and `currentUserId(req)`.
4. `routes_auth.cpp` (register, login, logout) and `routes_trades.cpp` (CRUD, list with filters, `/export`) using A's validators.
5. Templates and `style.css`.
6. Tests: `test_auth`, a few route tests, and the final demo script.

Viva topics: how Argon2id hashing works; how sessions and middleware work; how dependency injection swaps the in-memory and SQLite repositories.

## Interface Contracts

Agree on these in Phase 0 and do not change them without telling everyone.

| Contract | Owner | Signature (summary) |
| --- | --- | --- |
| `errors.hpp` | A | `AppError` (base), `ValidationError(field, msg)`, `DatabaseError`; all derive from `std::runtime_error` |
| `Trade` | A | Constructor as shown above; getters; `double profit() const`; `operator<`, `operator==`, `operator<<` |
| `TradeFilter` | A | `std::optional<std::string> ticker`; `std::optional<bool> winOnly` |
| `IMetric` | A | `virtual std::string name() const = 0`; `virtual double compute(const std::vector<Trade>&) const = 0`; virtual destructor |
| `Journal` | A | `explicit Journal(std::vector<Trade>)` (move-only); `computeAll()`, `addMetric(std::unique_ptr<IMetric>)`; `winRate()`, `profitFactor()`, `maxDrawdown()`, `totalPnL()`, `avgWin()`, `avgLoss()`, `equityCurve()`, `filterByResult(bool)` |
| `Validators` | A | `Trade validateTradeForm(const FormData&, int userId)`; throws `ValidationError` |
| `ITradeRepository` | B | `int add(const Trade&)`; `bool update(const Trade&)`; `bool remove(int userId, int tradeId)`; `std::optional<Trade> findById(int userId, int id)`; `std::vector<Trade> search(int userId, const TradeFilter&)` |
| `UserRepository` | B | `std::optional<int> create(user, hash)`; `std::optional<User> findByUsername(user)` |
| `AuthService` | C | `std::optional<int> registerUser(user, pass)`; `std::optional<int> login(user, pass)` returns the user id |
| `routes.hpp` | C | `registerAuthRoutes`, `registerTradeRoutes` (C); `registerAnalyticsRoutes` (A); each takes the Crow app and a `Deps` struct |

**Design decisions made up front:**

- Dates are stored as ISO text (`YYYY-MM-DD`), which sorts correctly as a string.
- The win/loss filter runs in memory through `Journal::filterByResult()`, not in SQL.
- Errors are exceptions derived from `std::runtime_error`, caught in the route layer.

## Phase-Wise Development Path (3 Weeks + 1 Spare)

### Phase 0: Setup and Contracts (Days 1-2, everyone together)

| Step | Task | Who |
| --- | --- | --- |
| 0.1 | Create the Git repo with branches `main`, `feature/domain`, `feature/data`, `feature/web`; merge via pull request only | C |
| 0.2 | Install dependencies and get the same CMake build working on all 3 machines | All |
| 0.3 | Smoke tests: Crow "Hello World" (C), SQLite open and create table (B), libsodium hash a string (C) | C, B, C |
| 0.4 | Write `errors.hpp`, `Trade`, `TradeFilter` headers | A |
| 0.5 | Write headers with stub bodies: `ITradeRepository`, `UserRepository` (B); `AuthService`, `routes.hpp` (C) | B, C |
| 0.6 | Agree on coding style, commit message format, and review rotation (A reviews B, B reviews C, C reviews A) | All |

**Checkpoint 0:** everyone can build; all headers are on `main`.

### Phase 1: Foundations in Parallel (Week 1)

|  | Tasks |
| --- | --- |
| **A** | `errors.hpp` hierarchy; `Trade` with `profit()` and the `<` / `==` / `<<` operators; `IMetric` interface and the first derived metric `WinRate`; `Journal` skeleton owning `unique_ptr<IMetric>`s with `computeAll()`, `totalPnL`, `avgWin`, `avgLoss`; first GoogleTest cases (long win, short win, loss, fees, empty journal, `WinRate` through the interface). |
| **B** | `Database` and `Statement` RAII classes; `schema.sql`; `UserRepository`; **`InMemoryTradeRepository`** (push by mid-week); tests on `:memory:`. |
| **C** | Crow skeleton in `main.cpp` with sessions; `AuthService` + `test_auth`; `base`, `login`, `register` templates; route stubs using a hard-coded vector until B's in-memory repository is ready. |

**Checkpoint 1:** `ctest` passes for A, B, and auth; the app runs with login pages and stub routes.

### Phase 2: Core Features (Week 2)

|  | Tasks |
| --- | --- |
| **A** | `ProfitFactor`, `MaxDrawdown`, `TotalPnL` metrics registered in `Journal`; `sumProfit` template; `equityCurve`, `filterByResult`; `validators.*` + tests; `test_metrics` including a custom metric added with `addMetric()`; edge-case tests (no losses, all losses, single trade). |
| **B** | `SqliteTradeRepository` (`add`, `update`, `remove`, `findById`, `search` with bound parameters, all scoped by `user_id`); repository tests including user isolation, run on both implementations. |
| **C** | `/register`, `/login`, `/logout`; `AuthGuard`; trade CRUD routes with A's validators; `trades.html`, `trade_form.html`; switch to `SqliteTradeRepository` once B's tests pass (one-line change in `main.cpp`). |

**Checkpoint 2:** a user can register, log in, add/edit/delete trades stored in SQLite, and sees only their own trades.

### Phase 3: Dashboard, Export and Polish (Week 3)

|  | Tasks |
| --- | --- |
| **A** | `routes_analytics.cpp` (`/`, `/api/equity-curve`); `dashboard.html`, `charts.js`; `sql/seed.sql`. |
| **B** | Connect `search()` to C's filter form; wrap all SQLite errors in `DatabaseError`; security pass (prepared statements and `user_id` checks everywhere). |
| **C** | Filter form on `/trades`; `/export` CSV; error messages; route tests; demo script. |

**Checkpoint 3 (feature complete):** seed data shows correct dashboard numbers and chart; filters work; export downloads a valid CSV.

### Phase 4: Spare Week (Testing and Submission)

| Step | Task | Who |
| --- | --- | --- |
| 4.1 | Run the manual test checklist and log bugs as GitHub issues | All |
| 4.2 | Build with `-fsanitize=address,undefined` and fix problems in own module | Each owner |
| 4.3 | Cross-review (A to B to C to A) for clarity and `const`/RAII correctness | All |
| 4.4 | Write the project report: each member writes the chapter for their own module | All |
| 4.5 | Screenshots, demo rehearsal, tag release `v1.0` | All, C tags |

This week is also your buffer: if Weeks 1-3 slip, use it to finish features instead of polishing.

## Team Workflow Rules

- **Ownership:** only the owner edits their files. Need a change elsewhere? Ask the owner or open a small pull request.
- **Branches:** each member works on a `feature/*` branch and merges via pull request reviewed by the next member in the rotation.
- **Sync:** merge to `main` at every checkpoint; short meeting twice a week.
- **Never break `main`:** it must always build and pass `ctest`.
- **If blocked,** use a stub: B's `InMemoryTradeRepository` or a hard-coded vector of trades.

## Risks and Mitigation

| Risk | Mitigation |
| --- | --- |
| Build differs across machines | Fix it in Phase 0 and write down the exact commands that worked |
| One member waits on another | Interfaces agreed up front; B ships the in-memory repository in Week 1; A's core tests need nobody |
| Merge conflicts | Strict file ownership; one route file per feature |
| B is the critical path for the database switch | C stays on the in-memory repository until B's tests pass |
| Running out of time | Cut CSV export first; it is independent. Core CRUD + dashboard + chart must be done by Checkpoint 3 |

## Limitations

- SQLite suits a few users, not heavy concurrent load.
- Plain HTTP on localhost; production would need HTTPS and CSRF tokens.
- All prices are entered manually; single currency; equities-style P&L only.

## Future Scope

Tags (strategy and emotion) with performance breakdown, position-size calculator, CSV import with duplicate detection, more metrics (expectancy, streaks, Sharpe), live market data and alerts, options/futures, PDF reports, 2FA, REST API tokens, PostgreSQL, Docker and CI/CD.

## Submission Checklist

- [ ] Source code with clear commit history (each member's commits visible)
- [ ] `README.md` and a short project report
- [ ] `sql/seed.sql` for the demo
- [ ] Screenshots of dashboard, trade list, and add-trade form
- [ ] Unit tests passing (`ctest`)
- [ ] Demo script: register, add or seed trades, view dashboard, filter, export

## License

MIT
