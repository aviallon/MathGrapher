#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <stack>

using namespace std;

const uint16_t MP_NUMBER = 1<<0;
const uint16_t MP_BLANK = 1<<1;
const uint16_t MP_XVAR = 1<<2;
const uint16_t MP_OPERATOR = 1<<3;
const uint16_t MP_BRACKET_L = 1<<4;
const uint16_t MP_BRACKET_R = 1<<5;
const uint16_t MP_OP_MINUS = 1<<6;
const uint16_t MP_OP_PLUS = 1<<7;
const uint16_t MP_OP_MUL = 1<<8;
const uint16_t MP_OP_DIV = 1<<9;
const uint16_t MP_OP_POW = 1<<10;
const uint16_t MP_OP_SIN = 1<<11;

class Element{
public:
	Element(float el, bool isVar = false){
		val = el;
		this->isVar = isVar;
	}
	
	Element(uint16_t op){
		oper = op;
	}
	
	bool isNum(){
		if(oper >= MP_OPERATOR){
			return false;
		}
		return true;
	}
	
	bool isOp(){
		if(oper >= MP_OPERATOR){
			return true;
		}
		return false;
	}
	
	string getOpRepr(){
		//cout << "[DBG] : " << (oper & MP_OP_MUL) << "\t";
//		if((oper & MP_OP_MUL) == MP_OPERATOR){
//			return "*";
//		}
		switch(oper){
			case MP_OP_MINUS:
				return "-";
				break;
			case MP_OP_PLUS:
				return "+";
				break;
			case MP_OP_DIV:
				return "/";
				break;
			case MP_OP_MUL:
				return "*";
				break;
			case MP_OP_POW:
				return "^";
				break;
			case MP_BRACKET_L:
				return "(";
				break;
			case MP_BRACKET_R:
				return ")";
		}
		return "/!\\";
	}
	
	friend ostream& operator<<(ostream &os, Element& el){
		if(el.isVar){
			os << "x";
			return os;
		}
		if(el.oper == -1){
			os << el.val;
		} else {
			os << el.getOpRepr()/* << "(" << el.oper << ")"*/;
		}
		//os << " ";
		return os;
	}
	
	float calc(float x1, float x2){
		if(!isOp()){
			cerr << "Warning ! Not an operator !!!" << endl;
			return -1;
		}
		//cout << "\t operator : " << oper << " (" << this->getOpRepr() << ")" << endl;
		//cout << "MP_OP_PLUS = " << MP_OP_PLUS << endl;
		switch(oper){
			case MP_OP_MINUS:
				return x1 - x2;
				break;
			case MP_OP_PLUS:
				return x1 + x2;
				break;
			case MP_OP_DIV:
				return x1 / x2;
				break;
			case MP_OP_MUL:
				return x1 * x2;
				break;
			case MP_OP_POW:
				return pow(x1, x2);
		}
		return x1;
	}
	
	float val;
	bool isVar = false;
	int16_t oper = -1;
};

class MathExpression{
public:
	MathExpression(string exprStr, bool parseNow = true){
		expr = exprStr;
		if(parseNow){
			parseExpr();
			shunting_yard();
		}
	}
	
	void parseExpr(){
		try{
			char cur_char = expr[0];
			string temp(1, cur_char);
			for(unsigned i = 1; i<=expr.length(); i++){
				cur_char = expr[i];
				//const bool prev_is_num = (getCharType(temp[temp.length()-1]) & getCharType(cur_char) || (getCharType(temp[temp.length()-1]) == MP_OP_MINUS ) );
				const bool nombre = getCharType(temp[temp.length()-1]) & getCharType(cur_char) == MP_NUMBER;
				//const bool nombre = (prev_is_same && getCharType(cur_char) == MP_NUMBER);
				cout << i << ", " << temp << endl;
	//			if(getCharType(cur_char) == MP_BLANK){
	//				continue;
	//			}
				//const bool symb_negatif = (getCharType(temp[temp.length()-2]) & MP_BLANK) && (getCharType(temp[temp.length()-1]) == MP_OP_MINUS) && getCharType(cur_char) & MP_NUMBER;
				if(nombre){
					temp += cur_char;
				} else {
					expr_stack.push_back(temp);
					temp.clear();
					temp += cur_char;
	//				if(i == expr.length()-1){
	//					expr_stack.push_back(temp);
	//				}
				}
			}
			expr_stack.erase(remove_if(expr_stack.begin(), expr_stack.end(), isStrBlank), expr_stack.end());
		} catch(...){
			expr_stack = vector<string>(1, "x");
			cout << "Error while parsing !" << endl;
		}
	}
	
	static bool isStrBlank(string str){
		if(str.find(" ") != str.npos){
			return true;
		}
		return false;
	}

	void _d_printExpVector(){
		cout << "Printing vector :" << endl;
		cout << expr << endl;
		for(unsigned i=0; i<expr_stack.size(); i++){
			cout << i << "\t: " << expr_stack[i] << endl;
		}
	}
	
	static bool isFloat(string str){
		istringstream iss(str);
		float f;
		iss >> noskipws >> f;
		return iss.eof() && !iss.fail();
	}
	
	float calculateElementStack(float x){
		float r = -1;
		
		stack<Element> stk;
//		stack<Element> stk2;
		for(unsigned i = 0; i<el_stack.size(); i++){
			Element el = el_stack[i];
			stk.push(el);
		}
//		stk2 = stk;
//		while(!stk2.empty()){
//			cout << stk2.top() << " ";
//			stk2.pop();
//		}
//		
//		cout << "\n";
		if(!stk.empty()){
			try{
				r = rpn(stk, x);
			} catch (...) {
				cout << "Error" << endl;
			}
		}
		
		return r;
	}
	
	float rpn(stack<Element> &stk, float x, unsigned depth = 0){
		float val1, val2, r=0;
	//	stringstream msg;
//		if(stk.empty()){
//			cout << "error" << endl;
//			throw "Error !";
//		}
		Element el = stk.top();
		stk.pop();
		//bool x_here = false;
		if(el.isOp()){
			val2 = rpn(stk, x, depth+1);
			val1 = rpn(stk, x, depth+1);
			r = el.calc(val1, val2);
		} else if(el.isVar){
			//x_here = true;
			r = x;
			//val1 = r;
		} else {
			r = el.val;
			//val1 = r;
		}
//		for(unsigned i = 0; i<depth; i++){
//			msg << "\t";
//		}
//		msg << depth << " (" << stk.size() << ") => ";
//		if(x_here){
//			msg << "x=";
//		}
//		msg << val1;
//		if(el.isOp()){
//			msg << " " << el.getOpRepr() << " " << val2;
//		}
//		msg << endl;
//		cout << msg.str();
		return r;
	}
	
	static bool hasHigherPrecedence(uint16_t op1, uint16_t op2){
		uint8_t op1Weight = getPrecedence(op1);
		uint8_t op2Weight = getPrecedence(op2);

		// If operators have equal precedence, return true if they are left associative. 
		// return false, if right associative. 
		// if operator is left-associative, left one should be given priority. 
		if(op1Weight == op2Weight)
		{
			if(getRightAssociativity(op1))
				return false;
			else
				return true;
		}
		return (op1Weight > op2Weight) ?  true : false;
	}
	
	void shunting_yard(){
		try{
			vector<Element> output_stack;
			
			stack<uint16_t> operator_stack;
			
			for(unsigned i = 0; i<expr_stack.size(); i++){
				string el = expr_stack[i];
				uint16_t el_type = getCharType(el[0]);
				if(el_type > MP_OPERATOR){
					while(!operator_stack.empty() && operator_stack.top()!=MP_BRACKET_L && hasHigherPrecedence(operator_stack.top(), el_type)){
						if(operator_stack.top() != MP_BRACKET_L && operator_stack.top() != MP_BRACKET_R){
							output_stack.push_back(Element(operator_stack.top()));
						}
						operator_stack.pop();
					}
					operator_stack.push(el_type);
				} else if(el_type == MP_NUMBER){
					float temp = stof(el);
					output_stack.push_back(Element(temp));
				} else if(el_type == MP_XVAR){
					output_stack.push_back(Element(0, true));
				} else if(el_type == MP_BRACKET_L){
					operator_stack.push(el_type);
				} else if(el_type == MP_BRACKET_R){
					while(!operator_stack.empty() && operator_stack.top() != MP_BRACKET_L){
						if(operator_stack.top() != MP_BRACKET_L && operator_stack.top() != MP_BRACKET_R){
							output_stack.push_back(Element(operator_stack.top()));
						}
						operator_stack.pop();
					}
					operator_stack.pop();
				}
			}
			
			while(!operator_stack.empty()){
				if(operator_stack.top() != MP_BRACKET_L && operator_stack.top() != MP_BRACKET_R){
					output_stack.push_back(Element(operator_stack.top()));
				}
				operator_stack.pop();
			}
			
			// Afichage de l'expression traitée
			
			for(unsigned i=0; i<expr_stack.size(); i++){
				cout << i << "\t: " << output_stack[i] << endl;
			}
			
			
			el_stack = output_stack;
		} catch(...){
			el_stack = vector<Element>(1, Element(0, true));
			cout << "Error while shunting yard" << endl;
		}
	}
	
	static uint8_t getPrecedence(uint16_t op){
		if(op == MP_OP_MINUS || op == MP_OP_PLUS){
			return 2;
		} else if(op == MP_OP_DIV || op == MP_OP_MUL){
			return 3;
		} else if(op == MP_OP_POW){
			return 4;
		} else if(op == MP_BRACKET_L || op == MP_BRACKET_R){
			return UINT8_MAX;
		}
	}
	
	static bool getRightAssociativity(uint16_t op){
		if(op == MP_OP_POW)
			return true;
		return false;
	}
	
	uint16_t getCharType(char c){
		string numbers = "0123456789.";
		string blanks = " ";
		
		if(numbers.find(c) != numbers.npos){
			return MP_NUMBER;
		}
		if(c == '-')
			return MP_OP_MINUS;
		if(c == '+')
			return MP_OP_PLUS;
		if(c == '*')
			return MP_OP_MUL;
		if(c == '/')
			return MP_OP_DIV;
		if(c == '^')
			return MP_OP_POW;
			
		if(c == '('){
			return MP_BRACKET_L;
		}
		if(c == ')'){
			return MP_BRACKET_R;
		}
		if(blanks.find(c) != blanks.npos){
			return MP_BLANK;
		}
		if(c == 'x'){
			return MP_XVAR;
		}
		
		return 0;
	}
private:
	string expr;
	vector<string> expr_stack;
	vector<Element> el_stack;
};


