#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>
#include <cctype>
using namespace std;

vector<vector<string>> transactions;
vector<string> headers;
double minSupport;
double minConfidence;

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

bool loadCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string cell;
        vector<string> transaction;
        
        while (getline(ss, cell, ',')) {
            string item = trim(cell);
            if (!item.empty()) {
                // Convert to lowercase for consistency
                for (auto &c : item) c = tolower(c);
                transaction.push_back(item);
            }
        }
        
        if (!transaction.empty()) {
            transactions.push_back(transaction);
        }
    }
    
    cout << "Loaded " << transactions.size() << " transactions" << endl;
    return true;
}

void setParameters() {
    cout << "\nParameter Selection" << endl;
    cout << "Enter minimum support (0.0-1.0): ";
    cin >> minSupport;
    cout << "Enter minimum confidence (0.0-1.0): ";
    cin >> minConfidence;
}

void exploreData() {
    cout << "\nData Exploration" << endl;
    
    map<string, int> itemCount;
    for (const auto& transaction : transactions) {
        for (const string& item : transaction) {
            itemCount[item]++;
        }
    }
    
    cout << "Total unique items: " << itemCount.size() << endl;
    cout << "Average transaction size: " << (double)itemCount.size() / transactions.size() << endl;
    
    cout << "Most frequent items:" << endl;
    vector<pair<int, string>> sortedItems;
    for (const auto& pair : itemCount) {
        sortedItems.push_back({pair.second, pair.first});
    }
    sort(sortedItems.rbegin(), sortedItems.rend());
    
    for (int i = 0; i < min(5, (int)sortedItems.size()); i++) {
        double support = (double)sortedItems[i].first / transactions.size();
        cout << sortedItems[i].second << ": " << sortedItems[i].first << " (" << support << ")" << endl;
    }
}

void preprocessData() {
    cout << "\nData Preprocessing" << endl;
    cout << "Removing duplicates within each transaction" << endl;
    
    for (auto &transaction : transactions) {
        set<string> uniqueItems(transaction.begin(), transaction.end());
        transaction.assign(uniqueItems.begin(), uniqueItems.end());
    }
    
    int totalItems = 0;
    for (const auto& transaction : transactions) {
        totalItems += transaction.size();
    }
    
    cout << "Total unique items across all transactions: " << totalItems << endl;
}

map<string, int> getItemSupport() {
    map<string, int> itemSupport;
    
    for (const auto& transaction : transactions) {
        set<string> uniqueItems(transaction.begin(), transaction.end());
        for (const string& item : uniqueItems) {
            itemSupport[item]++;
        }
    }
    
    return itemSupport;
}

map<set<string>, int> getItemsetSupport(const vector<set<string>>& itemsets) {
    map<set<string>, int> support;
    
    for (const auto& itemset : itemsets) {
        for (const auto& transaction : transactions) {
            set<string> transactionSet(transaction.begin(), transaction.end());
            
            bool contains = true;
            for (const string& item : itemset) {
                if (transactionSet.find(item) == transactionSet.end()) {
                    contains = false;
                    break;
                }
            }
            
            if (contains) {
                support[itemset]++;
            }
        }
    }
    
    return support;
}

void generateFrequentItemsets() {
    cout << "\nApplying Apriori Algorithm" << endl;
    
    int minSupportCount = (int)(minSupport * transactions.size());
    cout << "Minimum support count: " << minSupportCount << endl;
    
    // Generate frequent 1-itemsets
    map<string, int> itemSupport = getItemSupport();
    vector<string> frequentItems;
    
    cout << "\nFrequent 1-itemsets:" << endl;
    for (const auto& pair : itemSupport) {
        if (pair.second >= minSupportCount) {
            frequentItems.push_back(pair.first);
            double support = (double)pair.second / transactions.size();
            cout << "{" << pair.first << "}: " << support << endl;
        }
    }
    
    // Generate frequent 2-itemsets
    vector<set<string>> candidate2Itemsets;
    for (int i = 0; i < frequentItems.size(); i++) {
        for (int j = i + 1; j < frequentItems.size(); j++) {
            set<string> itemset = {frequentItems[i], frequentItems[j]};
            candidate2Itemsets.push_back(itemset);
        }
    }
    
    map<set<string>, int> itemset2Support = getItemsetSupport(candidate2Itemsets);
    vector<set<string>> frequent2Itemsets;
    
    cout << "\nFrequent 2-itemsets:" << endl;
    for (const auto& pair : itemset2Support) {
        if (pair.second >= minSupportCount) {
            frequent2Itemsets.push_back(pair.first);
            double support = (double)pair.second / transactions.size();
            cout << "{";
            bool first = true;
            for (const string& item : pair.first) {
                if (!first) cout << ", ";
                cout << item;
                first = false;
            }
            cout << "}: " << support << endl;
        }
    }
}

void generateAssociationRules() {
    cout << "\nGenerating Association Rules:" << endl;
    
    int minSupportCount = (int)(minSupport * transactions.size());
    
    // Get frequent 2-itemsets for rule generation
    vector<set<string>> candidate2Itemsets;
    map<string, int> itemSupport = getItemSupport();
    vector<string> frequentItems;
    
    for (const auto& pair : itemSupport) {
        if (pair.second >= minSupportCount) {
            frequentItems.push_back(pair.first);
        }
    }
    
    for (int i = 0; i < frequentItems.size(); i++) {
        for (int j = i + 1; j < frequentItems.size(); j++) {
            set<string> itemset = {frequentItems[i], frequentItems[j]};
            candidate2Itemsets.push_back(itemset);
        }
    }
    
    map<set<string>, int> itemset2Support = getItemsetSupport(candidate2Itemsets);
    
    cout << "Rules with confidence >= " << minConfidence << ":" << endl;
    
    for (const auto& pair : itemset2Support) {
        if (pair.second >= minSupportCount) {
            const set<string>& itemset = pair.first;
            vector<string> items(itemset.begin(), itemset.end());
            
            // Generate rules A -> B and B -> A
            for (int i = 0; i < items.size(); i++) {
                string antecedent = items[i];
                string consequent = items[1 - i];
                
                int antecedentSupport = itemSupport[antecedent];
                double confidence = (double)pair.second / antecedentSupport;
                
                if (confidence >= minConfidence) {
                    double support = (double)pair.second / transactions.size();
                    cout << antecedent << " -> " << consequent;
                    cout << " (support: " << support << ", confidence: " << confidence << ")" << endl;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <dataset.csv>" << endl;
        return 1;
    }
    
    if (!loadCSV(argv[1])) {
        cout << "Failed to load file" << endl;
        return 1;
    }
    
    setParameters();
    exploreData();
    preprocessData();
    generateFrequentItemsets();
    generateAssociationRules();
    
    return 0;
}