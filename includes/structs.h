#pragma once
#include <vector>
#include <exception>
#include <iostream>
#include <QVector3D>



typedef QVector3D QPoint3D;
typedef QVector3D RGB;


struct Dimensions {	int w, h, d;};

struct BoxInfo
{
    BoxInfo(QPoint3D _startingPoint, RGB _color, int _boxWidth, int _boxHeight, int _boxLength)
        :startingPoint(_startingPoint), color(_color), boxWidth(_boxWidth), boxHeight(_boxHeight), boxLength(_boxLength)
    {}

    QPoint3D startingPoint;
    RGB color;
    int boxWidth;
    int boxHeight;
    int boxLength;
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
