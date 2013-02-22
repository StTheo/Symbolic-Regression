#ifndef __Symbolic_Regression__Globals__
#define __Symbolic_Regression__Globals__

#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef pair<double, double> coordinatePair;
typedef vector<coordinatePair> dataSet;

extern string dataFileName;
extern unsigned int mu, lambda, k, maxInitialTreeDepth, maxTreeDepth, numberOfEvaluations, fragmentNumber,survivalSelection,parentSelection;
extern bool useMultiObjective;
extern dataSet referenceDataSet, original;
extern double mutationChoiceWeight, crossoverChoiceWeight,  xAsTerminalRate, numericalMutationStandardDeviation, growTerminalRate, phenotypeRoundingThreshold, parsimonyCoefficient;

extern double randomDouble();
extern void readDataSet(int argc, char **argv);

#endif