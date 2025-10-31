#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <algorithm>
using namespace std;

vector<string> split(const string &line)
{
    vector<string> result;
    stringstream ss(line);
    string item;
    while (getline(ss, item, ','))
    {
        result.push_back(item);
    }
    return result;
}

double calculateCorrelation(const vector<double> &x, const vector<double> &y)
{
    int n = x.size();
    if (n <= 1)
        return 0.0;

    double sum_x = 0.0, sum_y = 0.0;
    double sum_xy = 0.0, sum_x2 = 0.0, sum_y2 = 0.0;

    for (int i = 0; i < n; ++i)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
        sum_y2 += y[i] * y[i];
    }

    double numerator = n * sum_xy - sum_x * sum_y;
    double denominator = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));

    if (denominator == 0.0)
        return 0.0;
    return numerator / denominator;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <input_csv_file>" << endl;
        return 1;
    }

    string inputFilename = argv[1];
    ifstream fin(inputFilename);
    if (!fin)
    {
        cerr << "Error opening input file" << endl;
        return 1;
    }

    string line;
    vector<string> headers;
    vector<vector<double>> columns;

    // Read header
    if (getline(fin, line))
    {
        headers = split(line);
        columns.resize(headers.size());
    }

    // Read data
    while (getline(fin, line))
    {
        auto row = split(line);
        for (size_t i = 0; i < row.size(); i++)
        {
            try
            {
                columns[i].push_back(stod(row[i]));
            }
            catch (...)
            {
                cerr << "Warning: Invalid number in column " << headers[i] << endl;
            }
        }
    }
    fin.close();

    cout << "Available columns:\n";
    for (size_t i = 0; i < headers.size(); i++)
    {
        cout << i << ": " << headers[i] << " (" << columns[i].size() << " values)\n";
    }

    // Ask user which columns to correlate
    int col1, col2;
    cout << "\nEnter first column number to correlate: ";
    cin >> col1;
    cout << "Enter second column number to correlate: ";
    cin >> col2;

    if (col1 < 0 || col1 >= (int)headers.size() || col2 < 0 || col2 >= (int)headers.size())
    {
        cerr << "Invalid column numbers!" << endl;
        return 1;
    }

    if (columns[col1].size() != columns[col2].size())
    {
        cerr << "Column sizes don't match!" << endl;
        return 1;
    }

    // Calculate correlation
    double correlation = calculateCorrelation(columns[col1], columns[col2]);

    // Determine correlation type
    string type;
    if (correlation == 0.0)
        type = "No relationship";
    else if (correlation > 0.7)
        type = "Strong positive correlation";
    else if (correlation > 0.3)
        type = "Moderate positive correlation";
    else if (correlation > 0.0)
        type = "Weak positive correlation";
    else if (correlation > -0.3)
        type = "Weak negative correlation";
    else if (correlation > -0.7)
        type = "Moderate negative correlation";
    else
        type = "Strong negative correlation";

    // Output results
    cout << "\n=== Correlation Results ===" << endl;
    cout << "Column 1: " << headers[col1] << endl;
    cout << "Column 2: " << headers[col2] << endl;
    cout << "Pearson Correlation Coefficient: " << fixed << setprecision(6) << correlation << endl;
    cout << "Interpretation: " << type << endl;

    // Save to file
    ofstream fout("output.csv");
    fout << "Column1,Column2,Correlation,Type\n";
    fout << headers[col1] << "," << headers[col2] << "," << correlation << "," << type << "\n";
    fout.close();

    cout << "\nResults saved to output.csv" << endl;

    return 0;
}