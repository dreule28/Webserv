#include "LocationConfig.hpp"
#include "response.hpp"
#include "color.hpp"
#include <iostream>
#include <fstream>

// Helper function to create a test file
void createTestFile(const std::string &path, const std::string &content) {
	std::ofstream file(path);
	if (file.is_open()) {
		file << content;
		file.close();
	}
}

// Helper function to run a test
void runTest(const std::string &testName, bool passed) {
	if (passed) {
		std::cout << GREEN << "✓ " << testName << RESET << std::endl;
	} else {
		std::cout << RED << "✗ " << testName << RESET << std::endl;
	}
}

// Test the route matching function
void testRouteMatching(const std::vector<LocationConfig> &locations) {
	std::cout << CYAN << "\n=== Route Matching Tests ===" << RESET << std::endl;

	const LocationConfig *result = routeMatching("/test", locations);
	runTest("Route matching for /test", result != NULL && result->path == "/test");

	result = routeMatching("/api", locations);
	runTest("Route matching for /api", result != NULL && result->path == "/api");

	result = routeMatching("/api/v2/users", locations);
	runTest("Route matching for /api/v2/users (longest match)",
		result != NULL && result->path == "/api/v2");

	result = routeMatching("/nonexistent", locations);
	runTest("Route matching for nonexistent path", result != NULL && result->path == "/");

	result = routeMatching("/downloads/file.txt", locations);
	runTest("Route matching for /downloads/file.txt",
		result != NULL && result->path == "/downloads");
}

// Test the method checking function
void testMethodCheck(const std::vector<LocationConfig> &locations) {
	std::cout << CYAN << "\n=== Method Check Tests ===" << RESET << std::endl;

	const LocationConfig *loc = routeMatching("/test", locations);
	runTest("GET method allowed on /test", checkMethod("GET", loc) == true);
	runTest("POST method allowed on /test", checkMethod("POST", loc) == true);
	runTest("DELETE method not allowed on /test", checkMethod("DELETE", loc) == false);

	loc = routeMatching("/", locations);
	runTest("GET method allowed on /", checkMethod("GET", loc) == true);
	runTest("POST method not allowed on /", checkMethod("POST", loc) == false);

	loc = routeMatching("/api", locations);
	runTest("DELETE method allowed on /api", checkMethod("DELETE", loc) == true);
}

// Test the build path function
void testBuildPath(const std::vector<LocationConfig> &locations) {
	std::cout << CYAN << "\n=== Build Path Tests ===" << RESET << std::endl;

	const LocationConfig *loc = routeMatching("/test", locations);
	std::string path = buildRealPath(loc);
	runTest("Build path for /test", path == "./test.cpp");

	loc = routeMatching("/", locations);
	path = buildRealPath(loc);
	runTest("Build path for /", path == "/var/www/htmlindex.html");

	loc = routeMatching("/api", locations);
	path = buildRealPath(loc);
	runTest("Build path for /api", path == "/var/www/apiindex.html");
}

// Test file checking function
void testFileCheck() {
	std::cout << CYAN << "\n=== File Check Tests ===" << RESET << std::endl;

	// Create a test file
	createTestFile("test_readable.txt", "Test content");

	int result = checkFile("test_readable.txt", "GET");
	runTest("Check existing readable file", result != -1);

	result = checkFile("nonexistent_file.txt", "GET");
	runTest("Check nonexistent file", result == -1);

	// Clean up
	std::remove("test_readable.txt");
}

int main(void) {
	std::vector<LocationConfig> locations;

	LocationConfig loc1;
	loc1.path = "/";
	loc1.root = "/var/www/html";
	loc1.index = "index.html";
	loc1.auto_index = false;
	loc1.methods = {"GET"};
	locations.push_back(loc1);

	LocationConfig loc2;
	loc2.path = "/api";
	loc2.root = "/var/www/api";
	loc2.index = "index.html";
	loc2.auto_index = false;
	loc2.methods = {"GET", "POST", "DELETE"};
	locations.push_back(loc2);

	LocationConfig loc3;
	loc3.path = "/uploads";
	loc3.root = "/var/www/uploads";
	loc3.index = "index.html";
	loc3.auto_index = true;
	loc3.methods = {"GET", "POST"};
	locations.push_back(loc3);

	LocationConfig loc4;
	loc4.path = "/images";
	loc4.root = "/var/www/static/images";
	loc4.index = "index.html";
	loc4.auto_index = true;
	loc4.methods = {"GET"};
	locations.push_back(loc4);

	LocationConfig loc5;
	loc5.path = "/admin";
	loc5.root = "/var/www/admin";
	loc5.index = "dashboard.html";
	loc5.auto_index = false;
	loc5.methods = {"GET", "POST", "PUT", "DELETE"};
	locations.push_back(loc5);

	LocationConfig loc6;
	loc6.path = "/blog";
	loc6.root = "/var/www/blog";
	loc6.index = "index.html";
	loc6.auto_index = true;
	loc6.methods = {"GET"};
	locations.push_back(loc6);

	LocationConfig loc7;
	loc7.path = "/docs";
	loc7.root = "/var/www/documentation";
	loc7.index = "readme.html";
	loc7.auto_index = true;
	loc7.methods = {"GET"};
	locations.push_back(loc7);

	LocationConfig loc8;
	loc8.path = "/assets/css";
	loc8.root = "/var/www/static/css";
	loc8.index = "style.css";
	loc8.auto_index = false;
	loc8.methods = {"GET"};
	locations.push_back(loc8);

	LocationConfig loc9;
	loc9.path = "/assets/js";
	loc9.root = "/var/www/static/js";
	loc9.index = "app.js";
	loc9.auto_index = false;
	loc9.methods = {"GET"};
	locations.push_back(loc9);

	LocationConfig loc10;
	loc10.path = "/downloads";
	loc10.root = "/var/www/files";
	loc10.index = "index.html";
	loc10.auto_index = true;
	loc10.methods = {"GET", "POST"};
	locations.push_back(loc10);

	LocationConfig loc11;
	loc11.path = "/cgi-bin";
	loc11.root = "/usr/lib/cgi-bin";
	loc11.index = "index.cgi";
	loc11.auto_index = false;
	loc11.cgiEnabled = true;
	loc11.methods = {"GET", "POST"};
	locations.push_back(loc11);

	LocationConfig loc12;
	loc12.path = "/redirect";
	loc12.root = "/var/www/html";
	loc12.index = "index.html";
	loc12.auto_index = false;
	loc12.methods = {"GET"};
	locations.push_back(loc12);

	LocationConfig loc13;
	loc13.path = "/videos";
	loc13.root = "/var/www/media/videos";
	loc13.index = "index.html";
	loc13.auto_index = true;
	loc13.methods = {"GET"};
	locations.push_back(loc13);

	LocationConfig loc14;
	loc14.path = "/forum";
	loc14.root = "/var/www/forum";
	loc14.index = "index.php";
	loc14.auto_index = false;
	loc14.methods = {"GET", "POST", "PUT"};
	locations.push_back(loc14);

	LocationConfig loc15;
	loc15.path = "/api/v2";
	loc15.root = "/var/www/api/v2";
	loc15.index = "index.html";
	loc15.auto_index = false;
	loc15.methods = {"GET", "POST", "PUT", "DELETE"};
	locations.push_back(loc15);

	// Add a test location for the response function
	LocationConfig test_loc;
	test_loc.path = "/test";
	test_loc.root = "./";
	test_loc.index = "test.cpp";
	test_loc.auto_index = false;
	test_loc.methods = {"GET", "POST"};
	locations.push_back(test_loc);

	// Print header
	std::cout << BOLD << CYAN << "\n╔════════════════════════════════════════╗" << RESET << std::endl;
	std::cout << BOLD << CYAN << "║   Response Function Unit Test Suite   ║" << RESET << std::endl;
	std::cout << BOLD << CYAN << "╚════════════════════════════════════════╝" << RESET << std::endl;

	// Run unit tests
	testRouteMatching(locations);
	testMethodCheck(locations);
	testBuildPath(locations);
	testFileCheck();

	// Test the full response function with a real file
	std::cout << CYAN << "\n=== Integration Test: Response Function ===" << RESET << std::endl;

	// Create a test file that the response function can find
	createTestFile("test.cpp", "// This is a test file\n");

	std::cout << YELLOW << "Testing response function with URI: /test, Method: GET" << RESET << std::endl;
	std::string result = response(locations);

	if (result.empty()) {
		std::cout << YELLOW << "⚠ Response returned empty (expected - method execution not implemented)" << RESET << std::endl;
	} else {
		std::cout << GREEN << "✓ Response function succeeded!" << RESET << std::endl;
		std::cout << MAGENTA << "Result: " << result << RESET << std::endl;
	}

	// Clean up
	std::remove("test.cpp");

	// Print summary
	std::cout << BOLD << CYAN << "\n════════════════════════════════════════" << RESET << std::endl;
	std::cout << BOLD << GREEN << "Test suite completed!" << RESET << std::endl;
	std::cout << BOLD << CYAN << "════════════════════════════════════════\n" << RESET << std::endl;

	return 0;
}
