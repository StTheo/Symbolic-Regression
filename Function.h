#ifndef __Symbolic_Regression__Function__
#define __Symbolic_Regression__Function__

#include <iostream>
#include <vector>
#include "Globals.h"

using namespace std;

struct Function{
	double getValue(double);
	int getDepth();
	int functionType;
	double terminalValue;
	bool xAsTerminal;
	bool rChild;
	Function *p;
	Function *l;
	Function *r;
	Function();
	Function(Function const &);
	Function & operator= (Function const &);
	~Function();
	void checkNode(bool);
	string getFunction();
	void trim();
	void reduce();
	bool hasRChild();
	bool hasLChild();
	bool hasParent();
	bool isLChild();
	bool isRChild();
};

#endif
