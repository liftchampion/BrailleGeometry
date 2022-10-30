#pragma once

class Drawer {
  public:
    virtual ~Drawer() = default;

    virtual void drawSquare(int side) const = 0;
    virtual void drawTriangle(int side) const = 0;
    virtual void drawCircle(int radius) const = 0;
    virtual void drawRectangle(int side, double ratio) const = 0;
};
