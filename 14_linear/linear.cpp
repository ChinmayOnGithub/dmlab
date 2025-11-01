#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  ifstream file(argv[1]);
  string line;
  vector<double> X, Y;
  vector<string> headers;

  if (getline(file, line))
  {
    stringstream ss(line);
    string header;
    while (getline(ss, header, ','))
    {
      headers.push_back(header);
    }
  }

  if (headers.size() < 2)
  {
    cerr << "Error: Expected at least 2 columns for linear regression" << endl;
    return 1;
  }

  int xCol = 0, yCol = 1;
  
  cout << "Linear Regression Analysis:" << endl;
  cout << "Using standard format:" << endl;
  cout << "  X (Independent Variable): " << headers[xCol] << " (column " << xCol << ")" << endl;
  cout << "  Y (Dependent Variable): " << headers[yCol] << " (column " << yCol << ")" << endl;
  cout << endl;

  while (getline(file, line))
  {
    stringstream ss(line);
    vector<string> row;
    string cell;
    while (getline(ss, cell, ','))
    {
      row.push_back(cell);
    }
    
    if (row.size() > max(xCol, yCol))
    {
      try
      {
        X.push_back(stod(row[xCol]));
        Y.push_back(stod(row[yCol]));
      }
      catch (...)
      {
        cerr << "Warning: Invalid numeric data in row, skipping..." << endl;
      }
    }
  }
  file.close();

  int n = X.size();
  double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

  for (int i = 0; i < n; i++)
  {
    sumX += X[i];
    sumY += Y[i];
    sumXY += X[i] * Y[i];
    sumX2 += X[i] * X[i];
  }

  double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
  double intercept = (sumY - slope * sumX) / n;

  cout << "Linear Regression Equation:\n";
  cout << headers[yCol] << " = " << slope << " * " << headers[xCol] << " + " << intercept << "\n";

  double x_val;
  cout << "Enter " << headers[xCol] << " value to predict " << headers[yCol] << ": ";
  cin >> x_val;

  double y_pred = slope * x_val + intercept;
  cout << "Predicted " << headers[yCol] << " = " << y_pred << "\n";

  ofstream fout("output.csv");
  fout << headers[xCol] << "," << headers[yCol] << ",Predicted_" << headers[yCol] << "\n";
  for (int i = 0; i < n; i++)
  {
    fout << X[i] << "," << Y[i] << "," << (slope * X[i] + intercept) << "\n";
  }
  fout << x_val << ",," << y_pred << "\n";
  fout.close();

  cout << "Results saved to output.csv\n";

  return 0;
}