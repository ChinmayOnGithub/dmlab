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

  getline(file, line);
  while (getline(file, line))
  {
    stringstream ss(line);
    string x_val, y_val;
    getline(ss, x_val, ',');
    getline(ss, y_val, ',');
    X.push_back(stod(x_val));
    Y.push_back(stod(y_val));
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

  cout << "Equation: Y = " << slope << " * X + " << intercept << "\n";

  double x_val;
  cout << "Enter X value to predict Y: ";
  cin >> x_val;

  double y_pred = slope * x_val + intercept;
  cout << "Predicted Y = " << y_pred << "\n";

  ofstream fout("output.csv");
  fout << "X,Y,Predicted_Y\n";
  for (int i = 0; i < n; i++)
  {
    fout << X[i] << "," << Y[i] << "," << (slope * X[i] + intercept) << "\n";
  }
  fout << x_val << ",," << y_pred << "\n";
  fout.close();

  cout << "Results saved to output.csv\n";

  return 0;
}