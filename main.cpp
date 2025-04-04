#include <iostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SQUARE_ID 211
#define BLACK_ID 1010

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

struct HSV {
    uint8_t h;
    uint8_t s;
    uint8_t v;
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
} ;

namespace std {
    template<> struct hash<Rect>
    {
        std::size_t operator()(const Rect& r) const noexcept
        {
            return r.height *37 + r.width + r.x * 37 * 37 + r.y * 37 * 37 * 37;
        }
    };

    template<> struct hash<RGBA>
    {
        std::size_t operator()(const RGBA& r) const noexcept
        {
            return r.r *37 + r.g + r.b * 37 * 37 + r.a * 37 * 37 * 37;
        }
    };
}

class ImageLoader {
protected:
    int height;
    int width;
    std::vector<RGBA> pixels;
public:
    explicit ImageLoader (std::string const fileName) { // constructor;
        int channels;
        height = 0;
        width = 0;
        unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &channels, 4);
        pixels.reserve(height * width);
        for (int y = 0; y < height; y++) { // each row
            for (int x = 0; x < width; x++) {// each  4 at a time since each pixel is represented by 4 chars
                const unsigned char* offset = data + (y * width + x) * 4;
                //printf("%i", (y * *width + x) * 4);
                pixels.push_back({offset[0], offset[1], offset[2], offset[3]});
                //printf("%i %i  ", x, y);
            }
        }
        stbi_image_free(data);
    }


};

class GDRectLoader : ImageLoader {
private:
    std::unordered_set<RGBA> colorPalette;
    std::unordered_map<RGBA, std::unordered_set<Rect>> rects;
    float size;
    float x;
    float y;

    std::string rectToString(Rect rect, std::string hsvString) {

        //30 distance units = 1 size unit
        const float distSize = 30 * size;
        std::string output;
        output += "1,"; //Object ID
        output += std::to_string(SQUARE_ID);

        output += ",2,"; // x position
        output += std::to_string((x + ((rect.x + 0.5 * rect.width) * distSize)));

        output += ",3,"; // y position
        output += std::to_string((y - ((rect.y + 0.5 * rect.height) * distSize)));

        output += ",21,"; // color channel
        output += std::to_string(BLACK_ID);

        output += ",41,1"; // HSV enabled I hate robert topala

        output += ",43,"; // Main Color HSV
        output += hsvString;

        output += ",128,"; // X Scale
        output += std::to_string((rect.width * size));

        output += ",129,"; // Y Scale
        output += std::to_string((rect.height * size));

        output += ";"; // check to see if ending with semicolon is fine

        return output;
    }

    void setColorRect() {
        std::unordered_set<Rect> checked;


        RGBA curr = pixels[0];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; ) {
                curr = pixels[y * width + x];
                Rect rect = {x, y, 0, 0};
                while (pixels[y * width + x] == curr && x < width) {
                    x++;
                }
                rect.width = x - rect.x;

                int tempi = y;
                char isGood = 1;
                char alreadyChecked = 0;

                while (isGood && (tempi < height)) {
                    Rect testing = {rect.x, tempi, 1, rect.width };
                    if (checked.contains(testing)) {
                        alreadyChecked = 1;
                        break;
                    }
                    for (int k = rect.x; k < rect.x + rect.width; k++) {
                        checked.insert(testing);
                        if (pixels[tempi * width + k] != curr) {
                            isGood = 0;
                        }
                    }
                    if (isGood) {
                        tempi++;
                    }
                }

                if (!alreadyChecked) {
                    rect.height = tempi - rect.y;
                    //sameColorRect.insert(rect);
                    if (rects.contains(curr)) {
                        rects[curr].insert(rect);
                    } else {
                        std::unordered_set<Rect> temp;
                        temp.insert(rect);
                        rects[curr] = temp;
                    }
                    // printf("%i" ,rect.x);
                    // printf("%i" ,rect.y);
                    // printf("%i" ,rect.width);
                    // printf("%i " ,rect.height);
                }

            }
        }
        //rects.insert({color, sameColorRect});
    }

    void setRects() {

        setColorRect();

    }

    std::string hsvString(RGBA color) {
        float r = color.r / 255.0f;
        float g = color.g / 255.0f;
        float b = color.b / 255.0f;

        float cMax = std::max(r, std::max(g,b));
        float cMin = std::min(r, std::min(g,b));

        float del = cMax - cMin;
        float s = 0.0;
        if (cMax != 0.0) {
            s = del/cMax;
        }
        float h;
        if (r >= cMax) {
            h = (g-b) / del;
        } else if (g >= cMax) {
            h = (b-r) / del + 2;
        } else {
            h = (r-g) / del + 4;
        }
        if (del == 0.0) {
            h = 0.0;
        }

        while (h < -3.0) {
            h+=6;
        }
        h+=3;
        h = fmodf(h, 6); // now is value from 0 - 6. We want a value from -180 to 180
        h*=60;
        h-=180;

        float v = cMax;
        std::string output;
        output += std::to_string(h);
        output += "a";
        output += std::to_string(s);
        output += "a";
        output += std::to_string(v);
        output += "a1a1";
        return output;
    }

public:
    explicit GDRectLoader (std::string fileName) : ImageLoader(std::move(fileName)) {
        for (int i = 0; i < width * height; i++) { //Color Palette Construction
            if (pixels[i].a != 0) {
                colorPalette.insert(pixels[i]);
            }
        }
        setRects();
        size = 0.25;
        x = 255;
        y = 165;
    }

    std::string fullString () {
        std::string output;
        for (RGBA color : colorPalette) {
            std::unordered_set<Rect> temp = rects[color];
            for (Rect rect : temp) {
                output += rectToString(rect, hsvString(color));
            }
        }
        return output;
    }

    std::vector<std::string> splitByColorString () {
        std::vector<std::string> output;
        for (RGBA color : colorPalette) {
            std::string curr;
            std::unordered_set<Rect> temp = rects[color];
            for (Rect rect : temp) {
                curr += rectToString(rect, hsvString(color));
            }
            output.push_back(curr);
        }
        return output;
    }



};



