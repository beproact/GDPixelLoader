#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace loader {
    struct RGBA {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
        bool operator==(const RGBA& other) const {
            if (r == other.r && g == other.g && b == other.b && a == other.a)
                return true;
            return false;
        };
    } ;

    struct Rect{
        int x;
        int y;
        int height;
        int width;

        bool operator==(const Rect& other) const {
            if (x == other.x && y == other.y && height == other.height && width == other.width)
                return true;
            return false;
        };
    };
}

namespace std {
    template<> struct hash<loader::Rect>
    {
        size_t operator()(const loader::Rect& r) const noexcept
        {
            return r.height *37 + r.width + r.x * 37 * 37 + r.y * 37 * 37 * 37; // make better hash
        }
    };

    template<> struct hash<loader::RGBA>
    {
        size_t operator()(const loader::RGBA& r) const noexcept
        {
            return r.r *37 + r.g + r.b * 37 * 37 + r.a * 37 * 37 * 37;
        }
    };
}

namespace loader{
    class ImageRGB {
    public:
        int height;
        int width;
        std::vector<RGBA> pixels;
        explicit ImageRGB (std::string const& fileName);
    };

    class GDRectLoader {
        static constexpr int SQUARE_ID = 211;
        static constexpr int BLACK_ID = 1010;

        std::unordered_map<RGBA, std::unordered_set<Rect>> rects;
        ImageRGB image;
        double size;
        double xpos;
        double ypos;

        std::string rectToObjString(Rect const& rect, std::string const& hsvString) const;
        void setRects();
        static std::string hsvString(RGBA const& color);
    public:
        explicit GDRectLoader (std::string const& fileName, float const tsize, float const tx, float const ty);
        explicit GDRectLoader (std::string const& fileName, float tsize);
        std::string fullString () const;
        std::string fullStringColorLinked() const;
        std::string fullStringLinked() const;
        std::vector<std::string> splitByColorString () const;
    };
}

