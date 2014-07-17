#include "blacklist.h"

#include "main.h"
#include "string_utils.h"

#include <fstream>
using namespace std;

namespace {
	static string listedName = "Unnamed";
}

const std::string& getListedName() {
	return listedName;
}

bool onList(std::string fn, const std::string& listfn) {
	std::ifstream infile(getConfigFileName(listfn));
	string line;
	boost::algorithm::trim(fn);
	while(std::getline(infile, line)) {
		boost::algorithm::trim(line);
		if(line.size() == 0) continue;
		if(line[0] == '#') continue;
		size_t splitpos = line.find("||");
		string name = "Unnamed";
		if(splitpos != line.npos) {
			name = line.substr(splitpos + 2);
			line = line.substr(0, splitpos);
			boost::algorithm::trim(line);
			boost::algorithm::trim(name);
		}
		if(matchWildcard(fn, line)) {
			listedName = name;
			return true;
		}
	}
	return false;
}

