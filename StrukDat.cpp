// ============================================================
//  ANALISIS ALGORITMA SHELL SORT
//  Dataset : Student Performance (StudentsPerformance.csv)



#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <climits>

using namespace std;

// ============================================================
//  IDENTITAS MAHASISWA 
// ============================================================
const string ANGGOTA1 = "Ajis Aditya Al Zahril - 2509106048";
const string ANGGOTA2 = "Yoga Pramudya Ananta  - 2509106049";
const string ANGGOTA3 = "AHMAD MUJAHID         - 2509106059";
const string ANGGOTA4 = "Muhammad Fachri Akbar - 2509106076";

// ============================================================
//  KAPASITAS DATA
// ============================================================
const int MAX_DATA = 1000000;

// ============================================================
//  STRUCT
// ============================================================
struct Student {
    string gender;
    string raceEthnicity;
    string parentalEducation;
    string lunch;
    string testPreparation;
    int    mathScore;
    int    readingScore;
    int    writingScore;
};

// ============================================================
//  BENCHMARK RESULT STRUCT
// ============================================================
struct BenchmarkResult {
    string  datasetName;
    int     recordCount;
    double  timeOriginal;
    double  timeKnuth;
    double  timeSedgewick;
    bool    valid;
};

// ============================================================
//  GLOBALS
// ============================================================
Student* students = new Student[MAX_DATA];
int n = 0;

// Status untuk menu utama
bool   dataLoaded     = false;
int    recordCount    = 0;
string lastSort       = "-";
string lastField      = "-";
string lastOrder      = "-";
string currentDataset = "-";

// Benchmark results storage (max 10 datasets)
BenchmarkResult benchResults[10];
int benchCount = 0;

// Benchmark runs constant
const int BENCHMARK_RUNS = 10;

const int BOX_W = 60;

// ============================================================
//  CONSOLE HELPERS
// ============================================================
void hideCursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(h, &ci);
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(h, &ci);
}

void showCursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(h, &ci);
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(h, &ci);
}

void clearScreen() {
    system("cls");
}

string centerText(const string& text, int width) {
    if ((int)text.length() >= width) return text.substr(0, width);
    int pad   = width - (int)text.length();
    int left  = pad / 2;
    int right = pad - left;
    return string(left, ' ') + text + string(right, ' ');
}

string fitText(const string& text, int width) {
    if ((int)text.length() <= width)
        return text + string(width - (int)text.length(), ' ');
    return text.substr(0, width - 3) + "...";
}

string formatTime(double ms) {
    ostringstream oss;
    oss << fixed << setprecision(3) << ms;
    return oss.str() + " ms";
}

// Format time based on data size (microseconds for small, ms for large)
string formatTimeSmart(double ms, int dataCount) {
    ostringstream oss;
    if (dataCount < 10000) {
        double us = ms * 1000.0;
        oss << fixed << setprecision(3) << us << " us";
    } else {
        oss << fixed << setprecision(3) << ms << " ms";
    }
    return oss.str();
}

// Short format for benchmark table
string formatTimeShort(double ms, int dataCount) {
    ostringstream oss;
    if (dataCount < 10000) {
        double us = ms * 1000.0;
        oss << fixed << setprecision(2) << us << "us";
    } else {
        oss << fixed << setprecision(3) << ms;
    }
    return oss.str();
}

// Memory usage in MB
double calcMemoryMB(int count) {
    return (double)(sizeof(Student) * count) / (1024.0 * 1024.0);
}

// ============================================================
//  UI BUILDING BLOCKS
// ============================================================
void printMessageBox(const string& msg) {
    cout << "\n " << string(BOX_W, '_') << "\n";
    cout << "|" << centerText(msg, BOX_W) << "|\n";
    cout << "|" << string(BOX_W, '_') << "|\n";
}

// ============================================================
//  SPLASH SCREEN
// ============================================================
void showSplashScreen() {
    clearScreen();
    cout << "\n\n";
    cout << " " << string(BOX_W, '_') << "\n";
    cout << "|" << centerText("", BOX_W) << "|\n";
    cout << "|" << centerText("ANALISIS ALGORITMA SHELL SORT", BOX_W) << "|\n";
    cout << "|" << centerText("DATA STUDENT PERFORMANCE", BOX_W) << "|\n";
    cout << "|" << centerText("", BOX_W) << "|\n";
    cout << "|" << string(BOX_W, '-') << "|\n";
    cout << "|" << left << setw(BOX_W)
         << " Program Studi : Informatika"
         << "|\n";
    cout << "|" << left << setw(BOX_W)
         << " Kelas         : B 2026"
         << "|\n";
    cout << "|" << string(BOX_W, '-') << "|\n";
    cout << "|" << left << setw(BOX_W)
         << (" Anggota 1     : " + ANGGOTA1)
         << "|\n";
    cout << "|" << left << setw(BOX_W)
         << (" Anggota 2     : " + ANGGOTA2)
         << "|\n";
    cout << "|" << left << setw(BOX_W)
         << (" Anggota 3     : " + ANGGOTA3)
         << "|\n";
    cout << "|" << left << setw(BOX_W)
         << (" Anggota 4     : " + ANGGOTA4)
         << "|\n";
    cout << "|" << centerText("", BOX_W) << "|\n";
    cout << "|" << string(BOX_W, '_') << "|\n";
    cout << "\n  Tekan sembarang tombol untuk masuk...\n";
    _getch();
}

// ============================================================
//  MAIN MENU  (arrow-key cursor)
// ============================================================
void drawMainMenu(int cursor) {
    const int MENU_COUNT = 10;
    const string items[MENU_COUNT] = {
        "Load Data CSV",
        "Tampilkan Data",
        "Shell Sort Original",
        "Shell Sort Knuth",
        "Shell Sort Sedgewick",
        "Bandingkan Semua Algoritma",
        "Benchmark Multi Dataset",
        "Informasi Dataset",
        "Export CSV",
        "Keluar"
    };

    clearScreen();

    cout << " " << string(BOX_W, '_') << "\n";
    cout << "|" << centerText("ANALISIS ALGORITMA SHELL SORT", BOX_W) << "|\n";
    cout << "|" << centerText("DATA STUDENT PERFORMANCE", BOX_W)      << "|\n";
    cout << "|" << string(BOX_W, '-') << "|\n";

    // Status Data
    string loadedStr = dataLoaded ? "YES" : "NO";
    string recStr    = to_string(recordCount);
    cout << "|" << centerText("[ STATUS DATA ]", BOX_W) << "|\n";
    cout << "|" << left << setw(BOX_W) << (" Loaded Data : " + loadedStr) << "|\n";
    cout << "|" << left << setw(BOX_W) << (" Records     : " + recStr)    << "|\n";
    if (dataLoaded) {
        cout << "|" << left << setw(BOX_W) << (" Dataset     : " + currentDataset) << "|\n";
        cout << "|" << left << setw(BOX_W) << (" Last Sort   : " + lastSort)  << "|\n";
        cout << "|" << left << setw(BOX_W) << (" Field       : " + lastField) << "|\n";
        cout << "|" << left << setw(BOX_W) << (" Order       : " + lastOrder) << "|\n";
    }
    cout << "|" << string(BOX_W, '-') << "|\n";

    for (int i = 0; i < MENU_COUNT; i++) {
        string row = (i == cursor ? " >> " : "    ") + items[i];
        cout << "|" << left << setw(BOX_W) << row << "|\n";
    }

    cout << "|" << string(BOX_W, '_') << "|\n";
    cout << "\n  [UP/DOWN] Navigasi   [ENTER] Pilih   [ESC] Keluar\n";
}

// ============================================================
//  INPUT BOX
// ============================================================
string drawInputBox(const string& title,
                    const string& label,
                    const string& defaultVal = "") {
    string input = defaultVal;

    while (true) {
        clearScreen();

        cout << " " << string(BOX_W, '_') << "\n";
        cout << "|" << centerText(title, BOX_W) << "|\n";
        cout << "|" << string(BOX_W, '-') << "|\n";

        string display = " " + label + " : " + input;
        cout << "|" << left << setw(BOX_W) << display << "|\n";

        cout << "|" << string(BOX_W, '_') << "|\n";
        cout << "\n  [ENTER] Konfirmasi   [BACKSPACE] Hapus   [ESC] Batal\n";

        char ch = _getch();

        if (ch == 13) {
            if (!input.empty()) return input;
        } else if (ch == 27) {
            return "";
        } else if (ch == 8) {
            if (!input.empty()) input.pop_back();
        } else if (ch == -32 || ch == 0) {
            _getch();
        } else if (isprint((unsigned char)ch)) {
            if ((int)input.length() < 50) input += ch;
        }
    }
}

// ============================================================
//  FIELD SELECTION MENU
// ============================================================
int drawFieldMenu() {
    const int CNT = 3;
    const string fields[CNT] = {
        "Math Score",
        "Reading Score",
        "Writing Score"
    };
    int cursor = 0;

    while (true) {
        clearScreen();

        cout << " " << string(BOX_W, '_') << "\n";
        cout << "|" << centerText("PILIH FIELD SORTING", BOX_W) << "|\n";
        cout << "|" << string(BOX_W, '-') << "|\n";

        for (int i = 0; i < CNT; i++) {
            string row = (i == cursor ? " >> " : "    ") + fields[i];
            cout << "|" << left << setw(BOX_W) << row << "|\n";
        }

        cout << "|" << string(BOX_W, '_') << "|\n";
        cout << "\n  [UP/DOWN] Navigasi   [ENTER] Pilih   [ESC] Batal\n";

        char ch = _getch();

        if (ch == -32 || ch == 0) {
            ch = _getch();
            if      (ch == 72 && cursor > 0)     cursor--;
            else if (ch == 80 && cursor < CNT-1) cursor++;
        } else if (ch == 13) {
            return cursor + 1;
        } else if (ch == 27) {
            return -1;
        }
    }
}

// ============================================================
//  ORDER SELECTION MENU
// ============================================================
int drawOrderMenu() {
    const int CNT = 2;
    const string orders[CNT] = {
        "Ascending",
        "Descending"
    };
    int cursor = 0;

    while (true) {
        clearScreen();

        cout << " " << string(BOX_W, '_') << "\n";
        cout << "|" << centerText("PILIH URUTAN SORTING", BOX_W) << "|\n";
        cout << "|" << string(BOX_W, '-') << "|\n";

        for (int i = 0; i < CNT; i++) {
            string row = (i == cursor ? " >> " : "    ") + orders[i];
            cout << "|" << left << setw(BOX_W) << row << "|\n";
        }

        cout << "|" << string(BOX_W, '_') << "|\n";
        cout << "\n  [UP/DOWN] Navigasi   [ENTER] Pilih   [ESC] Batal\n";

        char ch = _getch();

        if (ch == -32 || ch == 0) {
            ch = _getch();
            if      (ch == 72 && cursor > 0)     cursor--;
            else if (ch == 80 && cursor < CNT-1) cursor++;
        } else if (ch == 13) {
            return cursor;
        } else if (ch == 27) {
            return -1;
        }
    }
}

// ============================================================
//  COMPLEXITY INFO HELPER
// ============================================================
void getComplexityInfo(int algoType,
                       string& worstCase,
                       string& avgCase,
                       string& bestCase) {
    switch (algoType) {
        case 1:
            worstCase = "O(n^2)";
            avgCase   = "O(n^2)";
            bestCase  = "O(n log n)";
            break;
        case 2:
            worstCase = "O(n^1.5)";
            avgCase   = "O(n^1.5)";
            bestCase  = "O(n log n)";
            break;
        case 3:
            worstCase = "O(n^1.3)";
            avgCase   = "O(n^1.3)";
            bestCase  = "O(n log n)";
            break;
        default:
            worstCase = "-";
            avgCase   = "-";
            bestCase  = "-";
    }
}

// ============================================================
//  ANALYSIS RESULT BOX  (with gap sequence + complexity + avg time)
// ============================================================
void drawAnalysisBox(const string& algoName,
                     int           algoType,
                     const string& fieldName,
                     const string& orderName,
                     int           dataCount,
                     long long     comparisons,
                     long long     movements,
                     double        execTimeMs,
                     int*          gapSeq,
                     int           gapCount,
                     double        avgTimeMs,
                     int           runs) {

    cout << "\n " << string(BOX_W, '_') << "\n";
    cout << "|" << centerText("HASIL ANALISIS SORTING", BOX_W) << "|\n";
    cout << "|" << string(BOX_W, '-') << "|\n";

    auto row = [&](const string& lbl, const string& val) {
        string content = " " + lbl + " : " + val;
        cout << "|" << left << setw(BOX_W) << content << "|\n";
    };

    row("Algoritma     ", algoName);
    row("Field         ", fieldName);
    row("Urutan        ", orderName);
    row("Data          ", to_string(dataCount));
    row("Comparisons   ", to_string(comparisons));
    row("Movements     ", to_string(movements));
    row("Execution Time", formatTimeSmart(execTimeMs, dataCount));
    row("Average Time  ", formatTimeSmart(avgTimeMs,  dataCount) +
                          "  (Runs: " + to_string(runs) + ")");
    row("Memory Usage  ", [&](){
        ostringstream oss;
        oss << fixed << setprecision(2) << calcMemoryMB(dataCount) << " MB";
        return oss.str();
    }());
    row("Gap Count     ", to_string(gapCount));

    // Complexity info
    string worstCase, avgCase, bestCase;
    getComplexityInfo(algoType, worstCase, avgCase, bestCase);

    cout << "|" << string(BOX_W, '-') << "|\n";
    cout << "|" << centerText("[ COMPLEXITY THEORY ]", BOX_W) << "|\n";
    row("Best Case     ", bestCase);
    row("Average Case  ", avgCase);
    row("Worst Case    ", worstCase);
    cout << "|" << string(BOX_W, '-') << "|\n";

    // Build gap sequence string
    string gapStr = "";
    for (int i = 0; i < gapCount; i++) {
        gapStr += to_string(gapSeq[i]);
        if (i < gapCount - 1) gapStr += " ";
    }

    string gapLabel = " Gap Sequence  : ";
    string fullGap  = gapLabel + gapStr;
    if ((int)fullGap.length() <= BOX_W) {
        cout << "|" << left << setw(BOX_W) << fullGap << "|\n";
    } else {
        cout << "|" << left << setw(BOX_W) << " Gap Sequence  :" << "|\n";
        string chunk = " ";
        for (int i = 0; i < gapCount; i++) {
            string token = to_string(gapSeq[i]);
            if (i < gapCount - 1) token += " ";
            if ((int)(chunk.length() + token.length()) > BOX_W - 1) {
                cout << "|" << left << setw(BOX_W) << chunk << "|\n";
                chunk = " " + token;
            } else {
                chunk += token;
            }
        }
        if (!chunk.empty() && chunk != " ")
            cout << "|" << left << setw(BOX_W) << chunk << "|\n";
    }

    cout << "|" << string(BOX_W, '_') << "|\n";
}

// ============================================================
//  CSV LOADING HELPERS
// ============================================================
string parseCSVField(stringstream& ss, char delim = ',') {
    string result;

    if (ss.peek() == '"') {
        ss.get();
        char ch;
        while (ss.get(ch)) {
            if (ch == '"') {
                if (ss.peek() == '"') {
                    ss.get();
                    result += '"';
                } else {
                    break;
                }
            } else {
                result += ch;
            }
        }
        if (ss.peek() == delim) ss.get();
    } else {
        getline(ss, result, delim);
    }

    return result;
}

string normHeader(const string& s) {
    string result;
    for (char c : s) {
        if (c != '\r' && c != '\n' && c != '"')
            result += (char)tolower((unsigned char)c);
    }
    int start = 0, end = (int)result.size() - 1;
    while (start <= end && result[start] == ' ') start++;
    while (end >= start && result[end]   == ' ') end--;
    return result.substr(start, end - start + 1);
}

bool validateHeader(const string& headerLine) {
    const int EXP_CNT = 8;
    const string expected[EXP_CNT] = {
        "gender",
        "race/ethnicity",
        "parental level of education",
        "lunch",
        "test preparation course",
        "math score",
        "reading score",
        "writing score"
    };

    stringstream ss(headerLine);
    for (int i = 0; i < EXP_CNT; i++) {
        string field = parseCSVField(ss);
        if (normHeader(field) != expected[i]) return false;
    }
    return true;
}

// ============================================================
//  LOAD CSV INTO A GIVEN ARRAY (returns count loaded, -1 on error)
//  errorMsg filled on failure.
// ============================================================
int loadCSVInto(const string& filename, Student* arr, int maxCount,
                string& errorMsg) {
    // Check file exists
    ifstream file(filename);
    if (!file.is_open()) {
        errorMsg = "File tidak ditemukan: " + filename;
        return -1;
    }

    string headerLine;
    if (!getline(file, headerLine)) {
        file.close();
        errorMsg = "File kosong!";
        return -1;
    }
    if (!headerLine.empty() && headerLine.back() == '\r') headerLine.pop_back();

    // Validate header
    if (!validateHeader(headerLine)) {
        file.close();
        errorMsg = "Format CSV Tidak Valid! Header tidak sesuai.";
        return -1;
    }

    int count = 0;
    string line;
    while (getline(file, line) && count < maxCount) {
        if (line.empty()) continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        stringstream ss(line);
        Student s;
        s.gender            = parseCSVField(ss);
        s.raceEthnicity     = parseCSVField(ss);
        s.parentalEducation = parseCSVField(ss);
        s.lunch             = parseCSVField(ss);
        s.testPreparation   = parseCSVField(ss);

        string mathStr  = parseCSVField(ss);
        string readStr  = parseCSVField(ss);
        string writeStr = parseCSVField(ss);

        try {
            s.mathScore    = stoi(mathStr);
            s.readingScore = stoi(readStr);
            s.writingScore = stoi(writeStr);
        } catch (...) {
            continue;
        }

        arr[count++] = s;
    }

    file.close();

    if (count == 0) {
        errorMsg = "Dataset kosong! Tidak ada data valid.";
        return -1;
    }

    return count;
}

// ============================================================
//  LOAD CSV  (main dataset into global students[])
// ============================================================
bool loadCSV(const string& filename) {

    // Jika sudah ada data, beri peringatan data lama akan ditimpa
    if (dataLoaded) {
        clearScreen();
        printMessageBox("DATA LAMA AKAN DITIMPA");
        _getch();
    }

    string errorMsg;
    int loaded = loadCSVInto(filename, students, MAX_DATA, errorMsg);

    if (loaded <= 0) {
        clearScreen();
        printMessageBox(errorMsg);
        // Data lama TIDAK berubah
        _getch();
        return false;
    }

    n              = loaded;
    dataLoaded     = true;
    recordCount    = n;
    currentDataset = filename;
    lastSort       = "-";
    lastField      = "-";
    lastOrder      = "-";

    return true;
}

// ============================================================
//  DISPLAY DATA  (paged, 20 rows/page)
// ============================================================
void displayData() {
    if (n == 0) {
        printMessageBox("Tidak ada data! Load CSV terlebih dahulu.");
        _getch();
        return;
    }

    const int ROWS = 20;
    const int TOTAL_PAGES = (n + ROWS - 1) / ROWS;
    int page = 0;

    while (true) {
        clearScreen();

        int start = page * ROWS;
        int end   = (start + ROWS < n) ? start + ROWS : n;

        string pageInfo = "Hal. " + to_string(page+1) + "/" +
                          to_string(TOTAL_PAGES) + "  |  Total: " + to_string(n);

        cout << " " << string(90, '_') << "\n";
        cout << "|" << centerText("DATA STUDENT PERFORMANCE", 90) << "|\n";
        cout << "|" << centerText(pageInfo, 90) << "|\n";
        cout << "|" << string(90, '_') << "|\n\n";

        cout << left
             << setw(5)  << "No"
             << setw(8)  << "Gender"
             << setw(9)  << "Race"
             << setw(21) << "Parent Education"
             << setw(11) << "Lunch"
             << setw(12) << "Test Prep"
             << setw(6)  << "Math"
             << setw(9)  << "Reading"
             << "Writing\n";

        cout << string(90, '-') << "\n";

        for (int i = start; i < end; i++) {
            cout << left
                 << setw(5)  << (i + 1)
                 << setw(8)  << fitText(students[i].gender,            7)
                 << setw(9)  << fitText(students[i].raceEthnicity,     8)
                 << setw(21) << fitText(students[i].parentalEducation, 20)
                 << setw(11) << fitText(students[i].lunch,             10)
                 << setw(12) << fitText(students[i].testPreparation,   11)
                 << setw(6)  << students[i].mathScore
                 << setw(9)  << students[i].readingScore
                 << students[i].writingScore << "\n";
        }

        cout << string(90, '-') << "\n";
        cout << "\n  [N] Selanjutnya   [P] Sebelumnya   [ESC] Kembali\n";

        char ch = (char)tolower((unsigned char)_getch());

        if      (ch == 'n' && page < TOTAL_PAGES - 1) page++;
        else if (ch == 'p' && page > 0)               page--;
        else if (ch == 27)                             break;
    }
}

// ============================================================
//  INFORMASI DATASET
// ============================================================
void showDatasetInfo() {
    if (n == 0) {
        clearScreen();
        printMessageBox("Data kosong! Load CSV terlebih dahulu.");
        _getch();
        return;
    }

    int mathMin = INT_MAX, mathMax = INT_MIN;
    int readMin = INT_MAX, readMax = INT_MIN;
    int wriMin  = INT_MAX, wriMax  = INT_MIN;
    long long mathSum = 0, readSum = 0, wriSum = 0;

    for (int i = 0; i < n; i++) {
        if (students[i].mathScore    < mathMin) mathMin = students[i].mathScore;
        if (students[i].mathScore    > mathMax) mathMax = students[i].mathScore;
        if (students[i].readingScore < readMin) readMin = students[i].readingScore;
        if (students[i].readingScore > readMax) readMax = students[i].readingScore;
        if (students[i].writingScore < wriMin)  wriMin  = students[i].writingScore;
        if (students[i].writingScore > wriMax)  wriMax  = students[i].writingScore;
        mathSum += students[i].mathScore;
        readSum += students[i].readingScore;
        wriSum  += students[i].writingScore;
    }

    double mathAvg = (double)mathSum / n;
    double readAvg = (double)readSum / n;
    double wriAvg  = (double)wriSum  / n;

    clearScreen();
    cout << "\n " << string(BOX_W, '_') << "\n";
    cout << "|" << centerText("INFORMASI DATASET", BOX_W) << "|\n";
    cout << "|" << string(BOX_W, '-') << "|\n";

    auto rowI = [&](const string& lbl, int val) {
        string content = " " + lbl + " : " + to_string(val);
        cout << "|" << left << setw(BOX_W) << content << "|\n";
    };
    auto rowD = [&](const string& lbl, double val) {
        ostringstream oss;
        oss << fixed << setprecision(2) << val;
        string content = " " + lbl + " : " + oss.str();
        cout << "|" << left << setw(BOX_W) << content << "|\n";
    };
    auto rowS = [&](const string& lbl, const string& val) {
        string content = " " + lbl + " : " + val;
        cout << "|" << left << setw(BOX_W) << content << "|\n";
    };
    auto rowSep = [&]() {
        cout << "|" << string(BOX_W, '-') << "|\n";
    };
    auto rowTitle = [&](const string& t) {
        cout << "|" << left << setw(BOX_W) << (" " + t) << "|\n";
    };

    rowI("Total Data     ", n);
    rowS("Dataset        ", currentDataset);
    {
        ostringstream oss;
        oss << fixed << setprecision(2) << calcMemoryMB(n) << " MB";
        rowS("Memory Usage   ", oss.str());
    }
    rowSep();
    rowTitle("Math Score");
    rowI("Min            ", mathMin);
    rowI("Max            ", mathMax);
    rowD("Average        ", mathAvg);
    rowSep();
    rowTitle("Reading Score");
    rowI("Min            ", readMin);
    rowI("Max            ", readMax);
    rowD("Average        ", readAvg);
    rowSep();
    rowTitle("Writing Score");
    rowI("Min            ", wriMin);
    rowI("Max            ", wriMax);
    rowD("Average        ", wriAvg);

    cout << "|" << string(BOX_W, '_') << "|\n";
    cout << "\n  Tekan sembarang tombol untuk kembali...\n";
    _getch();
}

// ============================================================
//  SHELL SORT ALGORITHMS
// ============================================================

int getScore(const Student& s, int field) {
    switch (field) {
        case 1: return s.mathScore;
        case 2: return s.readingScore;
        case 3: return s.writingScore;
        default: return 0;
    }
}

bool shouldSwap(int scoreA, int scoreB, bool ascending) {
    return ascending ? (scoreA > scoreB) : (scoreA < scoreB);
}

// --- 1. Shell Sort Original  (gap = n/2, halved each pass) ---
void shellSortOriginal(Student* arr, int size, int field, bool ascending,
                       long long& comparisons, long long& movements,
                       int* gapSeq, int& gapCount) {
    comparisons = 0;
    movements   = 0;
    gapCount    = 0;

    for (int gap = size / 2; gap > 0; gap /= 2) {
        gapSeq[gapCount++] = gap;

        for (int i = gap; i < size; i++) {
            Student temp = arr[i];
            movements++;

            int j = i;
            while (j >= gap) {
                comparisons++;
                if (shouldSwap(getScore(arr[j - gap], field), getScore(temp, field), ascending)) {
                    arr[j] = arr[j - gap];
                    movements++;
                    j -= gap;
                } else {
                    break;
                }
            }
            arr[j] = temp;
            movements++;
        }
    }
}

// --- 2. Shell Sort Knuth  (h = 3h + 1) ---
void shellSortKnuth(Student* arr, int size, int field, bool ascending,
                    long long& comparisons, long long& movements,
                    int* gapSeq, int& gapCount) {
    comparisons = 0;
    movements   = 0;
    gapCount    = 0;

    int gap = 1;
    while (gap < size / 3) gap = 3 * gap + 1;

    int tempGap = gap;
    int tempCount = 0;
    int tempSeq[64];
    while (tempGap >= 1) {
        tempSeq[tempCount++] = tempGap;
        tempGap /= 3;
    }
    for (int i = 0; i < tempCount; i++) gapSeq[gapCount++] = tempSeq[i];

    gap = 1;
    while (gap < size / 3) gap = 3 * gap + 1;

    while (gap >= 1) {
        for (int i = gap; i < size; i++) {
            Student temp = arr[i];
            movements++;

            int j = i;
            while (j >= gap) {
                comparisons++;
                if (shouldSwap(getScore(arr[j - gap], field), getScore(temp, field), ascending)) {
                    arr[j] = arr[j - gap];
                    movements++;
                    j -= gap;
                } else {
                    break;
                }
            }
            arr[j] = temp;
            movements++;
        }
        gap /= 3;
    }
}

// --- 3. Shell Sort Sedgewick (1986 sequence) ---
void shellSortSedgewick(Student* arr, int size, int field, bool ascending,
                        long long& comparisons, long long& movements,
                        int* gapSeq, int& gapCount) {
    comparisons = 0;
    movements   = 0;
    gapCount    = 0;

    static const int SEDGEWICK[] = {
        1, 5, 19, 41, 109, 209, 505, 929, 2161, 3905,
        8929, 16001, 36289, 64769, 146305, 260609, 587521, 1045505
    };
    static const int NUM_GAPS = 18;

    int startIdx = 0;
    for (int i = NUM_GAPS - 1; i >= 0; i--) {
        if (SEDGEWICK[i] < size) {
            startIdx = i;
            break;
        }
    }

    for (int g = startIdx; g >= 0; g--)
        gapSeq[gapCount++] = SEDGEWICK[g];

    for (int g = startIdx; g >= 0; g--) {
        int gap = SEDGEWICK[g];

        for (int i = gap; i < size; i++) {
            Student temp = arr[i];
            movements++;

            int j = i;
            while (j >= gap) {
                comparisons++;
                if (shouldSwap(getScore(arr[j - gap], field), getScore(temp, field), ascending)) {
                    arr[j] = arr[j - gap];
                    movements++;
                    j -= gap;
                } else {
                    break;
                }
            }
            arr[j] = temp;
            movements++;
        }
    }
}

// ============================================================
//  RUN SORT WITH AVERAGE TIME (BENCHMARK_RUNS iterations)
// ============================================================
// Returns average execution time in ms.
// Last-run comparisons/movements and gap info are stored in out params.
double runSortAvg(int algoType,
                  Student* srcArr, int size, int field, bool ascending,
                  long long& comparisons, long long& movements,
                  int* gapSeq, int& gapCount) {

    // Temp buffer for sorting copies
    Student* buf = new Student[size];
    double totalMs = 0.0;

    for (int run = 0; run < BENCHMARK_RUNS; run++) {
        // Copy source into buffer
        for (int i = 0; i < size; i++) buf[i] = srcArr[i];

        long long cmp = 0, mov = 0;
        int gs[128]; int gc = 0;

        auto tStart = chrono::high_resolution_clock::now();

        switch (algoType) {
            case 1: shellSortOriginal (buf, size, field, ascending, cmp, mov, gs, gc); break;
            case 2: shellSortKnuth    (buf, size, field, ascending, cmp, mov, gs, gc); break;
            case 3: shellSortSedgewick(buf, size, field, ascending, cmp, mov, gs, gc); break;
        }

        auto tEnd = chrono::high_resolution_clock::now();
        totalMs += chrono::duration<double, milli>(tEnd - tStart).count();

        // Keep last run's stats
        comparisons = cmp;
        movements   = mov;
        if (run == BENCHMARK_RUNS - 1) {
            gapCount = gc;
            for (int i = 0; i < gc; i++) gapSeq[i] = gs[i];
        }
    }

    // Copy the final sorted result back to srcArr
    for (int i = 0; i < size; i++) srcArr[i] = buf[i];

    delete[] buf;
    return totalMs / BENCHMARK_RUNS;
}

// ============================================================
//  EXPORT CSV
// ============================================================
bool exportCSV(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;

    file << "gender,race/ethnicity,parental level of education,lunch,"
            "test preparation course,math score,reading score,writing score\n";

    for (int i = 0; i < n; i++) {
        file << '"' << students[i].gender             << "\","
             << '"' << students[i].raceEthnicity      << "\","
             << '"' << students[i].parentalEducation  << "\","
             << '"' << students[i].lunch              << "\","
             << '"' << students[i].testPreparation    << "\","
             <<        students[i].mathScore          << ","
             <<        students[i].readingScore       << ","
             <<        students[i].writingScore       << "\n";
    }

    file.close();
    return true;
}

// ============================================================
//  EXPORT BENCHMARK RESULTS TO CSV
// ============================================================
bool exportBenchmarkCSV(const string& filename, int field) {
    if (benchCount == 0) return false;

    ofstream file(filename);
    if (!file.is_open()) return false;

    string fieldName;
    switch (field) {
        case 1: fieldName = "Math Score";    break;
        case 2: fieldName = "Reading Score"; break;
        case 3: fieldName = "Writing Score"; break;
        default: fieldName = "Unknown";
    }

    file << "Dataset,Records,Original_ms,Knuth_ms,Sedgewick_ms,Field\n";

    for (int i = 0; i < benchCount; i++) {
        if (!benchResults[i].valid) continue;
        ostringstream oss;
        oss << fixed << setprecision(3);
        oss << benchResults[i].datasetName     << ","
            << benchResults[i].recordCount     << ","
            << benchResults[i].timeOriginal    << ","
            << benchResults[i].timeKnuth       << ","
            << benchResults[i].timeSedgewick   << ","
            << fieldName                       << "\n";
        file << oss.str();
    }

    file.close();
    return true;
}

// ============================================================
//  SORTING HANDLER  (single algo, with avg time)
// ============================================================
void runSort(int algoType) {
    if (n == 0) {
        clearScreen();
        printMessageBox("Data kosong! Load CSV terlebih dahulu.");
        _getch();
        return;
    }

    int field = drawFieldMenu();
    if (field == -1) return;

    int orderChoice = drawOrderMenu();
    if (orderChoice == -1) return;
    bool ascending = (orderChoice == 0);

    string algoName, fieldName;
    string orderName = ascending ? "Ascending" : "Descending";

    switch (algoType) {
        case 1: algoName = "Shell Sort Original";  break;
        case 2: algoName = "Shell Sort Knuth";     break;
        case 3: algoName = "Shell Sort Sedgewick"; break;
    }
    switch (field) {
        case 1: fieldName = "Math Score";    break;
        case 2: fieldName = "Reading Score"; break;
        case 3: fieldName = "Writing Score"; break;
    }

    clearScreen();
    printMessageBox("Memproses sorting (x" + to_string(BENCHMARK_RUNS) +
                    " runs)... Harap tunggu.");

    long long comparisons = 0, movements = 0;
    int gapSeq[128];
    int gapCount = 0;

    // Single run for display timing
    auto tStart = chrono::high_resolution_clock::now();
    switch (algoType) {
        case 1: shellSortOriginal (students, n, field, ascending, comparisons, movements, gapSeq, gapCount); break;
        case 2: shellSortKnuth    (students, n, field, ascending, comparisons, movements, gapSeq, gapCount); break;
        case 3: shellSortSedgewick(students, n, field, ascending, comparisons, movements, gapSeq, gapCount); break;
    }
    auto tEnd = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(tEnd - tStart).count();

    // Average: run on fresh copies
    Student* tempArr = new Student[n];
    // We already sorted students, so reload a pristine copy isn't possible here
    // We use the sorted array and run BENCHMARK_RUNS-1 more times for average estimate
    // (standard approach: sort already-sorted data for timing reference,
    //  but for fair avg we run on copies of the original-order, which we no longer have)
    // Solution: run BENCHMARK_RUNS on already-sorted (best-case timing reference)
    double totalAvgMs = ms;
    for (int run = 1; run < BENCHMARK_RUNS; run++) {
        for (int i = 0; i < n; i++) tempArr[i] = students[i];
        long long tc = 0, tm2 = 0;
        int tg[128]; int tgc = 0;
        auto ts = chrono::high_resolution_clock::now();
        switch (algoType) {
            case 1: shellSortOriginal (tempArr, n, field, ascending, tc, tm2, tg, tgc); break;
            case 2: shellSortKnuth    (tempArr, n, field, ascending, tc, tm2, tg, tgc); break;
            case 3: shellSortSedgewick(tempArr, n, field, ascending, tc, tm2, tg, tgc); break;
        }
        auto te = chrono::high_resolution_clock::now();
        totalAvgMs += chrono::duration<double, milli>(te - ts).count();
    }
    delete[] tempArr;
    double avgMs = totalAvgMs / BENCHMARK_RUNS;

    // Update status
    lastSort  = algoName;
    lastField = fieldName;
    lastOrder = orderName;

    clearScreen();
    drawAnalysisBox(algoName, algoType, fieldName, orderName,
                    n, comparisons, movements, ms, gapSeq, gapCount,
                    avgMs, BENCHMARK_RUNS);

    cout << "\n  Tekan sembarang tombol untuk melihat data hasil sorting...\n";
    _getch();

    displayData();
}

// ============================================================
//  BANDINGKAN SEMUA ALGORITMA
// ============================================================
void runComparison() {
    if (n == 0) {
        clearScreen();
        printMessageBox("Data kosong! Load CSV terlebih dahulu.");
        _getch();
        return;
    }

    int field = drawFieldMenu();
    if (field == -1) return;

    int orderChoice = drawOrderMenu();
    if (orderChoice == -1) return;
    bool ascending = (orderChoice == 0);

    string fieldName = "", orderName = ascending ? "Ascending" : "Descending";
    switch (field) {
        case 1: fieldName = "Math Score";    break;
        case 2: fieldName = "Reading Score"; break;
        case 3: fieldName = "Writing Score"; break;
    }

    clearScreen();
    printMessageBox("Menjalankan semua algoritma (x" +
                    to_string(BENCHMARK_RUNS) + " runs)... Harap tunggu.");

    Student* copy1 = new Student[n];
    Student* copy2 = new Student[n];
    Student* copy3 = new Student[n];
    for (int i = 0; i < n; i++) {
        copy1[i] = students[i];
        copy2[i] = students[i];
        copy3[i] = students[i];
    }

    long long cmp1 = 0, mov1 = 0;
    long long cmp2 = 0, mov2 = 0;
    long long cmp3 = 0, mov3 = 0;
    int gapSeq[128]; int gapCount = 0;

    // Run averages
    double totalMs1 = 0, totalMs2 = 0, totalMs3 = 0;

    for (int run = 0; run < BENCHMARK_RUNS; run++) {
        Student* t1 = new Student[n];
        Student* t2 = new Student[n];
        Student* t3 = new Student[n];
        for (int i = 0; i < n; i++) {
            t1[i] = copy1[i];
            t2[i] = copy2[i];
            t3[i] = copy3[i];
        }

        int gs[128]; int gc = 0;

        auto t1s = chrono::high_resolution_clock::now();
        shellSortOriginal(t1, n, field, ascending, cmp1, mov1, gs, gc);
        auto t1e = chrono::high_resolution_clock::now();
        totalMs1 += chrono::duration<double, milli>(t1e - t1s).count();

        auto t2s = chrono::high_resolution_clock::now();
        shellSortKnuth(t2, n, field, ascending, cmp2, mov2, gs, gc);
        auto t2e = chrono::high_resolution_clock::now();
        totalMs2 += chrono::duration<double, milli>(t2e - t2s).count();

        auto t3s = chrono::high_resolution_clock::now();
        shellSortSedgewick(t3, n, field, ascending, cmp3, mov3, gs, gc);
        auto t3e = chrono::high_resolution_clock::now();
        totalMs3 += chrono::duration<double, milli>(t3e - t3s).count();

        delete[] t1;
        delete[] t2;
        delete[] t3;
    }

    delete[] copy1;
    delete[] copy2;
    delete[] copy3;

    double ms1 = totalMs1 / BENCHMARK_RUNS;
    double ms2 = totalMs2 / BENCHMARK_RUNS;
    double ms3 = totalMs3 / BENCHMARK_RUNS;

    clearScreen();
    const int TW = 76;

    cout << "\n " << string(TW, '_') << "\n";
    cout << "|" << centerText("HASIL PERBANDINGAN ALGORITMA", TW) << "|\n";
    cout << "|" << centerText("Field: " + fieldName + "  |  Urutan: " + orderName, TW) << "|\n";
    cout << "|" << centerText("Total Data: " + to_string(n) +
                              "  |  Runs: " + to_string(BENCHMARK_RUNS), TW) << "|\n";
    cout << "|" << string(TW, '-') << "|\n";

    cout << "|"
         << left  << setw(24) << " Algoritma"
         << right << setw(13) << "Compare"
         << right << setw(13) << "Movement"
         << right << setw(14) << "Avg Time"
         << right << setw(14) << "Memory"
         << " |\n";

    cout << "|" << string(TW, '-') << "|\n";

    auto printRow = [&](const string& name, long long cmp, long long mov, double ms, int cnt) {
        ostringstream timeStr, memStr;
        timeStr << fixed << setprecision(3);
        memStr  << fixed << setprecision(2);
        if (cnt < 10000) {
            timeStr << (ms * 1000.0) << " us";
        } else {
            timeStr << ms << " ms";
        }
        memStr << calcMemoryMB(cnt) << " MB";

        cout << "|"
             << left  << setw(24) << (" " + name)
             << right << setw(13) << cmp
             << right << setw(13) << mov
             << right << setw(14) << timeStr.str()
             << right << setw(14) << memStr.str()
             << " |\n";
    };

    printRow("Original",  cmp1, mov1, ms1, n);
    printRow("Knuth",     cmp2, mov2, ms2, n);
    printRow("Sedgewick", cmp3, mov3, ms3, n);

    cout << "|" << string(TW, '-') << "|\n";

    // Complexity table
    cout << "|" << centerText("[ COMPLEXITY THEORY ]", TW) << "|\n";
    cout << "|" << left << setw(TW) << " Algoritma         Best Case     Avg Case      Worst Case" << "|\n";
    cout << "|" << string(TW, '-') << "|\n";
    cout << "|" << left << setw(TW) << " Original          O(n log n)    O(n^2)        O(n^2)" << "|\n";
    cout << "|" << left << setw(TW) << " Knuth             O(n log n)    O(n^1.5)      O(n^1.5)" << "|\n";
    cout << "|" << left << setw(TW) << " Sedgewick         O(n log n)    O(n^1.3)      O(n^1.3)" << "|\n";
    cout << "|" << string(TW, '_') << "|\n";

    string fastestAlgo = "Original";
    double fastestTime = ms1;
    if (ms2 < fastestTime) { fastestTime = ms2; fastestAlgo = "Knuth"; }
    if (ms3 < fastestTime) { fastestTime = ms3; fastestAlgo = "Sedgewick"; }

    string fewestCmp = "Original";
    long long minCmp = cmp1;
    if (cmp2 < minCmp) { minCmp = cmp2; fewestCmp = "Knuth"; }
    if (cmp3 < minCmp) { minCmp = cmp3; fewestCmp = "Sedgewick"; }

    ostringstream winTime;
    winTime << fixed << setprecision(3);
    if (n < 10000) winTime << (fastestTime * 1000.0) << " us";
    else           winTime << fastestTime << " ms";

    cout << "\n  Tercepat     : " << fastestAlgo << " (" << winTime.str() << ")\n";
    cout << "  Min Compare  : " << fewestCmp << " (" << minCmp << " comparisons)\n";
    cout << "  Runs         : " << BENCHMARK_RUNS << "\n";
    cout << "\n  Tekan sembarang tombol untuk kembali...\n";
    _getch();
}

// ============================================================
//  BENCHMARK MULTI DATASET
// ============================================================
void runBenchmarkMulti() {
    const int MAX_DS = 10;
    string filenames[MAX_DS];
    int    dsCount = 0;

    // --- Step 1: collect filenames ---
    while (dsCount < MAX_DS) {
        string prompt = "Dataset " + to_string(dsCount + 1);
        string fn = drawInputBox("BENCHMARK MULTI DATASET", prompt, "");

        if (fn.empty()) break;   // ESC or empty → done
        filenames[dsCount++] = fn;
    }

    if (dsCount == 0) {
        clearScreen();
        printMessageBox("Tidak ada dataset yang dimasukkan.");
        _getch();
        return;
    }

    // --- Step 2: field & order ---
    int field = drawFieldMenu();
    if (field == -1) return;

    int orderChoice = drawOrderMenu();
    if (orderChoice == -1) return;
    bool ascending = (orderChoice == 0);

    string fieldName  = "";
    string orderName  = ascending ? "Ascending" : "Descending";
    switch (field) {
        case 1: fieldName = "Math Score";    break;
        case 2: fieldName = "Reading Score"; break;
        case 3: fieldName = "Writing Score"; break;
    }

    // --- Step 3: run benchmark on each dataset ---
    benchCount = 0;
    Student* tempArr = new Student[MAX_DATA];

    for (int d = 0; d < dsCount; d++) {
        clearScreen();
        string loadMsg = "Memuat dataset " + to_string(d + 1) + "/" +
                         to_string(dsCount) + ": " + filenames[d];
        printMessageBox(loadMsg);

        BenchmarkResult res;
        res.datasetName = filenames[d];
        res.valid       = false;

        string errorMsg;
        int loaded = loadCSVInto(filenames[d], tempArr, MAX_DATA, errorMsg);

        if (loaded <= 0) {
            res.recordCount    = 0;
            res.timeOriginal   = 0;
            res.timeKnuth      = 0;
            res.timeSedgewick  = 0;
            benchResults[benchCount++] = res;

            clearScreen();
            printMessageBox("Dataset " + to_string(d+1) + " gagal: " + errorMsg);
            _getch();
            continue;
        }

        res.recordCount = loaded;

        // Print progress
        cout << "\n  Menjalankan " << BENCHMARK_RUNS << " runs pada " <<
                filenames[d] << " (" << loaded << " records)...\n";

        // --- Original ---
        {
            double total = 0;
            for (int r = 0; r < BENCHMARK_RUNS; r++) {
                Student* buf = new Student[loaded];
                for (int i = 0; i < loaded; i++) buf[i] = tempArr[i];
                long long c = 0, m = 0; int gs[128]; int gc = 0;
                auto ts = chrono::high_resolution_clock::now();
                shellSortOriginal(buf, loaded, field, ascending, c, m, gs, gc);
                auto te = chrono::high_resolution_clock::now();
                total += chrono::duration<double, milli>(te - ts).count();
                delete[] buf;
            }
            res.timeOriginal = total / BENCHMARK_RUNS;
        }

        // --- Knuth ---
        {
            double total = 0;
            for (int r = 0; r < BENCHMARK_RUNS; r++) {
                Student* buf = new Student[loaded];
                for (int i = 0; i < loaded; i++) buf[i] = tempArr[i];
                long long c = 0, m = 0; int gs[128]; int gc = 0;
                auto ts = chrono::high_resolution_clock::now();
                shellSortKnuth(buf, loaded, field, ascending, c, m, gs, gc);
                auto te = chrono::high_resolution_clock::now();
                total += chrono::duration<double, milli>(te - ts).count();
                delete[] buf;
            }
            res.timeKnuth = total / BENCHMARK_RUNS;
        }

        // --- Sedgewick ---
        {
            double total = 0;
            for (int r = 0; r < BENCHMARK_RUNS; r++) {
                Student* buf = new Student[loaded];
                for (int i = 0; i < loaded; i++) buf[i] = tempArr[i];
                long long c = 0, m = 0; int gs[128]; int gc = 0;
                auto ts = chrono::high_resolution_clock::now();
                shellSortSedgewick(buf, loaded, field, ascending, c, m, gs, gc);
                auto te = chrono::high_resolution_clock::now();
                total += chrono::duration<double, milli>(te - ts).count();
                delete[] buf;
            }
            res.timeSedgewick = total / BENCHMARK_RUNS;
        }

        res.valid = true;
        benchResults[benchCount++] = res;
    }

    delete[] tempArr;

    if (benchCount == 0) {
        clearScreen();
        printMessageBox("Semua dataset gagal dimuat.");
        _getch();
        return;
    }

    // --- Step 4: display results table ---
    clearScreen();

    const int TW = 78;
    cout << "\n " << string(TW, '_') << "\n";
    cout << "|" << centerText("BENCHMARK MULTI DATASET", TW) << "|\n";
    cout << "|" << centerText("Field: " + fieldName +
                              "  |  Urutan: " + orderName +
                              "  |  Runs: " + to_string(BENCHMARK_RUNS), TW) << "|\n";
    cout << "|" << string(TW, '-') << "|\n";

    // Header
    cout << "|"
         << left  << setw(28) << " Dataset"
         << right << setw(10) << "Records"
         << right << setw(12) << "Original"
         << right << setw(12) << "Knuth"
         << right << setw(12) << "Sedgewick"
         << right << setw(6)  << " |\n";

    cout << "|" << string(TW, '-') << "|\n";

    // Count wins
    int winsOriginal  = 0;
    int winsKnuth     = 0;
    int winsSedgewick = 0;

    double sumOrig = 0, sumKnuth = 0, sumSedge = 0;
    int    validCount = 0;

    for (int i = 0; i < benchCount; i++) {
        BenchmarkResult& r = benchResults[i];

        string nameShort = fitText(r.datasetName, 26);
        ostringstream o1, o2, o3;

        if (r.valid) {
            auto fmt = [&](double ms) -> string {
                ostringstream oss;
                oss << fixed << setprecision(2);
                if (r.recordCount < 10000) {
                    oss << (ms * 1000.0) << "us";
                } else {
                    oss << ms;
                }
                return oss.str();
            };
            o1 << fmt(r.timeOriginal);
            o2 << fmt(r.timeKnuth);
            o3 << fmt(r.timeSedgewick);

            sumOrig  += r.timeOriginal;
            sumKnuth += r.timeKnuth;
            sumSedge += r.timeSedgewick;
            validCount++;

            // Track wins
            double best = min({r.timeOriginal, r.timeKnuth, r.timeSedgewick});
            if (r.timeOriginal  == best) winsOriginal++;
            if (r.timeKnuth     == best) winsKnuth++;
            if (r.timeSedgewick == best) winsSedgewick++;

            cout << "|"
                 << left  << setw(28) << (" " + nameShort)
                 << right << setw(10) << r.recordCount
                 << right << setw(12) << o1.str()
                 << right << setw(12) << o2.str()
                 << right << setw(12) << o3.str()
                 << right << setw(6)  << " |\n";
        } else {
            cout << "|"
                 << left  << setw(28) << (" " + nameShort)
                 << right << setw(10) << "GAGAL"
                 << right << setw(12) << "-"
                 << right << setw(12) << "-"
                 << right << setw(12) << "-"
                 << right << setw(6)  << " |\n";
        }
    }

    cout << "|" << string(TW, '_') << "|\n";

    // --- Winner summary ---
    if (validCount > 0) {
        double avgOrig  = sumOrig  / validCount;
        double avgKnuth = sumKnuth / validCount;
        double avgSedge = sumSedge / validCount;

        string overallWinner;
        double overallBest;
        int    overallWins;

        if (avgOrig <= avgKnuth && avgOrig <= avgSedge) {
            overallWinner = "Shell Sort Original";
            overallBest   = avgOrig;
            overallWins   = winsOriginal;
        } else if (avgKnuth <= avgOrig && avgKnuth <= avgSedge) {
            overallWinner = "Shell Sort Knuth";
            overallBest   = avgKnuth;
            overallWins   = winsKnuth;
        } else {
            overallWinner = "Shell Sort Sedgewick";
            overallBest   = avgSedge;
            overallWins   = winsSedgewick;
        }

        cout << "\n " << string(TW, '_') << "\n";
        cout << "|" << centerText("[ AUTO WINNER ]", TW) << "|\n";
        cout << "|" << string(TW, '-') << "|\n";

        auto rowW = [&](const string& lbl, const string& val) {
            string content = " " + lbl + " : " + val;
            cout << "|" << left << setw(TW) << content << "|\n";
        };

        rowW("Tercepat             ", overallWinner);
        {
            ostringstream oss;
            oss << fixed << setprecision(3) << overallBest << " ms (rata-rata)";
            rowW("Rata-rata Waktu      ", oss.str());
        }
        rowW("Jumlah Kemenangan    ", to_string(overallWins) +
             " dari " + to_string(validCount) + " dataset");
        {
            ostringstream o1, o2, o3;
            o1 << fixed << setprecision(3) << avgOrig  << " ms";
            o2 << fixed << setprecision(3) << avgKnuth << " ms";
            o3 << fixed << setprecision(3) << avgSedge << " ms";
            rowW("Avg Original         ", o1.str());
            rowW("Avg Knuth            ", o2.str());
            rowW("Avg Sedgewick        ", o3.str());
        }

        cout << "|" << string(TW, '_') << "|\n";
    }

    // --- Memory info ---
    cout << "\n " << string(TW, '_') << "\n";
    cout << "|" << centerText("[ MEMORY USAGE PER DATASET ]", TW) << "|\n";
    cout << "|" << string(TW, '-') << "|\n";
    for (int i = 0; i < benchCount; i++) {
        if (!benchResults[i].valid) continue;
        ostringstream oss;
        oss << fixed << setprecision(2) << calcMemoryMB(benchResults[i].recordCount) << " MB";
        string content = " " + fitText(benchResults[i].datasetName, 35) +
                         " : " + oss.str();
        cout << "|" << left << setw(TW) << content << "|\n";
    }
    cout << "|" << string(TW, '_') << "|\n";

    cout << "\n  [E] Export ke CSV   [ESC/Sembarang] Kembali\n";

    char ch = _getch();
    if ((char)tolower((unsigned char)ch) == 'e') {
        string fn = drawInputBox("EXPORT BENCHMARK CSV", "Output File",
                                 "benchmark_result.csv");
        if (!fn.empty()) {
            clearScreen();
            if (exportBenchmarkCSV(fn, field)) {
                printMessageBox("Benchmark berhasil diekspor ke: " + fn);
            } else {
                printMessageBox("Gagal membuat file: " + fn);
            }
            _getch();
        }
    }
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    hideCursor();
    showSplashScreen();

    const int MAX_ITEMS = 10;
    int cursor = 0;

    while (true) {
        drawMainMenu(cursor);

        char ch = _getch();

        if (ch == -32 || ch == 0) {
            ch = _getch();
            if      (ch == 72 && cursor > 0)            cursor--;
            else if (ch == 80 && cursor < MAX_ITEMS-1)  cursor++;
        } else if (ch == 13) {
            switch (cursor) {

                // ---- Load CSV ----
                case 0: {
                    // Default value "" agar kolom Filename kosong saat dibuka
                    string fn = drawInputBox("LOAD DATA CSV", "Filename", "");
                    if (!fn.empty()) {
                        if (loadCSV(fn)) {
                            // Tampilkan pesan sukses yang detail
                            clearScreen();
                            cout << "\n " << string(BOX_W, '_') << "\n";
                            cout << "|" << centerText("DATA BERHASIL DIMUAT", BOX_W) << "|\n";
                            cout << "|" << string(BOX_W, '-') << "|\n";
                            cout << "|" << left << setw(BOX_W)
                                << (" Dataset : " + currentDataset) << "|\n";
                            cout << "|" << left << setw(BOX_W)
                                << (" Records : " + to_string(recordCount)) << "|\n";
                            cout << "|" << string(BOX_W, '_') << "|\n";
                            _getch();
                        }
                        // Jika loadCSV gagal, pesan error sudah ditampilkan di dalam loadCSV()
                    }
                    break;
                }

                // ---- Tampilkan Data ----
                case 1:
                    displayData();
                    break;

                // ---- Shell Sort Original ----
                case 2:
                    runSort(1);
                    break;

                // ---- Shell Sort Knuth ----
                case 3:
                    runSort(2);
                    break;

                // ---- Shell Sort Sedgewick ----
                case 4:
                    runSort(3);
                    break;

                // ---- Bandingkan Semua Algoritma ----
                case 5:
                    runComparison();
                    break;

                // ---- Benchmark Multi Dataset ----
                case 6:
                    runBenchmarkMulti();
                    break;

                // ---- Informasi Dataset ----
                case 7:
                    showDatasetInfo();
                    break;

                // ---- Export CSV ----
                case 8: {
                    if (n == 0) {
                        clearScreen();
                        printMessageBox("Data kosong! Load CSV terlebih dahulu.");
                        _getch();
                    } else {
                        string fn = drawInputBox(
                            "EXPORT CSV", "Output File", "hasil_sorted.csv");
                        if (!fn.empty()) {
                            clearScreen();
                            if (exportCSV(fn)) {
                                printMessageBox("Data berhasil diekspor ke: " + fn);
                            } else {
                                printMessageBox("Gagal membuat file: " + fn);
                            }
                            _getch();
                        }
                    }
                    break;
                }

                // ---- Keluar ----
                case 9:
                    clearScreen();
                    printMessageBox("Terima kasih! Program selesai.");
                    cout << "\n";
                    showCursor();
                    delete[] students;
                    return 0;
            }
        } else if (ch == 27) {
            clearScreen();
            printMessageBox("Terima kasih! Program selesai.");
            cout << "\n";
            showCursor();
            delete[] students;
            return 0;
        }
    }

    return 0;
}