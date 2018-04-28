#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "scatterplot.h" // Base class: ScatterPlot
#include <serial/serial.h>
#include <string>
#include <sstream>
#include <iostream>
#include <locale>
#include <thread>
#include <utility>
#include <iterator>
#include <future>

using namespace std;

class Oscilloscope : public ScatterPlot
{
public:
	serial::Serial* arduino;
	float dt = 0;
	double avg = 0;
	bool triggered = 0;
	
	// trim from start (in place)
	static inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}
public:
	Oscilloscope(string ioaddr = "/dev/ttyACM0", int baud = 115200){
		arduino = new serial::Serial(ioaddr, baud, serial::Timeout::simpleTimeout(1000));
		joinPoints = true;
	}
	
	double getPeriod(){
//		long double max = getMax().y;
//		long double min = getMin().x;
		long double target = avg;
		
		
		int n = -1;
		bool first_set = false;
		Point first = points[0];
		Point last = points[0];
		Point prec = points[0];
		Point now = points[0];
		for(unsigned i = 1; i<points.size()-1; i++){
			now = points[i];
			if((prec < target && now > target && points[i+1] > target) || (prec > target && now < target && points[i+1] < target)){
				n++;
				if(!first_set){
					first = now;
					first_set = true;
				}
				last = now;
			}
			prec = now;
		}
		
		//cout << target << endl;
		//cout << "\t n:" << n << endl;
		
		return 2*(last.x-first.x)/double(n);
	}
	
	/**
	 * @class tokens
	 * @author 
	 * @date 04/26/18
	 * @file oscilloscope.h
	 * @brief Used to parse values returned by Arduino (separate by ; and |)
	 */
	struct tokens: std::ctype<char> 
	{
		tokens(): std::ctype<char>(get_table()) {}

		static std::ctype_base::mask const* get_table()
		{
			typedef std::ctype<char> cctype;
			static const cctype::mask *const_rc= cctype::classic_table();

			static cctype::mask rc[cctype::table_size];
			std::memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

			rc[';'] = std::ctype_base::space; 
			rc['$'] = std::ctype_base::space;
			rc['^'] = std::ctype_base::space; 
			return &rc[0];
		}
	};
	
	string readUntil(serial::Serial& srl, string separator = "\n"){
		stringstream ss;
		while(ss.str().find("\n") == ss.str().npos){
			ss << srl.read(1);
		}
		return ss.str();
	}
	
	long double getAverage(){
		return avg;
	}
	
	void updatePoints(){
		if(!arduino->isOpen())
			return;
			
		
		string response;
		response = readUntil(*arduino, "^");
		
		//string response = reponse.get();
		stringstream valss;
		trim(response);
		if(response.find("$BEGIN$") != response.npos){
			try{
				stringstream ss(response);
				ss.imbue(locale(locale(), new tokens()));
				istream_iterator<string> begin(ss);
				istream_iterator<string> end;
				vector<string> vstrings(begin, end);
				vstrings.erase(vstrings.begin());
				vstrings.erase(vstrings.end());
				dt = float(stoi(vstrings[0], nullptr, 10));
				triggered = bool(stoi(vstrings[2], nullptr, 10));
				//cout << dt << endl;
				float pas = float(dt)/vstrings.size();
				points.clear();
				for(unsigned i = 3; i<vstrings.size(); i++){
					float value = float(stoi(vstrings[i], nullptr, 16));
					addPoint(i*pas, 3.3*value/4096);
				}
				
				avg = ScatterPlot::getAverage();
			} catch (...) {
				cerr << "An error happened while parsing info sent by Arduino" << endl;
			}
			
			//cout << vals[0] << endl;
			
		}
	}
	
};

#endif // OSCILLOSCOPE_H
