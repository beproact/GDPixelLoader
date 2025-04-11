//
// Created by snappymink on 4/11/2025.
//
#include "pixelLoader.hpp"

using namespace loader;
int main() {
    /*int a = 0;
    GDRectLoader rect_loader = GDRectLoader("ben.png", 0.125, 5);
    std::string list = rect_loader.fullStringColorLinked();
    std::cout << list;*/
    for (uint8_t i = 0; i < 255; i++) {
        std::cout << std::to_string(ImageRGB::roundToPowTwo(i, 0)) << " ";
    }
}