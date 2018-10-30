#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdio>
#include <utility>
#include <functional>
#include <random>
#include <algorithm>
#include <regex>
#include <map>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cfloat>

using point = std::pair<long double, long double>;
using func_ld = std::function<long double(long double)>;

#define PI 3.14159265359

#include "allegro/allegro.h"
#include "mathparser.h"
#include "function.h"
#include "scatterplot.h"

//const double PI = asin(1)*2;