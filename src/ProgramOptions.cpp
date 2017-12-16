#include "ProgramOptions.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>
#include "args.hpp"
#include "utf8.h"
#include "HelpException.h"

Config helpers::parseCommandLine(int argc, const char* const argv[])
{
    Config config;
    std::string chars;

    args::ArgumentParser parser("NXEngine bitmat font renderer.", "");
    parser.helpParams.proglineShowFlags = true;

    args::HelpFlag arg_help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> arg_font_file(parser, "font-file", "Path to ttf font.", {'F', "font-file"}, args::Options::Required);
    args::ValueFlag<std::string> arg_chars(parser, "chars", "Required characters, for example: 32-64,92,120-126\ndefault value is 32-127", {'C', "chars"});
    args::ValueFlag<std::string> arg_chars_file(parser, "chars-file", "Optional path to UTF-8 text file with required characters (will be combined with chars)", {"chars-file"});
    args::ValueFlag<uint16_t> arg_font_size(parser, "font-size", "Font size, default 32.", {'S', "font-size"});
    args::ValueFlag<uint32_t> arg_texture_width(parser, "texture-width", "Font texture width. Default 256", {'W', "texture-width"});
    args::ValueFlag<uint32_t> arg_texture_height(parser, "texture-height", "Font texture height. Default 256", {'H', "texture-height"});
    args::ValueFlag<std::string> arg_output(parser, "output", "Output file name.", {'O', "output"}, args::Options::Required);
    args::Flag arg_antialias(parser, "antialias", "Render antialiased glyphs.", {'A', "antialias"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << "Usage:\n" << parser;
        throw std::logic_error("aborted");
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "\n\nUsage:\n" << parser;
        throw std::logic_error("aborted");
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "\n\nUsage:\n" << parser;
        throw std::logic_error("aborted");
    }

    if (!arg_chars)
        chars = "32-127";
    else
        chars = args::get(arg_chars);

    config.chars = parseCharsString(chars);
    if (arg_chars_file)
    {
        auto c = getCharsFromFile(args::get(arg_chars_file));
        config.chars.insert(c.begin(), c.end());
    }

    config.spacing.ver = 0;
    config.spacing.hor = 0;

    config.fontFile = args::get(arg_font_file);
    config.output = args::get(arg_output);

    if (arg_font_size)
      config.fontSize = args::get(arg_font_size);
    else
      config.fontSize = 32;

    if (arg_texture_width)
      config.textureSize.w = args::get(arg_texture_width);
    else
      config.textureSize.w = 256;

    if (arg_texture_height)
      config.textureSize.h = args::get(arg_texture_height);
    else
      config.textureSize.h = 256;

    if (arg_antialias)
      config.antialias = true;
    else
      config.antialias = false;

    return config;
}

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::set<uint32_t> helpers::parseCharsString(std::string str)
{
    // remove whitespace characters
    str.erase(std::remove_if(str.begin(), str.end(), std::bind( std::isspace<char>, std::placeholders::_1, std::locale::classic() )), str.end());

    if (str.empty())
        return std::set<uint32_t>();

    const std::regex e("^\\d{1,5}(-\\d{1,5})?(,\\d{1,5}(-\\d{1,5})?)*$");
    if (!std::regex_match(str, e))
        throw std::logic_error("invalid chars value");

    std::vector<std::string> ranges = split(str, ',');

    std::vector<std::pair<uint32_t, uint32_t>> charList;
    for (auto range: ranges)
    {
        std::vector<std::string> minMaxStr = split(range, '-');

        if (minMaxStr.size() == 1)
            minMaxStr.push_back(minMaxStr[0]);

        try
        {
            charList.emplace_back(std::stoi(minMaxStr[0]),
                                std::stoi(minMaxStr[1]));
        }
        catch(std::invalid_argument &)
        {
            throw std::logic_error("incorrect chars value (out of range)");
        }
        catch(std::out_of_range &)
        {
            throw std::logic_error("incorrect chars value (out of range)");
        }
    }

    std::set<uint32_t> result;
    for (auto range: charList)
    {
        //TODO: check too big result
        for (uint32_t v = range.first; v < range.second; ++v)
            result.insert(v);
        result.insert(range.second);
    }

    return result;
}

std::set<uint32_t> helpers::getCharsFromFile(const std::string& f)
{
    std::ifstream fs(f, std::ifstream::binary);
    if (!fs.is_open())
        throw std::runtime_error("chars file not found");

    std::string str((std::istreambuf_iterator<char>(fs)),
                    std::istreambuf_iterator<char>());

    std::set<uint32_t> result;

    std::string::iterator it = str.begin();

    while(it != str.end())
    {
        char32_t ch = utf8::next(it, str.end());
        result.insert(ch);
    }
    return result;
}

