#include "HttpResponse.hpp"
#include "Config/HttpRequest.hpp"
#include "Config/LocationConfig.hpp"

bool	checkMethod(const Methods method, const LocationConfig *location) {
	if (location == nullptr)
		return false;

	for (const auto& s: location->methods) {
		if (s == method)
			return true;
	}

	return false;
}
