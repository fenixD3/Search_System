#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <string_view>
#include <future>
#include "synhronizes.h"
using namespace std;

class InvertedIndex
{
public:
	void Add(string document);
	const vector<pair<size_t, size_t>>& Lookup(string_view word) const;

	const string& GetDocument(size_t id) const;

	size_t GetDocsCnt() const;
private:
	map<string_view, vector<pair<size_t, size_t>>> index_store;
	deque<string> docs;
};

class SearchServer
{
public:
	SearchServer() = default;
	explicit SearchServer(istream& document_input);
	void UpdateDocumentBase(istream& document_input);
	void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
	Synchronized<InvertedIndex> index;
	vector<future<void>> tasks;
};

vector<string_view> SplitIntoWords(string_view line);
//vector<string> SplitIntoWords(const string& line);
