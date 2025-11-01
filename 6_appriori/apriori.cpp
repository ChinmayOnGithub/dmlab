#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <cctype>

using namespace std;

string trim(const string &s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos)
        return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

vector<vector<string>> read_transactions(const string &filename)
{
    ifstream file(filename);
    vector<vector<string>> transactions;
    string line;

    if (!file.is_open())
    {
        cerr << "Error: Cannot open file " << filename << endl;
        return transactions;
    }

    vector<string> item_names;
    if (getline(file, line))
    {
        stringstream header_ss(line);
        string header_item;

        while (getline(header_ss, header_item, ','))
        {
            item_names.push_back(trim(header_item));
        }
    }

    int transaction_count = 0;
    while (getline(file, line))
    {
        if (line.empty())
            continue;

        vector<string> transaction_items;
        stringstream ss(line);
        string cell;

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
            transaction_count++;
        }
    }

    file.close();
    cout << "Processed " << transaction_count << " transactions with " << item_names.size() << " items" << endl;
    return transactions;
}

string create_itemset_key(const vector<string> &itemset)
{
    string key;
    for (size_t i = 0; i < itemset.size(); ++i)
    {
        if (i > 0)
            key += "|";
        key += itemset[i];
    }
    return key;
}

vector<vector<string>> generate_candidates(const vector<vector<string>> &frequent_sets)
{
    vector<vector<string>> candidates;
    int n = frequent_sets.size();
    if (n == 0)
        return candidates;

    int k = frequent_sets[0].size() + 1;
    unordered_set<string> frequent_set_keys;

    for (const auto &set : frequent_sets)
    {
        frequent_set_keys.insert(create_itemset_key(set));
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            bool can_join = true;
            for (int p = 0; p < k - 2; ++p)
            {
                if (frequent_sets[i][p] != frequent_sets[j][p])
                {
                    can_join = false;
                    break;
                }
            }
            if (!can_join)
                continue;

            vector<string> candidate = frequent_sets[i];
            candidate.push_back(frequent_sets[j].back());

            bool is_valid = true;
            for (int idx = 0; idx < k; ++idx)
            {
                vector<string> subset;
                for (int t = 0; t < k; ++t)
                {
                    if (t != idx)
                        subset.push_back(candidate[t]);
                }
                if (frequent_set_keys.find(create_itemset_key(subset)) == frequent_set_keys.end())
                {
                    is_valid = false;
                    break;
                }
            }

            if (is_valid)
            {
                candidates.push_back(candidate);
            }
        }
    }
    return candidates;
}

bool contains_itemset(const vector<string> &transaction, const vector<string> &itemset)
{
    size_t i = 0, j = 0;
    while (i < transaction.size() && j < itemset.size())
    {
        if (transaction[i] < itemset[j])
        {
            ++i;
        }
        else if (transaction[i] == itemset[j])
        {
            ++i;
            ++j;
        }
        else
        {
            return false;
        }
    }
    return j == itemset.size();
}

void write_results(const vector<pair<vector<string>, int>> &all_frequent,
                   int num_transactions, const string &input_file)
{
    string output_file = input_file;
    size_t last_slash = output_file.find_last_of("/\\");
    size_t last_dot = output_file.find_last_of(".");
    
    string base_name;
    if (last_slash != string::npos)
    {
        base_name = output_file.substr(last_slash + 1);
    }
    else
    {
        base_name = output_file;
    }
    
    if (last_dot != string::npos && last_dot > last_slash)
    {
        base_name = base_name.substr(0, last_dot - (last_slash != string::npos ? last_slash + 1 : 0));
    }
    
    output_file = base_name + "_frequent_itemsets.csv";

    ofstream output(output_file);
    if (!output.is_open())
    {
        cerr << "Error: Cannot create output file " << output_file << endl;
        return;
    }

    output << "itemset,count,support_percent\n";
    for (const auto &pair : all_frequent)
    {
        const vector<string> &itemset = pair.first;
        int count = pair.second;
        output << "\"";
        for (size_t j = 0; j < itemset.size(); ++j)
        {
            if (j > 0)
                output << ",";
            output << itemset[j];
        }
        output << "\"," << count << "," << (100.0 * count / num_transactions) << "\n";
    }
    output.close();
    cout << "Results written to " << output_file << endl;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout << "Usage: " << argv[0] << " <input_transactions.csv> <min_support_percent>" << endl;
        cout << "Example: " << argv[0] << " transactions.csv 25" << endl;
        return 1;
    }

    string input_file = argv[1];
    double min_support_percent = stod(argv[2]);

    if (min_support_percent <= 0 || min_support_percent > 100)
    {
        cerr << "Error: Minimum support must be between 0 and 100" << endl;
        return 1;
    }

    cout << "Reading transactions from: " << input_file << endl;
    vector<vector<string>> transactions = read_transactions(input_file);
    if (transactions.empty())
    {
        cerr << "Error: No transactions found or error reading file" << endl;
        return 1;
    }

    int num_transactions = transactions.size();
    int min_support_count = ceil((min_support_percent / 100.0) * num_transactions);

    cout << "Minimum support: " << min_support_percent << "% (" << min_support_count << " transactions)" << endl;

    unordered_map<string, int> item_counts;
    for (const auto &transaction : transactions)
    {
        for (const auto &item : transaction)
        {
            item_counts[item]++;
        }
    }

    vector<vector<string>> frequent_itemsets;
    vector<pair<vector<string>, int>> all_frequent;

    // frequent 1-itemsets
    for (const auto &pair : item_counts)
    {
        if (pair.second >= min_support_count)
        {
            vector<string> single_item = {pair.first};
            frequent_itemsets.push_back(single_item);
            all_frequent.push_back({single_item, pair.second});
        }
    }
    sort(frequent_itemsets.begin(), frequent_itemsets.end());

    cout << "Frequent 1-itemsets: " << frequent_itemsets.size() << endl;

    // Find larger itemsets
    for (int k = 2; !frequent_itemsets.empty(); ++k)
    {
        vector<vector<string>> candidates = generate_candidates(frequent_itemsets);
        if (candidates.empty())
            break;

        unordered_map<string, int> candidate_counts;
        for (const auto &candidate : candidates)
        {
            candidate_counts[create_itemset_key(candidate)] = 0;
        }

        for (const auto &transaction : transactions)
        {
            for (const auto &candidate : candidates)
            {
                if (contains_itemset(transaction, candidate))
                {
                    candidate_counts[create_itemset_key(candidate)]++;
                }
            }
        }

        vector<vector<string>> new_frequent;
        for (const auto &candidate : candidates)
        {
            string key = create_itemset_key(candidate);
            if (candidate_counts[key] >= min_support_count)
            {
                new_frequent.push_back(candidate);
                all_frequent.push_back({candidate, candidate_counts[key]});
            }
        }

        if (new_frequent.empty())
            break;
        cout << "Frequent " << k << "-itemsets: " << new_frequent.size() << endl;
        frequent_itemsets = new_frequent;
    }

    write_results(all_frequent, num_transactions, input_file);

    return 0;
}