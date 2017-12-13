#include "App.h"
#include "ProgramOptions.h"
#include <SDL.h>
#include <string>
#include <fstream>
#include <array>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include "sdlSavePng/savepng.h"
#include "FontInfo.h"

std::vector<rbp::RectSize> App::getSrcRects(const Glyphs &glyphs, int additionalWidth, int additionalHeight)
{
    std::vector<rbp::RectSize> result;
    for (auto& kv : glyphs)
    {
        const GlyphInfo& glyphInfo = kv.second;
        if (!glyphInfo.isEmpty())
        {
            rbp::RectSize rs;
            rs.width = glyphInfo.getWidth() + additionalHeight;
            rs.height = glyphInfo.getHeight() + additionalWidth;
            rs.tag = kv.first;
            result.push_back(rs);
        }
    }
    return result;
}

App::Glyphs App::collectGlyphInfo(TTF_Font *font,
                                  const std::set<uint32_t> &codes,
                                  uint32_t maxTextureSizeX,
                                  uint32_t maxTextureSizeY)
{
    int fontAscent = TTF_FontAscent(font);

    Glyphs glyphs;

    for (auto& id : codes)
    {
        if ((id > 0xFFFF) || (TTF_GlyphIsProvided(font, static_cast<Uint16>(id))==0))
        {
            std::cout << "warning: glyph " << id << " not found " << std::endl;
            continue;
        }

        GlyphInfo glyphInfo;
        TTF_GlyphMetrics(font, static_cast<Uint16>(id),
                             &glyphInfo.minx,
                             &glyphInfo.maxx,
                             &glyphInfo.miny,
                             &glyphInfo.maxy,
                             &glyphInfo.advance);

        if (fontAscent < glyphInfo.maxy)
            std::cerr << "Warning: invalid glyph (maxy > ascent)" << std::endl;

        if ( (glyphInfo.getWidth() > static_cast<int>(maxTextureSizeX)) || (glyphInfo.getHeight() > static_cast<int>(maxTextureSizeY)))
            throw std::runtime_error("no room for glyph");

        //TODO: add more checks for glyph.

        if (glyphInfo.isInvalid())
            throw std::runtime_error("invalid glyph (zero or negative width or height)");

        //TODO: emplace.
        glyphs[id] = glyphInfo;
    }

    return glyphs;
}

uint16_t App::arrangeGlyphs(Glyphs& glyphs, const Config& config)
{
    std::vector< rbp::RectSize > srcRects = getSrcRects(glyphs, config.spacing.hor,
                                                        config.spacing.ver);

    rbp::MaxRectsBinPack mrbp;
    uint16_t pageCount = 0;
    for (;;)
    {
        //TODO: check negative dimension.
        mrbp.Init(config.textureSize.w - config.spacing.hor, config.textureSize.h - config.spacing.ver);

        std::vector<rbp::Rect> readyRects;
        mrbp.Insert( srcRects, readyRects, rbp::MaxRectsBinPack::RectBestAreaFit );
        if ( readyRects.empty() )
        {
            if ( !srcRects.empty() )
                throw std::runtime_error("can not fit glyphs to texture");
            break;
        }

        for ( auto r: readyRects )
        {
            glyphs[r.tag].x = r.x + config.spacing.hor;
            glyphs[r.tag].y = r.y + config.spacing.ver;
            glyphs[r.tag].page = pageCount;
        }

        pageCount++;
    }

    return pageCount;
}

int App::getDigitCount(uint16_t x)
{
    return (x < 10 ? 1 :
            (x < 100 ? 2 :
             (x < 1000 ? 3 :
              (x < 10000 ? 4 :
               5))));
}

void App::execute(int argc, char* argv[])
{
    const Config config = helpers::parseCommandLine(argc, const_cast<const char**>(argv));

    const std::string dataFilePath = config.output + ".fnt";
    const std::string outputName = config.output;


//    boost::filesystem::create_directory(outputDirPath);


//    SDL_Init(0);
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(config.fontFile.c_str(), config.fontSize);
    if (!font)
        throw std::runtime_error("font file not found");

    Glyphs glyphs = collectGlyphInfo(font, config.chars, config.textureSize.w, config.textureSize.h);

    const uint16_t pageCount = arrangeGlyphs(glyphs, config);

    const int fontAscent = TTF_FontAscent(font);

    /////////////////////////////////////////////////////////////

    std::vector<std::string> pageNames;

    //TODO: should we decrement pageCount before calcualte?
    int pageNameDigits = getDigitCount(pageCount);

    for (size_t page = 0; page < pageCount; ++page)
    {
        SDL_Surface* outputSurface = SDL_CreateRGBSurface(0, config.textureSize.w, config.textureSize.h, 32,
                                      0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

        // If the color value contains an alpha component then the destination is simply
        // filled with that alpha information, no blending takes place.
//        SDL_FillRect(outputSurface, NULL, 0);

        for (auto kv: glyphs)
        {
            const GlyphInfo& glyph = kv.second;
            if (glyph.page != static_cast<int>(page))
                continue;
            SDL_Surface *glyphSurface;
            if (config.antialias)
                glyphSurface = TTF_RenderGlyph_Blended(font, kv.first, SDL_Color{255,255,255,255});
            else
                glyphSurface = TTF_RenderGlyph_Solid(font, kv.first, SDL_Color{255,255,255,255});

            int x = glyph.x - glyph.minx;
            int y = glyph.y - (fontAscent - glyph.maxy);
            if (!glyph.isEmpty())
            {
                SDL_Rect dstRect{x, y, glyph.getWidth(), glyph.getHeight()};
                SDL_BlitSurface(glyphSurface, NULL, outputSurface, &dstRect);
            }
            SDL_FreeSurface(glyphSurface);
        }

        std::stringstream ss;
        ss << outputName << "_" << std::setfill ('0') << std::setw(pageNameDigits) << page << ".png";
        std::string pageName = ss.str();
        pageNames.push_back(pageName);

        SDL_SavePNG(outputSurface, pageName.c_str());
    }

    /////////////////////////////////////////////////////////////

    FontInfo f;

    f.info.face = TTF_FontFaceFamilyName(font);
    f.info.size = config.fontSize;
    f.info.unicode = true;
    f.info.aa = static_cast<uint8_t>(config.antialias);
    f.info.padding.up = 0;
    f.info.padding.right = 0;
    f.info.padding.down = 0;
    f.info.padding.left = 0;
    f.info.spacing.horizontal = static_cast<uint8_t>(config.spacing.hor);
    f.info.spacing.vertical = static_cast<uint8_t>(config.spacing.ver);

    f.common.lineHeight = static_cast<uint16_t>(TTF_FontLineSkip(font));
    f.common.base = static_cast<uint16_t>(fontAscent);
    f.common.scaleW = static_cast<uint16_t>(config.textureSize.w);
    f.common.scaleH = static_cast<uint16_t>(config.textureSize.h);
    f.common.pages = pageCount;

    for (size_t i = 0; i < pageCount; ++i )
        f.pages.push_back(pageNames.at(i));

    for (auto kv: glyphs)
    {
        //TODO: page = 0 for empty flyphs.
        const GlyphInfo &glyph = kv.second;
        FontInfo::Char c;
        c.id = kv.first;
        if (!glyph.isEmpty())
        {
            c.x = static_cast<uint16_t>(glyph.x);
            c.y = static_cast<uint16_t>(glyph.y);
            c.width = static_cast<uint16_t>(glyph.getWidth());
            c.height = static_cast<uint16_t>(glyph.getHeight());
            c.page = static_cast<uint8_t>(glyph.page);
            c.xoffset = static_cast<int16_t>(glyph.minx);
            c.yoffset = static_cast<int16_t>(fontAscent - glyph.maxy);
        }
        c.xadvance = static_cast<int16_t>(glyph.advance);
        c.chnl = 15;

        f.chars.push_back(c);
    }

    f.writeToJsonFile(dataFilePath);
}
