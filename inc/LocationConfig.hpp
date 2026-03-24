#pragma once 

#include "webserv.hpp"

class LocationConfig {
    public:
        std::string path;

        std::string root;
        std::vector<std::string> methods;

        bool        cgiEnabled;
        std::string cgiExtensions;
        std::string cgiPath;

        std::string index;
        bool        auto_index;


        std::string uploadDir;
        LocationConfig() : cgiEnabled(false) {}

        LocationConfig& operator=(const LocationConfig &other) {
            if (this != &other) {
                *this = other;
            }
            return *this;
        }
};
