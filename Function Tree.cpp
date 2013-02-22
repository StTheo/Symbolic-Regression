#include "Function Tree.h"
#include <cmath>
#include <sstream>

FunctionTree::FunctionTree(){
	error = -1;
	rootNode = new Function;
	if ((randomDouble()) > 0.5) generateFull(rootNode, maxInitialTreeDepth);
	else generateGrow(rootNode, maxInitialTreeDepth);
	writeFunctionString();
	rootNode->reduce();
	calculateError();
}

FunctionTree::FunctionTree(FunctionTree const &original){
	rootNode = new Function(*original.rootNode);
	writeFunctionString();
	calculateError();
}

FunctionTree & FunctionTree::operator= (FunctionTree const &original){
	if (this!=&original) {
		// 1: allocate new memory and copy the elements
		Function * newRootNode = new Function(*original.rootNode);
		// 2: deallocate old memory
		delete rootNode;
		// 3: assign the new memory to the object
		rootNode = newRootNode;
		writeFunctionString();
		calculateError();
	}
	return *this;
}

FunctionTree::~FunctionTree(){
	if (rootNode!=NULL) delete rootNode;
}

double FunctionTree::getError(){
	if (error<0) {
		cout << "Error is out of bounds" << endl;
	}
	return error;
}

double FunctionTree::getStandardFitness(){
	return 1.0/(error+1);
}

double FunctionTree::getModifiedFitness(){
	return 1.0/(error+size()*parsimonyCoefficient+1);
}

void FunctionTree::checkTree(){
	rootNode->checkNode(true);
}

int FunctionTree::getTreeDepth(){
	return calculateTreeDepth(rootNode);
}

int FunctionTree::size(){
	checkTree();
	return calculateNodeCount(rootNode);
}


void FunctionTree::generateFull(Function *currentNode, int maxDepth){
	if (maxDepth > 1) {
		currentNode->l = new Function;
		currentNode->l->p = currentNode;
		currentNode->l->rChild = false;
		generateFull(currentNode->l, maxDepth-1);
		if (currentNode->functionType < 5) {
			currentNode->r = new Function;
			currentNode->r->p = currentNode;
			currentNode->r->rChild = true;
			generateFull(currentNode->r, maxDepth-1);
		}
	}
}

void FunctionTree::generateGrow(Function *currentNode, int maxDepth){
	if (maxDepth > 1) {
		currentNode->l = new Function;
		currentNode->l->p = currentNode;
		currentNode->l->rChild = false;
		if (randomDouble()>growTerminalRate) generateGrow(currentNode->l, maxDepth-1);
		if (currentNode->functionType < 5) {
			currentNode->r = new Function;
			currentNode->r->p = currentNode;
			currentNode->r->rChild = true;
			if (randomDouble()>growTerminalRate) generateGrow(currentNode->r, maxDepth-1);
		}
	}
}

bool FunctionTree::empty() const{
	return rootNode==NULL;
}

void FunctionTree::writeFunctionString(){
	functionString = "ƒ(x) = " + rootNode->getFunction();
}

void FunctionTree::calculateError(){
	error = 0;
	if (referenceDataSet.size()>0) {
		vector<pair<double, double> > graph = getGraph();
		for (int i = 0; i < referenceDataSet.size(); i++) {
			error += (graph[i].second-referenceDataSet[i].second)*(graph[i].second-referenceDataSet[i].second);
		}
		error = error * (referenceDataSet.size()-1)/((double)fragmentNumber+1)/ (double)referenceDataSet.size();
		double deriveError=0;
		for (int i = 0; i < referenceDataSet.size()-1; i++) {
			double m,n;
			if (i==0) {
				m = referenceDataSet[i+1].second-referenceDataSet[i].second;
				n = graph[i+1].second-graph[i].second;
			} else if(i == referenceDataSet.size()-1){
				m = (referenceDataSet[i].second-referenceDataSet[i-1].second);
				n = (graph[i].second-graph[i-1].second);
			} else {
				m = 0.5*(referenceDataSet[i+1].second-referenceDataSet[i-1].second);
				n = 0.5*(graph[i+1].second-graph[i-1].second);
			}
			deriveError += (m-n)*(m-n);
		}
		error = sqrt(error*error+deriveError*deriveError);
		if (error!=error) {
			error = INFINITY;
		}		
	}
}

string FunctionTree::getFunction(){
	return functionString;
}


int FunctionTree::calculateTreeDepth(Function* parent){
	if (parent!=NULL) return 1+max(calculateTreeDepth(parent->l),calculateTreeDepth(parent->r));
	else return 0;
}
int FunctionTree::calculateNodeCount(Function* parent){
	if(parent != NULL) return 1+calculateNodeCount(parent->l)+calculateNodeCount(parent->r);
	else return 0;
}

void FunctionTree::mutate(){
	int mutateIndex = rand()%size();
	Function *mutateNode = traverse(mutateIndex);
	Function *parentNode;
	parentNode = mutateNode->p;
	bool rightChild = mutateNode->rChild;
	delete mutateNode;
	mutateNode = new Function;
	if (parentNode!=NULL) {
		mutateNode->p = parentNode;
		mutateNode->rChild=rightChild;
		if (rightChild) {
			parentNode->r = mutateNode;
		} else {
			parentNode->l = mutateNode;
		}
	}
	if ((randomDouble()) > 0.5) generateFull(mutateNode, maxTreeDepth-mutateNode->getDepth());
	else generateGrow(mutateNode, maxTreeDepth-mutateNode->getDepth());
	if(mutateIndex==0) rootNode = mutateNode;
	trim();
	writeFunctionString();
	calculateError();
}

void FunctionTree::traverseRecursive(Function* parent, Function** writeNode, int* remaining){
	if (*remaining==0) {
		*writeNode = parent;
	}
	if((parent->l!=NULL)&&(*remaining > 0)){
		*remaining -= 1;
		traverseRecursive(parent->l, writeNode, remaining);
	}
	if((parent->r!=NULL)&&(*remaining > 0)){
		*remaining -= 1;
		traverseRecursive(parent->r, writeNode, remaining);
	}
}

Function *FunctionTree::traverse(int remaining){
	Function *writeNode = NULL;
	traverseRecursive(rootNode, &writeNode, &remaining);
	return writeNode;
}

void FunctionTree::crossover(FunctionTree* sibling){
	checkTree();
	sibling->checkTree();
	int recombineIndexOne = rand()%size();
	int recombineIndexTwo = rand()%sibling->size();
	Function *recombineNodeOne = traverse(recombineIndexOne);
	Function *recombineNodeTwo = sibling->traverse(recombineIndexTwo);
	Function *parentNodeOne = recombineNodeOne->p;
	Function *parentNodeTwo = recombineNodeTwo->p;
	if (recombineIndexOne>0) {
		if (recombineNodeOne->rChild) {
			parentNodeOne->r = recombineNodeTwo;
		} else {
			parentNodeOne->l = recombineNodeTwo;
		}
		recombineNodeTwo->p = parentNodeOne;
	} else {
		rootNode = recombineNodeTwo;
		recombineNodeTwo->p = NULL;
	}
	
	if (recombineIndexTwo>0) {
		if (recombineNodeTwo->rChild) {
			parentNodeTwo->r = recombineNodeOne;
		} else {
			parentNodeTwo->l = recombineNodeOne;
		}
		recombineNodeOne->p = parentNodeTwo;
	} else {
		sibling->rootNode = recombineNodeOne;
		recombineNodeOne->p = NULL;
	}
	swap(recombineNodeOne->rChild, recombineNodeTwo->rChild);

	trim();
	writeFunctionString();
	calculateError();
	
	sibling->trim();
	sibling->writeFunctionString();
	sibling->calculateError();
}

void FunctionTree::trim(){
//	if (getTreeDepth()>maxTreeDepth) {
//		rootNode->reduce();
//	}
	rootNode->trim();
	rootNode->reduce();
	calculateError();
}

bool FunctionTree::samePhenotype(FunctionTree* compare){
	bool same = true;
	for (int i = 0; i < referenceDataSet.size(); i++) if (abs(pow(rootNode->getValue(referenceDataSet[i].first)-referenceDataSet[i].second,2)-pow(compare->rootNode->getValue(referenceDataSet[i].first)-referenceDataSet[i].second,2))>phenotypeRoundingThreshold) same = false;
	return same;
}

vector<pair<double, double> > FunctionTree::getGraph(){
	vector<pair<double, double> > outGraph;
	for (int i = 0; i < referenceDataSet.size(); i++) {
		outGraph.push_back(pair<double, double> (referenceDataSet[i].first,rootNode->getValue(referenceDataSet[i].first)));
	}
	return outGraph;
}
