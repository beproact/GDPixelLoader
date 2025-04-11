#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cstdint>


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

        static uint8_t roundToPowTwo (uint8_t num, int pow);
        // range from 0-8 for color compression
        explicit ImageRGB (std::string const& fileName, uint8_t const& colorCompression);
        explicit ImageRGB (std::string const& fileName);
    };

    class GDRects {
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
        explicit GDRects (std::string const& fileName, float const tsize, float const tx, float const ty, uint8_t const& colorCompression);
        explicit GDRects (std::string const& fileName, float tsize, uint8_t const& colorCompression);
        std::string fullString () const;
        std::string fullStringColorLinked() const;
        std::string fullStringLinked() const;
        std::vector<std::string> splitByColorString () const;
    };
}

