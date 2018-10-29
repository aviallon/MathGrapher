#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdio>
#include <utility>
#include <functional>
#include <random>
#include "allegro/allegro.h"
#include "mathparser.h"
#include "function.h"
#include "scatterplot.h"

using namespace std;

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
	
	ScatterPlot getScatter(unsigned i){
		return scatter[i];
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
		
		pair<int, int> pixel;
		for(unsigned i = 0; i < sp.getN(); i++){
			Point p = sp.getPoint(i);
			if(p.x > x_min && p.x < x_max && p.y > y_min && p.y < y_max){
				pixel = pointToPixel(p.x, p.y);
				drawCross(pixel.first, pixel.second, 8, color, 1);
			}
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
		int step_x = max((int)(x_max-x_min)/8, 1);
		int step_y = max((int)(y_max-y_min)/8, 1);
		long double step_pi = max((int)((x_max-x_min)/(4*PI)), 1);
		for(long double i = 1; i <= x_max; i += step_x){
			drawAxisStep('x', i, 1);
		}
		
		for(long double i = -PI/2; i >= x_min; i -= (PI/2)*step_pi){
			drawAxisStep('x', i, PI);
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
	
	void hideFunction(unsigned i, bool val){
		functions[i].hidden = val;
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
	
	void zoomAt(pair<long double, long double> point, long double coeff){
		pair<long double, long double> origine;
		origine.first = (x_min + x_max)/2;
		origine.second = (y_min + y_max)/2;
		
		pair<long double, long double> new_origine;
		new_origine.first = (4*origine.first + point.first)/5;
		new_origine.second = (4*origine.second + point.second)/5;
		
		x_min = (x_min)*coeff;
		x_max = (x_max)*coeff;
		
		y_min = (y_min)*coeff;
		y_max = (y_max)*coeff;
	}
	
	int disp_width = 100;
	int disp_height = 100;
	
	pair<int, int> mousePos;
};

void redraw(Allegro* allegro, float fps){
	allegro->draw_rectangle(0, 0, allegro->getDisplayWidth(), allegro->getDisplayHeight(), allegro->rgb(255, 255, 255), 1, true);
	
	Window* win = (Window*)allegro->getContext();
	
	win->setDispSize(allegro->getDisplayWidth(), allegro->getDisplayHeight()-45);
	
	win->drawAxis();
	win->drawAllFunctions();
	win->drawAllScatterPlots();
	
	win->drawMousePos();
	
	allegro->draw_rectangle(0, allegro->getDisplayHeight()-45, allegro->getDisplayWidth(), allegro->getDisplayHeight(), allegro->rgb(100, 100, 100), 1, true);
	//win->reinitFunction(3);
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
		win->zoomAt(win->getMousePoint(), coeff);
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


long double f(long double x){
	return sin(x);
}

long double sigmoide(long double x){
	return 1/(1+exp(-100*x));
}

long double f3(long double x){
	return log(x);
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

long double log_10(long double x){
	return log(x)/log(10);
}



unsigned f_index = 0;
unsigned df_index = 0;
unsigned integf_index = 0;

bool integ = true;
bool deriv = true;

void update_func(Allegro* allegro, InputBox* inptbx){
	Window* win = (Window*)allegro->getContext();
	string y = inptbx->text;
	
	try{
		win->setFunction(f_index, Function(MathExpression(y), string("y=").append(y)));
		
		win->setFunction(df_index, Function(MathExpression(y), string("y=").append(y)).derivee());
		win->hideFunction(df_index, deriv);
		
		win->setFunction(integf_index, Function(MathExpression(y), string("y=").append(y)).primitive(0.01));
		win->hideFunction(integf_index, integ);
		
	}catch(...){
		cout << "Parsing error !" << endl;
	}
}

void toggle_integ(Allegro* allegro, Button* btn){
	Window* win = (Window*)allegro->getContext();
	
	integ = !integ;
	win->hideFunction(integf_index, integ);
}

void toggle_deriv(Allegro* allegro, Button* btn){
	Window* win = (Window*)allegro->getContext();
	
	deriv = !deriv;
	win->hideFunction(df_index, deriv);
}

unsigned inpt_id = 0;
unsigned tgl_integ_id = 0;
unsigned tgl_deriv_id = 0;

void winResized(Allegro* allegro, void* context, uint16_t ev, int w, int h){
	InputBox* formula = &(allegro->getGUI()->input_boxes[inpt_id-1]);
	formula->y = allegro->getDisplayHeight()-35;
	formula->width = (int)(allegro->getDisplayWidth()*0.8);
	
	//cout << tgl_integ_id << endl;
	Button* btn = &(allegro->getGUI()->buttons[tgl_integ_id-1]);
	btn->y = allegro->getDisplayHeight()-35;
	btn->x = (int)(allegro->getDisplayWidth()*0.8)+5;
	btn->width = (int)(allegro->getDisplayWidth()*0.08);
	
	//cout << tgl_integ_id << endl;
	btn = &(allegro->getGUI()->buttons[tgl_deriv_id-1]);
	btn->y = allegro->getDisplayHeight()-35;
	btn->x = (int)(allegro->getDisplayWidth()*0.88)+5+5;
	btn->width = (int)(allegro->getDisplayWidth()*0.08);
}

int main(int argc, char **argv)
{
	Allegro allegro_obj = Allegro();
	Allegro* allegro = &allegro_obj;
	
//	Allegro test = Allegro();
//	Allegro* test_ptr = &test;
	
	
	Window window(allegro, -6, 6, -5, 5);
	window.setStep(0.01);
	
//	window.addFunction(Function(function<long double(long double)>(
//	[=](long double x){
//		return 0.5*x*x;
//		}
//	)));
//	window.addFunction(Function(&f2).primitive(0.001));
//	window.addFunction(Function(&triangle).derivee(0.001));

	//window.addFunction(Function(function<long double(long double)>([=](long double x){return x*x-1;})));
//	Function f(&log);
//	//f.selected = true;
//	window.addFunction(Function(&sigmoide, "sigmoide"));
//	window.addFunction(f.derivee(0.001));
//	window.addFunction(Function(&log_10, "log"));
//	window.addFunction(Function(&exp, "exp"));
//	window.addFunction(Function(&cos, "cos(x)"));
//	window.addFunction(Function(&cos, "cos(x)").primitive(0.01));
//	window.addFunction(Function(&cos, "cos(x)").tangente(3));
//	window.addFunction(Function(function<long double(long double)>([=](long double x){return x;})));

//	ScatterPlot test;
//	test.addPoint(0, 2);
//	test.addPoint(3, 0.5);
//	test.addPoint(1, 0.7);
//	unsigned test_index = window.addScatter(test);


	string y = "sin(x)";

	f_index = window.addFunction(Function(MathExpression(y), string("y=").append(y)));
	
	df_index = window.addFunction(Function(MathExpression(y), string("y=").append(y)).derivee());
	window.hideFunction(df_index, deriv);
	
	integf_index = window.addFunction(Function(MathExpression(y), string("y=").append(y)).primitive(0.01));
	window.hideFunction(integf_index, integ);
	
	//window.addFunction(Function(MathExpression("sin x"), "test"));
	
	allegro->setContext((void*)(&window));
	
	allegro->init();
	allegro->createWindow(30, 500, 400);
	//allegro->createWindow(30, 500, 400);
	
	allegro->setRedrawFunction(&redraw);
	allegro->bindMouseMove(&mouseMove);
	allegro->bindMouseClick(&mouseClick);
	allegro->bindKeyDown(&key);
	allegro->bindWindowResized(&winResized);
	
	
	inpt_id = allegro->getGUI()->newInputBox(y, 3, allegro->getDisplayHeight()-35, 30, (int)(allegro->getDisplayWidth()*0.8), &update_func)->id;
	//cout << inpt_id << endl;
	allegro->getGUI()->input_boxes[0].setAuthorizedChars("01234567890.+-/^*%abcdefghijklmnopqrstuvwxyz() ");
	
	tgl_integ_id = allegro->getGUI()->newBtn("Primitive", (int)(allegro->getDisplayWidth()*0.8)+5, allegro->getDisplayHeight()-35, 30, (int)(allegro->getDisplayWidth()*0.08), &toggle_integ)->id;
	tgl_deriv_id = allegro->getGUI()->newBtn("Dérivée", (int)(allegro->getDisplayWidth()*0.88)+5+5, allegro->getDisplayHeight()-35, 30, (int)(allegro->getDisplayWidth()*0.08), &toggle_deriv)->id;
	
	allegro->gameLoop();
	return 0;
}
