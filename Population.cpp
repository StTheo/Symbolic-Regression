#include "Population.h"
#include <cmath>
#include <fstream>

Population::Population(){
	individuals = new vector<FunctionTree*>;
	for (int i = 0; i < mu; i++) {
		individuals->push_back(new FunctionTree());
	}
}

Population::~Population(){
	for (int i = 0; i < individuals->size(); i++) {
		delete individuals->at(i);
	}
	individuals->clear();
	delete individuals;
}

void Population::doIteration(){
	// Parent Selection
	switch (parentSelection) {
		default:
			fitnessProportionateSelection(false);
			break;
		case 1:
			fitnessProportionateSelection(true);
			break;
		case 2:
			kTournamentSelection(false);
			break;
		case 3:
			kTournamentSelection(true);
			break;
	}
	// Child Creation
	vector<FunctionTree*> breedingPool = *individuals;
	delete individuals;
	individuals = new vector<FunctionTree*>;

	while (individuals->size()<lambda) {
		if(((randomDouble())*(mutationChoiceWeight+crossoverChoiceWeight)>mutationChoiceWeight)&&(lambda-individuals->size() > 1)){
			FunctionTree* parent1 = breedingPool[rand()%breedingPool.size()];
			FunctionTree* parent2 = breedingPool[rand()%breedingPool.size()];
			FunctionTree* child1 = new FunctionTree(*parent1);
			FunctionTree* child2 = new FunctionTree(*parent2);
			child1->crossover(child2);
			individuals->push_back(child1);
			individuals->push_back(child2);
		} else {
			FunctionTree* parent = breedingPool[rand()%breedingPool.size()];
			FunctionTree* child = new FunctionTree(*parent);
			child->mutate();
			individuals->push_back(child);
		}
	}
	for (int i = 0; i < breedingPool.size(); i++) {
		delete breedingPool[i];
	}
	// Survival Selection
	switch (survivalSelection) {
		default:
			fitnessProportionateSelection(false);
			break;
		case 1:
			fitnessProportionateSelection(true);
			break;
		case 2:
			kTournamentSelection(false);
			break;
		case 3:
			kTournamentSelection(true);
			break;
	}
}

void Population::printErrors(){
	for (int i = 0; i < individuals->size(); i++) {
		cout << individuals->at(i)->getError() << endl;
	}
}


double Population::getMaximumFitness(){
	int maxIndex = 0;
	for (int i = 1; i < mu; i++) {
		if (individuals->at(i)->getStandardFitness()>individuals->at(maxIndex)->getStandardFitness()) maxIndex = i;
	}
	return individuals->at(maxIndex)->getStandardFitness();
}

double Population::getAverageFitness(){
	double sum = 0;
	for (int i = 0; i < mu; i++) {
		sum += individuals->at(i)->getStandardFitness();
	}
	return sum/mu;
}

double Population::getFitnessStandardDeviation(){
	double standardDeviation = 0;
	double average = getAverageFitness();
	for (int individualIndex = 0; individualIndex < individuals->size(); individualIndex++) standardDeviation += pow(individuals->at(individualIndex)->getModifiedFitness()-average,2);
	standardDeviation = sqrt(standardDeviation/individuals->size());
	return standardDeviation;
}


double Population::getMinimumError(){
	int minIndex = 0;
	for (int i = 1; i < mu; i++) {
		if (individuals->at(i)->getError() < individuals->at(minIndex)->getError()) minIndex = i;
	}
	return individuals->at(minIndex)->getError();
}


double Population::getAverageError(){
	double sum = 0;
	for (int i = 0; i < mu; i++) {
		sum += individuals->at(i)->getError();
	}
	return sum/individuals->size();
}

double Population::getErrorStandardDeviation(){
	double standardDeviation = 0;
	double average = getAverageError();
	for (int individualIndex = 0; individualIndex < individuals->size(); individualIndex++) standardDeviation += pow(individuals->at(individualIndex)->getError()-average,2);
	standardDeviation = sqrt(standardDeviation/individuals->size());
	return standardDeviation;
}


FunctionTree *Population::getBestIndividual(){
	int maxIndex = 0;
	for (int i = 1; i < mu; i++) {
		if (individuals->at(i)->getStandardFitness()>individuals->at(maxIndex)->getStandardFitness()) maxIndex = i;
	}
	return individuals->at(maxIndex);
}

void Population::printAllFunctions(){
	for (int i = 0; i < individuals->size(); i++) {
		FunctionTree * currentIndividual = individuals->at(i);
		cout << currentIndividual->getError() << "\t";
		cout << currentIndividual->getFunction() << endl;
	}
}

void Population::kTournamentSelection(bool withReplacement){
	vector<FunctionTree*> inPool = *individuals;
	delete individuals;
	individuals = new vector<FunctionTree*>;
	int currentMember = 0;
	if (withReplacement) {
		while (currentMember < mu) {
			int winner = -1;
			for (int t = 0; t < k; t++) {
				int selectionIndex = rand()%inPool.size();
				if (winner==-1) winner = selectionIndex;
				else if (inPool[selectionIndex]->getModifiedFitness() > inPool[winner]->getModifiedFitness()) winner = selectionIndex;
			}
			if (winner != -1) {
				individuals->push_back(new FunctionTree(*inPool[winner]));
			}
			currentMember++;
		}
	} else {
		vector<int> remainingIndividuals;
		for (int i = 0; i < inPool.size(); i++) {
			remainingIndividuals.push_back(i);
		}
		while (currentMember < mu) {
			int winIndex = -1;
			for (int t = 0; t < k; t++) {
				int selectionIndex = rand()%remainingIndividuals.size();
				if (winIndex==-1) {
					winIndex = selectionIndex;
				} else {
					if (inPool[remainingIndividuals[selectionIndex]]->getModifiedFitness() > inPool[remainingIndividuals[winIndex]]->getModifiedFitness()) {
						winIndex = remainingIndividuals[selectionIndex];
					}
				}
			}
			individuals->push_back(new FunctionTree(*inPool[remainingIndividuals[winIndex]]));
			remainingIndividuals[winIndex] = remainingIndividuals.back();
			remainingIndividuals.pop_back();
			currentMember++;
		}
	}
//	delete individuals;
	for (int i = 0; i < inPool.size(); i++) {
		delete inPool[i];
	}
}

void Population::fitnessProportionateSelection(bool useSigmaScaling){
	vector<FunctionTree*> inPool = *individuals;
	delete individuals;
	individuals = new vector<FunctionTree*>;
	vector<double> rangeMin(1,0);
	if (useSigmaScaling) {
		double average = 0;
		for (int individualIndex = 0; individualIndex < inPool.size(); individualIndex++){
			if (inPool[individualIndex]->getModifiedFitness() == INFINITY) {
				cout << "Issue Here";
			}
			average += inPool[individualIndex]->getModifiedFitness();
		}
		average/=inPool.size();
		
		double standardDeviation = 0;
		for (int individualIndex = 0; individualIndex < inPool.size(); individualIndex++) standardDeviation += pow(inPool[individualIndex]->getModifiedFitness()-average,2);
		standardDeviation = sqrt(standardDeviation/inPool.size());
		
		vector<double> expectedValue;
		for (int individualIndex = 0; individualIndex < inPool.size(); individualIndex++) expectedValue.push_back(standardDeviation==0 ? 1 : 1+(inPool[individualIndex]->getModifiedFitness()-average)/(2*standardDeviation));
		
		for (int individualIndex = 0; individualIndex < inPool.size(); individualIndex++) {
			rangeMin.push_back(rangeMin.back()+expectedValue[individualIndex]);
		}
//		cout << "\t" << average << "\t" << standardDeviation << endl;
	} else {
		for (int individualIndex = 0; individualIndex < inPool.size(); individualIndex++) {
			rangeMin.push_back(rangeMin.back()+inPool[individualIndex]->getModifiedFitness());
		}
	}
	while (individuals->size()<inPool.size()) {
		double randomValue = randomDouble()*rangeMin.back();
		int selectionIndex = 0;
		while (randomValue>rangeMin[selectionIndex+1]) {
			selectionIndex++;
		}
//		cout << selectionIndex << endl;
		individuals->push_back(new FunctionTree(*inPool[selectionIndex]));
	}
	for (int i = 0; i < inPool.size(); i++) {
		delete inPool[i];
	}
}

int Population::phenotypicDiversity(){
	int uniqueCount = 0;
	vector<int> remaining;
	for (int i = 0; i < individuals->size(); i++) remaining.push_back(i);
	while (!remaining.empty()) {
		FunctionTree* base = individuals->at(remaining[0]);
		int i = 1;
		while (i < remaining.size()) {
			if (base->samePhenotype(individuals->at(remaining[i]))) {
				remaining[i] = remaining.back();
				remaining.pop_back();
			} else i++;
		}
		remaining[0] = remaining.back();
		remaining.pop_back();
		uniqueCount++;
	}
	return uniqueCount;
}

double Population::getAverageSize(){
	double sum = 0;
	for (int i = 0; i < individuals->size(); i++) {
		sum += individuals->at(i)->size();
	}
	return sum/((double)individuals->size());
}
