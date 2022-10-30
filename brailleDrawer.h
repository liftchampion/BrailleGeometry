#pragma once

#include <array>
#include <vector>
#include <cmath>
#include <iostream>
#include <locale>
#include <codecvt>

#include "drawerInterface.h"

namespace {

struct Point {
    Point(double x, double y) : x(x), y(y)
    {}

    const double x;
    const double y;
};

template<size_t W, size_t H>
class DistancesMatrix {
  public:
    DistancesMatrix()
    {
        for (auto &line : matrix_) {
            for (auto &dot : line) {
                dot = std::numeric_limits<double>::max();
            }
        }
    }

    Point getCenter() const
    {
        return Point(W / 2., H / 2.);
    }

    const std::array<std::array<double, W>, H> &getMatrix() const
    {
        return matrix_;
    }

    template<typename ...Points>
    void setDistancesForPoly(const Points &... points)
    {

        static_assert(sizeof...(points) > 1, "must be more the one point");

        const std::array pointsArr = {std::ref(points)...};
        for (size_t i = 0; i < pointsArr.size(); ++i) {
            const Point &start = pointsArr[i];
            const Point &end = pointsArr[(i + 1) % pointsArr.size()];
            setDistancesForLineSegment(start, end);
        }
    }

    void setDistancesForCircle(const Point &center, const double radius)
    {
        for (size_t y = 0; y < H; ++y) {
            for (size_t x = 0; x < W; ++x) {
                const Point currPoint = Point(x, y);
                const double distanceBetweenCenterAndPoint = distanceBetweenPoints(currPoint, center);
                const double distance = std::abs(distanceBetweenCenterAndPoint - radius);
                matrix_[y][x] = std::min(distance, matrix_[y][x]);
            }
        }
    }

  private:
    void setDistancesForLineSegment(const Point &start, const Point &end)
    {
        for (size_t y = 0; y < H; ++y) {
            for (size_t x = 0; x < W; ++x) {
                const Point currPoint = Point(x, y);
                double distance = std::numeric_limits<double>::max();
                if (canDropPerpendicular(start, end, currPoint)) {
                    distance = distanceBetweenPointAndLine(currPoint, start, end);
                }
                else {
                    distance = std::min(distanceBetweenPoints(currPoint, start),
                                        distanceBetweenPoints(currPoint, end));
                }
                matrix_[y][x] = std::min(distance, matrix_[y][x]);
            }
        }
    }

    static bool canDropPerpendicular(const Point &segStart, const Point &segEnd, const Point &point)
    {
        const Point StartToEndVector = {segEnd.x - segStart.x, segEnd.y - segStart.y};
        const Point EndToStartVector = {-StartToEndVector.x, -StartToEndVector.y};

        const Point StartToPointVector = {point.x - segStart.x, point.y - segStart.y};
        const Point EndToPointVector = {point.x - segEnd.x, point.y - segEnd.y};

        const double dotProductOfStartToEndAndStartToPoint =
                StartToEndVector.x * StartToPointVector.x + StartToEndVector.y * StartToPointVector.y;
        const double dotProductOfEndToStartAndEndToPoint =
                EndToStartVector.x * EndToPointVector.x + EndToStartVector.y * EndToPointVector.y;

        return dotProductOfStartToEndAndStartToPoint >= 0 && dotProductOfEndToStartAndEndToPoint >= 0;
    }

    static double distanceBetweenPoints(const Point &a, const Point &b)
    {
        return std::sqrt(std::pow((b.x - a.x), 2) + std::pow((b.y - a.y), 2));
    }

    static double distanceBetweenPointAndLine(const Point &point, const Point &lineStart, const Point &lineEnd)
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
};

} // namespace

template <size_t H, size_t W>
class BrailleDrawer : public Drawer {
  public:
    BrailleDrawer(double lineWidth) : lineWidth_(lineWidth) {}

    void drawSquare(int side) const override
    {
        drawRectangle(side, 1.0);
    }

    void drawTriangle(int side) const override
    {
        DistancesMatrix<W, H> matrix;

        Point matrixCenter = matrix.getCenter();

        double height = (std::sqrt(3.) / 2.) * side;

        Point triangleTop(matrixCenter.x, matrixCenter.y + height / 2.);
        Point triangleRight(matrixCenter.x + side / 2., matrixCenter.y - height / 2.);
        Point triangleLeft(matrixCenter.x - side / 2., matrixCenter.y - height / 2.);

        matrix.setDistancesForPoly(triangleTop, triangleRight, triangleLeft);

        auto brailleRes = brailleStringsFromDistancesMatrix(matrix.getMatrix());

        drawAsUTF8(brailleRes);
    }

    void drawCircle(int diameter) const override
    {
        DistancesMatrix<W, H> matrix;

        Point matrixCenter = matrix.getCenter();

        matrix.setDistancesForCircle(matrixCenter, diameter / 2.);

        auto brailleRes = brailleStringsFromDistancesMatrix(matrix.getMatrix());

        drawAsUTF8(brailleRes);
    }

    void drawRectangle(int side, double ratio) const override
    {
        DistancesMatrix<W, H> matrix;

        Point matrixCenter = matrix.getCenter();

        const double verticalSide = side * ratio;
        const double horizontalSide = side;

        Point squareLeftUpper(matrixCenter.x - horizontalSide / 2., matrixCenter.y + verticalSide / 2.);
        Point squareRightUpper(matrixCenter.x + horizontalSide / 2., matrixCenter.y + verticalSide / 2.);
        Point squareRightLower(matrixCenter.x + horizontalSide / 2., matrixCenter.y - verticalSide / 2.);
        Point squareLeftLower(matrixCenter.x - horizontalSide / 2., matrixCenter.y - verticalSide / 2.);

        matrix.setDistancesForPoly(squareLeftUpper, squareRightUpper, squareRightLower, squareLeftLower);

        auto brailleRes = brailleStringsFromDistancesMatrix(matrix.getMatrix());

        drawAsUTF8(brailleRes);
    }

  private:

    std::vector<std::wstring> brailleStringsFromDistancesMatrix(const std::array<std::array<double, W>, H>& matrix) const
    {
        std::array<std::array<uint8_t, W / BRAILLE_WIDTH_>, H / BRAILLE_HEIGHT_> braillePureValues = {};

        static constexpr std::array<std::array<uint8_t, 2>, 4> BRAILLE_OFFSETS = {{{0, 3},
                                                                                   {1, 4},
                                                                                   {2, 5},
                                                                                   {6, 7}}};

        for (size_t y = 0; y < H; ++y) {
            for (size_t x = 0; x < W; ++x) {
                if (matrix[y][x] < lineWidth_) {
                    uint8_t brailleOffset = 1u << BRAILLE_OFFSETS[y % BRAILLE_HEIGHT_][x % BRAILLE_WIDTH_];
                    braillePureValues[y / BRAILLE_HEIGHT_][x / BRAILLE_WIDTH_] |= brailleOffset;
                }
            }
        }

        std::vector<std::wstring> result;
        result.resize(H / BRAILLE_HEIGHT_);
        for (size_t i = 0; i < H / BRAILLE_HEIGHT_; ++i) {
            result[i].resize(W / BRAILLE_WIDTH_);
            for (size_t j = 0; j < W / BRAILLE_WIDTH_; ++j) {
                wchar_t w_ch = BRAILLE_UNICODE_OFFSET_ + braillePureValues[i][j];
                result[i][j] = w_ch;
            }
        }

        return result;
    }

    static void drawAsUTF8(const std::vector<std::wstring>& wLines)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>> convert;
        for (const auto& wStr : wLines) {
            for (const auto& wChr : wStr) {
                std::cout << convert.to_bytes(wChr);
            }
            std::cout << "\n";
        }
        std::cout.flush();
    }

    const double lineWidth_;

    static constexpr wchar_t BRAILLE_UNICODE_OFFSET_ = L'\u2800';
    static constexpr int BRAILLE_HEIGHT_ = 4;
    static constexpr int BRAILLE_WIDTH_  = 2;
};