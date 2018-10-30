#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include "includes.h"

struct Point{
public:
	long double x, y;
	Point(long double x, long double y){
		this->x = x;
		this->y = y;
	}
	
};

class ScatterPlot {
protected:
	vector<Point> points;
public:
	bool hidden = false;
	bool selected = false;
	bool joinPoints = false;

	ScatterPlot(){
		
	}

	unsigned addPoint(Point p){
		points.push_back(p);
		return points.size()-1;
	}
	
	unsigned addPoint(long double x, long double y){
		points.push_back(Point(x, y));
		return points.size()-1;
	}
	
	Point getPoint(unsigned i){
		return points[i];
	}
	
	unsigned getN(){
		return points.size();
	}
};

#endif // SCATTERPLOT_H
