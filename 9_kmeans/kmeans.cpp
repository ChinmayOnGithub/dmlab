#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <unordered_set>

using namespace std;

struct Point
{
    vector<double> values;
    int cluster = -1;
};

double distance(const vector<double> &a, const vector<double> &b)
{
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); i++)
    {
        sum += pow(a[i] - b[i], 2);
    }
    return sqrt(sum);
}

vector<Point> readCSV(const string &filename, const vector<int> &columns)
{
    vector<Point> points;
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Error: Cannot open file " << filename << endl;
        return points;
    }

    string line;
    getline(file, line);

    while (getline(file, line))
    {
        stringstream ss(line);
        string value;
        vector<string> row;

        while (getline(ss, value, ','))
        {
            row.push_back(value);
        }

        Point p;
        for (int col : columns)
        {
            if (col - 1 < (int)row.size())
            {
                p.values.push_back(stod(row[col - 1]));
            }
        }

        if (!p.values.empty())
        {
            points.push_back(p);
        }
    }

    file.close();
    return points;
}

vector<vector<double>> initCentroids(const vector<Point> &points, int k)
{
    vector<vector<double>> centroids;
    unordered_set<int> chosen;

    srand(time(nullptr));
    while ((int)centroids.size() < k)
    {
        int idx = rand() % points.size();
        if (chosen.find(idx) == chosen.end())
        {
            centroids.push_back(points[idx].values);
            chosen.insert(idx);
        }
    }

    return centroids;
}

bool assignClusters(vector<Point> &points, const vector<vector<double>> &centroids)
{
    bool changed = false;

    for (auto &p : points)
    {
        int best = -1;
        double minDist = 1e9;

        for (int i = 0; i < (int)centroids.size(); i++)
        {
            double dist = distance(p.values, centroids[i]);
            if (dist < minDist)
            {
                minDist = dist;
                best = i;
            }
        }

        if (p.cluster != best)
        {
            p.cluster = best;
            changed = true;
        }
    }

    return changed;
}

vector<vector<double>> updateCentroids(const vector<Point> &points, int k)
{
    if (points.empty())
        return {};

    int dim = points[0].values.size();
    vector<vector<double>> centroids(k, vector<double>(dim, 0));
    vector<int> counts(k, 0);

    for (const auto &p : points)
    {
        if (p.cluster >= 0 && p.cluster < k)
        {
            for (int d = 0; d < dim; d++)
            {
                centroids[p.cluster][d] += p.values[d];
            }
            counts[p.cluster]++;
        }
    }

    for (int i = 0; i < k; i++)
    {
        if (counts[i] > 0)
        {
            for (int d = 0; d < dim; d++)
            {
                centroids[i][d] /= counts[i];
            }
        }
    }

    return centroids;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <input.csv>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream test(filename);
    if (!test.is_open())
    {
        cerr << "Error: Cannot open CSV file " << filename << endl;
        return 1;
    }

    string headerLine;
    getline(test, headerLine);
    test.close();

    stringstream ss(headerLine);
    string col;
    vector<string> columns;

    while (getline(ss, col, ','))
    {
        columns.push_back(col);
    }

    cout << "Available columns:" << endl;
    for (size_t i = 0; i < columns.size(); i++)
    {
        cout << (i + 1) << ". " << columns[i] << endl;
    }

    int numCols;
    cout << "Number of columns to select: ";
    cin >> numCols;

    vector<int> selected;
    for (int i = 0; i < numCols; i++)
    {
        int colNum;
        cout << "Enter column " << (i + 1) << " number: ";
        cin >> colNum;
        selected.push_back(colNum);
    }

    vector<Point> points = readCSV(filename, selected);

    if (points.empty())
    {
        cerr << "Error: No data points found" << endl;
        return 1;
    }

    int k;
    cout << "Enter number of clusters: ";
    cin >> k;

    if (k <= 0 || k > (int)points.size())
    {
        cerr << "Error: Invalid k value" << endl;
        return 1;
    }

    vector<vector<double>> centroids = initCentroids(points, k);

    int iterations = 0;
    bool changed;
    const int maxIter = 1000;

    do
    {
        changed = assignClusters(points, centroids);
        centroids = updateCentroids(points, k);
        iterations++;
    } while (changed && iterations < maxIter);

    ofstream out("output.csv");
    out << "Cluster";
    for (size_t i = 0; i < points[0].values.size(); i++)
    {
        out << ",Value" << (i + 1);
    }
    out << "\n";

    for (const auto &p : points)
    {
        out << p.cluster;
        for (double val : p.values)
        {
            out << "," << val;
        }
        out << "\n";
    }
    out.close();

    cout << "Clustering completed in " << iterations << " iterations" << endl;
    cout << "Results saved to output.csv" << endl;

    return 0;
}