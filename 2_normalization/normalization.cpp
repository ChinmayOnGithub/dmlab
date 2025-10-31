#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <algorithm>
#include <set>
using namespace std;

double calculateMean(const vector<double> &values)
{
  double sum = 0.0;
  for (double v : values)
    sum += v;
  return sum / values.size();
}

double calculateStdDev(const vector<double> &values, double mean)
{
  double variance_sum = 0.0;
  for (double v : values)
    variance_sum += (v - mean) * (v - mean);
  return sqrt(variance_sum / values.size());
}

vector<double> minMaxNormalize(const vector<double> &data, double new_min, double new_max)
{
  double data_min = *min_element(data.begin(), data.end());
  double data_max = *max_element(data.begin(), data.end());
  vector<double> normalized;

  for (double val : data)
  {
    if (data_max - data_min == 0)
      normalized.push_back(new_min);
    else
    {
      double normalized_val = ((val - data_min) / (data_max - data_min)) * (new_max - new_min) + new_min;
      normalized.push_back(normalized_val);
    }
  }
  return normalized;
}

vector<double> zScoreNormalize(const vector<double> &data)
{
  double mu = calculateMean(data);
  double sigma = calculateStdDev(data, mu);
  vector<double> normalized;

  for (double val : data)
  {
    normalized.push_back(sigma == 0 ? 0.0 : (val - mu) / sigma);
  }
  return normalized;
}

vector<double> decimalScalingNormalize(const vector<double> &data)
{
  double max_abs_val = 0.0;
  for (double val : data)
    max_abs_val = max(max_abs_val, fabs(val));

  int j = 0;
  while (max_abs_val >= 1)
  {
    max_abs_val /= 10.0;
    j++;
  }

  vector<double> normalized;
  for (double val : data)
    normalized.push_back(val / pow(10, j));
  return normalized;
}

vector<string> splitLine(const string &line, char delimiter = ',')
{
  vector<string> tokens;
  string token;
  stringstream ss(line);
  while (getline(ss, token, delimiter))
    tokens.push_back(token);
  return tokens;
}

vector<vector<string>> loadCSV(const string &filename, vector<string> &headers)
{
  ifstream file(filename);
  vector<vector<string>> data;
  if (!file.is_open())
    throw runtime_error("Could not open file " + filename);

  string line;
  if (getline(file, line))
    headers = splitLine(line);

  while (getline(file, line))
  {
    vector<string> row = splitLine(line);
    if (!row.empty())
      data.push_back(row);
  }
  file.close();
  return data;
}

void saveCSV(const string &filename, const vector<vector<string>> &data, const vector<string> &headers)
{
  ofstream file(filename);
  for (size_t i = 0; i < headers.size(); i++)
  {
    file << headers[i];
    if (i < headers.size() - 1)
      file << ",";
  }
  file << "\n";

  for (const auto &row : data)
  {
    for (size_t i = 0; i < row.size(); i++)
    {
      file << row[i];
      if (i < row.size() - 1)
        file << ",";
    }
    file << "\n";
  }
  file.close();
  cout << "Saved: " << filename << endl;
}

set<int> getColumnIndices(const vector<string> &headers)
{
  set<int> selected_columns;
  string input;

  cout << "Available columns:\n";
  for (size_t i = 0; i < headers.size(); i++)
  {
    cout << i << ": " << headers[i] << endl;
  }

  cout << "Enter column indices to normalize (comma-separated): ";
  getline(cin, input);

  stringstream ss(input);
  string token;
  while (getline(ss, token, ','))
  {
    try
    {
      int idx = stoi(token);
      if (idx >= 0 && idx < static_cast<int>(headers.size()))
      {
        selected_columns.insert(idx);
      }
    }
    catch (...)
    {
    }
  }

  return selected_columns;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  string filename = argv[1];
  vector<string> headers;
  vector<vector<string>> dataset;

  try
  {
    dataset = loadCSV(filename, headers);
  }
  catch (exception &e)
  {
    cerr << e.what() << endl;
    return 1;
  }

  set<int> columns_to_normalize = getColumnIndices(headers);
  if (columns_to_normalize.empty())
  {
    cerr << "No valid columns selected!" << endl;
    return 1;
  }

  int norm_type;
  cout << "\nSelect normalization type:\n";
  cout << "1. Min-Max Normalization\n";
  cout << "2. Z-Score Normalization\n";
  cout << "3. Decimal Scaling Normalization\n";
  cout << "Enter choice (1-3): ";
  cin >> norm_type;

  double new_min = 0.0, new_max = 1.0;
  if (norm_type == 1)
  {
    cout << "Enter new min value: ";
    cin >> new_min;
    cout << "Enter new max value: ";
    cin >> new_max;
  }

  vector<vector<string>> normalized_dataset = dataset;

  for (int col_idx : columns_to_normalize)
  {
    vector<double> column_data;
    for (const auto &row : dataset)
    {
      try
      {
        column_data.push_back(stod(row[col_idx]));
      }
      catch (...)
      {
        column_data.push_back(0.0);
      }
    }

    vector<double> normalized_column;
    switch (norm_type)
    {
    case 1:
      normalized_column = minMaxNormalize(column_data, new_min, new_max);
      break;
    case 2:
      normalized_column = zScoreNormalize(column_data);
      break;
    case 3:
      normalized_column = decimalScalingNormalize(column_data);
      break;
    default:
      cerr << "Invalid normalization type!" << endl;
      return 1;
    }

    for (size_t i = 0; i < normalized_dataset.size(); i++)
    {
      normalized_dataset[i][col_idx] = to_string(normalized_column[i]);
    }
  }

  saveCSV("output.csv", normalized_dataset, headers);
  cout << "Normalization Completed Successfully!\n";

  return 0;
}