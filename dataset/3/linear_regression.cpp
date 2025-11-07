#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

vector<vector<string>> raw;
vector<double> X, Y;
vector<string> headers;
int xColumn, yColumn;

string trim(string s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos) ? "" : s.substr(a, b - a + 1);
}

bool loadCSV(string filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    string line;
    getline(file, line);
    stringstream ss(line);
    string h;
    while (getline(ss, h, ',')) headers.push_back(trim(h));
    
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> row;
        string cell;
        while (getline(ss, cell, ',')) row.push_back(trim(cell));
        if (row.size() == headers.size()) raw.push_back(row);
    }
    
    cout << "Loaded " << raw.size() << " records" << endl;
    return true;
}

void selectAttributes() {
    cout << "\nAttributes:" << endl;
    for (int i = 0; i < headers.size(); i++)
        cout << i << ": " << headers[i] << endl;
    
    cout << "Select X (independent variable): ";
    cin >> xColumn;
    cout << "Select Y (dependent variable): ";
    cin >> yColumn;
}

void explore() {
    cout << "\nData Exploration" << endl;
    
    double minX = 1e9, maxX = -1e9, sumX = 0;
    double minY = 1e9, maxY = -1e9, sumY = 0;
    int cnt = 0;
    
    for (auto &r : raw) {
        try {
            double x = stod(r[xColumn]);
            double y;
            
            string yValue = r[yColumn];
            
            if (yValue.find('k') != string::npos) {
                yValue.erase(remove(yValue.begin(), yValue.end(), 'k'), yValue.end());
                y = stod(yValue);
            }
            else if (yValue == "poor") y = 1;
            else if (yValue == "average") y = 2;
            else if (yValue == "good") y = 3;
            else y = stod(yValue);
            
            sumX += x; sumY += y;
            minX = min(minX, x);
            maxX = max(maxX, x);
            minY = min(minY, y);
            maxY = max(maxY, y);
            cnt++;
        } catch (...) {}
    }
    
    cout << headers[xColumn] << ": min=" << minX << " max=" << maxX << " mean=" << sumX/cnt << endl;
    cout << headers[yColumn] << ": min=" << minY << " max=" << maxY << " mean=" << sumY/cnt << endl;
    if (headers[yColumn] == "performance") {
        cout << "Performance encoding: poor=1, average=2, good=3" << endl;
    }
}

void preprocess() {
    cout << "\nData Preprocessing" << endl;
    
    for (auto &r : raw) {
        try {
            double x = stod(r[xColumn]);
            double y;
            
            string yValue = r[yColumn];
            
            if (yValue.find('k') != string::npos) {
                yValue.erase(remove(yValue.begin(), yValue.end(), 'k'), yValue.end());
                y = stod(yValue);
            }
            else if (yValue == "poor") y = 1;
            else if (yValue == "average") y = 2;
            else if (yValue == "good") y = 3;
            else y = stod(yValue);
            
            X.push_back(x);
            Y.push_back(y);
        } catch (...) {}
    }
    
    cout << "Converted " << X.size() << " valid numeric rows" << endl;
}

void regressionAndPredict() {
    cout << "\nApplying Linear Regression" << endl;
    
    int n = X.size();
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    
    for (int i = 0; i < n; i++) {
        sumX += X[i];
        sumY += Y[i];
        sumXY += X[i] * Y[i];
        sumX2 += X[i] * X[i];
    }
    
    double m = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    double b = (sumY - m * sumX) / n;
    
    cout << "Regression coefficients:" << endl;
    cout << "Slope (m) = " << m << endl;
    cout << "Intercept (b) = " << b << endl;
    cout << "Equation: Y = " << m << "X + " << b << endl;
    
    double input;
    cout << "\nEnter " << headers[xColumn] << " to predict " << headers[yColumn] << ": ";
    cin >> input;
    
    double predicted = m * input + b;
    
    if (headers[yColumn] == "performance") {
        cout << "Predicted " << headers[yColumn] << " = " << predicted;
        if (predicted <= 1.5) cout << " (poor)" << endl;
        else if (predicted <= 2.5) cout << " (average)" << endl;
        else cout << " (good)" << endl;
    } else {
        cout << "Predicted " << headers[yColumn] << " = " << predicted << "k" << endl;
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
    explore();
    preprocess();
    regressionAndPredict();
    
    return 0;
}