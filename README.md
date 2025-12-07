
# Vector Sort

Basic C++ project that demonstrates two classic sorting algorithms over a vector of records loaded from a CSV file:

- Selection Sort (O(n^2))
- Quick Sort (average O(n log n), worst O(n^2))

It includes a small, colorized terminal UI, timing of operations, and a lightweight CSV parser to make the example practical and a bit more fun to explore.

https://github.com/user-attachments/assets/8325803d-8410-4961-9099-ee95fb315056

## What You’ll Learn

- Vectors, structs, and functions in modern C++
- Implementing and comparing Selection Sort vs. Quick Sort
- Recursion (Quick Sort + partition) and in-place swapping
- Basic file I/O and parsing CSV data
- Simple CLI menu loops, input validation, and measuring elapsed time

## Build

This project uses CMake and C++14.

```
# From project root
mkdir -p build
cmake -S . -B build
cmake --build build
```

This produces the executable `build/Vector_Sort`.

## Run

Recommended (macOS): double‑click `scripts/RunVectorSort.command` in Finder. It will
- check for CMake and Xcode Command Line Tools
- build the project into `build/`
- prompt you to choose a CSV (or use the default `data/eBid_Monthly_Sales.csv`)
- run the app and keep the Terminal window open

If you prefer the terminal, you can also run the binary directly. It will default to `data/eBid_Monthly_Sales.csv` if you don't pass a path.

```
# With explicit CSV
./build/Vector_Sort path/to/your.csv

# Or rely on default file name in CWD
./build/Vector_Sort
```

Once running, use the menu to:

- Load bids from the CSV
- Display all bids
- Selection sort all bids
- Quick sort all bids
- View brief algorithm notes/flowcharts
- Add a bid manually (also attempts to append to the CSV)

## CSV Format

The example expects a headered CSV. It maps the following columns:

- Title → column 0
- Auction ID → column 1
- Fund → column 8
- Winning Bid → column 4 (e.g., "$1,234.56")

When adding a bid from the menu, the program appends a row using the same column layout, leaving fields it doesn’t track empty.

Included CSVs (in `data/` folder):
- `data/eBid_Monthly_Sales.csv` — full dataset (~12,025 rows)
- `data/eBid_Monthly_Sales_Dec_2016.csv` — small sample (76 rows)

Important:
- The parser is written for the included eBid CSV schema and column order. If you use your own CSV, it must match this header/column layout; otherwise values may be read incorrectly.
- For a quick start, use one of the included files: `data/eBid_Monthly_Sales.csv` (large) or `data/eBid_Monthly_Sales_Dec_2016.csv` (small).

## Project Layout

```
vector-sort/
├── src/                          # Source code
│   ├── VectorSorting.cpp         # Main program, menu UI, sorting, timing
│   ├── CSVparser.cpp             # CSV parser implementation
│   └── CSVparser.hpp             # CSV parser header
├── data/                         # CSV data files
│   ├── eBid_Monthly_Sales.csv    # Full dataset (~12,025 rows)
│   └── eBid_Monthly_Sales_Dec_2016.csv  # Small sample (76 rows)
├── scripts/                      # Helper scripts
│   └── RunVectorSort.command     # macOS double-click launcher
├── CMakeLists.txt                # CMake build configuration
├── Makefile                      # Convenience targets (make build, make run)
├── README.md
└── License.txt
```

## Notes

- Terminal colors and ASCII borders are purely cosmetic for readability
- Sorting is performed alphabetically on the bid `title`
- Timing uses `std::chrono::steady_clock` for stable wall-clock measurements

## Benchmarks

From a fresh clone and build on Apple Clang (local run), using the included `eBid_Monthly_Sales.csv` (≈12,025 rows):

- Load CSV: ~0.150 s (≈150,000 µs)
- Quick Sort: ~0.030 s (≈29,500 µs)
- Selection Sort: ~3.98 s (≈3,980,000 µs)

Timings will vary by machine, compiler, and background load, but they illustrate the expected O(n log n) vs. O(n^2) behavior.

## License

See `License.txt` for license details.

## macOS Quick Start

- Double‑click `scripts/RunVectorSort.command` in Finder. It will build then run, and let you choose a CSV or use the default `data/eBid_Monthly_Sales.csv`.

If macOS warns that the file is not executable, run once:

```
chmod +x scripts/RunVectorSort.command
```

## Makefile Shortcuts

If you prefer the terminal:

```
make            # builds
make run        # builds then runs (use CSV=/path/to/file.csv to point at a file)
make clean      # removes the build folder
```

Examples:

```
make run CSV=data/eBid_Monthly_Sales.csv
make run CSV=data/eBid_Monthly_Sales_Dec_2016.csv
```
