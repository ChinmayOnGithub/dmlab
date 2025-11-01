#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;

double median_of_range(const vector<double> &v, size_t lo, size_t hi)
{
  size_t len = hi - lo + 1;
  if (len % 2)
    return v[lo + len / 2];
  return (v[lo + len / 2 - 1] + v[lo + len / 2]) / 2.0;
}

void five_number_summary(const vector<double> &data, double &minv, double &q1, double &med, double &q3, double &maxv)
{
  vector<double> v = data;
  sort(v.begin(), v.end());
  minv = v.front();
  maxv = v.back();
  size_t n = v.size();
  med = (n % 2) ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2.0;
  if (n % 2)
  {
    q1 = median_of_range(v, 0, n / 2 - 1);
    q3 = median_of_range(v, n / 2 + 1, n - 1);
  }
  else
  {
    q1 = median_of_range(v, 0, n / 2 - 1);
    q3 = median_of_range(v, n / 2, n - 1);
  }
}

vector<double> detect_outliers(const vector<double> &data, double q1, double q3)
{
  double iqr = q3 - q1, lower = q1 - 1.5 * iqr, upper = q3 + 1.5 * iqr;
  vector<double> o;
  for (double x : data)
    if (x < lower || x > upper)
      o.push_back(x);
  return o;
}

map<string, vector<double>> read_csv(const string &path)
{
  ifstream f(path);
  map<string, vector<double>> cols;
  string line;
  if (!getline(f, line))
    return cols;

  vector<string> headers;
  stringstream hs(line);
  string h;
  while (getline(hs, h, ','))
    headers.push_back(h);
  for (auto &x : headers)
    cols[x];

  while (getline(f, line))
  {
    stringstream ss(line);
    string val;
    int i = 0;
    while (getline(ss, val, ','))
    {
      try
      {
        cols[headers[i]].push_back(stod(val));
      }
      catch (...)
      {
      }
      i++;
    }
  }
  return cols;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  auto cols = read_csv(argv[1]);
  if (cols.empty())
  {
    cout << "No numeric columns found.\n";
    return 1;
  }

  vector<string> keys;
  cout << "Available numeric columns:\n";
  for (auto &kv : cols)
    if (!kv.second.empty())
    {
      keys.push_back(kv.first);
      cout << keys.size() << ". " << kv.first << "\n";
    }

  int ch;
  cout << "Select column: ";
  cin >> ch;
  if (ch < 1 || ch > (int)keys.size())
  {
    cout << "Invalid selection.\n";
    return 1;
  }

  auto data = cols[keys[ch - 1]];
  double minv, q1, med, q3, maxv;
  five_number_summary(data, minv, q1, med, q3, maxv);
  auto outliers = detect_outliers(data, q1, q3);

  double iqr = q3 - q1;
  double lower_bound = q1 - 1.5 * iqr;
  double upper_bound = q3 + 1.5 * iqr;

  vector<double> filtered;
  for (double x : data)
    if (x >= lower_bound && x <= upper_bound)
      filtered.push_back(x);

  sort(filtered.begin(), filtered.end());
  double whisker_min = minv;
  double whisker_max = maxv;
  if (!filtered.empty())
  {
    whisker_min = filtered.front();
    whisker_max = filtered.back();
  }

  cout << "\nFive-Number Summary:\n";
  cout << " Min: " << minv << "\n";
  cout << " Q1: " << q1 << "\n";
  cout << " Median: " << med << "\n";
  cout << " Q3: " << q3 << "\n";
  cout << " Max: " << maxv << "\n";
  cout << " IQR: " << iqr << "\n";

  cout << "\nBox Plot Whiskers:\n";
  cout << " Lower Whisker: " << whisker_min << "\n";
  cout << " Upper Whisker: " << whisker_max << "\n";

  cout << "\n Outliers: ";
  if (outliers.empty())
    cout << "None\n";
  else
  {
    for (double x : outliers)
      cout << x << " ";
    cout << "\n";
  }

  ofstream fout("output.csv");
  fout << "Statistic,Value\n";
  fout << "Min," << minv << "\n";
  fout << "Q1," << q1 << "\n";
  fout << "Median," << med << "\n";
  fout << "Q3," << q3 << "\n";
  fout << "Max," << maxv << "\n";
  fout << "IQR," << iqr << "\n";
  fout << "Lower_Whisker," << whisker_min << "\n";
  fout << "Upper_Whisker," << whisker_max << "\n";
  if (!outliers.empty())
  {
    fout << "Outliers,";
    for (size_t i = 0; i < outliers.size(); ++i)
    {
      fout << outliers[i];
      if (i + 1 < outliers.size())
        fout << " ";
    }
    fout << "\n";
  }
  fout.close();

  cout << "\nResults saved to output.csv\n";
}
