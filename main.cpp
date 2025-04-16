#include "gdPixelLoader.hpp"
#include "clipboardxx.hpp"

using namespace loader;
int main() {
    clipboardxx::clipboard clipboard;
    std::string filePath;

    std::cout << "Type the file name or path:";
    std::cin >> filePath;
    ImageRGB img = ImageRGB(filePath, 5);

    GDRects rect_loader = GDRects(&img, 0.125);
    std::string list = rect_loader.fullStringColorLinked();
    if (list.empty()) {
        std::cout << "Failed to read file \n";
        return 1;
    }
    std::cout << "GD Level String has been copied to your clipboard. \n";
    clipboard << list;
}