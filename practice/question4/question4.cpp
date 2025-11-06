/*
QUESTION 4: DATA NORMALIZATION
==============================

TODO: Implement Min-Max normalization and Z-score standardization

HINTS:
- Min-Max formula: (x - min) / (max - min)
- Z-score formula: (x - mean) / std_dev
- Standard deviation: sqrt(variance)
- Variance: sum((x - mean)²) / n
- Handle mixed data types (string names + numeric scores)

COMPILE: g++ -o q4 question4.cpp
RUN: ./q4 scores.csv
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
using namespace std;

// TODO: Add your universal CSV functions here

vector<string>headers;
vector<vector<string>> data;

bool readCSV(const string filename) {
    ifstream file(filename);

    if(!file.is_open()) {
        cout<<"cant open the file"<<endl;
        return false;
    }

    string line;

    if(getline(file, line)) {
        stringstream ss(line);
        string header;
        while(getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }

    while(getline(file, line)) {
        stringstream ss(line);
        string cell;
        vector<string>row;

        while(getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        if(row.size() == headers.size()) {
            data.push_back(row);
        }
    }

    file.close();
    cout<<"data loaded"<<endl;
    return true;
}

void showColumns() {
    for(int i = 0; i<headers.size(); i++) {
        cout<<i<<": "<<headers[i]<<endl;
    }
}

vector<int> selectColumns() {
    showColumns();

    cout<<"Enter the number of columns to select: ";
    int n; 
    cin>>n;

    vector<int> selected;
    for(int i = 0; i<n; i++) {
        cout<<"select column "<<i<<":";
        int col;
        cin>>col;
        selected.push_back(col);
    }
    return selected;
}

vector<double> getColumn(int colIndex) {
    vector<double>column;
    for(auto row : data) {
        try {
            column.push_back(stod(row[colIndex]));
        } catch(...) {
            column.push_back(0);
        }

    }
    return column;
}

double findMin(vector<double> values) {
    double minimum = values[0];
    for(int i = 0; i<values.size(); i++) {
        minimum = min(minimum, values[i]);
    }
    return minimum;
}

double findMax(vector<double> values) {
    double maximum = values[0];
    for(int i = 0; i<values.size(); i++) {
        maximum = max(maximum, values[i]);
    }
    return maximum;
}

double findMean(vector<double> values) {
    double sum = 0.0;
    for(int i = 0; i<values.size(); i++) {
        sum += values[i];
    }
    return sum/values.size();
}

double findStdDev(vector<double> values) {
    double sum = 0.0;
    double mean = findMean(values);
    for(auto value : values ) {
        sum+=(value - mean) * (value - mean);
    }

    return sqrt(sum/values.size());

}

void applyMinMaxNormalization(vector<double> &values, double new_min, double new_max) {
    double old_min = findMin(values);
    double old_max = findMax(values);
    // double mean = findMean(values);

    for(int i = 0; i<values.size(); i++) {
        values[i] = (values[i] - old_min) /(old_max - old_min) * (new_max - new_min) + new_min;
    }
}

void applyZScoreStandardization(vector<double> &values) {
    double mean = findMean(values);
    double sd = findStdDev(values);

    for(int i = 0; i<values.size(); i++) {
        values[i] = (values[i] - mean)/sd;
    }
}

void saveCSV(const string filename) {
    ofstream file(filename);

    if(!file.is_open()) {
        cout<<"Cant write"<<endl;
    }

    for(int i  = 0; i<headers.size(); i++) {
        file<<headers[i];
        if(i < headers.size()-1)file<<',';
    }
    file<<endl;

    // data
    for(auto row : data) {
        for(int i = 0; i<row.size(); i++) {
            file<<row[i];
            if(i<row.size()-1)file<<',';
        }
        file<<endl;
    }

    file.close();
}

// TODO: Add normalization functions:
// - findMin(vector<double> values)
// - findMax(vector<double> values) 
// - findMean(vector<double> values)
// - findStdDev(vector<double> values)
// - applyMinMaxNormalization(vector<double>& values)
// - applyZScoreStandardization(vector<double>& values)

int main(int argc, char* argv[]) {
    // TODO: Implement normalization solution

    if(argc <= 1) {
        cout<<"invalid no of arguments"<<endl;
        return 1;
    }

    if(!readCSV(argv[1])) {
        cout<<"Error reading the csv"<<endl;
    }

    cout<<"Select columns to perform normalization"<<endl;
    vector<int> cols = selectColumns();

    cout<<"enter new min: ";
    double new_min;
    cin>>new_min;
    cout<<"enter new max: ";
    double new_max;
    cin>>new_max;

    for(auto x : cols) {
        vector<double> column = getColumn(x);
        applyMinMaxNormalization(column, new_min, new_max);

        for(int i = 0; i<data.size(); i++) {
            data[i][x] = to_string(column[i]);
        }
    }
    
    saveCSV("output.csv");
    
    return 0;
}