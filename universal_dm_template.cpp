#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iomanip>
using namespace std;

// Global variables - easy to remember
vector<vector<double>> data;
vector<string> headers;
vector<int> selectedCols;

// Simple utility functions
double mean(vector<double> v) {
    double sum = 0;
    for(double x : v) sum += x;
    return sum / v.size();
}

double variance(vector<double> v) {
    double m = mean(v);
    double sum = 0;
    for(double x : v) sum += (x-m)*(x-m);
    return sum / v.size();
}

// Step 1: Attribute Selection
void step1_attributeSelection() {
    cout << "\n=== STEP 1: ATTRIBUTE SELECTION ===" << endl;
    
    cout << "Available columns:" << endl;
    for(int i = 0; i < headers.size(); i++) {
        cout << i << ": " << headers[i] << endl;
    }
    
    cout << "Enter number of columns to use: ";
    int n;
    cin >> n;
    
    selectedCols.clear();
    for(int i = 0; i < n; i++) {
        cout << "Enter column " << (i+1) << ": ";
        int col;
        cin >> col;
        selectedCols.push_back(col);
    }
    
    cout << "Selected attributes: ";
    for(int col : selectedCols) {
        cout << headers[col] << " ";
    }
    cout << endl;
}

// Step 2: Data Exploration
void step2_dataExploration() {
    cout << "\n=== STEP 2: DATA EXPLORATION ===" << endl;
    
    cout << "Dataset: " << data.size() << " records, " 
         << selectedCols.size() << " attributes" << endl;
    
    for(int col : selectedCols) {
        vector<double> values;
        for(int i = 0; i < data.size(); i++) {
            values.push_back(data[i][col]);
        }
        
        sort(values.begin(), values.end());
        double m = mean(values);
        double v = variance(values);
        
        cout << "\n" << headers[col] << ":" << endl;
        cout << "  Min: " << values[0] << endl;
        cout << "  Max: " << values.back() << endl;
        cout << "  Mean: " << fixed << setprecision(2) << m << endl;
        cout << "  Variance: " << v << endl;
    }
}

// Step 3: Preprocessing
void step3_preprocessing() {
    cout << "\n=== STEP 3: PREPROCESSING ===" << endl;
    
    cout << "1. Min-Max Normalization" << endl;
    cout << "2. No preprocessing" << endl;
    cout << "Choose (1/2): ";
    
    int choice;
    cin >> choice;
    
    if(choice == 1) {
        cout << "Applying Min-Max normalization..." << endl;
        
        for(int col : selectedCols) {
            double minVal = data[0][col];
            double maxVal = data[0][col];
            
            // Find min and max
            for(int i = 0; i < data.size(); i++) {
                minVal = min(minVal, data[i][col]);
                maxVal = max(maxVal, data[i][col]);
            }
            
            // Normalize
            for(int i = 0; i < data.size(); i++) {
                if(maxVal != minVal) {
                    data[i][col] = (data[i][col] - minVal) / (maxVal - minVal);
                }
            }
            
            cout << "  " << headers[col] << ": [" << minVal << "," << maxVal << "] -> [0,1]" << endl;
        }
    }
    
    cout << "First 3 records after preprocessing:" << endl;
    for(int i = 0; i < min(3, (int)data.size()); i++) {
        cout << "Record " << i << ": ";
        for(int col : selectedCols) {
            cout << fixed << setprecision(3) << data[i][col] << " ";
        }
        cout << endl;
    }
}

// Step 4: Apply DM Method (placeholder)
void step4_applyMethod() {
    cout << "\n=== STEP 4: APPLY DATA MINING METHOD ===" << endl;
    
    cout << "Available methods:" << endl;
    cout << "a) Classification" << endl;
    cout << "b) Clustering" << endl;
    cout << "c) Association" << endl;
    cout << "d) Prediction" << endl;
    
    cout << "\n// INSERT YOUR ALGORITHM HERE //" << endl;
    cout << "// Use: data[i][selectedCols[j]] to access processed data //" << endl;
    cout << "// Example: K-means, DBSCAN, Naive Bayes, etc. //" << endl;
}

// Load CSV data
bool loadData(string filename) {
    ifstream file(filename);
    if(!file.is_open()) {
        cout << "Error opening file!" << endl;
        return false;
    }
    
    string line;
    
    // Read headers
    if(getline(file, line)) {
        stringstream ss(line);
        string header;
        while(getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }
    
    // Read data
    while(getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<double> row;
        
        while(getline(ss, cell, ',')) {
            try {
                row.push_back(stod(cell));
            } catch(...) {
                row.push_back(0); // Default for non-numeric
            }
        }
        
        if(row.size() == headers.size()) {
            data.push_back(row);
        }
    }
    
    cout << "Loaded " << data.size() << " records" << endl;
    return true;
}

// Main function
int main(int argc, char* argv[]) {
    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <input.csv>" << endl;
        return 1;
    }
    
    cout << "DATA MINING PROCESS - PART A" << endl;
    cout << "=============================" << endl;
    
    // Load data
    if(!loadData(argv[1])) {
        return 1;
    }
    
    // Execute 4 steps
    step1_attributeSelection();
    step2_dataExploration();
    step3_preprocessing();
    step4_applyMethod();
    
    cout << "\nProcess completed!" << endl;
    return 0;
}