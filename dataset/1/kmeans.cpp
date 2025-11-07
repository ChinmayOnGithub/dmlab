#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

vector<vector<double>> data;
vector<string> headers;
int xColumn, yColumn, k;

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
        vector<double> row;
        
        while (getline(ss, cell, ',')) {
            try { row.push_back(stod(trim(cell))); }
            catch (...) { row.push_back(0); }
        }
        
        if (row.size() == headers.size())
            data.push_back(row);
    }
    
    cout << "Loaded " << data.size() << " records" << endl;
    return true;
}

void selectAttributes() {
    cout << "\nAvailable Columns:" << endl;
    for (int i = 0; i < headers.size(); i++)
        cout << i << ": " << headers[i] << endl;
    
    cout << "Select X attribute index: ";
    cin >> xColumn;
    
    cout << "Select Y attribute index: ";
    cin >> yColumn;
    
    cout << "Enter number of clusters (K): ";
    cin >> k;
}

void exploreData() {
    cout << "\nData Exploration" << endl;
    
    double xSum = 0, ySum = 0;
    double xMin = 1e9, xMax = -1e9, yMin = 1e9, yMax = -1e9;
    
    for (auto& row : data) {
        double x = row[xColumn];
        double y = row[yColumn];
        
        xSum += x; ySum += y;
        xMin = min(xMin, x); xMax = max(xMax, x);
        yMin = min(yMin, y); yMax = max(yMax, y);
    }
    
    cout << headers[xColumn] << " min: " << xMin << " max: " << xMax << " mean: " << xSum / data.size() << endl;
    cout << headers[yColumn] << " min: " << yMin << " max: " << yMax << " mean: " << ySum / data.size() << endl;
}

void preprocessData() {
    cout << "\nData Preprocessing" << endl;
    cout << "Extracting selected attributes for clustering" << endl;
    
    vector<vector<double>> points;
    
    for (auto& row : data)
        points.push_back({row[xColumn], row[yColumn]});
    
    data = points;
    
    cout << "Prepared " << data.size() << " 2D points" << endl;
}

double dist(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

vector<int> assignClusters(const vector<vector<double>>& centroids) {
    vector<int> cluster(data.size());
    
    for (int i = 0; i < data.size(); i++) {
        double x = data[i][0], y = data[i][1];
        double bestDist = dist(x, y, centroids[0][0], centroids[0][1]);
        int best = 0;
        
        for (int c = 1; c < k; c++) {
            double d = dist(x, y, centroids[c][0], centroids[c][1]);
            if (d < bestDist) {
                bestDist = d;
                best = c;
            }
        }
        cluster[i] = best;
    }
    return cluster;
}

vector<vector<double>> updateCentroids(const vector<int>& cluster) {
    vector<vector<double>> newCentroids(k, vector<double>(2, 0));
    vector<int> count(k, 0);
    
    for (int i = 0; i < data.size(); i++) {
        int c = cluster[i];
        newCentroids[c][0] += data[i][0];
        newCentroids[c][1] += data[i][1];
        count[c]++;
    }
    
    for (int c = 0; c < k; c++)
        if (count[c] > 0) {
            newCentroids[c][0] /= count[c];
            newCentroids[c][1] /= count[c];
        }
    
    return newCentroids;
}

bool same(const vector<vector<double>>& a, const vector<vector<double>>& b) {
    for (int i = 0; i < k; i++)
        if (fabs(a[i][0] - b[i][0]) > 0.0001 || fabs(a[i][1] - b[i][1]) > 0.0001)
            return false;
    return true;
}

void performClustering() {
    cout << "\nApplying K-Means Clustering" << endl;
    
    vector<vector<double>> centroids;
    
    for (int i = 0; i < k; i++)
        centroids.push_back(data[i]);
    
    bool changed = true;
    int iteration = 1;
    
    while (changed && iteration <= 15) {
        vector<int> clusters = assignClusters(centroids);
        vector<vector<double>> newCentroids = updateCentroids(clusters);
        
        cout << "Iteration " << iteration << endl;
        for (int c = 0; c < k; c++)
            cout << "Centroid " << c << ": (" << newCentroids[c][0] << ", " << newCentroids[c][1] << ")" << endl;
        
        changed = !same(centroids, newCentroids);
        centroids = newCentroids;
        iteration++;
    }
    
    cout << "Clustering completed" << endl;
    

    vector<int> finalClusters = assignClusters(centroids);
    vector<int> clusterSizes(k, 0);
    
    for (int cluster : finalClusters) {
        clusterSizes[cluster]++;
    }
    
    cout << "Final Results:" << endl;
    for (int i = 0; i < k; i++) {
        cout << "Cluster " << i << ": " << clusterSizes[i] << " points" << endl;
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
    performClustering();
    
    return 0;
}