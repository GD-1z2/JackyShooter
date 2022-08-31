#pragma once

#include <array>
#include <jacky-common/defs.hpp>
#include <string>

uint loadTexture(const std::string &path, bool linear = true);

uint loadCubeMap(const std::array<std::string, 6> &faces);
