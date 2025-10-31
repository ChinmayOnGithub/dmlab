#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cmath>
#include <iomanip>
using namespace std;

double entropy(map<string, int> &freq)
{
  double total = 0, e = 0;
  for (auto &f : freq)
    total += f.second;
  for (auto &f : freq)
  {
    if (f.second == 0)
      continue;
    double p = f.second / total;
    e -= p * log2(p);
  }
  return e;
}

double gini(map<string, int> &freq)
{
  double total = 0, g = 1;
  for (auto &f : freq)
    total += f.second;
  for (auto &f : freq)
  {
    double p = f.second / total;
    g -= p * p;
  }
  return g;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  ifstream fin(argv[1]);
  if (!fin.is_open())
  {
    cout << "Error opening file.\n";
    return 1;
  }

  string header;
  getline(fin, header);
  vector<string> headers;
  stringstream hs(header);
  string tmp;
  while (getline(hs, tmp, ','))
    headers.push_back(tmp);

  vector<vector<string>> data;
  while (getline(fin, tmp))
  {
    if (tmp.empty())
      continue;
    stringstream ss(tmp);
    vector<string> row;
    string val;
    while (getline(ss, val, ','))
      row.push_back(val);
    data.push_back(row);
  }
  fin.close();

  cout << "\nAvailable columns:\n";
  for (int i = 0; i < headers.size(); i++)
    cout << i + 1 << ". " << headers[i] << "\n";

  string input;
  cout << "\nEnter class (target) column number or name: ";
  cin >> input;
  int classCol = -1;
  if (isdigit(input[0]))
    classCol = stoi(input);
  else
  {
    for (int i = 0; i < headers.size(); i++)
      if (headers[i] == input)
        classCol = i + 1;
  }
  if (classCol < 1 || classCol > headers.size())
  {
    cout << "Invalid target column.\n";
    return 1;
  }

  vector<string> classLabels;
  for (auto &r : data)
    classLabels.push_back(r[classCol - 1]);
  map<string, int> parentFreq;
  for (auto &c : classLabels)
    parentFreq[c]++;
  double parentEntropy = entropy(parentFreq);
  double parentGini = gini(parentFreq);

  cout << "\nParent Entropy: " << parentEntropy << "\n";
  cout << "Parent Gini: " << parentGini << "\n\n";

  ofstream fout("output.csv");
  fout << fixed << setprecision(6);
  fout << "Attribute,Gain,Gini\n";

  double bestGain = -1, bestGini = 1;
  string bestGainAttr, bestGiniAttr;

  for (int c = 0; c < headers.size(); c++)
  {
    if (c + 1 == classCol)
      continue;

    map<string, map<string, int>> freq;
    for (int i = 0; i < data.size(); i++)
      freq[data[i][c]][classLabels[i]]++;

    double total = data.size();
    double weightedEntropy = 0, weightedGini = 0;
    for (auto &kv : freq)
    {
      double subset = 0;
      for (auto &x : kv.second)
        subset += x.second;
      weightedEntropy += (subset / total) * entropy(kv.second);
      weightedGini += (subset / total) * gini(kv.second);
    }

    double gain = parentEntropy - weightedEntropy;
    fout << headers[c] << "," << gain << "," << weightedGini << "\n";

    if (gain > bestGain)
    {
      bestGain = gain;
      bestGainAttr = headers[c];
    }
    if (weightedGini < bestGini)
    {
      bestGini = weightedGini;
      bestGiniAttr = headers[c];
    }
  }

  fout << "\nBest by Gain," << bestGainAttr << "," << bestGain
       << "\nBest by Gini," << bestGiniAttr << "," << bestGini << "\n";
  fout.close();

  cout << "\nBest attribute by Gain: " << bestGainAttr << " (" << bestGain << ")\n";
  cout << "Best attribute by Gini: " << bestGiniAttr << " (" << bestGini << ")\n";
  cout << "Results written to output.csv\n";

  return 0;
}