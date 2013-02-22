#ifndef __Symbolic_Regression__Population__
#define __Symbolic_Regression__Population__

#include "Function Tree.h"
#include "Globals.h"

class Population {
	vector<FunctionTree*> *individuals;
	void kTournamentSelection(bool);
	void fitnessProportionateSelection(bool);
public:
	Population();
	~Population();
	void doIteration();
	void printErrors();
	double getMaximumFitness();
	double getAverageFitness();
	double getFitnessStandardDeviation();
	double getMinimumError();
	double getAverageError();
	double getErrorStandardDeviation();
	double getAverageSize();
	FunctionTree *getBestIndividual();
	void printAllFunctions();
	int phenotypicDiversity();
};

#endif
