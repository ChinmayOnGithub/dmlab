#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
using namespace std;

string trim(const string &str)
{
  size_t start = str.find_first_not_of(" \t\n\r");
  size_t end = str.find_last_not_of(" \t\n\r");
  if (start == string::npos || end == string::npos)
    return "";
  return str.substr(start, end - start + 1);
}

bool isNumeric(const string &str)
{
  if (str.empty())
    return false;
  if (str == "N/A" || str == "NA" || str == "-" || str == "null")
    return false;

  bool hasDecimal = false, hasDigit = false;
  for (char c : str)
  {
    if (c == '.' && !hasDecimal)
      hasDecimal = true;
    else if (!isdigit(c) && c != ' ' && c != ',')
      return false;
    else if (isdigit(c))
      hasDigit = true;
  }
  return hasDigit;
}

double toDouble(const string &str)
{
  string cleanStr;
  for (char c : str)
    if (c != ',')
      cleanStr += c;
  try
  {
    return stod(cleanStr);
  }
  catch (...)
  {
    return 0.0;
  }
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  string inputFilename = argv[1];
  ifstream inputFile(inputFilename);
  if (!inputFile.is_open())
  {
    cerr << "Error: Could not open input file " << inputFilename << endl;
    return 1;
  }

  vector<vector<string>> data;
  vector<string> headers;
  string line;

  if (getline(inputFile, line))
  {
    stringstream ss(line);
    string header;
    while (getline(ss, header, ','))
      headers.push_back(trim(header));
  }

  while (getline(inputFile, line))
  {
    stringstream ss(line);
    string cell;
    vector<string> row;

    while (getline(ss, cell, ','))
      row.push_back(trim(cell));
    if (row.size() >= 3)
      data.push_back(row);
  }
  inputFile.close();

  if (headers.size() < 3)
  {
    cerr << "Error: Expected at least 3 columns (row dimension, column dimension, measure)" << endl;
    return 1;
  }

  int rowDim = 0, colDim = 1, measureCol = 2;
  
  cout << "T-Weight D-Weight Analysis:" << endl;
  cout << "Using standard format:" << endl;
  cout << "  Row Dimension: " << headers[rowDim] << " (column " << rowDim << ")" << endl;
  cout << "  Column Dimension: " << headers[colDim] << " (column " << colDim << ")" << endl;
  cout << "  Measure: " << headers[measureCol] << " (column " << measureCol << ")" << endl;
  cout << endl;

  map<string, map<string, double>> pivotTable;
  map<string, double> rowTotals;
  map<string, double> colTotals;
  double grandTotal = 0.0;

  vector<string> rowNames, colNames;
  for (const auto &row : data)
  {
    string rowName = row[rowDim];
    string colName = row[colDim];

    if (find(rowNames.begin(), rowNames.end(), rowName) == rowNames.end())
      rowNames.push_back(rowName);
    if (find(colNames.begin(), colNames.end(), colName) == colNames.end())
      colNames.push_back(colName);

    double value = isNumeric(row[measureCol]) ? toDouble(row[measureCol]) : 0.0;
    pivotTable[rowName][colName] = value;
    rowTotals[rowName] += value;
    colTotals[colName] += value;
    grandTotal += value;
  }

  ofstream outputFile("output.csv");
  if (!outputFile.is_open())
  {
    cerr << "Error: Could not create output file output.csv" << endl;
    return 1;
  }

  outputFile << headers[rowDim] << "\\" << headers[colDim] << ",";
  for (const auto &col : colNames)
    outputFile << col << ",,,";
  outputFile << "Total,," << endl;

  outputFile << ",";
  for (size_t i = 0; i < colNames.size(); i++)
    outputFile << "Count,t-weight,d-weight,";
  outputFile << "Count,t-weight,d-weight" << endl;

  for (const auto &row : rowNames)
  {
    outputFile << row << ",";
    double rowTotal = rowTotals[row];

    for (const auto &col : colNames)
    {
      double value = pivotTable[row][col];
      double tWeight = (rowTotal > 0) ? (value / rowTotal) * 100 : 0;
      double dWeight = (colTotals[col] > 0) ? (value / colTotals[col]) * 100 : 0;

      outputFile << value << "," << fixed << setprecision(2) << tWeight << "%," << dWeight << "%,";
    }

    double totalDWeight = (grandTotal > 0) ? (rowTotal / grandTotal) * 100 : 0;
    outputFile << rowTotal << "," << fixed << setprecision(2) << "100.00%," << totalDWeight << "%" << endl;
  }

  outputFile << "Total,";
  for (const auto &col : colNames)
  {
    double colTotal = colTotals[col];
    double tWeight = (grandTotal > 0) ? (colTotal / grandTotal) * 100 : 0;
    outputFile << colTotal << "," << fixed << setprecision(2) << tWeight << "%,100.00%,";
  }

  outputFile << grandTotal << ",100.00%,100.00%" << endl;
  outputFile.close();

  cout << "Successfully generated pivot table: output.csv" << endl;
  return 0;
}