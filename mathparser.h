#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <stack>
#include <regex>
#include <map>
#include <chrono>
#include <functional>
#include <cstdlib>
#include <ctime>

using namespace std;

vector<string> resplit(const string& s, string rg_str = "\\s+"){
	cmatch cm;
	string reg_str = string("(.*?)(")+rg_str+string(")");
	string str = s+string(" ");
	unsigned a = 0;
	unsigned b = 1;
	string subs = str.substr(a, b-a);
	vector<string> elements;
	while(b <= str.length()){
		subs = str.substr(a, b-a);
		if(std::regex_match(subs.c_str(), cm, std::regex(reg_str), std::regex_constants::match_default)){
			for(unsigned i=1; i<cm.size(); i++){
				string cmi(cm[i]);
				if(!std::regex_match(cmi.c_str(), std::regex("\\s*"))){
					elements.push_back(string(cm[i]));
				}
			}
			a = b;
			b = b+1;
		} else {
			b++;
		}
	}
	return elements;
}

void print_str_vec(vector<string>& strvec){
	printf("[");
	for(unsigned i=0;i<strvec.size(); i++){
		if(i<strvec.size()-1){
			printf("\"%s\", ", strvec[i].c_str());
		} else {
			printf("\"%s\"", strvec[i].c_str());
		}
	}
	printf("]\n");
}

template<typename T> void print_vec(vector<T>& dvec){
	printf("[");
	for(unsigned i=0;i<dvec.size(); i++){
		if(i<dvec.size()-1){
			stringstream ss;
			ss << dvec[i];
			printf("\"%s\", ", ss.str().c_str());
		} else {
			stringstream ss;
			ss << dvec[i];
			printf("\"%s\"", ss.str().c_str());
		}
	}
	printf("]\n");
}

class MathExpression{
private:
	static map<string,int> operators_prec;
	static map<string,function<long double(long double)> > functions;
	static map<string,long double> constantes;
	static void init(){
		if(initialized)
			return;
			
		srand (static_cast <unsigned> (time(0)));
		// initialize operators precendece
		operators_prec["^"] = 6;
		operators_prec["/"] = 5;
		operators_prec["*"] = 5;
		operators_prec["+"] = 4;
		operators_prec["-"] = 4;
		operators_prec["%"] = 3;
		operators_prec["("] = 0;
		
		// initialize functions
		functions["sin"] = function<long double(long double)>(
			[=](long double x){
				return sin(x);
			}
		);
		functions["cos"] = function<long double(long double)>(
			[=](long double x){
				return cos(x);
			}
		);
		functions["tan"] = function<long double(long double)>(
			[=](long double x){
				return tan(x);
			}
		);
		functions["acos"] = function<long double(long double)>(
			[=](long double x){
				return acos(x);
			}
		);
		functions["asin"] = function<long double(long double)>(
			[=](long double x){
				return asin(x);
			}
		);
		functions["atan"] = function<long double(long double)>(
			[=](long double x){
				return atan(x);
			}
		);
		functions["sqrt"] = function<long double(long double)>(
			[=](long double x){
				return sqrt(x);
			}
		);
		functions["exp"] = function<long double(long double)>(
			[=](long double x){
				return exp(x);
			}
		);
		functions["ln"] = function<long double(long double)>(
			[=](long double x){
				return log(x);
			}
		);
		functions["abs"] = function<long double(long double)>(
			[=](long double x){
				return abs(x);
			}
		);
		functions["rand"] = function<long double(long double)>(
			[=](long double x){
				return static_cast<long double> (rand()) / static_cast<long double> (RAND_MAX);
			}
		);
		
		initialized = true;
	}
	static bool initialized;
public:
	MathExpression(string exprStr, bool parseNow = true){
		init();
		expr = exprStr;
		vector<string> splitted = resplit(exprStr, "[^0-9a-zPI.]|[ \\(\\)]");
		print_vec(splitted);
		if(parseNow){
			expr_stack = splitted;
			shunting_yard2();
			printf("NPI : ");
			print_vec(npi_stack);
			//printf("f(2) = %f\n", float(npi(npi_stack, 2)));
		}
	}
	
//	static bool isFloat(string str){
//		istringstream iss(str);
//		long double f;
//		iss >> noskipws >> f;
//		return iss.eof() && !iss.fail();
//	}
	
	long double calculateElementStack(long double x){
		return npi(npi_stack, x);
	}
	
	
	bool isOp(string el){
		string ops("^+*/%-");
		return ops.find(el) != ops.npos;
		//return std::regex_match(el.c_str(), std::regex("[^+*/%-]"));
	}
	
	bool isFunc(string el){
		return functions.find(el) != functions.end();
	}
	
	bool isConstante(string el){
		return constantes.find(el) != constantes.end();
	}
	
	template<typename T> T pop(vector<T>& pile){
		T r;
		if(pile.size()>0){
			r = pile.at(pile.size()-1);
			pile.pop_back();
		}
		return r;
	}
	
	long double modulo(long double a, long double b){
		return a/b - long(a/b);
	}
	
	long double npi(vector<string> pile, long double x=3){
		vector<long double> operandes;
		for(unsigned i=0; i<pile.size(); i++){
			string el = pile[i];
			if(el.compare("x") == 0){
				//printf("variable x = %f\n", float(x));
				operandes.push_back(x);
			} else if(isConstante(el)){
				operandes.push_back(constantes[el]);
			} else if(isOp(el)){
				//printf("operandes.size() = %d\n", int(operandes.size()));
				//print_vec(operandes);
				long double var2 = pop(operandes);
				long double var1 = pop(operandes);
				//printf("Operation : %f %s %f\n", float(var1), el.c_str(), float(var2));
				switch(el[0]){
					case '+':
						operandes.push_back(var1 + var2);
						break;
					case '-':
						operandes.push_back(var1 - var2);
						break;
					case '*':
						operandes.push_back(var1 * var2);
						break;
					case '/':
						operandes.push_back(var1 / var2);
						break;
					case '^':
						operandes.push_back(pow(var1, var2));
						break;
					case '%':
						operandes.push_back(modulo(var1, var2));
						break;
					default:
						operandes.push_back(var1 + var2);
				}
			} else if(isFunc(el)){
				long double var = pop(operandes);
				operandes.push_back(functions[el](var));
			} else {
				//printf("nombre : %s\n", el.c_str());
				operandes.push_back(std::stold(el));
				//operandes.push_back(2);
			}
		}
		return pop(operandes);
	}
	
	// Fonctionne
	void shunting_yard2(){
		vector<string> operators;
		vector<string> pile;
		for(unsigned i=0; i<expr_stack.size(); i++){
			string el = expr_stack[i];
			if(!el.compare("(")){
				operators.push_back("(");
			} else if(!el.compare(")")){
				while(operators.at(operators.size()-1).compare("(") != 0){
					pile.push_back(operators.at(operators.size()-1));
					operators.pop_back();
				}
				operators.pop_back();
			} else if(operators_prec.find(el) != operators_prec.end() or functions.find(el) != functions.end()){
				while(operators.size() > 0 && prec(el) <= prec(operators.at(operators.size()-1))){
					pile.push_back(operators.at(operators.size()-1));
					operators.pop_back();
				}
				operators.push_back(el);
			} else {
				pile.push_back(el);
			}
		}
		while(operators.size() > 0){
			pile.push_back(pop(operators));
		}
		npi_stack = pile;
	}
	
	int prec(string el){
		if(operators_prec.find(el) != operators_prec.end()){
			return operators_prec[el];
		} else if(functions.find(el) != functions.end()){
			return 7;
		} else {
			return 0;
		}
	}
private:
	string expr;
	vector<string> expr_stack;
	vector<string> npi_stack;
};
map<string, int> MathExpression::operators_prec;
map<string, function<long double(long double)> > MathExpression::functions;
map<string, long double> MathExpression::constantes;
bool MathExpression::initialized = false;