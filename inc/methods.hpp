#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

class HttpRequest;

std::string		get_method(const std::string &file_path, bool is_dir, const HttpRequest &request);
int				post_method(const std::string &file_path, const std::string &content);
int				delete_method(const std::string &file_path);
