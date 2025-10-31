#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <map>
using namespace std;

struct Point
{
  int index;
  vector<double> values;
  Point(int i, const vector<double> &v) : index(i), values(v) {}
};

vector<Point> dataset;
vector<vector<double>> distanceMatrix;
double eps;
int minPts;

vector<Point> loadData(const string &filename)
{
  ifstream file(filename);
  vector<Point> data;
  string line;

  if (!file.is_open())
  {
    cerr << "Error: Cannot open file " << filename << endl;
    return data;
  }

  getline(file, line);

  int index = 0;
  while (getline(file, line))
  {
    if (line.empty())
      continue;
    stringstream ss(line);
    string cell;
    vector<double> values;

    getline(ss, cell, ',');

    while (getline(ss, cell, ','))
    {
      try
      {
        values.push_back(stod(cell));
      }
      catch (...)
      {
        cerr << "Warning: Invalid number -> " << cell << endl;
      }
    }

    if (!values.empty())
    {
      data.push_back(Point(index, values));
      index++;
    }
  }

  return data;
}

double euclideanDistance(const vector<double> &a, const vector<double> &b)
{
  double sum = 0;
  for (size_t i = 0; i < a.size(); i++)
  {
    sum += pow(a[i] - b[i], 2);
  }
  return sqrt(sum);
}

void computeDistanceMatrix()
{
  int n = dataset.size();
  distanceMatrix.resize(n, vector<double>(n, 0));

  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      double dist = euclideanDistance(dataset[i].values, dataset[j].values);
      distanceMatrix[i][j] = dist;
      distanceMatrix[j][i] = dist;
    }
  }
}

vector<int> getNeighbors(int pointIndex)
{
  vector<int> neighbors;
  for (int i = 0; i < dataset.size(); i++)
  {
    if (distanceMatrix[pointIndex][i] <= eps)
    {
      neighbors.push_back(i);
    }
  }
  return neighbors;
}

void expandCluster(int point, vector<int> &neighbors, int clusterId,
                   vector<bool> &visited, vector<int> &cluster, vector<string> &pointType)
{
  cluster[point] = clusterId;
  size_t i = 0;

  while (i < neighbors.size())
  {
    int nIdx = neighbors[i];

    if (!visited[nIdx])
    {
      visited[nIdx] = true;
      vector<int> nNeighbors = getNeighbors(nIdx);

      if (nNeighbors.size() >= minPts)
      {
        for (int x : nNeighbors)
        {
          if (find(neighbors.begin(), neighbors.end(), x) == neighbors.end())
          {
            neighbors.push_back(x);
          }
        }
      }
    }

    if (cluster[nIdx] == 0)
    {
      cluster[nIdx] = clusterId;
    }

    i++;
  }
}

void dbscan(vector<int> &cluster, vector<string> &pointType)
{
  int clusterId = 1;
  vector<bool> visited(dataset.size(), false);
  cluster.assign(dataset.size(), 0);
  pointType.assign(dataset.size(), "");

  for (int i = 0; i < dataset.size(); i++)
  {
    vector<int> neighbors = getNeighbors(i);
    if (neighbors.size() >= minPts)
    {
      pointType[i] = "Core";
    }
  }

  for (int i = 0; i < dataset.size(); i++)
  {
    if (visited[i])
      continue;
    visited[i] = true;

    vector<int> neighbors = getNeighbors(i);

    if (neighbors.size() < minPts)
    {
      cluster[i] = -1;
      if (pointType[i].empty())
        pointType[i] = "Noise";
    }
    else
    {
      clusterId++;
      cluster[i] = clusterId;
      expandCluster(i, neighbors, clusterId, visited, cluster, pointType);
    }
  }

  for (int i = 0; i < dataset.size(); i++)
  {
    if (cluster[i] != -1 && pointType[i].empty())
    {
      pointType[i] = "Border";
    }
  }
}

void printResults(const vector<int> &cluster, const vector<string> &pointType)
{
  cout << "\nDBSCAN Results (ε = " << eps << ", minPts = " << minPts << "):\n";

  map<int, vector<int>> clusterMap;
  for (int i = 0; i < cluster.size(); i++)
  {
    clusterMap[cluster[i]].push_back(i);
  }

  cout << "\nClusters:\n";
  for (auto &pair : clusterMap)
  {
    if (pair.first == -1)
    {
      cout << "Noise: ";
    }
    else
    {
      cout << "Cluster " << pair.first << ": ";
    }
    for (int idx : pair.second)
    {
      cout << "P" << idx << " ";
    }
    cout << endl;
  }

  cout << "\nPoint details:\n";
  for (int i = 0; i < dataset.size(); i++)
  {
    cout << "Point " << i << " -> ";
    if (cluster[i] == -1)
    {
      cout << "Noise";
    }
    else
    {
      cout << "Cluster " << cluster[i];
    }
    cout << " -> " << pointType[i] << endl;
  }

  int noise = count(cluster.begin(), cluster.end(), -1);
  int maxCluster = *max_element(cluster.begin(), cluster.end());

  cout << "\nSummary:\n";
  cout << "Total points: " << dataset.size() << "\n";
  cout << "Noise points: " << noise << "\n";
  cout << "Clusters found: " << (maxCluster > 0 ? maxCluster : 0) << "\n";

  for (int c = 1; c <= maxCluster; c++)
  {
    int cnt = count(cluster.begin(), cluster.end(), c);
    cout << "Cluster " << c << " size: " << cnt << "\n";
  }
}

void saveResults(const vector<int> &cluster, const vector<string> &pointType, const string &filename)
{
  ofstream out(filename);
  out << "Point,Cluster,Type\n";
  for (int i = 0; i < dataset.size(); i++)
  {
    out << "P" << i << ",";
    if (cluster[i] == -1)
    {
      out << "Noise";
    }
    else
    {
      out << cluster[i];
    }
    out << "," << pointType[i] << "\n";
  }
  out.close();
  cout << "Saved results to: " << filename << "\n";
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " <data.csv>\n";
    return 1;
  }

  string file = argv[1];
  dataset = loadData(file);
  if (dataset.empty())
  {
    cerr << "No valid data found.\n";
    return 1;
  }

  cout << "Enter epsilon (ε): ";
  while (!(cin >> eps) || eps <= 0)
  {
    cin.clear();
    cin.ignore(1000, '\n');
    cout << "Invalid input. Enter a positive number for epsilon: ";
  }

  cout << "Enter minimum points (minPts): ";
  while (!(cin >> minPts) || minPts <= 0)
  {
    cin.clear();
    cin.ignore(1000, '\n');
    cout << "Invalid input. Enter a positive integer for minPts: ";
  }

  computeDistanceMatrix();

  vector<int> cluster;
  vector<string> pointType;
  dbscan(cluster, pointType);

  printResults(cluster, pointType);
  saveResults(cluster, pointType, "dbscan_results.csv");

  return 0;
}