#include "response.hpp"
#include "LocationConfig.hpp"

bool	checkMethod(const std::string &method, const LocationConfig *location) {
	for (const auto& s: location->methods) {
		if (s == method)
			return true;
	}

	return false;
}
