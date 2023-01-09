#ifndef DEFS_H__
#define DEFS_H__

#include <cstdint>
#include <string>

#define VERSION(x, y, z) ((x << 16) + ((uint8_t)y << 8) + z)
#define BUILD_DATE std::string(__DATE__)

#define FIRM_VER VERSION(1, 0, 0)

#endif // DEFS_H__