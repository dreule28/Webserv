#include "response.hpp"
#include "LocationConfig.hpp"

std::string buildRealPath(const LocationConfig &loc) {
	std::stringstream ss;

	ss << loc.root << loc.root;

	return ss.str();
}
