/*
MINIMAL EXAM TEMPLATE - PART A COMPLIANT
========================================
COMPILE: g++ -o exam exam_universal_template.cpp
RUN: ./exam dataset.csv
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <map>
#include <iomanip>
using namespace std;

vector<vector<string>> data;
vector<string> headers;
vector<int> selectedCols;

// 1. DATA LOADING
bool readCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    string line;
    if (getline(file, line)) {
        stringstream ss(line);
        string header;
        while (getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }
    
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        if (row.size() == headers.size()) {
            data.push_back(row);
        }
    }
    
    file.close();
    cout << "Loaded " << data.size() << " records, " << headers.size() << " attributes" << endl;
    return true;
}

// 2. ATTRIBUTE SELECTION
void showColumns() {
    cout << "\nAvailable columns:" << endl;
    for (int i = 0; i < headers.size(); i++) {
        cout << i << ": " << headers[i] << endl;
    }
}

void analyzeAttributes() {
    cout << "\n=== ATTRIBUTE ANALYSIS ===" << endl;
    for (int i = 0; i < headers.size(); i++) {
        map<string, int> values;
        for (const auto& row : data) {
            values[row[i]]++;
        }
        
        cout << headers[i] << ": ";
        if (values.size() == data.size()) {
            cout << "REMOVE (Unique ID)" << endl;
        } else if (values.size() == 1) {
            cout << "REMOVE (Constant)" << endl;
        } else {
            cout << "KEEP (" << values.size() << " unique values)" << endl;
        }
    }
}

vector<int> selectAttributes() {
    analyzeAttributes();
    showColumns();
    
    cout << "\nEnter number of attributes to select: ";
    int n;
    cin >> n;
    
    vector<int> selected;
    for (int i = 0; i < n; i++) {
        cout << "Enter attribute " << (i+1) << " index: ";
        int idx;
        cin >> idx;
        selected.push_back(idx);
    }
    return selected;
}

// 3. DATA EXPLORATION
void exploreData() {
    cout << "\n=== DATA EXPLORATION ===" << endl;
    cout << "Dataset: " << data.size() << " records, " << selectedCols.size() << " selected attributes" << endl;
    
    for (int col : selectedCols) {
        cout << "\n" << headers[col] << ":" << endl;
        
        // Try numeric analysis
        vector<double> nums;
        bool isNumeric = true;
        for (const auto& row : data) {
            try {
                nums.push_back(stod(row[col]));
            } catch (...) {
                isNumeric = false;
                break;
            }
        }
        
        if (isNumeric) {
            sort(nums.begin(), nums.end());
            double sum = 0;
            for (double x : nums) sum += x;
            double mean = sum / nums.size();
            
            cout << "  Type: Numeric" << endl;
            cout << "  Min: " << nums[0] << ", Max: " << nums.back() << endl;
            cout << "  Mean: " << fixed << setprecision(2) << mean << endl;
        } else {
            map<string, int> counts;
            for (const auto& row : data) {
                counts[row[col]]++;
            }
            cout << "  Type: Categorical (" << counts.size() << " unique values)" << endl;
            for (const auto& p : counts) {
                cout << "    " << p.first << ": " << p.second << endl;
            }
        }
    }
}

// 4. PREPROCESSING
vector<double> getNumericColumn(int col) {
    vector<double> result;
    for (const auto& row : data) {
        try {
            result.push_back(stod(row[col]));
        } catch (...) {
            result.push_back(0);
        }
    }
    return result;
}

void preprocessData() {
    cout << "\n=== PREPROCESSING ===" << endl;
    cout << "1. Min-Max Normalization [0,1]" << endl;
    cout << "2. Z-Score Standardization (mean=0, std=1)" << endl;
    cout << "3. Handle Missing Values" << endl;
    cout << "4. No preprocessing" << endl;
    cout << "Choice (1-4): ";
    
    int choice;
    cin >> choice;
    
    if (choice == 1) {
        cout << "Applying Min-Max normalization..." << endl;
        for (int col : selectedCols) {
            try {
                stod(data[0][col]);
                vector<double> vals = getNumericColumn(col);
                double minVal = *min_element(vals.begin(), vals.end());
                double maxVal = *max_element(vals.begin(), vals.end());
                
                cout << "  " << headers[col] << ": [" << minVal << "," << maxVal << "] -> [0,1]" << endl;
                
                for (int i = 0; i < data.size(); i++) {
                    if (maxVal != minVal) {
                        double norm = (vals[i] - minVal) / (maxVal - minVal);
                        data[i][col] = to_string(norm);
                    }
                }
            } catch (...) {
                cout << "  " << headers[col] << ": Skipped (non-numeric)" << endl;
            }
        }
    } else if (choice == 2) {
        cout << "Applying Z-Score standardization..." << endl;
        for (int col : selectedCols) {
            try {
                stod(data[0][col]);
                vector<double> vals = getNumericColumn(col);
                double sum = 0;
                for (double x : vals) sum += x;
                double mean = sum / vals.size();
                
                double variance = 0;
                for (double x : vals) variance += (x - mean) * (x - mean);
                double stdDev = sqrt(variance / vals.size());
                
                cout << "  " << headers[col] << ": mean=" << mean << ", std=" << stdDev << endl;
                
                for (int i = 0; i < data.size(); i++) {
                    if (stdDev != 0) {
                        double zscore = (vals[i] - mean) / stdDev;
                        data[i][col] = to_string(zscore);
                    }
                }
            } catch (...) {
                cout << "  " << headers[col] << ": Skipped (non-numeric)" << endl;
            }
        }
    } else if (choice == 3) {
        cout << "Handling missing values..." << endl;
        for (int col : selectedCols) {
            int missing = 0;
            for (const auto& row : data) {
                if (row[col].empty() || row[col] == "?" || row[col] == "NA") {
                    missing++;
                }
            }
            if (missing > 0) {
                cout << "  " << headers[col] << ": Found " << missing << " missing values" << endl;
                // Replace with most frequent value for simplicity
                map<string, int> counts;
                for (const auto& row : data) {
                    if (!row[col].empty() && row[col] != "?" && row[col] != "NA") {
                        counts[row[col]]++;
                    }
                }
                string mostFrequent = counts.begin()->first;
                for (const auto& p : counts) {
                    if (p.second > counts[mostFrequent]) {
                        mostFrequent = p.first;
                    }
                }
                
                for (auto& row : data) {
                    if (row[col].empty() || row[col] == "?" || row[col] == "NA") {
                        row[col] = mostFrequent;
                    }
                }
                cout << "    Replaced with: " << mostFrequent << endl;
            }
        }
    } else {
        cout << "No preprocessing applied." << endl;
    }
}

// 5. DATA MINING METHODS
void applyMethod() {
    cout << "\n=== DATA MINING METHOD ===" << endl;
    cout << "a) Classification  b) Prediction  c) Clustering  d) Association" << endl;
    cout << "Choose method (a/b/c/d): ";
    
    char choice;
    cin >> choice;
    
    cout << "\n// INSERT YOUR ALGORITHM HERE //" << endl;
    cout << "// Available data: data[row][col] //" << endl;
    cout << "// Selected columns: selectedCols vector //" << endl;
    cout << "// Use getNumericColumn(col) for calculations //" << endl;
    
    switch (choice) {
        case 'a': 
            cout << "CLASSIFICATION: Implement Naive Bayes/Decision Tree" << endl;
            break;
        case 'b': 
            cout << "PREDICTION: Implement Linear Regression/Correlation" << endl;
            break;
        case 'c': 
            cout << "CLUSTERING: Implement K-Means/Hierarchical/DBSCAN" << endl;
            break;
        case 'd': 
            cout << "ASSOCIATION: Implement Apriori/Association Rules" << endl;
            break;
    }
}

// 6. SAVE RESULTS
void saveResults() {
    ofstream file("output.csv");
    for (int i = 0; i < headers.size(); i++) {
        file << headers[i];
        if (i < headers.size() - 1) file << ",";
    }
    file << endl;
    
    for (const auto& row : data) {
        for (int i = 0; i < row.size(); i++) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << endl;
    }
    file.close();
    cout << "Results saved to output.csv" << endl;
}

// MAIN - COMPLETE PART A WORKFLOW
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <dataset.csv>" << endl;
        return 1;
    }
    
    cout << "=== DATA MINING PROCESS - PART A ===" << endl;
    
    // Step 1: Load Data
    if (!readCSV(argv[1])) return 1;
    
    // Step 2: Attribute Selection
    selectedCols = selectAttributes();
    
    // Step 3: Data Exploration
    exploreData();
    
    // Step 4: Preprocessing
    preprocessData();
    
    // Step 5: Apply Method
    applyMethod();
    
    // Step 6: Save Results
    saveResults();
    
    cout << "\n=== PROCESS COMPLETED ===" << endl;
    return 0;
}

/*
MEMORIZE THESE 6 FUNCTIONS:
==========================
1. readCSV() - Load data
2. selectAttributes() - Pick columns + justify
3. exploreData() - Show statistics
4. preprocessData() - Normalize if needed
5. applyMethod() - Your algorithm goes here
6. saveResults() - Save to CSV

EXAM USAGE:
==========
1. Copy this template (5 minutes)
2. Replace applyMethod() with your algorithm
3. Run with any dataset - gets full Part A marks!
*/