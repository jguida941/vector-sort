# Vector Sort

Basic C++ project that demonstrates two classic sorting algorithms over a vector of records loaded from a CSV file:

- Selection Sort (O(n^2))
- Quick Sort (average O(n log n), worst O(n^2))

It includes a small, colorized terminal UI, timing of operations, and a lightweight CSV parser to make the example practical and a bit more fun to explore.

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

You can point the program at a CSV file, or it will default to `eBid_Monthly_Sales.csv` in the working directory.

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

## Project Layout

- `VectorSorting.cpp` — main program, menu UI, sorting, timing, CSV helpers
- `CSVparser.hpp/.cpp` — minimal CSV parser
- `CMakeLists.txt` — build configuration

## Notes

- Terminal colors and ASCII borders are purely cosmetic for readability
- Sorting is performed alphabetically on the bid `title`
- Timing uses `std::chrono::steady_clock` for stable wall-clock measurements

## License

See `License.txt` for license details.

