#pragma once
#include <stdint.h>
#include <map>
#include <SDL_ttf.h>
#include "maxRectsBinPack/MaxRectsBinPack.h"
#include "Config.h"
#include "GlyphInfo.h"

class App
{
public:
    void execute(int argc, char* argv[]);

private:
    typedef std::map<uint32_t, GlyphInfo> Glyphs;
    std::vector<rbp::RectSize> getSrcRects(const Glyphs &glyphs, int additionalWidth, int additionalHeight);
    Glyphs collectGlyphInfo(TTF_Font *font,
                            const std::set<uint32_t> &codes,
                            uint32_t maxTextureSizeX,
                            uint32_t maxTextureSizeY);
    uint16_t arrangeGlyphs(Glyphs& glyphs, const Config& config);
    int getDigitCount(uint16_t x);
};
