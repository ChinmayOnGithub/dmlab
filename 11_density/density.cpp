#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
using namespace std;

vector<double> loadData(const string &filename) {
    ifstream file(filename);
    vector<double> data;
    string line;

    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return data;
    }

    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;
        if (getline(ss, cell, ',')) {
            try {
                data.push_back(stod(cell));
            } catch (...) {
                cerr << "Warning: Invalid number -> " << cell << endl;
            }
        }
    }

    return data;
}

vector<int> getNeighbors(const vector<double> &data, int index, double eps) {
    vector<int> neighbors;
    for (int i = 0; i < data.size(); ++i) {
        if (abs(data[index] - data[i]) <= eps)
            neighbors.push_back(i);
    }
    return neighbors;
}

void expandCluster(const vector<double> &data, vector<int> &assignments, int index,
                   vector<int> &neighbors, int clusterId, double eps, int minPts) {
    assignments[index] = clusterId;

    for (size_t i = 0; i < neighbors.size(); ++i) {
        int nIdx = neighbors[i];

        if (assignments[nIdx] == -1) {
            assignments[nIdx] = clusterId;

            vector<int> nNeighbors = getNeighbors(data, nIdx, eps);
            if (nNeighbors.size() >= minPts) {
                neighbors.insert(neighbors.end(), nNeighbors.begin(), nNeighbors.end());
            }
        }
    }
}

void dbscan(const vector<double> &data, double eps, int minPts, vector<int> &assignments) {
    int clusterId = 1;
    assignments.assign(data.size(), -1);

    for (int i = 0; i < data.size(); ++i) {
        if (assignments[i] != -1) continue;

        vector<int> neighbors = getNeighbors(data, i, eps);
        if (neighbors.size() < minPts) {
            assignments[i] = 0;
        } else {
            expandCluster(data, assignments, i, neighbors, clusterId, eps, minPts);
            clusterId++;
        }
    }
}

void printResults(const vector<double> &data, const vector<int> &assignments, double eps, int minPts) {
    cout << "\nDBSCAN Results (ε = " << eps << ", minPts = " << minPts << "):\n";
    for (int i = 0; i < data.size(); ++i) {
        cout << "Value: " << data[i] << " -> ";
        if (assignments[i] == 0)
            cout << "Noise\n";
        else
            cout << "Cluster " << assignments[i] << "\n";
    }

    int noise = count(assignments.begin(), assignments.end(), 0);
    int maxCluster = *max_element(assignments.begin(), assignments.end());

    cout << "\nSummary:\n";
    cout << "Total points: " << data.size() << "\n";
    cout << "Noise points: " << noise << "\n";
    cout << "Clusters found: " << maxCluster << "\n";

    for (int c = 1; c <= maxCluster; ++c) {
        int cnt = count(assignments.begin(), assignments.end(), c);
        cout << "Cluster " << c << " size: " << cnt << "\n";
    }
}

void saveResults(const vector<double> &data, const vector<int> &assignments, const string &filename) {
    ofstream out(filename);
    out << "Value,Cluster\n";
    for (int i = 0; i < data.size(); ++i)
        out << data[i] << "," << assignments[i] << "\n";
    out.close();
    cout << "Saved results to: " << filename << "\n";
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <data.csv>\n";
        return 1;
    }

    string file = argv[1];
    vector<double> data = loadData(file);
    if (data.empty()) {
        cerr << "No valid data found.\n";
        return 1;
    }

    double eps;
    int minPts;

    cout << "Enter epsilon (ε): ";
    while (!(cin >> eps) || eps <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Enter a positive number for epsilon: ";
    }

    cout << "Enter minimum points (minPts): ";
    while (!(cin >> minPts) || minPts <= 0) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Enter a positive integer for minPts: ";
    }

    vector<int> assignments;
    dbscan(data, eps, minPts, assignments);
    printResults(data, assignments, eps, minPts);
    saveResults(data, assignments, "dbscan_results.csv");

    return 0;
}
