#ifndef __Symbolic_Regression__Operation__
#define __Symbolic_Regression__Operation__

#include <iostream>
using namespace std;

class Operation {
public:
	double getValue();
	string getFunction();
};

string addition(string, string);
double addition(double, double);


#endif