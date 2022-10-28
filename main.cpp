#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <array>
#include <codecvt>
#include <locale>
#include <iomanip>
#include <fcntl.h>

/// Drawer interface

class Drawer {
  public:
    virtual void drawSquare(int side) const = 0;
    virtual void drawTriangle(int side) const = 0;
    virtual void drawCircle(int radius) const = 0;
    virtual void drawRectangle(int side, double ratio) const = 0;
};

/// figures classes

class Figure {
  public:
    explicit Figure(int keySize) : keySize_(keySize) {}

    virtual std::string getIntroduction() const
    {
        return "Figure " + name() + " with " + keyParameterName() + ": " + std::to_string(keySize_);
    }

    virtual void beDrawen(const Drawer& drawer) const = 0;

  protected:
    const int keySize_;

  private:

    virtual std::string name() const = 0;
    virtual std::string keyParameterName() const = 0;
};

class Square : public Figure {
  public:
    explicit Square(int side) : Figure(side) {}

    void beDrawen(const Drawer& drawer) const override
    {
        drawer.drawSquare(keySize_);
    }

  private:
    std::string name() const override
    {
        return "Square";
    }

    std::string keyParameterName() const override
    {
        return "side";
    }
};

class Triangle : public Figure {
public:
    explicit Triangle(int side) : Figure(side) {}

    void beDrawen(const Drawer& drawer) const override
    {
        drawer.drawTriangle(keySize_);
    }

private:
    std::string name() const override
    {
        return "Triangle";
    }

    std::string keyParameterName() const override
    {
        return "side";
    }
};

class Circle : public Figure {
public:
    explicit Circle(int radius) : Figure(radius) {}

    void beDrawen(const Drawer& drawer) const override
    {
        drawer.drawCircle(keySize_);
    }

private:
    std::string name() const override
    {
        return "Circle";
    }

    std::string keyParameterName() const override
    {
        return "radius";
    }
};

class Rectangle : public Figure {
public:
    explicit Rectangle(int side, double ratio) : Figure(side), ratio_(ratio) {}

    std::string getIntroduction() const override
    {
        return Figure::getIntroduction() + " and ratio: " + std::to_string(ratio_);
    }

    void beDrawen(const Drawer& drawer) const override
    {
        drawer.drawRectangle(keySize_, ratio_);
    }

private:
    std::string name() const override
    {
        return "Rectangle";
    }

    std::string keyParameterName() const override
    {
        return "side";
    }

    const double ratio_;
};


/// drawer class

template <size_t W, size_t H>
class DrawingMatrix {
public:
    struct Point {
        Point(double x, double y) : x(x), y(y) {}
        const double x;
        const double y;
    };

    DrawingMatrix() {
        for (auto& line : matrix_) {
            for (auto& dot : line) {
                dot = std::numeric_limits<double>::max();
            }
        }
    }

    void setDistancesForLineSegment(const Point& start, const Point& end)
    {
        for (size_t y = 0; y < H; ++y) {
            for (size_t x = 0; x < W; ++x) {
                const Point currPoint = Point(x, y);
                double distance = std::numeric_limits<double>::max();
                if (canDropPerpendicular(start, end, currPoint)) {
                    distance = distanceBetweenPointAndLine(currPoint, start, end);
                } else {
                    distance = std::min(distanceBetweenPoints(currPoint, start),
                                        distanceBetweenPoints(currPoint, end));
                }
                matrix_[y][x] = std::min(distance, matrix_[y][x]);
            }
        }

        for (const auto& line : matrix_) {
            for (const auto& dot : line) {
                std::cout << std::fixed << std::setprecision(2) << dot << " ";
            }
            std::cout << std::endl;
        }
    }

    std::vector<std::wstring> toBraille(double width) const
    {
        // todo gg! ?????
        std::array<std::array<uint8_t, W / braille_width_>, H / braille_height_> braillePureValues = {};

        static constexpr std::array<std::array<uint8_t, 2>, 4> BRAILLE_OFFSETS = {{{0, 3},
                                                                                   {1, 4},
                                                                                   {2, 5},
                                                                                   {6, 7}}};

        for (size_t y = 0; y < H; ++y) {
            for (size_t x = 0; x < W; ++x) {
                if (matrix_[y][x] < width) {
                    uint8_t brailleOffset = 1 << BRAILLE_OFFSETS[y % braille_height_][x % braille_width_];
                    braillePureValues[y / braille_height_][x / braille_width_] |= brailleOffset;
                }
            }
        }

        std::vector<std::wstring> result;
        result.resize(H / braille_height_);
        for (int i = H / braille_height_ - 1; i >= 0; --i) {
            result[i].resize(W / braille_width_);
            for (int j = 0; j < W / braille_width_; ++j) {
                wchar_t w_ch = braille_unicode_offset_ + braillePureValues[i][j];
                result[i][j] = w_ch;
            }
        }

        return result;
    }

  private:
    static bool canDropPerpendicular(const Point& segStart, const Point& segEnd, const Point& point)
    {
        const double dotProductWithStart = point.x * segStart.x + point.y * segStart.y;
        const double dotProductWithEnd = point.x * segEnd.x + point.y * segEnd.y;

        return dotProductWithStart >= 0 && dotProductWithEnd >= 0;
    }

    static double distanceBetweenPoints(const Point& a, const Point& b)
    {
        return std::sqrt(std::pow((b.x - a.x), 2) + std::pow((b.y - a.y), 2));
    }

    static double distanceBetweenPointAndLine(const Point& point, const Point& lineStart, const Point& lineEnd)
    {
        const double numerator = std::abs(
                    (lineEnd.y - lineStart.y) * point.x
                    - (lineEnd.x - lineStart.x) * point.y
                    + lineEnd.x * lineStart.y
                    - lineEnd.y * lineStart.x
                );
        const double denominator = std::sqrt(
                    std::pow((lineEnd.y - lineStart.y), 2)
                    + std::pow((lineEnd.x - lineStart.x), 2)
                );
        return numerator / denominator;
    }

    std::array<std::array<double, W>, H> matrix_ = {};

    static constexpr wchar_t braille_unicode_offset_ = L'\u2800'; // todo gg! naming!!!
    static constexpr int braille_height_ = 4;
    static constexpr int braille_width_  = 2;
};

class BrailleDrawer : public Drawer {
  public:
    void drawSquare(int side) const override
    {
        DrawingMatrix<100, 100> matrix;

        std::cout << "drawSquare with side: " << side << std::endl; // todo gg!

        matrix.setDistancesForLineSegment({20, 60}, {50, 60});
        auto brailleRes = matrix.toBraille(1.5);
        for (const auto& str : brailleRes) {
            std::wcout << str << std::endl;
        }
    }
    void drawTriangle(int side) const override
    {
        std::cout << "drawTriangle with side: " << side << std::endl;
    }
    void drawCircle(int radius) const override
    {
        std::cout << "drawCircle with radius: " << radius << std::endl;
    }
    void drawRectangle(int side, double ratio) const override
    {
        std::cout << "drawRectangle with side " << side << " and ratio " << ratio << std::endl;
    }

  private:

};


/// helper functions


static std::mt19937& getRandomGenerator()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    return gen;
}

static int getRandomSize()
{
    static std::uniform_int_distribution<> distribution(10, 20);
    return distribution(getRandomGenerator());
}

static double getRandomRatio()
{
    static std::uniform_int_distribution<> distribution(20, 80);
    return distribution(getRandomGenerator()) / 100.;
}


/// main


int main() {
    // interract with user
    std::cout << "Pass a string with figures. (\"TRSC\" for ex)" << std::endl;

    std::string userInput = "ttrrsscc"; // todo gg!

//    std::cin >> userInput; // todo gg!

    std::cout << userInput << std::endl; // todo gg! del it

    // create vector of figures
    std::vector<std::unique_ptr<Figure>> figures;

    for (const char c : userInput) {
        switch (tolower(c)) {
            case 's': {
                figures.emplace_back(std::make_unique<Square>(getRandomSize()));
                break;
            }
            case 't': {
                figures.emplace_back(std::make_unique<Triangle>(getRandomSize()));
                break;
            }
            case 'c': {
                figures.emplace_back(std::make_unique<Circle>(getRandomSize()));
                break;
            }
            case 'r': {
                figures.emplace_back(std::make_unique<Rectangle>(getRandomSize(), getRandomRatio()));
                break;
            }
        }
    }

    // tell user what we are gonna to draw
    std::cout << "You requested to draw: " << std::endl;
    for (const auto& f : figures) {
        std::cout << "< " << f->getIntroduction() << std::endl;
    }

    // set utf16 output
//    _setmode(_fileno(stdout), _O_U16TEXT);

    // draw figures
    BrailleDrawer brailleDrawer;
    for (const auto& f : figures) {
        f->beDrawen(brailleDrawer);
    }

    return 0;
}
