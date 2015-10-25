#include "blacklist.h"

#include "main.h"
#include "utils/string_utils.h"

#include <fstream>
using namespace std;

#include <boost/filesystem.hpp>

namespace {
	static string listedName = "Unnamed";

	bool onListInternal(std::string fn, const std::string& listfn) {
		auto path = getConfigFileName(listfn);
		if(!boost::filesystem::exists(path)) return false;
		std::ifstream infile(path);
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

}

const std::string& getListedName() {
	// initialize name from whitelist on first request
	static bool inited = false;
	if(!inited) {
		onList(getExeFileName(), "whitelist");
	}
	return listedName;
}

bool onList(std::string fn, const std::string& listfn) {
	return onListInternal(fn, listfn + ".txt") || onListInternal(fn, listfn + "_user.txt");
}

