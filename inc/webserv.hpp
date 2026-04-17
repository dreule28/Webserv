#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <cstring>
#include <set>
#include <utility>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <map>

#include "../inc/Colors.hpp"
#include "Config/ConfigParser.hpp"
#include "socket/sockets.hpp"
#include "Config/Config.hpp"

// webServ.hpp
// void printConfig(const Config& config);
void webServ(char *filename);
