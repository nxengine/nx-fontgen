#include <stdexcept>
#include <fstream>
#include <iostream>
#include "FontInfo.h"
#include "json.hpp"

std::string FontInfo::getCharSetName(uint8_t charSet)
{
    std::string str;

    switch(charSet)
    {
        case 0: // ANSI_CHARSET
            str = "ANSI";
            break;
        case 1: // DEFAULT_CHARSET
            str = "DEFAULT";
            break;
        case 2: // SYMBOL_CHARSET
            str = "SYMBOL";
            break;
        case 128: // SHIFTJIS_CHARSET
            str = "SHIFTJIS";
            break;
        case 129: // HANGUL_CHARSET
            str = "HANGUL";
            break;
        case 134: // GB2312_CHARSET
            str = "GB2312";
            break;
        case 136: // CHINESEBIG5_CHARSET
            str = "CHINESEBIG5";
            break;
        case 255: // OEM_CHARSET
            str = "OEM";
            break;
        case 130: // JOHAB_CHARSET
            str = "JOHAB";
            break;
        case 177: // HEBREW_CHARSET
            str = "HEBREW";
            break;
        case 178: // ARABIC_CHARSET
            str = "ARABIC";
            break;
        case 161: // GREEK_CHARSET
            str = "GREEK";
            break;
        case 162: // TURKISH_CHARSET
            str = "TURKISH";
            break;
        case 163: // VIETNAMESE_CHARSET
            str = "VIETNAMESE";
            break;
        case 222: // THAI_CHARSET
            str = "THAI";
            break;
        case 238: // EASTEUROPE_CHARSET
            str = "EASTEUROPE";
            break;
        case 204: // RUSSIAN_CHARSET
            str = "RUSSIAN";
            break;
        case 77:  // MAC_CHARSET
            str = "MAC";
            break;
        case 186: // BALTIC_CHARSET
            str = "BALTIC";
            break;

        default:
            str = std::to_string(charSet);
    }

    return str;
}


void FontInfo::writeToJsonFile(const std::string &fileName) const
{
    //TODO: test

    nlohmann::json j;

    nlohmann::json infoNodePadding;
    infoNodePadding["up"] = info.padding.up;
    infoNodePadding["right"] = info.padding.right;
    infoNodePadding["down"] = info.padding.down;
    infoNodePadding["left"] = info.padding.left;

    nlohmann::json infoNodeSpacing;
    infoNodeSpacing["horizontal"] = info.spacing.horizontal;
    infoNodeSpacing["vertical"] = info.spacing.vertical;

    nlohmann::json infoNode;
    infoNode["size"] = info.size;
    infoNode["smooth"] = info.smooth;
    infoNode["unicode"] = info.unicode;
    infoNode["italic"] = info.bold;
    infoNode["bold"] = info.bold;
    infoNode["charset"] = info.charset;
    infoNode["stretchH"] = info.stretchH;
    infoNode["aa"] = info.aa;
    infoNode["padding"] = infoNodePadding;
    infoNode["spacing"] = infoNodeSpacing;
    infoNode["outline"] = info.outline;
    infoNode["face"] = info.face;

    nlohmann::json commonNode;
    commonNode["lineHeight"] = common.lineHeight;
    commonNode["base"] = common.base;
    commonNode["scaleW"] = common.scaleW;
    commonNode["scaleH"] = common.scaleH;
    commonNode["pages"] = common.pages;
    commonNode["packed"] = common.packed;
    commonNode["alphaChnl"] = common.alphaChnl;
    commonNode["redChnl"] = common.redChnl;
    commonNode["greenChnl"] = common.greenChnl;
    commonNode["blueChnl"] = common.blueChnl;

    nlohmann::json charsNode = nlohmann::json::array();
    for(auto c: chars)
    {
        nlohmann::json charNode;
        charNode["id"] = c.id;
        charNode["x"] = c.x;
        charNode["y"] = c.y;
        charNode["width"] = c.width;
        charNode["height"] = c.height;
        charNode["xoffset"] = c.xoffset;
        charNode["yoffset"] = c.yoffset;
        charNode["xadvance"] = c.xadvance;
        charNode["page"] = c.page;
        charNode["chnl"] = c.chnl;
        charsNode.push_back(charNode);
    }

    nlohmann::json kerningsNode = nlohmann::json::array();
    for(auto k: kernings)
    {
        nlohmann::json kerningNode;
        kerningNode["first"] = k.first;
        kerningNode["second"] = k.second;
        kerningNode["amount"] = k.amount;
        kerningsNode.push_back(kerningNode);
    }

    j["info"] = infoNode;
    j["common"] = commonNode;
    j["pages"] = pages;
    j["chars"] = charsNode;
    j["kernings"] = kerningsNode;

    std::ofstream f(fileName);
    f << j.dump(4);
}

void FontInfo::testPages() const
{
    if (!pages.empty())
    {
        size_t l = pages[0].length();
        if (!l)
            throw std::runtime_error("page name is empty");
        for (size_t i = 1; i < pages.size(); ++i)
            if (l != pages[i].length())
                throw std::runtime_error("page names have different length");
    }

    if (common.pages != pages.size())
        throw std::runtime_error("common.pages != pages.size()");
}
