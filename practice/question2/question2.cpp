/*
QUESTION 2: SIMPLE K-MEANS CLUSTERING (K=2)
===========================================

TODO: Implement K-Means clustering with K=2

HINTS:
- Use Euclidean distance: sqrt((x1-x2)² + (y1-y2)²)
- Initialize centroids as first 2 data points
- Keep updating until centroids don't change
- Show each iteration step by step

COMPILE: g++ -o q2 question2.cpp
RUN: ./q2 points.csv
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

vector<vector<double>>data;
vector<string>headers;

bool readCSV(const string filename) {
    ifstream file(filename);

    if(!file.is_open()) {
        return false;
    }

    // input headers
    string line;
    if(getline(file, line)) {
        stringstream ss(line);
        string header;
        while(getline(ss, header, ',')) {
            headers.push_back(header);
        }
    }

    // input data

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
    cout<<"Loaded columns "<<headers.size()<< " Rows:"<<data.size();
    return true;   
}

void showColumns() {
    for(int i = 0; i<headers.size(); i++) {
        cout<<i<<": "<<headers[i]<<endl;
    }
}

vector<int> selectColumns() {
    showColumns();

    cout<<"you can select 2 columns here";
    int n = 2;
    vector<int>selected;
    for(int i  = 0; i<n; i++) {
        cout<<"select column "<<(i+1)<<" index: ";
        int col;
        cin>>col;
        selected.push_back(col);
    }
    return selected;
}

vector<double> getColumn(int columnIndex) {
    vector<double>column;
    for(auto row : data) {
        column.push_back(row[columnIndex]);
    }
    return column;
}

double findDistance(double x1, double y1, double x2 , double y2) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

double findMean(vector<double>col) {
    double sum = 0.0;
    for(auto x : col) sum+=x;
    return sum/col.size();
}

vector<int> assignToClusters (vector<vector<double>> &centroids) {
    vector<int> assignments;
    for(int i = 0; i<data.size(); i++) {
        double x = data[i][0];
        double y = data[i][1];

        int bestCluster = 0;
        double minDist = findDistance(x, y, centroids[0][0], centroids[0][1]);

        for(int c = 1; c<centroids.size(); c++) {
            double dist = findDistance(x, y, centroids[c][0], centroids[c][1]);
            if(dist<minDist) {
                minDist = dist;
                bestCluster = c;
            }
        }

        assignments.push_back(bestCluster);
    }

    return assignments;
}

vector<vector<double>> updateCentroids(vector<int>& assignments) {
    vector<vector<double>> newCentroids;
    
    // For each cluster (0 and 1)
    for(int c = 0; c < 2; c++) {
        double sumX = 0, sumY = 0;
        int count = 0;
        
        // Find all points assigned to this cluster
        for(int i = 0; i < assignments.size(); i++) {
            if(assignments[i] == c) {  // Point belongs to cluster c
                sumX += data[i][0];
                sumY += data[i][1];
                count++;
            }
        }
        
        // Calculate new centroid (mean of assigned points)
        if(count > 0) {
            newCentroids.push_back({sumX/count, sumY/count});
        } else {
            newCentroids.push_back({0, 0});  // No points assigned
        }
    }
    
    return newCentroids;
}

bool centroidsEqual(vector<vector<double>>& old, vector<vector<double>>& newCent) {
    for(int i = 0; i < old.size(); i++) {
        if(abs(old[i][0] - newCent[i][0]) > 0.1 || 
           abs(old[i][1] - newCent[i][1]) > 0.1) {
            return false;  // Changed
        }
    }
    return true;  // No change
}




int main(int argc, char* argv[]) {
    // TODO: Implement K-Means clustering
    if(argc <= 1) {
        cout<<"invalid no of arguments; expected 2"<<endl;
        return 1;
    }

    if(!readCSV(argv[1])) {
        cout<<"Cant Read csv file"<<endl;
        return 1;
    }

    int k; 
    cout<<"select number of clusters: ";
    cin>>k;

    // randomly initialize the k clusters
    vector<vector<double>> centroids;
    for(int i = 0; i<k; i++) {
        centroids.push_back({data[i][0], data[i][1]});
    }

    bool changed = true;
    int iterations = 1;

    // somehow find distance matrix

    while(changed) {
        cout<<"Iteration "<<iterations<<":"<<endl;
        vector<int> assignments = assignToClusters(centroids); 
        vector<vector<double>> newCentroids = updateCentroids(assignments);
        cout << "Old centroids: ";
        for(auto c : centroids) cout << "(" << c[0] << "," << c[1] << ") ";
        cout << endl;
        
        cout << "New centroids: ";
        for(auto c : newCentroids) cout << "(" << c[0] << "," << c[1] << ") ";
        cout << endl;
    
        changed = !centroidsEqual(centroids, newCentroids);
        cout << "Changed: " << (changed ? "YES" : "NO") <<endl;

        centroids = newCentroids;
        iterations++;
        if(iterations > 10) {
            cout<<"max reached"<<endl;
            break;
        }
    }

    cout<<iterations<<endl;
    
    return 0;
}