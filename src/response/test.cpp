#include "LocationConfig.hpp"

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
}
