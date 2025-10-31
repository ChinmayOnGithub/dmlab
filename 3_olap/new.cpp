#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
using namespace std;

vector<vector<string>> dataset;
vector<string> headers;

void loadCSV(string filename)
{
  ifstream file(filename);
  string line;

  getline(file, line);
  stringstream ss(line);
  string header;
  while (getline(ss, header, ','))
    headers.push_back(header);

  while (getline(file, line))
  {
    stringstream ss(line);
    vector<string> row;
    string value;
    while (getline(ss, value, ','))
      row.push_back(value);
    dataset.push_back(row);
  }
  file.close();
}

void saveCSV(string filename, vector<vector<string>> result)
{
  ofstream file(filename);
  for (auto &row : result)
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
}

int findColumn(string name)
{
  for (size_t i = 0; i < headers.size(); i++)
  {
    if (headers[i] == name)
      return i;
  }
  return -1;
}

void rollup()
{
  cout << "Available dimensions: ";
  for (auto &h : headers)
    cout << h << " ";
  cout << "\nEnter dimension: ";

  string dimension;
  cin >> dimension;

  int col = findColumn(dimension);
  if (col == -1)
  {
    cout << "Column not found!\n";
    return;
  }

  map<string, int> counts;
  for (auto &row : dataset)
    counts[row[col]]++;

  cout << "\nRoll-up by " << dimension << ":\n";
  for (auto &pair : counts)
    cout << "  " << pair.first << ": " << pair.second << " records\n";
}

void slice()
{
  cout << "Available dimensions: ";
  for (auto &h : headers)
    cout << h << " ";
  cout << "\nEnter dimension: ";

  string dimension, value;
  cin >> dimension;
  cout << "Enter value: ";
  cin >> value;

  int col = findColumn(dimension);
  if (col == -1)
  {
    cout << "Column not found!\n";
    return;
  }

  vector<vector<string>> result;
  for (auto &row : dataset)
  {
    if (row[col] == value)
      result.push_back(row);
  }

  saveCSV("slice_output.csv", result);
  cout << "Found " << result.size() << " records\n";
  cout << "Slice saved to slice_output.csv\n";
}

void dice()
{
  cout << "Available dimensions: ";
  for (auto &h : headers)
    cout << h << " ";
  cout << "\nEnter first dimension: ";

  string dim1, val1, dim2, val2;
  cin >> dim1;
  cout << "Enter first value: ";
  cin >> val1;
  cout << "Enter second dimension: ";
  cin >> dim2;
  cout << "Enter second value: ";
  cin >> val2;

  int col1 = findColumn(dim1);
  int col2 = findColumn(dim2);
  if (col1 == -1 || col2 == -1)
  {
    cout << "Column not found!\n";
    return;
  }

  vector<vector<string>> result;
  for (auto &row : dataset)
  {
    if (row[col1] == val1 && row[col2] == val2)
      result.push_back(row);
  }

  saveCSV("dice_output.csv", result);
  cout << "Found " << result.size() << " records\n";
  cout << "Dice saved to dice_output.csv\n";
}

void cube()
{
  vector<vector<string>> cubeResult;

  for (size_t i = 0; i < headers.size(); i++)
  {
    map<string, int> counts;
    for (auto &row : dataset)
      counts[row[i]]++;

    vector<string> cubeRow;
    cubeRow.push_back("CUBE-" + headers[i]);
    for (auto &pair : counts)
    {
      cubeRow.push_back(pair.first + ":" + to_string(pair.second));
    }
    cubeResult.push_back(cubeRow);
  }

  saveCSV("cube_output.csv", cubeResult);
  cout << "CUBE operation completed\n";
  cout << "Results saved to cube_output.csv\n";
}

void pivot()
{
  cout << "Available columns: ";
  for (auto &h : headers)
    cout << h << " ";
  cout << "\nEnter row dimension: ";

  string rowDim, colDim, measure;
  cin >> rowDim;
  cout << "Enter column dimension: ";
  cin >> colDim;
  cout << "Enter measure: ";
  cin >> measure;

  int rowCol = findColumn(rowDim);
  int colCol = findColumn(colDim);
  int measureCol = findColumn(measure);

  if (rowCol == -1 || colCol == -1 || measureCol == -1)
  {
    cout << "Columns not found!\n";
    return;
  }

  map<string, map<string, double>> pivotTable;
  map<string, double> rowTotals;
  map<string, double> colTotals;
  double grandTotal = 0;

  for (auto &row : dataset)
  {
    string rowVal = row[rowCol];
    string colVal = row[colCol];
    double measureVal = stod(row[measureCol]);

    pivotTable[rowVal][colVal] += measureVal;
    rowTotals[rowVal] += measureVal;
    colTotals[colVal] += measureVal;
    grandTotal += measureVal;
  }

  vector<vector<string>> pivotResult;
  vector<string> headerRow = {rowDim + "\\" + colDim};
  for (auto &col : colTotals)
    headerRow.push_back(col.first);
  headerRow.push_back("Total");
  pivotResult.push_back(headerRow);

  for (auto &row : pivotTable)
  {
    vector<string> pivotRow = {row.first};
    for (auto &col : colTotals)
    {
      pivotRow.push_back(to_string(row.second[col.first]));
    }
    pivotRow.push_back(to_string(rowTotals[row.first]));
    pivotResult.push_back(pivotRow);
  }

  vector<string> totalRow = {"Total"};
  for (auto &col : colTotals)
    totalRow.push_back(to_string(col.second));
  totalRow.push_back(to_string(grandTotal));
  pivotResult.push_back(totalRow);

  saveCSV("pivot_output.csv", pivotResult);
  cout << "Pivot table saved to pivot_output.csv\n";
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    cout << "Usage: " << argv[0] << " <input.csv>\n";
    return 1;
  }

  loadCSV(argv[1]);
  cout << "Loaded " << dataset.size() << " records with " << headers.size() << " columns\n\n";

  while (true)
  {
    cout << "OLAP Operations:\n";
    cout << "1. Roll-up (Aggregate by dimension)\n";
    cout << "2. Slice (Filter by value)\n";
    cout << "3. Dice (Filter by multiple values)\n";
    cout << "4. CUBE (Multi-dimensional aggregation)\n";
    cout << "5. Pivot (Cross-tabulation)\n";
    cout << "6. Exit\n";
    cout << "Choose operation (1-6): ";

    string choice;
    cin >> choice;
    cout << "\n";

    if (choice == "1")
      rollup();
    else if (choice == "2")
      slice();
    else if (choice == "3")
      dice();
    else if (choice == "4")
      cube();
    else if (choice == "5")
      pivot();
    else if (choice == "6")
      break;
    else
      cout << "Invalid choice! Please enter 1-6\n";

    cout << "\n";
  }

  return 0;
}