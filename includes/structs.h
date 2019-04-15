#pragma once
#include <vector>
#include <exception>
#include <iostream>
#include <QVector3D>



typedef QVector3D QPoint3D;
typedef QVector3D RGB;

enum class GA_Method { PureGenetic, HybridGenetic};




struct APoint
{
	APoint(int _x, int _y, int _z)
		: x(_x), y(_y), z(_z)
	{}

	int x;
	int y;
	int z;
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


struct GenerationData
{
	GenerationData(const std::vector<BoxInfo>& _bestCreatureBoxInfo,
                       float _avarageFittness,
                       int _bestCreatureFittness,
                       int _bestOverallFittness,
                       float containerVolume,
                       float maxiumValue)
        :bestCreatureBoxInfo(_bestCreatureBoxInfo), avarageFittness(_avarageFittness),
         bestCreatureFittness(_bestCreatureFittness), bestOverallFittness(_bestOverallFittness)
	{
            calculateOverallVolumeValue(containerVolume, maxiumValue);
        }
        
        GenerationData(std::vector<BoxInfo>&& _bestCreatureBoxInfo,
                       float _avarageFittness,
                       int _bestCreatureFittness,
                       int _bestOverallFittness,
                       float containerVolume,
                       float maxiumValue)
        :bestCreatureBoxInfo(std::move(_bestCreatureBoxInfo)), avarageFittness(_avarageFittness),
         bestCreatureFittness(_bestCreatureFittness), bestOverallFittness(_bestOverallFittness)
	{
            calculateOverallVolumeValue(containerVolume, maxiumValue);
        }
        
        void calculateOverallVolumeValue(float containerVolume, float maxiumValue)
        {
		int overallValue = 0;
		int overallVolume = 0;
		for (BoxInfo& boxinfo : bestCreatureBoxInfo)
		{
			overallValue += boxinfo.value;
			overallVolume += boxinfo.boxWidth * boxinfo.boxHeight * boxinfo.boxLength;
		}
		
		bestCreatureVolumeFilled = overallVolume / containerVolume;
		bestCreatureValuePercentage = overallValue / maxiumValue;            
        }
	
	std::vector<BoxInfo> bestCreatureBoxInfo;
	float avarageFittness;
	int bestCreatureFittness;
	float bestCreatureVolumeFilled;
	float bestCreatureValuePercentage;
	int bestOverallFittness;
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
