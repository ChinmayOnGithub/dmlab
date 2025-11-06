#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

// Universal data structure
vector<vector<double>> data;
vector<string> headers;

// Universal CSV reader - MEMORIZE THIS!
bool readCSV(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return false;
    }
    
    string line;
    
    // Read headers
    if (getline(file, line)) {
        stringstream ss(line);
        string header;
        while (getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }
    
    // Read data rows
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<double> row;
        
        while (getline(ss, cell, ',')) {
            try {
                row.push_back(stod(cell)); // Convert to double
            } catch (...) {
                row.push_back(0); // Default for non-numeric
            }   
        }
        
        if (row.size() == headers.size()) {
            data.push_back(row);
        }
    }
    
    file.close();
    cout << "Loaded " << data.size() << " records with " << headers.size() << " columns" << endl;
    return true;
}

// Show available columns - MEMORIZE THIS!
void showColumns() {
    cout << "Available columns:" << endl;
    for (int i = 0; i < headers.size(); i++) {
        cout << i << ": " << headers[i] << endl;
    }
}

// Get user column selection - MEMORIZE THIS!
vector<int> selectColumns() {
    showColumns();
    
    cout << "Enter number of columns to use: ";
    int n;
    cin >> n;
    
    vector<int> selected;
    for (int i = 0; i < n; i++) {
        cout << "Enter column " << (i+1) << " index: ";
        int col;
        cin >> col;
        selected.push_back(col);
    }
    
    return selected;
}

// Save results to CSV - MEMORIZE THIS!
void saveCSV(string filename, vector<string> resultHeaders, vector<vector<string>> results) {
    ofstream file(filename);
    
    // Write headers
    for (int i = 0; i < resultHeaders.size(); i++) {
        file << resultHeaders[i];
        if (i < resultHeaders.size() - 1) file << ",";  
    }
    file << endl;
    
    // Write data
    for (auto& row : results) {
        for (int i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << endl;
    }
    
    file.close();
    cout << "Results saved to " << filename << endl;
}

// EXAMPLE USAGE - Replace this with your algorithm
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input.csv>" << endl;
        return 1;
    }
    
    // Step 1: Read CSV
    if (!readCSV(argv[1])) {
        return 1;
    }
    
    // Step 2: Select columns (if needed)
    vector<int> cols = selectColumns();
    
    // Step 3: Your algorithm here
    cout << "\n=== INSERT YOUR ALGORITHM HERE ===" << endl;
    
    // Example: Print selected data
    cout << "Selected data:" << endl;
    for (int i = 0; i < min(5, (int)data.size()); i++) {
        cout << "Row " << i << ": ";
        for (int col : cols) {
            cout << data[i][col] << " ";
        }
        cout << endl;
    }
    
    // Step 4: Save results (example)
    vector<string> resultHeaders = {"Result1", "Result2"};
    vector<vector<string>> results = {
        {"Value1", "Value2"},
        {"Value3", "Value4"}
    };
    saveCSV("output.csv", resultHeaders, results);
    
    return 0;
}

/*
MEMORIZE THESE 4 FUNCTIONS:
1. readCSV(filename) - loads data into global vectors
2. showColumns() - displays available columns  
3. selectColumns() - gets user column selection
4. saveCSV(filename, headers, data) - saves results

USAGE FOR ANY ALGORITHM:
1. Call readCSV() to load data
2. Call selectColumns() if you need specific columns
3. Access data with: data[row][col]
4. Call saveCSV() to save results

WORKS FOR ALL 14 QUESTIONS!
*/