#include "blacklist.h"

#include "main.h"
#include "string_utils.h"

#include <fstream>
using namespace std;


bool onBlacklist(std::string fn) {
	std::ifstream infile(getAssetFileName("blacklist.txt"));
	string line;
	boost::algorithm::trim(fn);
	while(std::getline(infile, line)) {
		boost::algorithm::trim(line);
		if(matchWildcard(fn, line)) return true;
	}
	return false;
}
