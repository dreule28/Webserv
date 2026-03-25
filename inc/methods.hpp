#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#define GET		get_method
#define POST	post_method
#define DELETE	delete_method

std::string		get_method(const std::string &file_path);
int				post_method(const std::string &file_path, const std::string &content);
int				delete_method(const std::string &file_path);
