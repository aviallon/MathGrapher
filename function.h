#ifndef FUNCTION_H
#define FUNCTION_H

#include "includes.h"

class Function{
	point last_primitive_val = point(0, 0);
	bool initializing = false;
public:
	Function(long double(*fptr)(long double), string nme = ""){
		func_ptr = fptr;
		
		name = nme;
	}
	
	Function(func_ld f, string nme = ""){
		object = true;
		f_obj = f;
		
		name = nme;
	}
	
	Function(const MathExpression& mexp, string nme = ""){
		object = true;
		f_obj = [=](long double x){
			MathExpression m = mexp;
			return m.calculateElementStack(x);
		};
		
		name = nme;
	}
	
	Function derivee(const long double set_pas = 0.05){
		func_ld f;
		if(object){
			const func_ld f_copy = f_obj;
			f = [=](long double x){
				//const func_ld f_copy = f_obj;
				long double pas = set_pas;
				return (f_copy(x)-f_copy(x-pas))/(pas);
			};
		} else {
			f = [=](long double x){
				long double pas = set_pas;
				return (func_ptr(x)-func_ptr(x-pas))/(pas);
			};
		}
		string new_name = "df";
		if(name != ""){
			new_name = "d(" + name + ")";
		}
		return Function(f, new_name);
	}
	
	Function primitive(const long double set_pas = 0.05){
		function<long double(long double)> f;
		long double x0 = 0;
		long double aire0 = 0;
		if(initializing){
			x0 = last_primitive_val.first;
			aire0 = last_primitive_val.second;
		}
		if(object){
			const function<long double(long double)> f_copy = f_obj; 
			f = [=](long double x){
				long double pas = set_pas;
				long double aire = aire0;
				int n = abs(round((x-x0)/pas));
				if( x < x0 )
					pas = -pas;
				#pragma omp parallel for reduction(+:aire)
				for(int i = 0; i <= n; i++){
					long double g = x0 + i*pas;
					long double d = g + pas;
//					long double m = g + pas/2;
					long double temp = f_copy(g)*pas + (f_copy(d)-f_copy(g))*pas/2;
					aire += temp;
				}

				return aire;
			};
		} else {
			f = [=](long double x){
				long double pas = set_pas;
				long double aire = aire0;
				int n = abs(round((x-x0)/pas));
				if( x < x0 )
					pas = -pas;
				#pragma omp parallel for reduction(+:aire)
				for(int i = 0; i <= n; i++){
					long double g = x0 + i*pas;
					long double d = g + pas;
					long double temp = func_ptr(g)*pas + (func_ptr(d)-func_ptr(g))*pas/2;
			
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
	
	Function tangente(const long double xa){
		const long double fprimea = this->derivee(0.01)(xa);
		const long double fa = (*this)(xa);
		func_ld f;
		f = [=](long double x){
			return fprimea*(x-xa) + fa;
		};
		
		string new_name = "T:f";
		if(name != ""){
			new_name = "T:(" + name + ")";
		}
		
		return Function(f, new_name);
	}
	
	long double operator()(long double x){
		last_primitive_val = point(0, 0);
		if(object){
			return f_obj(x);
		}else{
			return func_ptr(x);
		}
	}

	bool initValues(long double debut, long double fin, long double pas){
		if(debut > fin){
			return false;
		}
		if(values.size() > 0){
			values.clear();
		}
		int n = (int)((fin-debut)/pas);
		
		last_primitive_val = point(0, 0);
		initializing = true;
		
		for(int i = 0; i <= n; i++){
			long double val = 0;
			if(object)
				val = f_obj(debut + pas*i);
			else
				val = func_ptr(debut + pas*i);
			values.push_back(val);
			last_primitive_val.first = debut + pas*i;
			last_primitive_val.second = val;
		}
		initializing = false;
		
		x_min = debut;
		x_max = fin;
		this->pas = pas;
		
		initialized = true;
		
		return true;
	}
	
	unsigned getNumberOfValues(){
		return values.size();
	}
	
	long double getValue(int n){
		return values[n];
	}
	
	string getName(){
		return name;
	}
	
	void setName(string n){
		name = n;
	}

	long double x_min = -5, x_max = 5;
	long double pas = 0.1;

	bool selected = false;
	bool hidden = false;
	bool initialized = false;
private:
	long double (*func_ptr)(long double);
	func_ld f_obj;
	bool object = false;
	
	string name = "";
	
	vector<long double> values;
};

#endif