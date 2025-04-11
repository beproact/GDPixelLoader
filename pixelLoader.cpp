#pragma once
#include "pixelLoader.hpp"
#include <string>


using namespace loader;

ImageRGB::ImageRGB (std::string const& fileName) { // constructor;
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

std::string GDRectLoader::rectToObjString(Rect const& rect, std::string const& hsvString) const {

            //30 distance units = 1 size unit
            const double distSize = 30 * size;
            std::string output;
            output += "1,"; //Object ID
            output += std::to_string(SQUARE_ID);

            output += ",2,"; // x position
            output += std::to_string((xpos + ((rect.x + 0.5 * rect.width) * distSize)));

            output += ",3,"; // y position
            output += std::to_string((ypos - ((rect.y + 0.5 * rect.height) * distSize) - 90));
            //no clue why i need to subtract 90

            output += ",21,"; // color channel
            output += std::to_string(BLACK_ID);

            output += ",41,1"; // HSV enabled I hate robert topala

            output += ",43,"; // Main Color HSV
            output += hsvString;

            output += ",128,"; // X Scale
            output += std::to_string((rect.width * size));

            output += ",129,"; // Y Scale
            output += std::to_string((rect.height * size));

            //output += ";"; // check to see if ending with semicolon is fine

            return output;
        }

void GDRectLoader::setRects() {
            std::unordered_set<Rect> checked;


            RGBA curr = image.pixels[0];
            for (int y = 0; y < image.height; y++) {
                for (int x = 0; x < image.width; ) {
                    curr = image.pixels[y * image.width + x];

                    Rect rect = {x, y, 0, 0};
                    while (image.pixels[y * image.width + x] == curr && x < image.width) {
                        x++;
                    }
                    if(curr.a == 0) {
                    	continue;
                    }
                    rect.width = x - rect.x;

                    int tempi = y;
                    char isGood = 1;
                    char alreadyChecked = 0;

                    while (isGood && (tempi < image.height)) {
                        Rect testing = {rect.x, tempi, 1, rect.width };
                        if (checked.contains(testing)) {
                            alreadyChecked = 1;
                            break;
                        }
                        for (int k = rect.x; k < rect.x + rect.width; k++) {
                            if (image.pixels[tempi * image.width + k] != curr) {
                                isGood = 0;
                                break;
                                //can I break here???
                            }
                            checked.insert(testing);
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

std::string GDRectLoader::hsvString(RGBA const& color) {
            double const r = color.r / 255.0;
            double const g = color.g / 255.0;
            double const b = color.b / 255.0;

            double const cMax = std::max(r, std::max(g,b));
            double const cMin = std::min(r, std::min(g,b));

            double const del = cMax - cMin;

            double h;
            double s = 0.0;
            double v = cMax;

            if (cMax != 0.0) {
                s = del/cMax;
            }

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
            h = fmodf(h, 6); // now is value from 0 to 6. We want a value from -180 to 180
            h*=60;
            h-=180;


            //why the does this need to be
            if(h == 0 && (s > 0.98 && v > 0.6) || (s >0.99 && v > 0.3 )){
                h++;
            }

            std::string output;
            output += std::to_string(h);
            output += "a";
            output += std::to_string(s);
            output += "a";
            output += std::to_string(v);
            output += "a1a1";
            return output;
        }

GDRectLoader::GDRectLoader (::std::string const& fileName, float const tsize, float const tx, float const ty): rects(),
            image(fileName) {
            //image = ImageRGB(fileName);
            setRects();
            size = tsize;
            xpos = tx;
            ypos = ty;
        }

GDRectLoader::GDRectLoader (std::string const& fileName, float tsize) : GDRectLoader(fileName, tsize, 0, 0) {}

std::string GDRectLoader::fullString () const {
            std::string output;
            for (std::pair<RGBA, std::unordered_set<Rect>> pair : rects) {
                //std::unordered_set<Rect> temp = pair.second;
                for (Rect rect : pair.second) {
                    output += rectToObjString(rect, hsvString(pair.first)) + ";";
                }
            }
            return output;
        }

std::string GDRectLoader::fullStringColorLinked() const {
        	std::string output;
            int m_lastUsedLinkedID = 0;
            for(std::pair<RGBA, std::unordered_set<Rect>> pair : rects) {
            	//std::unordered_set<Rect> temp = rects[color];
                (m_lastUsedLinkedID)++;
                for (Rect rect : pair.second) {
                	output += rectToObjString(rect, hsvString(pair.first));
                    if(!pair.second.empty()) {
                    	output += ",108," + std::to_string(m_lastUsedLinkedID);
                    }
                    output += ";";
                }
            }
            return output;
        }

std::string GDRectLoader::fullStringLinked() const {
        	std::string output;
            int m_lastUsedLinkedID = 0;
            for(std::pair<RGBA, std::unordered_set<Rect>> pair : rects) {
            	//std::unordered_set<Rect> temp = rects[pair.first];
                (m_lastUsedLinkedID)++;
                for (Rect rect : pair.second) {
                	output += rectToObjString(rect, hsvString(pair.first));
                    if(pair.second.size() > 1) {
                    	output += ",108,1";
                    }
                    output += ";";
                }
            }
            return output;
        }

std::vector<std::string> GDRectLoader::splitByColorString () const {
            std::vector<std::string> output;
            for (std::pair<RGBA, std::unordered_set<Rect>> pair : rects) {
                std::string curr;
                //std::unordered_set<Rect> temp = rects[color];
                for (Rect rect : pair.second) {
                    curr += rectToObjString(rect, hsvString(pair.first)) + ";";
                }
                output.push_back(curr);
            }
            return output;
        }
;

int main() {
	int a = 0;
    GDRectLoader rect_loader = GDRectLoader("Crystal Shards.png", 1);
    std::string list =  rect_loader.fullStringColorLinked();
    std::cout << list;
     /*for (std::string string : list) {
         std::cout << string << "\n";
     }*/
     return 0;
}
