#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include <vector>
#include <algorithm>
using namespace std;

class Point{
public:
	long double x, y;
	Point(long double x = 0, long double y = 0){
		this->x = x;
		this->y = y;
	}
	
	bool operator<(const Point& p){
		return this->y < p.y;
	}
	
	bool operator>(const Point& p){
		return this->y > p.y;
	}
	
	bool operator<(long double d){
		return this->y < d;
	}
	bool operator>(long double d){
		return this->y > d;
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
	
	Point getMax(){
		return *std::max_element(points.begin(), points.end(), [](Point i, Point j){
			return i.y<j.y;
		});
	}
	
	Point getMin(){
		return *std::min_element(points.begin(), points.end(), [](Point i, Point j){
			return i.y<j.y;
		});
	}
	
	long double getAverage(){
		long double sum = 0;
		#pragma omp simd reduction(+:sum)
		for(unsigned i = 0; i<points.size(); i++){
			sum += points[i].y;
		}
		
		return sum/points.size();
	}
};

#endif // SCATTERPLOT_H
