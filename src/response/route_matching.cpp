#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include "response.hpp"
#include "LocationConfig.hpp"

const LocationConfig	*routeMatching(
	const std::string &uri, const std::vector<LocationConfig> &locations) {
	const LocationConfig	*best = NULL;
	size_t		best_len = 0;

	for (size_t i = 0; i < locations.size(); i++) {
		const std::string &prefix = locations[i].path;

		if (uri.compare(0, prefix.size(), prefix) == 0 && prefix.size() > best_len) {
				best_len = prefix.size();
				best = &locations[i];
		}
	}

	return best;
}
