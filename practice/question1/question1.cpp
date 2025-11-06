/*
QUESTION 1: BASIC STATISTICS AND CORRELATION
============================================

TODO: Implement correlation coefficient calculation

HINTS:
- Use the universal CSV reader pattern
- Remember the correlation formula: r = Σ(xi-x̄)(yi-ȳ) / √[Σ(xi-x̄)²Σ(yi-ȳ)²]
- Calculate mean first, then use it for correlation
- Don't forget to interpret the result (strong/moderate/weak)

COMPILE: g++ -o q1 question1.cpp
RUN: ./q1 students.csv
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
using namespace std;

// TODO: Add your code here
// Remember to follow the universal CSV reader pattern!

vector<vector<double>> data;
vector<string>headers;

bool readCSV(const string &filename) {
    ifstream file(filename);
    if(!file.is_open()) {
        cout<<"error opening the file";
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
        vector<double>row;

        while(getline(ss, cell, ',')) {
            try {

                row.push_back(stod(cell));
            } catch(...) {
                row.push_back(0);
                // i can handle the missing value here.
            }
        }

        if(row.size() == headers.size()) {
            data.push_back(row);
        }
    }

    file.close();
    cout<<"loaded "<<data.size()<<" Records & "<<headers.size()<<"Columns";
    return true;
}

void showColumns() {
    cout<<"Available columns: "<<endl;
    for(int i = 0; i<headers.size(); i++) {
        cout<<i<<": "<<headers[i]<<endl;
    }
}

vector<int> selectColumns() {
    showColumns();

    cout<<"you can only select 2 columns"<<endl;
    // cout<<"select number of columns you want to select: ";
    int n  = 2;
    // cin>>n;
    
    vector<int> selected;
    for(int i = 0; i<n; i++) {
        cout<<"enter the column "<<(i+1)<<" index:";
        int col;
        cin>>col;
        selected.push_back(col);
    }
    return selected;
}

void saveCSV(string filename, vector<string> resHeaders, vector<vector<double>> resData) {
    ofstream file(filename);
    
    for(int i = 0; i<resHeaders.size(); i++) {
        file<<resHeaders[i];
        if(i<resHeaders.size()-1) file<<',';
    }
    file<<endl;

    // now data

    for(auto row:resData) {
        for(int i = 0; i<row.size(); i++) {
            file << row[i];
            if(i < row.size()-1) file<<',';
        }
        file<<endl;
    }

    file.close();
    cout<<"Results saved to "<<filename<<endl;
}

double findMean(vector<double> values) {
    double sum = 0.0;
    for(auto x : values) {
        sum+=x;
    }
    return sum/values.size();
}

vector<double> getColumn(int columnIndex) {
    vector<double> column;
    for(int i = 0; i<data.size(); i++) {
        column.push_back(data[i][columnIndex]);
    }
    return column;
}

double findCorrelation(vector<double> &x, vector<double> &y) {
    int n = x.size();

    if(n<=1) {
        return 0.0;
    }

    double mean_x = findMean(x);
    double mean_y = findMean(y);

    double sum_xxyy = 0.0;
    double sum_xx2 = 0.0;
    double sum_yy2 = 0.0;

    for(int i = 0; i<n; i++) {
        sum_xxyy += (x[i] - mean_x) * (y[i] - mean_y);
        sum_xx2 += (x[i] - mean_x) * (x[i] - mean_x);
        sum_yy2 += (y[i] - mean_y) * (y[i] - mean_y);
    }

    double result = sum_xxyy / sqrt(sum_xx2 * sum_yy2);

    return result;
}


int main(int argc, char* argv[]) {
// TODO: Implement the solution

    if(argc != 2) {
        cout<<"invalid no of arguments"<<endl;
        return 1;
    }

    string filename = argv[1];

    if(!readCSV(filename)) {
        return 1;
    }

    vector<int> columnSelected = selectColumns();
    vector<double>x = getColumn(columnSelected[0]);
    vector<double>y = getColumn(columnSelected[1]);
    double res = findCorrelation(x, y);
    cout<<(res < 0 ? "Negative Correlation" : "Positive Correlation")<<endl;

    cout<<res<<endl;

    // saveCSV("output.csv", headers, data);
    
    return 0;
}