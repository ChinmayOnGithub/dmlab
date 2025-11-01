#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
using namespace std;

vector<vector<string>> dataset;
vector<string> headers;

void loadData(const string &filename)
{
  ifstream file(filename);
  string line;

  getline(file, line);
  stringstream ss(line);
  string header;
  while (getline(ss, header, ','))
  {
    headers.push_back(header);
  }

  while (getline(file, line))
  {
    stringstream ss(line);
    vector<string> row;
    string value;
    while (getline(ss, value, ','))
    {
      row.push_back(value);
    }
    dataset.push_back(row);
  }
  file.close();
}

double getPrior(const string &targetClass, int targetCol)
{
  int count = 0;
  for (auto &row : dataset)
  {
    if (row[targetCol] == targetClass)
      count++;
  }
  return (double)count / dataset.size();
}

bool isNumeric(const string &str)
{
  try
  {
    stod(str);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

double getConditional(const string &attr, const string &val, const string &targetClass, int attrCol, int targetCol)
{
  int total = 0, count = 0;

  for (auto &row : dataset)
  {
    if (row[targetCol] == targetClass)
    {
      total++;
      if (row[attrCol] == val)
        count++;
    }
  }

  if (total == 0)
  {
    return 0;
  }

  return (double)count / total;
}

double getGaussianProbability(double value, const string &targetClass, int attrCol, int targetCol)
{
  vector<double> values;
  
  for (auto &row : dataset)
  {
    if (row[targetCol] == targetClass)
    {
      try
      {
        values.push_back(stod(row[attrCol]));
      }
      catch (...)
      {
        continue;
      }
    }
  }
  
  if (values.empty())
    return 0;
  
  double mean = 0;
  for (double v : values)
    mean += v;
  mean /= values.size();
  
  double variance = 0;
  for (double v : values)
    variance += (v - mean) * (v - mean);
  variance /= values.size();
  
  if (variance == 0)
    return (value == mean) ? 1.0 : 0.0;
  
  double stddev = sqrt(variance);
  double exponent = -((value - mean) * (value - mean)) / (2 * variance);
  return (1.0 / (stddev * sqrt(2 * M_PI))) * exp(exponent);
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  loadData(argv[1]);

  cout << "Available columns:\n";
  for (int i = 0; i < headers.size(); i++)
  {
    cout << i + 1 << ". " << headers[i] << "\n";
  }

  string input;
  cout << "Enter target column number or name: ";
  cin >> input;

  int targetCol = -1;
  if (isdigit(input[0]))
    targetCol = stoi(input) - 1;
  else
  {
    for (int i = 0; i < headers.size(); i++)
    {
      if (headers[i] == input)
        targetCol = i;
    }
  }

  if (targetCol < 0 || targetCol >= headers.size())
  {
    cout << "Invalid target column.\n";
    return 1;
  }

  vector<string> featureCols;
  for (int i = 0; i < headers.size(); i++)
  {
    if (i != targetCol)
      featureCols.push_back(headers[i]);
  }

  map<string, string> testData;
  map<string, bool> isNumericFeature;
  
  for (auto &feature : featureCols)
  {
    int featureCol = -1;
    for (int i = 0; i < headers.size(); i++)
    {
      if (headers[i] == feature)
        featureCol = i;
    }
    
    bool numeric = true;
    for (int i = 0; i < min(5, (int)dataset.size()); i++)
    {
      if (!isNumeric(dataset[i][featureCol]))
      {
        numeric = false;
        break;
      }
    }
    isNumericFeature[feature] = numeric;
    
    cout << "Enter value for " << feature;
    if (numeric)
      cout << " (numeric)";
    else
      cout << " (categorical)";
    cout << ": ";
    cin >> input;
    testData[feature] = input;
  }

  map<string, int> classCount;
  for (auto &row : dataset)
  {
    classCount[row[targetCol]]++;
  }

  ofstream fout("output.csv");
  fout << "Class,Probability\n";

  double totalProb = 0;
  map<string, double> classProbabilities;

  for (auto &cls : classCount)
  {
    string className = cls.first;
    double probability = getPrior(className, targetCol);

    for (auto &feature : featureCols)
    {
      int attrCol = -1;
      for (int i = 0; i < headers.size(); i++)
      {
        if (headers[i] == feature)
          attrCol = i;
      }
      
      if (isNumericFeature[feature])
      {
        double value = stod(testData[feature]);
        probability *= getGaussianProbability(value, className, attrCol, targetCol);
      }
      else
      {
        probability *= getConditional(feature, testData[feature], className, attrCol, targetCol);
      }
    }

    classProbabilities[className] = probability;
    totalProb += probability;
  }

  string prediction;
  double maxProb = -1;

  for (auto &cp : classProbabilities)
  {
    double normalizedProb = cp.second / totalProb;
    fout << cp.first << "," << normalizedProb << "\n";

    if (normalizedProb > maxProb)
    {
      maxProb = normalizedProb;
      prediction = cp.first;
    }
  }

  fout.close();

  cout << "\nPrediction: " << prediction << " (Probability: " << maxProb << ")\n";
  cout << "Results saved to output.csv\n";

  return 0;
}