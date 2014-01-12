#include "blacklist.h"

#include "main.h"

#include <fstream>
using namespace std;

#include <boost/algorithm/string.hpp>

bool onBlacklist(const std::string& fn) {
	std::ifstream infile(getAssetFileName("blacklist.txt"));
	string line;
	while (std::getline(infile, line)) {
		if(boost::iequals(getExeFileName(), line)) return true;
	}
	return false;
}
