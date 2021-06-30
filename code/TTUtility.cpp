#include "TTUtility.hpp"

#include <algorithm>
#include <string>

int atoi(const char* begin, const char* end) {
	return std::stoi(std::string(begin, end-begin));
}

bool isNumber(const std::string &s) {
	return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool isPositiveInteger(const std::string &s) {
	return !s.empty() && (std::count_if(s.begin(), s.end(), ::isdigit) == s.size());
}

std::size_t index(const std::thread::id id) {
	static std::size_t nextindex = 0;
	static std::mutex my_mutex;
	static std::map<std::thread::id, std::size_t> ids;
	std::lock_guard<std::mutex> lock(my_mutex);
	if(ids.find(id) == ids.end()) ids[id] = nextindex++;
	return ids[id];
}
