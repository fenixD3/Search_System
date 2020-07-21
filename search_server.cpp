#include "search_server.h"
#include "iterator_range.h"
#include "new_profile.h"
#include "parse.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <numeric>

static void UpdateIndex(istream& document_input, Synchronized<InvertedIndex>& index)
{
	InvertedIndex new_ind;
	for (string current_document; getline(document_input, current_document); )
		new_ind.Add(move(current_document));
	swap(index.GetAccess().ref_to_value, new_ind);
}

void SearchServer::UpdateDocumentBase(istream& document_input)
{
	tasks.push_back(async(UpdateIndex, ref(document_input), ref(index)));
}

SearchServer::SearchServer(istream& document_input)
{
	//UpdateDocumentBase(document_input);
	UpdateIndex(document_input, index);
}

/*static void MakeQueryAnswer(istream& query_input, ostream& search_results_output, Synchronized<InvertedIndex>& index)
{
	vector<pair<size_t, int64_t>> docid_count;
	for (string current_query; getline(query_input, current_query); )
	{
		const auto words = SplitIntoWords(current_query);
		{
			auto access = index.GetAccess();
			docid_count.assign(access.ref_to_value.GetDocsCnt(), {0, 0});
			for (const auto &word : words)
				for (const auto& [docid, raiting] : access.ref_to_value.Lookup(word))
				{
					docid_count[docid].first = docid;
					docid_count[docid].second += raiting;
				}
		}

		partial_sort(docid_count.begin(), docid_count.begin() + min<size_t>(docid_count.size(), 5), docid_count.end(),
			[](pair<size_t, int64_t>& lhs,  pair<size_t, int64_t>& rhs)
			{
				return make_pair<size_t, int64_t>(lhs.second, -lhs.first) >
					make_pair<size_t, int64_t>(rhs.second, -rhs.first);
			});

		search_results_output << current_query << ':';
		for (auto[docid, hitcount] : Head(docid_count, 5))
		{
			if (!hitcount)
				break ;
			search_results_output << " {" << "docid: " << docid << ", "
								  << "hitcount: " << hitcount << '}';
		}
		search_results_output << endl;
	}
}*/

static void MakeQueryAnswer(istream& query_input, ostream& search_results_output, Synchronized<InvertedIndex>& index)
{
	vector<size_t> docid_count;
	vector<int64_t> doc_number;
	for (string current_query; getline(query_input, current_query); )
	{
		const auto words = SplitIntoWords(current_query);
		{
			auto access = index.GetAccess();
			docid_count.assign(access.ref_to_value.GetDocsCnt(), 0);
			doc_number.resize(access.ref_to_value.GetDocsCnt());
			for (const auto &word : words)
				for (const auto& [docid, raiting] : access.ref_to_value.Lookup(word))
					docid_count[docid] += raiting;
		}

		iota(doc_number.begin(), doc_number.end(), 0);
		partial_sort(doc_number.begin(), doc_number.begin() + min<size_t>(doc_number.size(), 5), doc_number.end(),
					 [&docid_count](const int64_t& l_doc, const int64_t& r_doc)
					 {
						 return make_pair(docid_count[l_doc], -l_doc) > make_pair(docid_count[r_doc], -r_doc);
					 });

		search_results_output << current_query << ':';
		for (const auto& docid : Head(doc_number, 5))
		{
			if (!docid_count[docid])
				break ;
			search_results_output << " {" << "docid: " << docid << ", "
								  << "hitcount: " << docid_count[docid] << '}';
		}
		search_results_output << endl;
	}
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output)
{
	tasks.push_back(async(
		MakeQueryAnswer, ref(query_input), ref(search_results_output), ref(index)));
}

void InvertedIndex::Add(string document)
{
	docs.push_back(move(document));

	const size_t docid = docs.size() - 1;
	for (string_view word : SplitIntoWords(docs.back()))
	{
		if (index_store[word].empty() || index_store[word].back().first != docid)
			index_store[word].push_back({docid, 1});
		else
			index_store[word].back().second++;
	}
}

const vector<pair<size_t, size_t>>& InvertedIndex::Lookup(string_view word) const
{
	static vector<pair<size_t, size_t>> result;
	if (auto it = index_store.find(word); it != index_store.end())
		return it->second;
	else
		return result;
}

const string& InvertedIndex::GetDocument(size_t id) const
{
	return docs[id];
}

size_t InvertedIndex::GetDocsCnt() const
{
	return docs.size();
}
