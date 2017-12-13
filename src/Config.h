#pragma once

#include <set>
#include <string>
#include <cstdint>

struct Config
{
    struct Size
    {
        uint32_t w;
        uint32_t h;
    };

    struct Spacing
    {
        int ver;
        int hor;
    };

    std::string fontFile;
    std::set<uint32_t> chars;
    uint16_t fontSize;
    Spacing spacing;
    Size textureSize;
    std::string output;
    bool antialias;
};
