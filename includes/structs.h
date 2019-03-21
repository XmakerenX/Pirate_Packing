#pragma once
#include <vector>
#include <exception>
#include <iostream>
#include <QVector3D>



typedef QVector3D QPoint3D;
typedef QVector3D RGB;

enum class GA_Method { PureGenetic, HybridGenetic};

struct Dimensions 
{
    Dimensions()
        :w(0), h(0), d(0)
    {}
    Dimensions(long unsigned int _w, long unsigned int _h, long unsigned int _d)
        :w(_w), h(_h), d(_d)
    {}
    
    Dimensions(const Dimensions& copy)
        :w(copy.w), h(copy.h), d(copy.d)
    {}
    
    long unsigned int w, h, d;
};

struct BoxInfo
{
    BoxInfo(QPoint3D _startingPoint, RGB _color, int _boxWidth, int _boxHeight, int _boxLength, int _value)
        :startingPoint(_startingPoint), color(_color),
         boxWidth(_boxWidth), boxHeight(_boxHeight), boxLength(_boxLength), value(_value)
    {}

    QPoint3D startingPoint;
    RGB color;
    int boxWidth;
    int boxHeight;
    int boxLength;
    int value;
};



typedef std::vector<int> Chromozome;
struct Position
{
	int pos_x;
	int pos_y;
};
struct CantFitException : public std::exception
{
	const char * what() const throw ()
	{
		return "Could fit the item inside the container";
	}
};
struct Color
{
	Color(){}
	Color(const Color& c)
		:r(c.r), g(c.g), b(c.b)
	{}

	unsigned char r;
	unsigned char g;
	unsigned char b;
};
