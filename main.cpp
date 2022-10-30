#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <locale>

#include "geometricShapes.h"
#include "brailleDrawer.h"
#include "funDrawer.h"

static std::mt19937& getRandomGenerator()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    return gen;
}

static int getRandomSize()
{
    static std::uniform_int_distribution<> distribution(30, 60);
    return distribution(getRandomGenerator());
}

static double getRandomRatio()
{
    static std::uniform_int_distribution<> distribution(40, 80);
    return distribution(getRandomGenerator()) / 100.;
}

int main() {
    std::setlocale(LC_ALL, "");

    // interract with user
    std::cout << "Pass a string with shapes (\"TRSC\" for ex)" << std::endl;

    std::string userInput;
    std::cin >> userInput;

    // create vector of figures
    std::vector<std::unique_ptr<Shape>> shapes;

    for (const char c : userInput) {
        switch (tolower(c)) {
            case 's': {
                shapes.emplace_back(std::make_unique<Square>(getRandomSize()));
                break;
            }
            case 't': {
                shapes.emplace_back(std::make_unique<Triangle>(getRandomSize()));
                break;
            }
            case 'c': {
                shapes.emplace_back(std::make_unique<Circle>(getRandomSize()));
                break;
            }
            case 'r': {
                shapes.emplace_back(std::make_unique<Rectangle>(getRandomSize(), getRandomRatio()));
                break;
            }
        }
    }

    // tell user what we are gonna to draw
    std::cout << "You requested to draw: " << std::endl;
    for (const auto& s : shapes) {
        std::cout << "< " << s->getIntroduction() << std::endl;
    }

    // create drawers
    std::vector<std::unique_ptr<Drawer>> drawers;
    drawers.emplace_back(std::make_unique<BrailleDrawer<70, 100>>(2.5));
    drawers.emplace_back(std::make_unique<FunDrawer>());

    // draw figures
    for (const auto& drawer : drawers) {
        for (const auto& s : shapes) {
            s->beDrawen(*drawer);
        }
    }

    return 0;
}
