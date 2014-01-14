#include "blacklist.h"

#include "main.h"

#include <fstream>
using namespace std;

#include <boost/algorithm/string.hpp>

bool onBlacklist(std::string fn) {
	std::ifstream infile(getAssetFileName("blacklist.txt"));
	string line;
	boost::algorithm::trim(fn);
	while(std::getline(infile, line)) {
		boost::algorithm::trim(line);
		if(boost::iequals(fn, line)) return true;
	}
	return false;
}
