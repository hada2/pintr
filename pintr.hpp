/*
 * Copyright (c) 2022 Hiroki Hada
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */

#pragma once

using namespace std;

#include "pin.H"

#include <iomanip>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <string>
#include <cstring>

#define PINTR_VERSION (0.01)

#define PADDING(x) (string(x) + string(24 - string(x).length(), ' '))
#define APPEND_DELIM(x, y) ((x).empty() ? string("") : string(y) + string(x))
#define JOIN(x, y, z) ((!(x).empty() && !(y).empty()) ? (x + z + y) : (x + y))
#define DELIM "|"

#define REFRESH_INTERVAL_MS (500)
#define THREAD_MAX (30)
#define PC_HISTORY_SIZE (3)

