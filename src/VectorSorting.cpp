//============================================================================
// Name        : VectorSorting.cpp
// Author      : Justin Paul Guida
// Version     : 1.0
// Description : Vector Sorting Algorithms
//============================================================================

#include <algorithm>
#include <iostream>
// Record start/end times using steady_clock to get accurate elapsed duration
// Accounts for OS scheduling, multithreading, and background process interference
#include <chrono> 
#include "CSVparser.hpp"
#include <iomanip> // for std::fixed and std::setprecision (needed for added box prints)
#include <limits>  // for std::numeric_limits used in input pause
#include <cctype>  // for std::isspace used in string cleanup
#include <fstream>  // for std::ofstream used to append to CSV
#include <sstream>  // for std::ostringstream used to format amount


using namespace std;

// ANSI colors for terminal output
const char *CY = "\x1b[36m"; //cyan
const char *GR = "\x1b[32m"; //green
const char *R = "\x1b[0m"; // reset
const char *YL = "\x1b[33m"; //yellow


//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch);

// Small helper so I don't copy/paste the same pause code everywhere.
// Shows a prompt and waits so the user actually has time to read messages to console
void pauseForUser() {
    cout << YL << "Press Enter to return to menu..." << R << std::flush;
    // Discard any already-buffered input (e.g., an accidental extra Enter)
    cin.clear();
    cin.sync();
    // Now wait for a fresh Enter from the user
    std::string _pauseLine;
    std::getline(cin, _pauseLine);
}

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifiers
    string title;
    string fund;
    double amount;

    Bid() {
        amount = 0.0; //initialize amount to 0.0
    }
};

//-----------------------------------------------------------------------------
// Simple CSV helpers
//-----------------------------------------------------------------------------

// Escape a field for CSV output: wrap in quotes if needed and escape quotes
static string csvEscape(const string &field) {
     // checks for special chars
    bool needsQuotes = field.find_first_of(",\n\r\"") != string::npos; 
    // if no special chars, return as is
    if (!needsQuotes) return field; 

    string out; // output string
    // reserve space for efficiency
    out.reserve(field.size() + 2); 
    out.push_back('"'); 
    for (char c: field) {
        // In CSV, quotes are escaped by doubling them
        if (c == '"') out.push_back('"');
        out.push_back(c);
    }
    out.push_back('"'); 
    return out;
}

// Append a bid to the given CSV file using the same column ordering used by loadBids
// Columns (21 total):
static bool appendBidToCsv(const Bid &bid, const string &csvPath) {
    // Open in append mode; don't truncate existing data
    std::ofstream out(csvPath, std::ios::app);
    if (!out.is_open()) {
        return false; // couldn't open file
    }

    // Minimal row: fill known columns; leave others empty
    // Note: loadBids reads Title [0], Auction ID [1], Fund [8], Winning Bid [4]
    // Format amount like the source file (with a leading $)
    std::ostringstream amt;
    amt << '$' << std::fixed << std::setprecision(2) << bid.amount;

    // Build the row with 21 comma-separated columns
    // Keep it straightforward: empty fields are just empty strings between commas
    out
            << csvEscape(bid.title) << ',' // 0 Title
            << csvEscape(bid.bidId) << ',' // 1 Auction ID
            << "" << ',' // 2 Department
            << "" << ',' // 3 Close Date
            << amt.str() << ',' // 4 Winning Bid
            << "" << ',' // 5 CC Fee
            << "" << ',' // 6 Fee Percent
            << "" << ',' // 7 Auction Fee Subtotal
            << csvEscape(bid.fund) << ',' // 8 Fund
            << "" << ',' // 9 Auction Fee Total
            << "" << ',' // 10 Pay Status
            << "" << ',' // 11 Paid Date
            << "" << ',' // 12 Asset #
            << "" << ',' // 13 Inventory ID
            << "" << ',' // 14 Decal /Vehicle ID
            << "" << ',' // 15 VTR Number
            << "" << ',' // 16 Receipt Number
            << "" << ',' // 17 Cap
            << "" << ',' // 18 Expenses
            << "" << ',' // 19 Net Sales
            << "" << "\n"; // 20 Business Unit + newline

    return true;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    // Print with labels similar to CSV, TODO: double check columns
    cout << "Auction ID: " << bid.bidId
            << " | Title: " << bid.title
            << " | Winning Bid: $" << fixed << setprecision(2) << bid.amount
            << " | Fund: " << bid.fund << endl;
    return;
}

/**
 * Prompt user for bid information using console (std::in)
 *
 * @return Bid struct containing the bid info
 */
Bid getBid() {
    Bid bid; // local object to hold bid info

    // Read an ID (could be anything like B-123), so use getline
    // Use CSV column names so inputs match the file format
    cout << "Enter Auction ID: ";
    getline(cin, bid.bidId);

    // Read a title (allow spaces)
    cout << "Enter Auction Title: ";
    getline(cin, bid.title);

    // Read fund as a full line so values like "GENERAL FUND" work
    cout << "Enter Fund: ";
    getline(cin, bid.fund);

    // Read amount as a line (accepts symbols etc: "$1,234.56"). White-space and symbols will be stripped
    cout << "Enter Winning Bid: ";
    string strAmount; // temp string to hold amount input
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$'); // convert to double after  stripping $

    return bid; // hand the fully-filled bid back
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
vector<Bid> loadBids(string csvPath) {
    cout << "Loading CSV file " << csvPath << endl;

    // Define a vector data structure to hold a collection of bids.
    vector<Bid> bids;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        //loop to read rows of a CSV file
        //unsigned int to match csv::Parser rowCount() method
        for (unsigned int i = 0; i < file.rowCount(); i++) {
            // Data structure and add to the collection of bids, **hardcoded for now**
            Bid bid;
            bid.title = file[i][0];
            bid.bidId = file[i][1];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');
            //***TODO: This is hardcoded!***

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bids.push_back(bid);
        }
    } catch (csv::Error &e) {
        // catch any errors, print error message to console
        std::cerr << e.what() << std::endl;
    }
    return bids;
}


/**
 * Partition the vector of bids into two parts, low and high
 *
 * @param bids Address of the vector<Bid> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int partition(vector<Bid> &bids, int begin, int end) {
    //set low and high equal to begin and end
    int low = begin;
    int high = end;

    // Calculate the middle element as middlePoint (int)
    //Pivot point in the middle of the vector
    int middlePoint = begin + (end - begin) / 2;

    // Set Pivot as middlePoint element title to compare (string)
    string pivot = bids[middlePoint].title;


    //bool because done is either true or false
    bool done = false;
    while (!done) {
        // keep incrementing low index while bids[low].title < Pivot
        while (bids[low].title < pivot) {
            low += 1;
        }
        // keep decrementing high index while Pivot < bids[high].title
        while (pivot < bids[high].title) {
            high -= 1;
        }

        /* If there are zero or one elements remaining,
            all bids are partitioned. Return high */
        if (low >= high) {
            done = true;
        } else {
            // else swap the low and high bids (built in vector method)
            swap(bids[low], bids[high]);

            // low needs to go towards high that is -->, then increment ++
            low += 1;
            //high needs to go towards low that is  <--, then decrement --
            high -= 1;
        }
    }
    //return high because it is the last element in low partition
    return high;
}

/**
 * Perform a quick sort on bid title
 * Average performance: O(n log(n))
 * Worst case performance O(n^2))
 *
 * @param bids address of the vector<Bid> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void quickSort(vector<Bid> &bids, int begin, int end) {
    /* Base case: If there are 1 or zero bids to sort,
     partition is already sorted otherwise if begin is greater
     than or equal to end then return*/

    if (begin >= end) {
        //bids are already sorted, return.
        return;
    }

    /* Partition bids into low and high such that
     midpoint is location of last element in low */
    int partitionIndex = partition(bids, begin, end);

    // recursively sort left partition
    quickSort(bids, begin, partitionIndex);
    // recursively sort the right partition
    quickSort(bids, partitionIndex + 1, end);
}


/**
 * Perform a selection sort on bid title
 * Average performance: O(n^2))
 * Worst case performance O(n^2))
 *
 * @param bid address of the vector<Bid>
 *            instance to be sorted
 */
void selectionSort(vector<Bid> &bids) {
    //define minindex of the current minimum bid)
    int minIndex;
    //get size of the bids vector, hold in size_t variable
    size_t size = bids.size();

    
    for (size_t pos = 0; pos < size - 1; ++pos) {
        minIndex = pos;

        //loop over the remaining elements to the right of position; declare as j to avoid confuction with size_t pos
        for (size_t j = pos + 1; j < size; ++j) {
            if (bids[j].title < bids[minIndex].title) {
                minIndex = j;
            }
        }
        // if the minIndex is not the pos swap
        if (minIndex != pos) {
            swap(bids[pos], bids[minIndex]);
        }
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    // Removes the specified character (ex '$')
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    // Removes common numeric formatting characters like commas
    str.erase(remove(str.begin(), str.end(), ','), str.end());
    // Trim spaces just in case (extra whitespace)
    str.erase(remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }), str.end());
    // Convert cleaned string to double
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char *argv[]) {
    // process the command line arguments
    string csvPath;
    switch (argc) {
        case 2:
            csvPath = argv[1]; //argv[1] first argument after executable
            break;
        default:
            csvPath = "data/eBid_Monthly_Sales.csv"; //path to default CSV file. TODO: Allow user to import their own CSV
    }

    // Define a vector to hold all the bids
    vector<Bid> bids;
    // Timer variables: using steady_clock for precise wall-clock measurement instead of CPU ticks
    std::chrono::steady_clock::time_point start, end;
    //Variable to hold the duration in microseconds
    std::chrono::duration<double> duration;

    int choice = 0;
    while (choice != 9) {
        // DASHBOARD HEADER
        std::cout << "\x1b[2J\x1b[H"; // clear screen + home cursor

        // Top dashboard header (shows current state pre-load, post-load, post sort)
        // Border Box and ANSII colors
        std::cout
                << CY << "┌──────────────────────── Vector Sorting Dashboard ────────────────────────┐\n" << R
                << CY << "│ " << R << "Loaded bids: " << GR << bids.size() << R << "\n" // shows how many bids are loaded
                << CY << "│ " << R << "CSV: " << YL << csvPath << R << "\n"            // shows the CSV file in use
                << CY << "└──────────────────────────────────────────────────────────────────────────┘\n" << R;

        // Menu box top
        std::cout << CY
                << "┌───────────────────────────────── MENU ───────────────────────────────────┐\n" << R;
        cout << "  Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Selection Sort All Bids" << endl;
        cout << "  4. Quick Sort All Bids" << endl;
        // New items: explanation (5), flowcharts (6), and add-a-bid (7)
        cout << "  5. Why Quick Sort Is Faster (T(n))" << endl; // Information on Big-O for education
        cout << "  6. View Flowcharts (Selection vs Quick)" << endl; // simple visual steps for both
        cout << "  7. Add a Bid Manually" << endl; // Add a bid to the in-memory list and append to CSV
        cout << "  9. Exit" << endl;
        // Dashboard Menu Bottom Border
        std::cout << CY
                << "└──────────────────────────────────────────────────────────────────────────┘\n" << R;


        //Prompts a choice in YL(yellow), reads choice, STD flush to ensure prompt shows before input
        std::cout << YL << "➤ " << R << "Enter choice: " << GR << std::flush;
        std::string choiceLine;
        std::getline(cin, choiceLine);
        std::cout << R; // resets color

        // trim whitespace
        auto ltrim = [](std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch){ return !std::isspace(ch); }));
        };
        auto rtrim = [](std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
        };
        ltrim(choiceLine); rtrim(choiceLine);

        if (choiceLine.empty()) {
            choice = 0;
        } else {
            try {
                size_t idx = 0;
                int parsed = std::stoi(choiceLine, &idx);
                if (idx != choiceLine.size()) {
                    throw std::invalid_argument("trailing");
                }
                choice = parsed;
            } catch (...) {
                cout << "\n" << "**Invalid input. Please enter a number.**" << "\n";
                pauseForUser();
                choice = 0;
            }
        }

        switch (choice) {
            case 1:
                // Start time with steady_clcck::now(); for start time
                start = std::chrono::steady_clock::now();
            // Load the bids
                bids = loadBids(csvPath);
            //Capture the end time using stead_clock::now(); for end time
                end = std::chrono::steady_clock::now(); // Records the time after loading bids using stead_clock::now()
            // Calculate elapsed time and
                duration = end - start;

            // The bordered summary display the results just once. **Fixed**

            // BORDER (case 1)
                std::cout
                        << "\n" << CY << "┌────────────────────── Load Bids ──────────────────────┐\n" << R
                        << CY << "│ " << R << "Bids processed: " << GR << bids.size() << R << "\n"
                        // Shows bids processed in green
                        << CY << "│ " << R << "Time: " << YL // yellow for time
                        << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
                        //shows duration in microseconds for more prescision
                        << " microseconds" << R << "\n"
                        << CY << "│ " << R << "Seconds: " << YL //
                        << std::fixed << std::setprecision(3) << duration.count()
                        //set precision to 3 decimal places for better readablity
                        << " s" << R << "\n"
                        << CY << "│ " << R << "You may now sort (3 = selection, 4 = quick sort)" << "\n"
                        //User instruction for selection or quick sort
                        << CY << "└───────────────────────────────────────────────────────┘" << R << "\n";
            //  END ADD BORDER

            //break out of case 1 so we don't fall through to case 2
            // Pause and return to menu using helper function
                pauseForUser();
                break;


            case 2:
                // const auto is modern C++11 way ato avoid copying each bid
                for (const auto &bid: bids) {
                    displayBid(bid);
                }
                cout << endl;
                pauseForUser();
                break;

            // FIXME (1b): Invoke the selection sort and report timing results
            case 3:
                // ensure bids arent empty
                if (bids.empty()) {
                    //if empty give user a error message as reminder to load bids fist
                    cout << "**ERROR: No bids loaded. Please press 1 to load bids first.**" << endl;
                    break;
                }

            // start timing selection sort
                start = std::chrono::steady_clock::now();
            // perform selection sort
                selectionSort(bids);
            // stop timing
                end = std::chrono::steady_clock::now();
            // compute elapsed duration
                duration = end - start;
            // bordered summary for selection sort
                std::cout
                        << "\n" << CY << "┌──────────────────── Selection Sort ────────────────────┐\n" << R
                        << CY << "│ " << R << "Bids sorted: " << GR << bids.size() << R << "\n"
                        << CY << "│ " << R << "Time: " << YL
                        << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
                        << " microseconds" << R << "\n"
                        << CY << "│ " << R << "Seconds: " << YL
                        << std::fixed << std::setprecision(3) << duration.count()
                        << " s" << R << "\n"
                        << CY << "└───────────────────────────────────────────────────────┘" << R << "\n";

            //pause helper function
                pauseForUser();
                break;


            //Declare the case 4 option
            case 4:
                //Make sure bids are not empty
                if (bids.empty()) {
                    //Error message printed to console if user didn't load bids
                    cout << "**ERROR: No bids loaded. Please press 1 to load bids first.**" << endl;
                    break;
                }
            //Capture start time for quicksort
                start = std::chrono::steady_clock::now();
            //Invoke quicksort with bids vector, 0, and size -1
                quickSort(bids, 0, bids.size() - 1);
            //Capture end time for quicksort
                end = std::chrono::steady_clock::now();
            //Compute duration
                duration = end - start;

            // bordered summary around quick sort for better UI, formatting, and readability
                std::cout
                        << "\n" << CY << "┌───────────────────── Quick Sort ──────────────────────┐\n" << R
                        << CY << "│ " << R << "Bids sorted: " << GR << bids.size() << R << "\n"
                        // print bids sorted to console
                        << CY << "│ " << R << "Time: " << YL // yellow for time
                        << std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
                        // microseconds like load bids/selection sort for consistency
                        << " microseconds" << R << "\n"
                        << CY << "│ " << R << "Seconds: " << YL //Also show seconds
                        << std::fixed << std::setprecision(3) << duration.count()
                        << " s" << R << "\n"
                        << CY << "└───────────────────────────────────────────────────────┘" << R << "\n";


            // Helper function to pause
                pauseForUser();
                break;

            // New case 5: explain the reason Quicksort is typically faster than Selection Sort
            case 5:
                // Draw a bordered box and print a concise T(n) comparison and rationale
                std::cout
                        // Same full-width border as the dashboard so it lines up.

                        << "\n" << CY <<
                        "┌──────────────────────────────────────────────────────────────────────────┐\n" << R
                        // Title for this section
                        << CY << "│ " << YL << "Why Quick Sort Is Faster (T(n))" << R << "\n"
                        // blank spacer line to make it breathe a bit
                        << CY << "│ " << R << "" << "\n"
                        // Subsection: quick summary of runtime
                        << CY << "│ " << GR << "Complexity Summary" << R << "\n"
                        << CY << "│   • " << R << "Selection Sort:  T(n) = Θ(n^2) (always)" << "\n"
                        // selection always quadratic
                        << CY << "│   • " << R << "Quick Sort:     Expected T(n) = Θ(n log n); worst Θ(n^2)" << "\n"
                        // quicksort average/worst
                        // another spacer
                        << CY << "│ " << R << "" << "\n"
                        // Subsection: why quicksort usually wins
                        << CY << "│ " << GR << "Why Quicksort Wins On Average" << R << "\n"
                        << CY << "│   • " << R << "Divide-and-conquer → about log2(n) levels; each level touches ~n" <<
                        "\n" // levels and work
                        << CY << "│     " << R << "⇒ total ~n log n operations." << "\n" // total work
                        << CY << "│   • " << R << "Selection Sort does nested scans: ~n(n-1)/2 comparisons" << "\n"
                        // nested loops
                        << CY << "│     " << R << "⇒ total ~n^2 operations." << "\n" // total work
                        << CY << "│   • " << R << "For big n, n log n ≪ n^2 (e.g., 10,000: ~133k vs 100M)." << "\n"
                        // quick intuition
                        // spacer
                        << CY << "│ " << R << "" << "\n"
                        // Subsection: practical bits
                        << CY << "│ " << GR << "Practical Notes" << R << "\n"
                        << CY << "│   • " << R << "Pivot choice matters; randomized or median-of-three helps." << "\n"
                        // pivot strategy
                        << CY << "│   • " << R << "Quicksort is in-place and cache-friendly (nice constants)." << "\n"
                        // constant-factor note
                        << CY << "│   • " << R << "With decent pivots, it stays near Θ(n log n)." << "\n" // expectation
                        // Bottom border uses the same width as the header/menu.
                        << CY << "└──────────────────────────────────────────────────────────────────────────┘" << R <<
                        "\n"; // bottom border

            // Helper function to pause
                pauseForUser();
                break;

            // New case 6: Simple ASCII flowcharts for Selection Sort and Quick Sort
            case 6:
                // Top border (same width as the rest).
                std::cout << "\n" << CY <<
                        "┌──────────────────────────────────────────────────────────────────────────┐\n" << R;
            // Title line for the flowchart section in yellow for emphasis
                std::cout << CY << "│ " << YL << "Algorithm Flowcharts" << R << "\n";
            // Spacer for readability
                std::cout << CY << "│ " << R << "" << "\n";
            // Sub-title for Selection Sort flow
                std::cout << CY << "│ " << GR << "Selection Sort Flow" << R << "\n";
            // Flow steps (kept short so they fit the box)
                std::cout << CY << "│   " << R << "Start → i = 0 .. n-2" << "\n";
                std::cout << CY << "│   " << R << "  min = i" << "\n";
                std::cout << CY << "│   " << R << "  for j = i+1 .. n-1" << "\n";
                std::cout << CY << "│   " << R << "    if a[j] < a[min] → min = j" << "\n";
                std::cout << CY << "│   " << R << "  end for" << "\n";
                std::cout << CY << "│   " << R << "  swap(a[i], a[min])" << "\n";
                std::cout << CY << "│   " << R << "repeat i++ → End" << "\n";
            // Spacer between the two charts
                std::cout << CY << "│ " << R << "" << "\n";
            // Sub-title for Quick Sort flow
                std::cout << CY << "│ " << GR << "Quick Sort Flow" << R << "\n";
                std::cout << CY << "│   " << R << "Start → quickSort(A, lo, hi)" << "\n";
                std::cout << CY << "│   " << R << "  if lo >= hi → return" << "\n";
                std::cout << CY << "│   " << R << "  p = partition(A, lo, hi)" << "\n";
                std::cout << CY << "│   " << R << "  quickSort(A, lo, p)" << "\n";
                std::cout << CY << "│   " << R << "  quickSort(A, p+1, hi)" << "\n";
                std::cout << CY << "│   " << R << "End (recursion unwinds)" << "\n";
            // Bottom border (same width).
                std::cout << CY << "└──────────────────────────────────────────────────────────────────────────┘" << R
                        << "\n";
                pauseForUser();
                break;

            // New case 7: manually add a bid to the in-memory list
            case 7: {
                // Get a new bid from the user. getBid() handles strings + amount parsing.
                Bid newBid = getBid(); // collect fields

                // Add it to in-memory list
                bids.push_back(newBid);

                // Try to persist the new bid so it survives reloads and restarts
                bool saved = appendBidToCsv(newBid, csvPath);

                // Show a confirmation box with same border width
                std::cout
                        << "\n" << CY << "┌────────────────────────── Add Bid ──────────────────────────┐\n" << R
                        << CY << "│ " << R << (saved
                                                   ? "New bid added and saved to CSV."
                                                   : "New bid added (could not save to CSV).") << "\n"
                        << CY << "│ " << R << "Total bids in memory: " << GR << bids.size() << R << "\n"
                        << CY << "│ " << R << "Bid: " << YL << newBid.bidId << R << ": " << newBid.title << " | $" <<
                        fixed << setprecision(2) << newBid.amount << " | " << newBid.fund << "\n"
                        << CY << "└──────────────────────────────────────────────────────────────┘" << R << "\n";
                pauseForUser();
                break;
            }
            default: ; // default switch, do nothing for now
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
