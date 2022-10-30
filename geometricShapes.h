#pragma once

#include <string>

#include "drawerInterface.h"

class Shape {
  public:
    explicit Shape(int keySize) : keySize_(keySize) {}

    virtual ~Shape() = default;

    virtual std::string getIntroduction() const
    {
        return "Shape " + name() + " with " + keyParameterName() + ": " + std::to_string(keySize_);
    }

    virtual void beDrawen(const Drawer& drawer) const = 0;

  protected:
    const int keySize_;

  private:

    virtual std::string name() const = 0;
    virtual std::string keyParameterName() const = 0;
};

class Square : public Shape {
  public:
    explicit Square(int side) : Shape(side) {}

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

class Triangle : public Shape {
  public:
    explicit Triangle(int side) : Shape(side) {}

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

class Circle : public Shape {
  public:
    explicit Circle(int diameter) : Shape(diameter) {}

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
        return "diameter";
    }
};

class Rectangle : public Shape {
  public:
    explicit Rectangle(int side, double ratio) : Shape(side), ratio_(ratio) {}

    std::string getIntroduction() const override
    {
        return Shape::getIntroduction() + " and ratio: " + std::to_string(ratio_);
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
