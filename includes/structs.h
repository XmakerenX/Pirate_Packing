#pragma once
#include <vector>
#include <exception>
#include <iostream>
#include <QVector3D>



typedef QVector3D QPoint3D;
typedef QVector3D RGB;

enum class GA_Method { PureGenetic, HybridGenetic};



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


struct GenerationData
{
	GenerationData()
		:avarageFittness(0), bestCreature_Fittness(0), bestCreature_VolumeFilled(0), bestCreature_ValuePercentage(0)
	{}
	float avarageFittness;
	std::vector<BoxInfo> bestCreature_BoxInfo;
	int bestCreature_Fittness;
	float bestCreature_VolumeFilled;
	float bestCreature_ValuePercentage;

	int bestFittnessUntillThisGeneration;
};

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
		return "Could not fit the item inside the container";
	}
};
//------------------------------------------------------------------
struct InvalidInputException : public std::exception
{
	std::string errorMsg = "data is invalid";
	const char * what() const throw ()
	{
		return errorMsg.c_str();
	}
	void setErrorMsg(std::string newErrorMsg)
	{
		errorMsg = newErrorMsg;
	}
};
//------------------------------------------------------------------
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
