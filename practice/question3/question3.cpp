/*
QUESTION 3: NAIVE BAYES CLASSIFICATION
======================================

TODO: Implement Naive Bayes classifier for tennis dataset

HINTS:
- Handle categorical data (strings)
- Use maps to count occurrences: map<string, int>
- Calculate P(attribute=value|class) = count(attribute=value AND class) / count(class)
- Apply Bayes theorem: P(class|X) = P(X|class) × P(class)
- Test case: (Sunny, Hot, High, False)

COMPILE: g++ -o q3 question3.cpp
RUN: ./q3 tennis.csv
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <iomanip>
using namespace std;

// TODO: Add your code here

vector<string> headers;
vector<vector<double>> data;

bool readCSV(string filename) {
    ifstream file(filename);
    if(!file.is_open()) {
        cout<<"Cant open file"<<endl;
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

    // read data now.
    while(getline(file, line)) {
        stringstream ss(line);
        vector<double> row;
        string cell;
        while(getline(ss, cell, ',')) {
            row.push_back(stod(cell));
        }

        if(row.size() == headers.size()) {
            data.push_back(row);
        }
    }

    file.close();
    cout<<"data loaded "<< headers.size()<< "rows :" << data.size()<<endl;

    return true;

}

void showColumns() {

}

vector<int> selectColumns() {
    showColumns();
}

vector<double> getColumn(columnIndex) {

}

void saveCSV(const string filename, vector<string> resHeaders, vector<vector<double>> data) {

}


string classify(vector<string> testCase) {
    double probYes = findPriorProbability()
}





int main(int argc, char* argv[]) {
    // TODO: Implement Naive Bayes classifier
    
    return 0;
}