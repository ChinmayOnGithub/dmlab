#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <limits>
#include <cctype>
using namespace std;

vector<vector<double>> dataset;
vector<string> pointNames;
string linkageMethod;

double euclideanDistance(const vector<double> &a, const vector<double> &b)
{
  double sum = 0;
  for (size_t i = 0; i < a.size(); i++)
  {
    sum += pow(a[i] - b[i], 2);
  }
  return sqrt(sum);
}

void loadSingleColumnData(const string &filename)
{
  ifstream file(filename);
  string line;

  // Skip header line
  if (!getline(file, line))
  {
    cerr << "Error: Empty file or cannot read header" << endl;
    return;
  }

  cout << "Header: " << line << endl;

  while (getline(file, line))
  {
    // Skip empty lines
    if (line.empty())
      continue;

    stringstream ss(line);
    string name;
    string valueStr;
    double value;

    // Read name (first column)
    if (!getline(ss, name, ','))
    {
      cerr << "Error: Cannot read name from line: " << line << endl;
      continue;
    }

    // Read value (second column)
    if (!getline(ss, valueStr))
    {
      cerr << "Error: Cannot read value from line: " << line << endl;
      continue;
    }

    // Trim whitespace from value string
    valueStr.erase(0, valueStr.find_first_not_of(" \t"));
    valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

    try
    {
      value = stod(valueStr);
      pointNames.push_back(name);
      dataset.push_back({value});
      cout << "Loaded: " << name << " = " << value << endl;
    }
    catch (const exception &e)
    {
      cerr << "Error converting value '" << valueStr << "' to double: " << e.what() << endl;
    }
  }
  file.close();
}

void loadMultiColumnData(const string &filename)
{
  ifstream file(filename);
  string line;

  // Read and parse header
  if (!getline(file, line))
  {
    cerr << "Error: Empty file" << endl;
    return;
  }

  stringstream ss(line);
  string header;
  vector<string> headers;

  while (getline(ss, header, ','))
  {
    headers.push_back(header);
  }

  cout << "Found " << headers.size() - 1 << " data columns" << endl;

  // Read data rows
  while (getline(file, line))
  {
    if (line.empty())
      continue;

    stringstream ss(line);
    vector<double> row;
    string value;
    string name;

    // First column is the name
    if (!getline(ss, name, ','))
    {
      cerr << "Error: Cannot read name from line: " << line << endl;
      continue;
    }

    pointNames.push_back(name);

    // Read data values
    while (getline(ss, value, ','))
    {
      if (!value.empty())
      {
        try
        {
          row.push_back(stod(value));
        }
        catch (const exception &e)
        {
          cerr << "Error converting value '" << value << "' to double: " << e.what() << endl;
          row.push_back(0.0);
        }
      }
    }
    dataset.push_back(row);
  }
  file.close();
}

vector<vector<double>> computeDistanceMatrix()
{
  int n = dataset.size();
  vector<vector<double>> distMatrix(n, vector<double>(n, 0));

  for (int i = 0; i < n; i++)
  {
    for (int j = i + 1; j < n; j++)
    {
      double dist = euclideanDistance(dataset[i], dataset[j]);
      distMatrix[i][j] = dist;
      distMatrix[j][i] = dist;
    }
  }
  return distMatrix;
}

bool isSingleColumnData(const string &filename)
{
  ifstream file(filename);
  string line;

  if (!getline(file, line))
  {
    cerr << "Error: Cannot read file" << endl;
    return false;
  }

  stringstream ss(line);
  string token;
  int count = 0;
  while (getline(ss, token, ','))
  {
    count++;
  }

  file.close();

  cout << "Column count in header: " << count << endl;
  return count == 2;
}

string mergeClusters(const string &a, const string &b)
{
  vector<string> temp = {a, b};
  sort(temp.begin(), temp.end());
  return "(" + temp[0] + "+" + temp[1] + ")";
}

vector<string> getAllPoints(const string &cluster)
{
  vector<string> result;
  string temp = cluster;

  // Remove parentheses
  temp.erase(remove(temp.begin(), temp.end(), '('), temp.end());
  temp.erase(remove(temp.begin(), temp.end(), ')'), temp.end());

  stringstream ss(temp);
  string item;
  while (getline(ss, item, '+'))
  {
    result.push_back(item);
  }
  return result;
}

double calculateDistance(const string &cluster1, const string &cluster2,
                         const vector<vector<double>> &distanceMatrix,
                         const vector<string> &allPointNames)
{
  vector<string> points1 = getAllPoints(cluster1);
  vector<string> points2 = getAllPoints(cluster2);

  vector<int> indices1, indices2;

  for (const auto &point : points1)
  {
    auto it = find(allPointNames.begin(), allPointNames.end(), point);
    if (it != allPointNames.end())
    {
      indices1.push_back(it - allPointNames.begin());
    }
  }

  for (const auto &point : points2)
  {
    auto it = find(allPointNames.begin(), allPointNames.end(), point);
    if (it != allPointNames.end())
    {
      indices2.push_back(it - allPointNames.begin());
    }
  }

  if (linkageMethod == "single")
  {
    double minDist = numeric_limits<double>::max();
    for (int i : indices1)
    {
      for (int j : indices2)
      {
        if (i != j)
        {
          double dist = distanceMatrix[i][j];
          if (dist < minDist)
            minDist = dist;
        }
      }
    }
    return minDist;
  }
  else if (linkageMethod == "complete")
  {
    double maxDist = 0;
    for (int i : indices1)
    {
      for (int j : indices2)
      {
        if (i != j)
        {
          double dist = distanceMatrix[i][j];
          if (dist > maxDist)
            maxDist = dist;
        }
      }
    }
    return maxDist;
  }
  else if (linkageMethod == "average")
  {
    double total = 0;
    int count = 0;
    for (int i : indices1)
    {
      for (int j : indices2)
      {
        if (i != j)
        {
          total += distanceMatrix[i][j];
          count++;
        }
      }
    }
    return count > 0 ? total / count : 0;
  }
  return 0;
}

void cluster(const vector<vector<double>> &distanceMatrix)
{
  ofstream output("output.csv");
  output << "Step,Cluster1,Cluster2,Distance\n";

  vector<string> clusters = pointNames;
  int step = 1;

  cout << "\nHierarchical Clustering with " << linkageMethod << " linkage\n";
  cout << "Initial clusters: ";
  for (const auto &c : clusters)
    cout << c << " ";
  cout << "\n\n";

  while (clusters.size() > 1)
  {
    string p1, p2;
    double min_dist = numeric_limits<double>::max();

    for (size_t i = 0; i < clusters.size(); i++)
    {
      for (size_t j = i + 1; j < clusters.size(); j++)
      {
        double dist = calculateDistance(clusters[i], clusters[j], distanceMatrix, pointNames);
        if (dist < min_dist)
        {
          min_dist = dist;
          p1 = clusters[i];
          p2 = clusters[j];
        }
      }
    }

    cout << "Step " << step << ": Merging " << p1 << " & " << p2 << " (distance: " << min_dist << ")\n";
    output << step << "," << p1 << "," << p2 << "," << min_dist << "\n";

    string new_cluster = mergeClusters(p1, p2);

    clusters.erase(remove(clusters.begin(), clusters.end(), p1), clusters.end());
    clusters.erase(remove(clusters.begin(), clusters.end(), p2), clusters.end());
    clusters.push_back(new_cluster);

    step++;
  }

  output.close();
  cout << "\nClustering complete! Results saved to output.csv\n";
  cout << "Final cluster: " << clusters[0] << endl;
}

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    cout << "Usage: " << argv[0] << " <input_file> <linkage_method>\n";
    cout << "Linkage methods: single, complete, average\n";
    cout << "Example: " << argv[0] << " data.csv single\n";
    return 1;
  }

  string filename = argv[1];
  linkageMethod = argv[2];

  if (linkageMethod != "single" && linkageMethod != "complete" && linkageMethod != "average")
  {
    cout << "Error: Invalid linkage method. Use: single, complete, or average\n";
    return 1;
  }

  vector<vector<double>> distanceMatrix;

  if (isSingleColumnData(filename))
  {
    cout << "Loading single-column data from: " << filename << endl;
    loadSingleColumnData(filename);
    cout << "Computing distance matrix from 1D data..." << endl;
    distanceMatrix = computeDistanceMatrix();
  }
  else
  {
    cout << "Loading multi-dimensional data from: " << filename << endl;
    loadMultiColumnData(filename);
    cout << "Computing distance matrix..." << endl;
    distanceMatrix = computeDistanceMatrix();
  }

  cout << "Loaded " << pointNames.size() << " points" << endl;

  // Print the distance matrix for debugging
  cout << "\nDistance Matrix:" << endl;
  for (size_t i = 0; i < pointNames.size(); i++)
  {
    cout << pointNames[i] << ": ";
    for (size_t j = 0; j < pointNames.size(); j++)
    {
      cout << distanceMatrix[i][j] << " ";
    }
    cout << endl;
  }

  cluster(distanceMatrix);
  return 0;
}