#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>

using namespace std;

// Structure to hold CSV data
struct CSVRecord
{
  vector<string> fields;
};

// Utility functions
string trim(const string &str)
{
  size_t start = str.find_first_not_of(" \t\r\n");
  if (start == string::npos)
    return "";
  size_t end = str.find_last_not_of(" \t\r\n");
  return str.substr(start, end - start + 1);
}

string toLower(const string &str)
{
  string result = str;
  transform(result.begin(), result.end(), result.begin(), ::tolower);
  return result;
}

// Load CSV data with error handling and proper CSV parsing
vector<CSVRecord> loadCSV(const string &filename, vector<string> &headers)
{
  vector<CSVRecord> records;
  ifstream file(filename);

  if (!file.is_open())
  {
    cerr << "Error: Could not open file " << filename << endl;
    return records;
  }

  string line;
  // Read headers
  if (getline(file, line))
  {
    stringstream ss(line);
    string header;
    while (getline(ss, header, ','))
    {
      headers.push_back(trim(header));
    }
  }

  // Read data records
  while (getline(file, line))
  {
    if (line.empty())
      continue;

    CSVRecord record;
    stringstream ss(line);
    string field;
    bool inQuotes = false;
    string currentField;

    // Improved CSV parsing to handle quoted fields with commas
    for (size_t i = 0; i < line.length(); i++)
    {
      char c = line[i];

      if (c == '"')
      {
        inQuotes = !inQuotes;
      }
      else if (c == ',' && !inQuotes)
      {
        record.fields.push_back(trim(currentField));
        currentField.clear();
      }
      else
      {
        currentField += c;
      }
    }

    // Don't forget the last field
    if (!currentField.empty())
    {
      record.fields.push_back(trim(currentField));
    }

    // Ensure all records have same number of fields as headers
    if (record.fields.size() == headers.size())
    {
      records.push_back(record);
    }
    else if (record.fields.size() > 0) // Only warn if we actually have fields
    {
      cerr << "Warning: Skipped record with " << record.fields.size()
           << " fields (expected " << headers.size() << "): " << line << endl;
    }
  }

  file.close();
  return records;
}

// Find column index with case-insensitive comparison
int findColumnIndex(const vector<string> &headers, const string &columnName)
{
  string lowerColumnName = toLower(trim(columnName));
  for (size_t i = 0; i < headers.size(); i++)
  {
    if (toLower(headers[i]) == lowerColumnName)
    {
      return i;
    }
  }
  return -1;
}

// Rollup operation - count records by dimension
void rollup(const vector<CSVRecord> &records, const vector<string> &headers, const string &dimension)
{
  int colIndex = findColumnIndex(headers, dimension);
  if (colIndex == -1)
  {
    cerr << "Error: Dimension '" << dimension << "' not found!" << endl;
    cerr << "Available dimensions: ";
    for (size_t i = 0; i < headers.size(); i++)
    {
      cerr << headers[i] << (i < headers.size() - 1 ? ", " : "");
    }
    cerr << endl;
    return;
  }

  map<string, int> counts;
  for (const auto &record : records)
  {
    counts[record.fields[colIndex]]++;
  }

  cout << "Roll-up by " << dimension << ":" << endl;
  cout << "-------------------" << endl;
  for (const auto &pair : counts)
  {
    cout << pair.first << ": " << pair.second << " records" << endl;
  }
  cout << "Total: " << records.size() << " records" << endl;
}

// Slice operation - filter records by dimension value
void slice(const vector<CSVRecord> &records, const vector<string> &headers,
           const string &dimension, const string &value)
{
  int colIndex = findColumnIndex(headers, dimension);
  if (colIndex == -1)
  {
    cerr << "Error: Dimension '" << dimension << "' not found!" << endl;
    return;
  }

  string lowerValue = toLower(trim(value));
  cout << "Slice on " << dimension << " = " << value << ":" << endl;
  cout << "-------------------" << endl;
  bool found = false;
  int count = 0;

  // Print headers
  for (size_t i = 0; i < headers.size(); i++)
  {
    cout << (i > 0 ? " | " : "") << headers[i];
  }
  cout << endl;

  for (const auto &record : records)
  {
    if (toLower(record.fields[colIndex]) == lowerValue)
    {
      found = true;
      count++;
      for (size_t i = 0; i < record.fields.size(); i++)
      {
        cout << (i > 0 ? " | " : "") << record.fields[i];
      }
      cout << endl;
    }
  }

  if (!found)
  {
    cout << "No records found for " << dimension << " = " << value << endl;
  }
  else
  {
    cout << "Found " << count << " records" << endl;
  }
}

// Dice operation - filter records by multiple criteria
void dice(const vector<CSVRecord> &records, const vector<string> &headers,
          const string &dim1, const string &val1,
          const string &dim2, const string &val2)
{
  int colIndex1 = findColumnIndex(headers, dim1);
  int colIndex2 = findColumnIndex(headers, dim2);

  if (colIndex1 == -1 || colIndex2 == -1)
  {
    cerr << "Error: One or more dimensions not found!" << endl;
    if (colIndex1 == -1)
      cerr << "  Dimension not found: " << dim1 << endl;
    if (colIndex2 == -1)
      cerr << "  Dimension not found: " << dim2 << endl;
    return;
  }

  string lowerVal1 = toLower(trim(val1));
  string lowerVal2 = toLower(trim(val2));
  cout << "Dice on " << dim1 << " = " << val1 << " and " << dim2 << " = " << val2 << ":" << endl;
  cout << "-------------------" << endl;
  bool found = false;
  int count = 0;

  // Print headers
  for (size_t i = 0; i < headers.size(); i++)
  {
    cout << (i > 0 ? " | " : "") << headers[i];
  }
  cout << endl;

  for (const auto &record : records)
  {
    if (toLower(record.fields[colIndex1]) == lowerVal1 &&
        toLower(record.fields[colIndex2]) == lowerVal2)
    {
      found = true;
      count++;
      for (size_t i = 0; i < record.fields.size(); i++)
      {
        cout << (i > 0 ? " | " : "") << record.fields[i];
      }
      cout << endl;
    }
  }

  if (!found)
  {
    cout << "No records found for the specified criteria" << endl;
  }
  else
  {
    cout << "Found " << count << " records" << endl;
  }
}

// Parse command line input
vector<string> parseCommand(const string &command)
{
  vector<string> tokens;
  string token;
  bool inQuotes = false;
  char quoteChar = '"';

  for (char c : command)
  {
    if ((c == '"' || c == '\'') && !inQuotes)
    {
      inQuotes = true;
      quoteChar = c;
    }
    else if (c == quoteChar && inQuotes)
    {
      inQuotes = false;
      if (!token.empty())
      {
        tokens.push_back(trim(token));
        token.clear();
      }
    }
    else if (c == ' ' && !inQuotes)
    {
      if (!token.empty())
      {
        tokens.push_back(trim(token));
        token.clear();
      }
    }
    else
    {
      token += c;
    }
  }

  if (!token.empty())
  {
    tokens.push_back(trim(token));
  }

  return tokens;
}

// Display available commands
void showHelp()
{
  cout << "\nAvailable commands:" << endl;
  cout << "  rollup <dimension>      - Count records by dimension" << endl;
  cout << "  slice <dim> <value>     - Filter records by dimension value" << endl;
  cout << "  dice <dim1> <val1> <dim2> <val2> - Filter by multiple criteria" << endl;
  cout << "  help                    - Show this help message" << endl;
  cout << "  exit                    - Exit the program" << endl;
  cout << "\nNote: Use quotes for values with spaces, e.g., slice City \"New York\"" << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Usage: " << argv[0] << " <csv_file>" << endl;
    return 1;
  }

  vector<string> headers;
  vector<CSVRecord> records = loadCSV(argv[1], headers);

  if (records.empty())
  {
    cerr << "Error: No data loaded or file not found." << endl;
    return 1;
  }

  cout << "Loaded " << records.size() << " records with " << headers.size() << " columns." << endl;
  cout << "Available columns: ";
  for (size_t i = 0; i < headers.size(); i++)
  {
    cout << headers[i] << (i < headers.size() - 1 ? ", " : "");
  }
  cout << endl;

  showHelp();

  string command;
  while (true)
  {
    cout << "\nEnter command: ";
    getline(cin, command);

    vector<string> tokens = parseCommand(command);
    if (tokens.empty())
      continue;

    string operation = toLower(tokens[0]);

    if (operation == "exit")
    {
      break;
    }
    else if (operation == "help")
    {
      showHelp();
    }
    else if (operation == "rollup" && tokens.size() >= 2)
    {
      rollup(records, headers, tokens[1]);
    }
    else if (operation == "slice" && tokens.size() >= 3)
    {
      slice(records, headers, tokens[1], tokens[2]);
    }
    else if (operation == "dice" && tokens.size() >= 5)
    {
      dice(records, headers, tokens[1], tokens[2], tokens[3], tokens[4]);
    }
    else
    {
      cerr << "Error: Invalid command or insufficient parameters." << endl;
      showHelp();
    }
  }

  return 0;
}