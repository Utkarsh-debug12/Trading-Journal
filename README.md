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

## 🛠️ Technology Stack
* **Core Engine:** C++ (Object-Oriented Principles, `std::vector`)
* **Binding Layer:** `pybind11` (bridges C++ and Python)
* **Web Framework:** Python (Flask, Jinja2)
* **Frontend UI:** HTML5, CSS (Bootstrap recommended), JavaScript (Chart.js)
* **Database:** SQLite
* **Containerization [NEW]:** Docker (Simplifies setup of the hybrid C++/Python environment)

## 🏗️ System Architecture
The application follows a distinct three-tier architecture emphasizing a clear separation of concerns:

### 1. The Data Tier (SQLite)
A persistent local database storing trade records. The schema is expanded to meet professional expectations:
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

### 2. The Application Tier (Python/Flask)
Acts as the controller and routing mechanism.
* Handles HTTP requests (GET/POST) from the browser.
* Manages user sessions and authentication logic.
* Interacts with the SQLite database to fetch or store data.
* Instantiates C++ objects from the fetched data using the compiled `trading_engine` module.
* Passes the C++ objects to Jinja2 templates for rendering.

### 3. The Processing Tier (C++ Engine)
The analytical powerhouse of the application.
* **Trade Class:** A C++ class representing individual trades, utilizing `const` correctness for read-only metric calculations (e.g., `calculateProfit()`), heavily factoring in fees and direction.
* **Journal Class:** A class utilizing `std::vector<Trade>` to process complex aggregate data efficiently, calculating advanced trading metrics like Profit Factor (gross profit / gross loss), Maximum Drawdown, and Expectancy.
* **pybind11 Wrapper:** Exposes the C++ classes and methods so they can be imported as a standard Python module.

## 🚀 Installation and Setup Guide

### Prerequisites
Ensure the following software is installed on your system:
* A C++ Compiler (GCC, Clang, or MSVC) supporting C++11 or higher.
* Python 3.x
* `pip` (Python package installer)
* *Optional: Docker & Docker Compose (for containerized setup)*

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
   ```

### Compiling the C++ Engine
You must compile the C++ code into a shared object file that Python can import. Run the following command in the terminal within your project directory:

```bash
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) trading_engine.cpp -o trading_engine$(python3-config --extension-suffix)
```
*Note: This command generates a `.so` (Linux/macOS) or `.pyd` (Windows) file.*

### Running the Application
Once the module is compiled, start the Flask development server:
```bash
python app.py
```
Navigate to `http://127.0.0.1:5000` in your web browser to view the application.

## 🧪 Testing [NEW]
* **Python Tier:** Run `pytest tests/` to execute unit tests for Flask routing, authentication, and database logic.
* **C++ Tier:** Include Google Test (`gtest`) to write unit tests for mathematical correctness within the `Trade` and `Journal` classes.

## 🔮 Future Expansion Considerations
* **SQLite Integration Logic:** Currently, database retrieval logic resides in Python for simplicity. For maximum performance in a larger app, the SQLite C API could be integrated directly into the C++ `Journal` class.
* **Handling `std::vector`:** Use `pybind11`'s `<pybind11/stl.h>` header to automatically convert C++ `std::vector` to Python lists when passing data between the languages.
* **Advanced Dashboarding:** Expand the UI with a responsive, non-reloading dashboard utilizing AJAX/WebSockets for real-time analytics updates.
