#pragma once

#include <iostream>
#include <locale>
#include <codecvt>

#include "drawerInterface.h"

class FunDrawer : public Drawer {
  public:

    void drawSquare(int) const override
    {
        std::cout << "□" << std::endl;
    }

    void drawTriangle(int) const override
    {
        std::cout << "△" << std::endl;
    }

    void drawCircle(int) const override
    {
        std::cout << "○" << std::endl;
    }

    void drawRectangle(int, double) const override
    {
        std::cout << "▭" << std::endl;
    }
};