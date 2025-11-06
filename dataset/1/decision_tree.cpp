#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <cmath>
#include <algorithm>
using namespace std;

vector<vector<string>> data;
vector<string> headers;
vector<int> selectedAttributes;
int targetColumn;

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

bool loadCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    string line;
    if (getline(file, line)) {
        stringstream ss(line);
        string h;
        while (getline(ss, h, ',')) headers.push_back(trim(h));
    }
    
    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ',')) row.push_back(trim(cell));
        if (row.size() == headers.size()) data.push_back(row);
    }
    
    cout << "Loaded " << data.size() << " records" << endl;
    return true;
}

void selectAttributes() {
    cout << "\nAvailable Columns:" << endl;
    for (int i = 0; i < headers.size(); i++)
        cout << i << ": " << headers[i] << endl;
    
    cout << "Select target column: ";
    cin >> targetColumn;
    
    int n;
    cout << "Number of predictor attributes: ";
    cin >> n;
    
    selectedAttributes.resize(n);
    for (int i = 0; i < n; i++) {
        cout << "Select attribute " << i + 1 << ": ";
        cin >> selectedAttributes[i];
    }
}

void exploreData() {
    cout << "\nData Exploration" << endl;
    
    map<string, int> classCount;
    for (const auto& row : data) {
        classCount[row[targetColumn]]++;
    }
    
    cout << "Target distribution:" << endl;
    for (const auto& pair : classCount) {
        cout << pair.first << ": " << pair.second << " instances" << endl;
    }
}

void preprocessData() {
    cout << "\nData Preprocessing" << endl;
    cout << "Converting numerical attributes to categorical" << endl;
    
    for (int col : selectedAttributes) {
        bool isNumeric = true;
        vector<double> values;
        
        for (const auto& row : data) {
            try {
                values.push_back(stod(row[col]));
            } catch (...) {
                isNumeric = false;
                break;
            }
        }
        
        if (isNumeric && values.size() > 0) {
            sort(values.begin(), values.end());
            double median = values[values.size() / 2];
            
            for (auto& row : data) {
                try {
                    double val = stod(row[col]);
                    row[col] = (val <= median) ? "low" : "high";
                } catch (...) {}
            }
            cout << "Converted " << headers[col] << " to categorical (threshold: " << median << ")" << endl;
        }
    }
}

double calculateEntropy(const vector<string>& classes) {
    map<string, int> count;
    for (const string& cls : classes) count[cls]++;
    
    double entropy = 0;
    int total = classes.size();
    
    for (const auto& pair : count) {
        double prob = (double)pair.second / total;
        if (prob > 0) entropy -= prob * log2(prob);
    }
    return entropy;
}

void buildDecisionTree() {
    cout << "\nBuilding Decision Tree" << endl;
    
    vector<string> allClasses;
    for (const auto& row : data) {
        allClasses.push_back(row[targetColumn]);
    }
    
    double totalEntropy = calculateEntropy(allClasses);
    cout << "Total entropy: " << totalEntropy << endl;
    
    cout << "Information Gain for each attribute:" << endl;
    
    for (int attr : selectedAttributes) {
        map<string, vector<string>> subsets;
        for (const auto& row : data) {
            subsets[row[attr]].push_back(row[targetColumn]);
        }
        
        double weightedEntropy = 0;
        for (const auto& subset : subsets) {
            double weight = (double)subset.second.size() / data.size();
            weightedEntropy += weight * calculateEntropy(subset.second);
        }
        
        double gain = totalEntropy - weightedEntropy;
        cout << headers[attr] << ": " << gain << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <dataset.csv>" << endl;
        return 1;
    }
    
    if (!loadCSV(argv[1])) {
        cout << "Error reading CSV" << endl;
        return 1;
    }
    
    selectAttributes();
    exploreData();
    preprocessData();
    buildDecisionTree();
    
    return 0;
}