#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
using namespace std;

using Transaction = vector<string>;
using Itemset = set<string>;

string trim(const string &s)
{
  size_t start = s.find_first_not_of(" \t\r\n");
  if (start == string::npos)
    return "";
  size_t end = s.find_last_not_of(" \t\r\n");
  return s.substr(start, end - start + 1);
}

vector<Transaction> read_transactions(const string &filename)
{
  ifstream fin(filename);
  vector<Transaction> transactions;
  string line;

  if (getline(fin, line))
  {
  }

  while (getline(fin, line))
  {
    if (line.empty())
      continue;
    Transaction t;
    stringstream ss(line);
    string cell;

    vector<string> item_names;
    int col_index = 0;
    while (getline(ss, cell, ','))
    {
      if (col_index == 0)
      {
        col_index++;
        continue;
      }

      string trimmed = trim(cell);
      if (trimmed == "1")
      {
        item_names.push_back("Item" + to_string(col_index));
      }
      col_index++;
    }

    if (!item_names.empty())
    {
      sort(item_names.begin(), item_names.end());
      transactions.push_back(item_names);
    }
  }
  fin.close();
  return transactions;
}

vector<Transaction> read_binary_transactions(const string &filename)
{
  ifstream fin(filename);
  vector<Transaction> transactions;
  string line;

  if (!fin.is_open())
  {
    cerr << "Error: Cannot open file " << filename << endl;
    return transactions;
  }

  vector<string> item_names;
  if (getline(fin, line))
  {
    stringstream header_ss(line);
    string header_item;

    getline(header_ss, header_item, ',');

    while (getline(header_ss, header_item, ','))
    {
      item_names.push_back(trim(header_item));
    }
  }

  while (getline(fin, line))
  {
    if (line.empty())
      continue;

    Transaction transaction_items;
    stringstream ss(line);
    string cell;

    getline(ss, cell, ',');

    int item_index = 0;
    while (getline(ss, cell, ','))
    {
      string trimmed_cell = trim(cell);
      if (trimmed_cell == "1" && item_index < item_names.size())
      {
        transaction_items.push_back(item_names[item_index]);
      }
      item_index++;
    }

    if (!transaction_items.empty())
    {
      sort(transaction_items.begin(), transaction_items.end());
      transactions.push_back(transaction_items);
    }
  }

  fin.close();
  cout << "Read " << transactions.size() << " transactions with " << item_names.size() << " items" << endl;
  return transactions;
}

int count_support(const vector<Transaction> &transactions, const Itemset &itemset)
{
  int count = 0;
  for (const auto &t : transactions)
  {
    bool present = true;
    for (const auto &item : itemset)
    {
      if (find(t.begin(), t.end(), item) == t.end())
      {
        present = false;
        break;
      }
    }
    if (present)
      count++;
  }
  return count;
}

vector<Itemset> generate_subsets(const Itemset &items)
{
  vector<string> elems(items.begin(), items.end());
  vector<Itemset> subsets;
  int n = elems.size();
  for (int mask = 1; mask < (1 << n) - 1; ++mask)
  {
    Itemset s;
    for (int i = 0; i < n; ++i)
    {
      if (mask & (1 << i))
        s.insert(elems[i]);
    }
    if (!s.empty() && s.size() < items.size())
    {
      subsets.push_back(s);
    }
  }
  return subsets;
}

Itemset parse_itemset(const string &itemset_str)
{
  Itemset result;
  string clean_str = itemset_str;

  if (clean_str.size() >= 2 && clean_str.front() == '"' && clean_str.back() == '"')
  {
    clean_str = clean_str.substr(1, clean_str.size() - 2);
  }

  stringstream ss(clean_str);
  string item;
  while (getline(ss, item, ','))
  {
    string trimmed = trim(item);
    if (!trimmed.empty())
      result.insert(trimmed);
  }
  return result;
}

void generate_association_rules(const vector<Transaction> &transactions,
                                const vector<pair<Itemset, int>> &frequent_itemsets,
                                double min_confidence,
                                const string &output_file)
{
  ofstream fout(output_file);
  fout << "Antecedent,Consequent,Support,Confidence\n";
  int total_tx = transactions.size();
  int rules_count = 0;

  map<Itemset, int> support_map;
  for (const auto &p : frequent_itemsets)
  {
    support_map[p.first] = p.second;
  }

  for (const auto &p : frequent_itemsets)
  {
    const Itemset &itemset = p.first;
    int itemset_count = p.second;
    if (itemset.size() < 2)
      continue;

    vector<Itemset> subsets = generate_subsets(itemset);
    for (const auto &subset : subsets)
    {
      Itemset remaining;
      set_difference(itemset.begin(), itemset.end(),
                     subset.begin(), subset.end(),
                     inserter(remaining, remaining.begin()));

      auto subset_it = support_map.find(subset);
      if (subset_it == support_map.end())
      {
        continue; 
      }
      int subset_count = subset_it->second;

      if (subset_count == 0)
        continue;

      double confidence = (double)itemset_count / subset_count * 100.0;
      double support = (double)itemset_count / total_tx * 100.0;

      if (confidence >= min_confidence && confidence <= 100.0)
      {
        fout << "\"";
        auto it = subset.begin();
        fout << *it;
        for (++it; it != subset.end(); ++it)
          fout << "," << *it;
        fout << "\",";

        fout << "\"";
        it = remaining.begin();
        fout << *it;
        for (++it; it != remaining.end(); ++it)
          fout << "," << *it;
        fout << "\",";

        fout << support << "," << confidence << "\n";
        rules_count++;
      }
    }
  }
  fout.close();
  cout << "Generated " << rules_count << " association rules -> " << output_file << endl;
}

int main(int argc, char **argv)
{
  if (argc < 4)
  {
    cout << "Usage: " << argv[0] << " <transactions.csv> <frequent_itemsets.csv> <min_confidence%>" << endl;
    cout << "Example: " << argv[0] << " transactions.csv frequent_itemsets.csv 60" << endl;
    return 1;
  }

  string tx_file = argv[1];
  string freq_file = argv[2];
  double min_confidence = stod(argv[3]);

  cout << "Reading transactions..." << endl;
  vector<Transaction> transactions = read_binary_transactions(tx_file);
  cout << "Read " << transactions.size() << " transactions" << endl;

  vector<pair<Itemset, int>> frequent_itemsets;
  ifstream fin(freq_file);
  if (!fin.is_open())
  {
    cerr << "Error: Cannot open frequent itemsets file " << freq_file << endl;
    return 1;
  }

  string line;
  getline(fin, line);

  while (getline(fin, line))
  {
    if (line.empty())
      continue;

    string itemset_str, count_str, support_str;
    stringstream ss(line);

    if (line[0] == '"')
    {
      size_t end_quote = line.find('"', 1);
      if (end_quote == string::npos)
      {
        cerr << "Warning: Malformed line (missing closing quote): " << line << endl;
        continue;
      }
      itemset_str = line.substr(0, end_quote + 1);
      string rest = line.substr(end_quote + 2);
      stringstream rest_ss(rest);
      getline(rest_ss, count_str, ',');
      getline(rest_ss, support_str, ',');
    }
    else
    {
      getline(ss, itemset_str, ',');
      getline(ss, count_str, ',');
      getline(ss, support_str, ',');
    }

    count_str = trim(count_str);

    try
    {
      Itemset itemset = parse_itemset(itemset_str);
      int count = stoi(count_str);
      frequent_itemsets.push_back({itemset, count});
    }
    catch (const std::invalid_argument &e)
    {
      cerr << "Error: Cannot convert count '" << count_str << "' to integer in line: " << line << endl;
      continue;
    }
  }
  fin.close();

  cout << "Successfully read " << frequent_itemsets.size() << " frequent itemsets" << endl;

  if (frequent_itemsets.empty())
  {
    cerr << "Error: No frequent itemsets were successfully parsed!" << endl;
    return 1;
  }

  generate_association_rules(transactions, frequent_itemsets, min_confidence, "association_rules.csv");

  return 0;
}