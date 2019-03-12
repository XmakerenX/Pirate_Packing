#pragma once
#include <vector>
#include <exception>
#include <iostream>


typedef std::vector<int> Chromozome;
typedef struct Position
{
	int pos_x;
	int pos_y;
}Position;
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
