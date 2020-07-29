#include "search_server.h"
#include <sstream>
#include <iterator>

static void Trim(string_view& str)
{
	str.remove_prefix(min(str.size(), str.find_first_not_of(' ')));
}

static string_view ReadWord(string_view& line)
{
	string_view word;
	Trim(line);
	size_t pos = line.find(' ');
	word = line.substr(0, (pos == string_view::npos) ? line.size() : pos);
	line.remove_prefix((pos == string_view::npos) ? line.size() : pos);
	return word;
}

vector<string_view> SplitIntoWords(string_view line)
{
	vector<string_view> views;
	for (string_view word = ReadWord(line); !word.empty(); word = ReadWord(line))
		views.push_back(word);
	return views;
}
