#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdio>
#include <utility>
#include <functional>
#include <random>
#include <serial/serial.h>
#include "allegro/allegro.h"

using namespace std;

#include "function.h"
#include "scatterplot.h"
#include "oscilloscope.h"

const double PI = asin(1)*2;

class Window{
protected:
	vector<Function> functions;
	vector<ScatterPlot> scatter;
	
	vector<Color> colorWheel;
	
	int selectedFunction = -1;
	
	Allegro* allegro;
	
	long double x_min = -5, x_max = 5, y_min = -5, y_max = 5;
	long double pas = 0.1;
	long double pas_original = 0.1;
	
	
	void drawCross(int x, int y, int s, Color color, int width = 1){
		allegro->draw_line(x-s/2 - 1, y, x+s/2, y, color.toAllegro(), width);
		allegro->draw_line(x, y-s/2 - 1, x, y+s/2, color.toAllegro(), width);
	}
public:
	Window(Allegro* allegro, long double x_min = -5, long double x_max = 5, long double y_min = -5, long double y_max = 5){
		this->x_min = x_min;
		this->x_max = x_max;
		this->y_min = y_min;
		this->y_max = y_max;
		this->allegro = allegro;
		
		colorWheel.push_back(Color(255, 0, 0));
		colorWheel.push_back(Color(0, 255, 0));
		colorWheel.push_back(Color(0, 0, 255));
		colorWheel.push_back(Color(128, 128, 0));
		colorWheel.push_back(Color(128, 0, 128));
		colorWheel.push_back(Color(0, 128, 128));
		colorWheel.push_back(Color(128, 128, 128));
	}
	
	unsigned addFunction(Function f){
		cout << "Adding function " << functions.size()+1 << "..." << endl << "\tCalculating values...";
		cout.flush();
		f.initValues(x_min, x_max, pas);
		cout << " Done!" << endl;
		functions.push_back(f);
		return functions.size()-1;
	}
	
	unsigned addScatter(ScatterPlot sp){
		scatter.push_back(sp);
		return scatter.size()-1;
	}
	
	void setFunction(unsigned i, Function f){
		cout << "Changing function " << i << "..." << endl << "\tCalculating values...";
		cout.flush();
		f.initValues(x_min, x_max, pas);
		cout << " Done!" << endl;
		functions[i]=f;
	}
	
	void setScatter(unsigned i, ScatterPlot sp){
		scatter[i]=sp;
	}
	
	Function getFunction(unsigned i){
		return functions[i];
	}
	
	ScatterPlot* getScatter(unsigned i){
		return &scatter[i];
	}
	
	pair<int, int> pointToPixel(long double x, long double y){
		pair<int, int> pixel;
		pixel.first = (x-x_min)*disp_width/(x_max - x_min);
		pixel.second = disp_height - (y-y_min)*disp_height/(y_max - y_min);
		return pixel;
	}
	
	pair<long double, long double> pixelToPoint(long double x, long double y){
		pair<long double, long double> point;
		point.first = (x)*(x_max-x_min)/disp_width + x_min;
		point.second = (disp_height - y)*(y_max - y_min)/disp_height + y_min;
		return point;
	}
	
	void drawFunction(unsigned i, Color color){
		Function func = functions[i];
		
		if(func.hidden){
			return;
		}
		
		long double prec_val = func.getValue(func.x_min);
		pair<int, int> pixel = pointToPixel(func.x_min, prec_val);
		int x_prec = pixel.first;
		int y_prec = pixel.second;
		
		for(unsigned i = 0; i < func.getNumberOfValues(); i++ ){
			long double val = func.getValue(i);
			pixel = pointToPixel(func.x_min + i*pas, val);
			int x = pixel.first;
			int y = pixel.second;
			if(abs(val - prec_val) < 1000 && abs(val) < INFINITY && abs(prec_val) < INFINITY){
				if(i != 0)
					allegro->draw_line(x_prec, y_prec, x, y, color.toAllegro(), 1+func.selected*2);
			}
			x_prec = x;
			y_prec = y;
			prec_val = val;
		}
		
		//pair<int, int> namePos = pointToPixel(x_min + (x_max - x_min)*0.1, func(x_min + (x_max - x_min)*0.1));
		allegro->draw_text(disp_width - 4, 5+15*i, func.getName(), color.toAllegro(), ALLEGRO_ALIGN_RIGHT);
	}
	
	void drawScatter(unsigned i, Color color){
		ScatterPlot sp = scatter[i];
		if(sp.hidden){
			return;
		}
		
		pair<int, int> pixel, prev_pixel;
		
		Point p1 = sp.getPoint(0);
		prev_pixel = pointToPixel(p1.x, p1.y);
		
		for(unsigned i = 1; i < sp.getN(); i++){
			Point p = sp.getPoint(i);
			//if(p.x > x_min && p.x < x_max && p.y > y_min && p.y < y_max){
				pixel = pointToPixel(p.x, p.y);
				if(sp.joinPoints){
					allegro->draw_line(prev_pixel.first, prev_pixel.second, pixel.first, pixel.second, color.toAllegro(), 1);
				} else {
					drawCross(pixel.first, pixel.second, 8, color, 1);
				}
				prev_pixel = pixel;
			//}
		}
	}
	
	void drawAllFunctions(){
		for(unsigned i = 0; i < functions.size(); i++){
			Color color = colorWheel[i%colorWheel.size()];
			drawFunction(i, color);
		}
	}
	
	void drawAllScatterPlots(){
		for(unsigned i = 0; i < scatter.size(); i++){
			Color color = colorWheel[i%colorWheel.size()];
			drawScatter(i, color);
		}
	}
	
	void drawAxisStep(char direction, long double num, long double denominator){
		pair<int, int> point;
		stringstream numstr;
		if(denominator == 1){
			numstr << floor(num);
		} else if (denominator == PI){
			numstr << round(num/PI*100)/100 << "π";
		} else {
			//numstr << floor(num);
			numstr << round(num*denominator*100)/100 << "/" << round(denominator*100)/100;
		}
		if(direction == 'y'){
			point = pointToPixel(0, num);
			allegro->draw_line(point.first - 5, point.second, point.first + 4, point.second);
			allegro->draw_text(point.first + 8, point.second - 8, numstr.str(), allegro->black);
		} else if (direction == 'x'){
			point = pointToPixel(num, 0);
			allegro->draw_line(point.first, point.second - 5, point.first, point.second + 4);
			allegro->draw_text(point.first, point.second + 8, numstr.str(), allegro->black);
		}
	}
	
	void drawAxis(){
		pair<int, int> origine = pointToPixel(0.0, 0.0);
		
		allegro->draw_line(0, origine.second, disp_width, origine.second, allegro->rgb(100, 100, 100), 1);
		allegro->draw_line(origine.first, disp_height, origine.first, 0, allegro->rgb(100, 100, 100), 1);
		
		allegro->draw_text(origine.first, origine.second, "O", allegro->black, ALLEGRO_ALIGN_LEFT);
		
		// On affiche les traits sur les axes
		pair<int, int> point;
		int step_x = max((int)(x_max-x_min)/10, 1);
		int step_y = max((int)(y_max-y_min)/8, 1);
		long double step_pi = max((int)((x_max-x_min)/(4*PI)), 1);
		for(long double i = 0.1; i <= x_max; i += step_x){
			drawAxisStep('x', i, 1);
		}
		
		//for(long double i = -PI/2; i >= x_min; i -= (PI/2)*step_pi){
		for(long double i = -0.1; i >= x_min; i -= step_x){
			drawAxisStep('x', i, 1);
		}
		
		for(long double i = 1.0; i <= y_max; i += step_y){
			drawAxisStep('y', i, 1);
		}
		
		for(long double i = -1; i >= y_min; i -= step_y){
			drawAxisStep('y', i, 1);
		}
	}
	
	pair<long double, long double> getMousePoint(){
		return pixelToPoint(mousePos.first, mousePos.second);
	}
	
	void drawMousePos(){
		pair<long double, long double> mp = getMousePoint();
		
		if(allegro->isKeyDown(ALLEGRO_KEY_LSHIFT)){
			int y = mousePos.second;
			if(selectedFunction >= 0){
				mp.second = functions[selectedFunction](mp.first);
				y = pointToPixel(0, mp.second).second;
			}
			allegro->draw_line(mousePos.first, 0, mousePos.first, disp_height, allegro->rgb(150, 150, 150), 1);
			allegro->draw_line(0, y, disp_width, y, allegro->rgb(150, 150, 150), 1);
		}
		
		stringstream x_disp, y_disp;
		x_disp << "x = " << mp.first;
		y_disp << "y = " << mp.second;
		
		allegro->draw_text(0, 0, x_disp.str(), allegro->black, ALLEGRO_ALIGN_LEFT);
		allegro->draw_text(0, 15, y_disp.str(), allegro->black, ALLEGRO_ALIGN_LEFT);
	}
	
	void setStep(long double pas){
		if(pas > 0.0){
			this->pas_original = pas;
			this->pas = pas;
		} else {
			throw(1);
		}
	}
	
	long double getStep(){
		return pas;
	}
	
	int getFunctionNearestPoint(pair<long double, long double> point){
		pair<long double, int> nearestFunc;
		nearestFunc.second = -1;
		for(unsigned i = 0; i < functions.size(); i++){
			long double val = functions[i](point.first);
			long double e = abs(point.second - val);
			if(e < 0.5){
				nearestFunc.first = e;
				nearestFunc.second = i;
			}
		}
		return nearestFunc.second;
	}
	
	void selectFunction(int i){
		for(unsigned u = 0; u < functions.size(); u++){
			functions[u].selected = false;
		}
		if(i >= 0) {
			functions[i].selected = true;
		}
		selectedFunction = i;
	}
	
	void toggleHideFunction(unsigned i){
		functions[i].hidden = not(functions[i].hidden);
	}
	
	void reinitFunction(unsigned i){
		functions[i].initValues(x_min, x_max, pas);
	}
	
	void reinitAllFunctions(){
		pas = max(pas_original*abs((long double)(x_max - x_min))/11, (long double)0.001);
		for(unsigned i = 0; i<functions.size(); i++){
			reinitFunction(i);
		}
	}
	
	void setDispSize(int w, int h){
		disp_width = w;
		disp_height = h;
	}
	
	void zoom(long double coeff){
//		pair<long double, long double> origine;
//		origine.first = (x_min + x_max)/2;
//		origine.second = (y_min + y_max)/2;
		
	//	x_min = (x_min)*coeff;
	//	x_max = (x_max)*coeff;
		
		y_min = (y_min)*coeff;
		y_max = (y_max)*coeff;
	}
	
	void setXmax(int xmax){
		x_max = (long double)(xmax);
	}
	
	int disp_width = 100;
	int disp_height = 100;
	
	pair<int, int> mousePos;
};


string subunitsConvert(double n, string unit = ""){
	string sub = "";
	if(n > 2000){
		sub = "k";
		n /= 1000;
	} else if(n > 2000000){
		sub = "M";
		n /= 1000000;
	} else if(n < 0.005){
		sub = "µ";
		n *= 1000000;
	} else if(n < 0.5){
		sub = "m";
		n *= 1000;
	}
	
	n = round(n*100)/100;
		
	stringstream ss;
	ss << n << " " << sub << unit;
	return ss.str();
}

unsigned oscilloscope_index;
Oscilloscope* osc;
bool stop = false;
bool update = true;
bool paused = false;
bool updateAnalysis = true;
Point Pmin, Pmax;
long double avg;
double periode = 0;
unsigned stimeInptbx_index;
unsigned triggerBtn_index;

void redraw(Allegro* allegro, float fps){
	allegro->draw_rectangle(0, 0, allegro->getDisplayWidth(), allegro->getDisplayHeight(), allegro->rgb(255, 255, 255), 1, true);
	
	Window* win = (Window*)allegro->getContext();
	
	win->setDispSize(allegro->getDisplayWidth(), allegro->getDisplayHeight()-60);
	
	win->drawAxis();
	win->drawAllFunctions();
	win->drawAllScatterPlots();
	
	win->drawMousePos();
	
	stringstream ss;
	ss << "VMax = " << subunitsConvert(Pmax.y, "V");
	string max = ss.str();
	
	ss.str(string());
	ss << "VMin = " << subunitsConvert(Pmin.y, "V");
	string min = ss.str();
	
	ss.str(string());
	ss << "V~ = " << subunitsConvert(avg, "V");
	string avgstr = ss.str();
	
	ss.str(string());
	ss << "T = " << subunitsConvert(periode/1000000, "s");
	string periodestr = ss.str();
	
	double freq = 1/(periode/1000000);
	ss.str(string());
	ss << "F = " << subunitsConvert(freq, "Hz");
	string freqstr = ss.str();
	
	allegro->draw_rectangle(0, allegro->getDisplayHeight()-60, allegro->getDisplayWidth(), allegro->getDisplayHeight(), allegro->rgb(100, 100, 100), 1, true
);
	ALLEGRO_COLOR yellow = allegro->rgb(250, 250, 0);
	allegro->draw_text(0, allegro->getDisplayHeight()-57, max, yellow, ALLEGRO_ALIGN_LEFT);
	allegro->draw_text(0, allegro->getDisplayHeight()-44, min, yellow, ALLEGRO_ALIGN_LEFT);
	allegro->draw_text(0, allegro->getDisplayHeight()-29, avgstr, yellow, ALLEGRO_ALIGN_LEFT);
	allegro->draw_text(0, allegro->getDisplayHeight()-16, periodestr, yellow, ALLEGRO_ALIGN_LEFT);
	allegro->draw_text(100, allegro->getDisplayHeight()-57, freqstr, yellow, ALLEGRO_ALIGN_LEFT);
}

void mouseMove(Allegro* allegro, void* context, uint16_t event, int x, int y){
	Window* win = (Window*)allegro->getContext();
	if(event & Allegro::MOUSE_MOVED){
		win->mousePos.first = x;
		win->mousePos.second = y;
	}
	
	if(event & Allegro::MOUSE_WHEELED && allegro->isKeyDown(ALLEGRO_KEY_LCTRL)){
		
		long double coeff = 1;
		if(x < 0){
			coeff = 0.99;
		} else {
			coeff = 1.01;
		}
		//long double coeff = min(max(0.5-(x+50)/100, 0.999), 1.001);
		win->zoom(coeff);
	}
}

void mouseClick(Allegro* allegro, void* context, uint16_t event, int x, int y){
	Window* win = (Window*)allegro->getContext();
	if(event & Allegro::MOUSE_L_CLICKED && event & Allegro::MOUSE_DOWN){
		int select = win->getFunctionNearestPoint(win->getMousePoint());
		win->selectFunction(select);
		
		//cout << select << endl;
	}
}

void key(Allegro* allegro, void* context, uint16_t ev, uint8_t keycode){
	Window* win = (Window*)allegro->getContext();
	if(ev & Allegro::KEY_DOWN){
		if(allegro->isKeyDown(ALLEGRO_KEY_LCTRL) && keycode == ALLEGRO_KEY_R){
			win->reinitAllFunctions();
		}
	}
}

long double square(long double x){
	long double r = 0;
	for(int i = 1; i<=2*500; i+=2){
		r += sin(i*x)/i;
	}
	return r;
}

long double triangle(long double x){
	long double r = 0;
	for(int i = 1; i<=2*500; i+=2){
		r += cos(i*x)/(i*i);
	}
	return r;
}

void winResized(Allegro* allegro, void* context, uint16_t ev, int w, int h){
	allegro->getGUI()->input_boxes[stimeInptbx_index].y = h-50;
	allegro->getGUI()->buttons[triggerBtn_index].y = h-50;
	//allegro->getGUI()->input_boxes[stimeInptbx_index].x = allegro->getDisplayHeight()-35;
}

void animate(Allegro* allegro, float FPS){
	if(!update || paused)
		return;
		
	if(osc->getN() < 100)
		return;
		
	Window* win = (Window*)allegro->getContext();
	win->setScatter(oscilloscope_index, *osc);
	Pmin = osc->getMin();
	Pmax = osc->getMax();
	avg = osc->getAverage();
	periode = osc->getPeriod();
	update = false;
}

void updatePts(){
	while(!stop){
		osc->updatePoints();
		update = true;
		this_thread::sleep_for(chrono::microseconds(100000));
	}
}

void changeSampleTime(Allegro* allegro, InputBox* inptbx){
	serial::Serial* arduino = osc->arduino;
	Window* win = (Window*)allegro->getContext();
	
	int stime = stoi(inptbx->text, nullptr, 10);
	if(stime > 10e7 || stime < 10e2)
		return;
		
	win->setXmax(stime);
	stringstream ss;
	ss << "#" << stime;
	arduino->write(ss.str());
	cout << "Changed sample time to " << stime << endl;
}

void toggleTrigger(Allegro* allegro, Button* btn){
	serial::Serial* arduino = osc->arduino;
	//Window* win = (Window*)allegro->getContext();
	if(osc->triggered)
		btn->name = "Untriggered";
	else
		btn->name = "Triggered";
	
	arduino->write("!!");
	cout << "Toggled trigger" << endl;
}

int main(int argc, char **argv)
{
	Allegro allegro_obj = Allegro();
	Allegro* allegro = &allegro_obj;
	
	
	Window window(allegro, -3, 1000, -5, 5);
	window.setStep(0.01);
	
	Oscilloscope oscillo("/dev/ttyACM0");
	osc = &oscillo;
	
	oscillo.updatePoints();
	
	window.addScatter(oscillo);
	
	std::thread bg(updatePts);
	
	allegro->setContext((void*)(&window));
	
	allegro->init();
	allegro->createWindow(30, 500, 400);
	//allegro->createWindow(30, 500, 400);
	
	allegro->setRedrawFunction(&redraw);
	allegro->setAnimateFunction(&animate);
	allegro->bindMouseMove(&mouseMove);
	allegro->bindMouseClick(&mouseClick);
	allegro->bindKeyDown(&key);
	allegro->bindWindowResized(&winResized);
	
	InputBox stimeBox(allegro, "1000", 230, allegro->getDisplayHeight()-50, 30, 100, &changeSampleTime);
	stimeBox.authorized_chars = "0123456789";
	allegro->getGUI()->input_boxes.push_back(stimeBox);
	stimeInptbx_index = allegro->getGUI()->input_boxes.size()-1;
	
	allegro->getGUI()->newBtn("Untriggered", 340, allegro->getDisplayHeight()-50, 30, 50, &toggleTrigger);
	triggerBtn_index = allegro->getGUI()->buttons.size()-1;
	
	allegro->gameLoop();
	
	stop = true;
	bg.join();
	return 0;
}
