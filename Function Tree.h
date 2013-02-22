#ifndef __Symbolic_Regression__Function_Tree__
#define __Symbolic_Regression__Function_Tree__
#include <iostream>
#include <vector>
#include "Globals.h"
#include "Function.h"

using namespace std;

/*
 functionType Reference:
 0 = l+r
 1 = l-r
 2 = l*r
 3 = l/r
 4 = l^r
 5 = sin(l)
 6 = cos(l)
 anything else: l+r
 */

class FunctionTree {
private:
	Function* rootNode;
	string functionString;
	double error;
	void generateFull(Function*,int);
	void generateGrow(Function*,int);
	int calculateTreeDepth(Function*);
	int calculateNodeCount(Function*);
	void traverseRecursive(Function*,Function**,int*);
	Function* traverse(int);
	void calculateError();
public:
	FunctionTree();
	FunctionTree(FunctionTree const &);
	FunctionTree & operator= (FunctionTree const &);
	~FunctionTree();
	void writeFunctionString();
	double getError();
	double getStandardFitness();
	double getModifiedFitness();
	void checkTree();
	int getTreeDepth();
	int size();
	bool empty() const;
	void mutate();
	void crossover(FunctionTree*);
	string getFunction();
	void trim();
	bool samePhenotype(FunctionTree*);
	vector<pair<double, double> > getGraph();
};

#endif