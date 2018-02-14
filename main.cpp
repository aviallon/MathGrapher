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

using namespace std;

const float PI = asin(1)*2;

class Function{
public:
	Function(float(*fptr)(float), string nme = ""){
		func_ptr = fptr;
		
		name = nme;
	}
	
	Function(function<float(float)> f, string nme = ""){
		object = true;
		f_obj = f;
		
		name = nme;
	}
	
	Function(const MathExpression& mexp, string nme = ""){
		object = true;
		f_obj = [=](float x){
			MathExpression m = mexp;
			return m.calculateElementStack(x);
		};
		
		name = nme;
	}
	
	Function derivee(const float set_pas = 0.05){
		function<float(float)> f;
		if(object){
			const function<float(float)> f_copy = f_obj;
			f = [=](float x){
				//const function<float(float)> f_copy = f_obj;
				float pas = set_pas;
				return (f_copy(x)-f_copy(x-pas))/(pas);
			};
		} else {
			f = [=](float x){
				float pas = set_pas;
				return (func_ptr(x)-func_ptr(x-pas))/(pas);
			};
		}
		string new_name = "df";
		if(name != ""){
			new_name = "d(" + name + ")";
		}
		return Function(f, new_name);
	}
	
	Function primitive(const float set_pas = 0.05){
		function<float(float)> f;
		if(object){
			const function<float(float)> f_copy = f_obj; 
			f = [=](float x){
				float pas = set_pas;
				float aire = 0;
				int n = abs(round((x-0)/pas));
				if( x < 0 )
					pas = -pas;
				#pragma omp parallel for reduction(+:aire)
				for(int i = 0; i <= n; i++){
					float g = i*pas;
					float d = g + pas;
//					float m = g + pas/2;
					float temp = f_copy(g)*pas + (f_copy(d)-f_copy(g))*pas/2;
					aire += temp;
				}
				

				return aire;
			};
		} else {
			f = [=](float x){
				float pas = set_pas;
				float aire = 0;
				int n = abs(round((x-0)/pas));
				if( x < 0 )
					pas = -pas;
				#pragma omp parallel for reduction(+:aire)
				for(int i = 0; i <= n; i++){
					float g = i*pas;
					float d = g + pas;
					float temp = func_ptr(g)*pas + (func_ptr(d)-func_ptr(g))*pas/2;
					
					aire += temp;
				}

				return aire;
			};
		}
		
		string new_name = "\u222Bf";
		if(name != ""){
			new_name = "\u222B(" + name + ")";
		}
		
		return Function(f, new_name);
	}
	
	Function tangente(const float xa){
		const float fprimea = this->derivee(0.01)(xa);
		const float fa = (*this)(xa);
		function<float(float)> f;
		f = [=](float x){
			return fprimea*(x-xa) + fa;
		};
		
		string new_name = "T:f";
		if(name != ""){
			new_name = "T:(" + name + ")";
		}
		
		return Function(f, new_name);
	}
	
	float operator()(float x){
		if(object)
			return f_obj(x);
		else
			return func_ptr(x);
	}

	bool initValues(float debut, float fin, float pas){
		if(debut > fin){
			return false;
		}
		if(values.size() > 0){
			values.clear();
		}
		int n = (int)((fin-debut)/pas);
		
		for(int i = 0; i <= n; i++){
			float val = 0;
			if(object)
				val = f_obj(debut + pas*i);
			else
				val = func_ptr(debut + pas*i);
			values.push_back(val);
		}
		
		x_min = debut;
		x_max = fin;
		this->pas = pas;
		
		return true;
	}
	
	unsigned getNumberOfValues(){
		return values.size();
	}
	
	float getValue(int n){
		return values[n];
	}
	
	string getName(){
		return name;
	}
	
	void setName(string n){
		name = n;
	}

	float x_min = -5, x_max = 5;
	float pas = 0.1;

	bool selected = false;
private:
	float (*func_ptr)(float);
	function<float(float)> f_obj;
	bool object = false;
	
	string name = "";
	
	vector<float> values;
};

class Window{
public:
	Window(Allegro* allegro, float x_min = -5, float x_max = 5, float y_min = -5, float y_max = 5){
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
	
	void setFunction(unsigned i, Function f){
		cout << "Changing function " << i << "..." << endl << "\tCalculating values...";
		cout.flush();
		f.initValues(x_min, x_max, pas);
		cout << " Done!" << endl;
		functions[i]=f;
	}
	
	Function getFunction(unsigned i){
		return functions[i];
	}
	
	pair<int, int> pointToPixel(float x, float y){
		pair<int, int> pixel;
		pixel.first = (x-x_min)*disp_width/(x_max - x_min);
		pixel.second = disp_height - (y-y_min)*disp_height/(y_max - y_min);
		return pixel;
	}
	
	pair<float, float> pixelToPoint(float x, float y){
		pair<float, float> point;
		point.first = (x)*(x_max-x_min)/disp_width + x_min;
		point.second = (disp_height - y)*(y_max - y_min)/disp_height + y_min;
		return point;
	}
	
	void drawFunction(unsigned i, Color color){
		Function func = functions[i];
		
		float prec_val = func.getValue(func.x_min);
		pair<int, int> pixel = pointToPixel(func.x_min, prec_val);
		int x_prec = pixel.first;
		int y_prec = pixel.second;
		
		for(unsigned i = 0; i < func.getNumberOfValues(); i++ ){
			float val = func.getValue(i);
			pixel = pointToPixel(func.x_min + i*pas, func.getValue(i));
			int x = pixel.first;
			int y = pixel.second;
			if(abs(val - prec_val) < 1000){
				allegro->draw_line(x_prec, y_prec, x, y, color.toAllegro(), 1+func.selected*2);
			}
			x_prec = x;
			y_prec = y;
			prec_val = val;
		}
		
		//pair<int, int> namePos = pointToPixel(x_min + (x_max - x_min)*0.1, func(x_min + (x_max - x_min)*0.1));
		allegro->draw_text(disp_width - 4, 5+15*i, func.getName(), color.toAllegro(), ALLEGRO_ALIGN_RIGHT);
	}
	
	void drawAllFunctions(){
		for(unsigned i = 0; i < functions.size(); i++){
			Color color = colorWheel[i%colorWheel.size()];
			drawFunction(i, color);
		}
	}
	
	void drawAxisStep(char direction, float num, float denominator){
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
		float step_pi = max((int)((x_max-x_min)/(4*PI)), 1);
		for(float i = 1; i <= x_max; i += step_x){
			drawAxisStep('x', i, 1);
		}
		
		for(float i = -PI/2; i >= x_min; i -= (PI/2)*step_pi){
			drawAxisStep('x', i, PI);
		}
		
		for(float i = 1.0; i <= y_max; i += step_y){
			drawAxisStep('y', i, 1);
		}
		
		for(float i = -1; i >= y_min; i -= step_y){
			drawAxisStep('y', i, 1);
		}
	}
	
	pair<float, float> getMousePoint(){
		return pixelToPoint(mousePos.first, mousePos.second);
	}
	
	void drawMousePos(){
		pair<float, float> mp = getMousePoint();
		
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
		
		allegro->draw_text(0, 0, x_disp.str(), allegro->black, ALLEGRO_ALIGN_LEFT, allegro->getDefaultFont(25));
		allegro->draw_text(0, 15, y_disp.str(), allegro->black, ALLEGRO_ALIGN_LEFT);
	}
	
	void setStep(float pas){
		if(pas > 0.0){
			this->pas_original = pas;
			this->pas = pas;
		} else {
			throw(1);
		}
	}
	
	float getStep(){
		return pas;
	}
	
	int getFunctionNearestPoint(pair<float, float> point){
		pair<float, int> nearestFunc;
		nearestFunc.second = -1;
		for(unsigned i = 0; i < functions.size(); i++){
			float val = functions[i](point.first);
			float e = abs(point.second - val);
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
	
	void reinitFunction(unsigned i){
		functions[i].initValues(x_min, x_max, pas);
	}
	
	void reinitAllFunctions(){
		pas = max(pas_original*abs((float)(x_max - x_min))/11, 0.001f);
		for(unsigned i = 0; i<functions.size(); i++){
			reinitFunction(i);
		}
	}
	
	void setDispSize(int w, int h){
		disp_width = w;
		disp_height = h;
	}
	
	void zoomAt(pair<float, float> point, float coeff){
		pair<float, float> origine;
		origine.first = (x_min + x_max)/2;
		origine.second = (y_min + y_max)/2;
		
		pair<float, float> new_origine;
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
protected:
	vector<Function> functions;
	
	vector<Color> colorWheel;
	
	int selectedFunction = -1;
	
	Allegro* allegro;
	
	float x_min = -5, x_max = 5, y_min = -5, y_max = 5;
	float pas = 0.1;
	float pas_original = 0.1;
};

void redraw(Allegro* allegro, float fps){
	allegro->draw_rectangle(0, 0, allegro->getDisplayWidth(), allegro->getDisplayHeight(), allegro->rgb(255, 255, 255), 1, true);
	
	Window* win = (Window*)allegro->getContext();
	
	win->setDispSize(allegro->getDisplayWidth(), allegro->getDisplayHeight()-45);
	
	win->drawAxis();
	win->drawAllFunctions();
	
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
		
		float coeff = 1;
		if(x < 0){
			coeff = 0.99;
		} else {
			coeff = 1.01;
		}
		//float coeff = min(max(0.5-(x+50)/100, 0.999), 1.001);
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


float f(float x){
	return sin(x);
}

float f2(float x){
	return x;
}

float f3(float x){
	return log(x);
}

float square(float x){
	float r = 0;
	for(int i = 1; i<=2*500; i+=2){
		r += sin(i*x)/i;
	}
	return r;
}

float triangle(float x){
	float r = 0;
	for(int i = 1; i<=2*500; i+=2){
		r += cos(i*x)/(i*i);
	}
	return r;
}

float log_10(float x){
	return log(x)/log(10);
}

float f5(float x){
	
	if(((int)x)%2==0){
		return log(abs(x)+1) + ((float)(rand() % 10))/40;
	} else {
		return sin(x);
	}
}

unsigned f_index = 0;
unsigned df_index = 0;
unsigned integf_index = 0;

void update_func(Allegro* allegro, InputBox* inptbx){
	Window* win = (Window*)allegro->getContext();
	string y = inptbx->text;
	
	try{
		win->setFunction(f_index, Function(MathExpression(y), string("y=").append(y)));
		win->setFunction(df_index, Function(MathExpression(y), string("y=").append(y)).derivee());
		win->setFunction(integf_index, Function(MathExpression(y), string("y=").append(y)).primitive());
		
	}catch(...){
		cout << "Bad !" << endl;
	}
}

void winResized(Allegro* allegro, void* context, uint16_t ev, int w, int h){
	InputBox* formula = &(allegro->getGUI()->input_boxes[0]);
	formula->y = allegro->getDisplayHeight()-35;
	formula->width = (int)(allegro->getDisplayWidth()*0.8);
}

int main(int argc, char **argv)
{
	Allegro allegro_obj = Allegro();
	Allegro* allegro = &allegro_obj;
	
//	Allegro test = Allegro();
//	Allegro* test_ptr = &test;
	
	
	Window window(allegro, -6, 6, -5, 5);
	window.setStep(0.01);
	
//	window.addFunction(Function(function<float(float)>(
//	[=](float x){
//		return 0.5*x*x;
//		}
//	)));
//	window.addFunction(Function(&f2).primitive(0.001));
//	window.addFunction(Function(&triangle).derivee(0.001));

	//window.addFunction(Function(function<float(float)>([=](float x){return x*x-1;})));
//	Function f(&log);
//	//f.selected = true;
//	window.addFunction(f);
//	window.addFunction(f.derivee(0.001));
	window.addFunction(Function(&log_10, "log"));
	window.addFunction(Function(&exp, "exp"));
//	window.addFunction(Function(&cos, "cos(x)"));
//	window.addFunction(Function(&cos, "cos(x)").primitive(0.01));
//	window.addFunction(Function(&cos, "cos(x)").tangente(3));
//	window.addFunction(Function(function<float(float)>([=](float x){return x;})));

	string y = "x^2";

	f_index = window.addFunction(Function(MathExpression(y), string("y=").append(y)));
	df_index = window.addFunction(Function(MathExpression(y), string("y=").append(y)).derivee());
	integf_index = window.addFunction(Function(MathExpression(y), string("y=").append(y)).primitive());
	
	allegro->setContext((void*)(&window));
	
	allegro->init();
	allegro->createWindow(30, 500, 400);
	//allegro->createWindow(30, 500, 400);
	
	allegro->setRedrawFunction(&redraw);
	allegro->bindMouseMove(&mouseMove);
	allegro->bindMouseClick(&mouseClick);
	allegro->bindKeyDown(&key);
	allegro->bindWindowResized(&winResized);
	
	
	allegro->getGUI()->newInputBox(y, 3, allegro->getDisplayHeight()-35, 30, (int)(allegro->getDisplayWidth()*0.8), &update_func);
	allegro->getGUI()->input_boxes[0].setAuthorizedChars("01234567890.+-/^*x() ");
	
	allegro->gameLoop();
	return 0;
}
