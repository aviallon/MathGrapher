#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

class MathExpression{
public:
	MathExpression(stringstream expr){
		stringstream ss;
		while(expr.get(ss, " ")){
			expr_str.push_back(ss.str());
			
			// Clears the stringstream
			wstringstream temp;
			ss.swap(temp);
		}
	}

	void _d_printExpVector(){
		for(unsigned i=0; i<expr_str.size(); i++){
			cout << i << "\t: " << expr_str[i] << endl;
		}
	}
private:
	vector<string> expr_str;
}


