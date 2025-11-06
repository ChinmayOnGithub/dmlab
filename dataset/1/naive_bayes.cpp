#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
using namespace std;

vector<vector<string>> data;
vector<string> headers;
vector<int> selectedAttributes;
int targetColumn;
map<string, int> classFrequency;
map<string, int> attributeClassFrequency;

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
        string header;
        while (getline(ss, header, ',')) headers.push_back(trim(header));
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
    cout << "\nAttribute Selection:" << endl;
    for (int i = 0; i < headers.size(); i++) {
        cout << i << ": " << headers[i] << endl;
    }
    
    cout << "Target column: ";
    cin >> targetColumn;
    
    int n;
    cout << "Number of predictors: ";
    cin >> n;
    
    selectedAttributes.resize(n);
    for (int i = 0; i < n; i++) {
        cout << "Select attribute " << i + 1 << ": ";
        cin >> selectedAttributes[i];
    }
}

void exploreData() {
    cout << "\nData Exploration:" << endl;
    
    for (auto& row : data) {
        classFrequency[row[targetColumn]]++;
    }
    
    cout << "Class distribution:" << endl;
    for (auto& pair : classFrequency) {
        cout << pair.first << ": " << pair.second << " instances" << endl;
    }
}

void preprocessData() {
    cout << "\nData Preprocessing:" << endl;
    cout << "Building frequency tables..." << endl;
    
    int missingCount = 0;
    for (auto& row : data) {
        for (int i = 0; i < row.size(); i++) {
            if (row[i].empty() || row[i] == "?" || row[i] == "NULL") {
                row[i] = "unknown";
                missingCount++;
            }
        }
    }
    if (missingCount > 0) {
        cout << "Handled " << missingCount << " missing values" << endl;
    }
    
    for (auto& row : data) {
        string classVal = row[targetColumn];
        for (int attr : selectedAttributes) {
            string key = headers[attr] + ":" + row[attr] + "|" + classVal;
            attributeClassFrequency[key]++;
        }
    }
    
    cout << "Frequency table built with " << attributeClassFrequency.size() << " entries" << endl;
}

double prior(string c) {
    return (double)classFrequency[c] / data.size();
}

double conditionalProb(string attr, string val, string c) {
    string key = attr + ":" + val + "|" + c;
    int numerator = attributeClassFrequency[key] + 1;
    int denominator = classFrequency[c] + selectedAttributes.size();
    return (double) numerator / denominator;
}

string classify() {
    vector<string> testCase;
    
    cout << "\nClassification - Enter test values:" << endl;
    for (int attr : selectedAttributes) {
        cout << headers[attr] << ": ";
        string val;
        cin >> val;
        testCase.push_back(val);
    }
    
    // Get the actual class names from the data
    vector<string> classNames;
    for (auto& pair : classFrequency) {
        classNames.push_back(pair.first);
    }
    
    double prob1 = prior(classNames[0]);
    double prob2 = prior(classNames[1]);
    
    cout << "\nCalculating probabilities:" << endl;
    cout << "P(" << classNames[0] << ") = " << prob1 << endl;
    cout << "P(" << classNames[1] << ") = " << prob2 << endl;
    
    for (int i = 0; i < selectedAttributes.size(); i++) {
        int attr = selectedAttributes[i];
        prob1 *= conditionalProb(headers[attr], testCase[i], classNames[0]);
        prob2 *= conditionalProb(headers[attr], testCase[i], classNames[1]);
    }
    
    cout << "P(" << classNames[0] << "|X) = " << prob1 << endl;
    cout << "P(" << classNames[1] << "|X) = " << prob2 << endl;
    
    return (prob1 > prob2) ? classNames[0] : classNames[1];
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <dataset.csv>" << endl;
        return 1;
    }
    
    if (!loadCSV(argv[1])) {
        cout << "Failed to load file" << endl;
        return 1;
    }
    
    selectAttributes();
    exploreData();
    preprocessData();
    
    string result = classify();
    cout << "\nPrediction: " << result << endl;
    
    return 0;
}