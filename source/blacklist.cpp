#include "blacklist.h"

#include "main.h"
#include "string_utils.h"

#include <fstream>
using namespace std;


bool onList(std::string fn, const std::string& listfn) {
	std::ifstream infile(getConfigFileName(listfn));
	string line;
	boost::algorithm::trim(fn);
	while(std::getline(infile, line)) {
		boost::algorithm::trim(line);
		if(line.size() == 0) continue;
		if(line[0] == '#') continue;
		if(matchWildcard(fn, line)) return true;
	}
	return false;
}

