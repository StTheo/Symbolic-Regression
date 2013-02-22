#include "Globals.h"
#include <fstream>

string dataFileName = "Default.txt";

unsigned int mu = 1000;
unsigned int lambda = 2*mu;
unsigned int k = 10;
unsigned int maxInitialTreeDepth = 4;
unsigned int maxTreeDepth = 10;
unsigned int numberOfEvaluations = 10000;
unsigned int fragmentNumber = 10;
unsigned int survivalSelection=2;
unsigned int parentSelection=1;

double mutationChoiceWeight = 0.05;
double crossoverChoiceWeight = 0.95;
double numericalMutationStandardDeviation = 100;
double xAsTerminalRate = 0.1;
double growTerminalRate = 0.2;
double phenotypeRoundingThreshold = 0.01;
double parsimonyCoefficient = 5;

dataSet referenceDataSet;
dataSet original;
double cubicInterpolation(double p0, double p1,double m0,double m1, double t){
	return (2*t*t*t-3*t*t+1)*p0+(t*t*t-2*t*t+t)*m0+(-2*t*t*t+3*t*t)*p1+(t*t*t-t*t)*m1;
}

double randomDouble(){
	return rand() / ((double)RAND_MAX+1);
}

void readDataSet(int argc, char **argv){
	if (argc>1) dataFileName.assign(argv[1]);
	ifstream dataFile;
	dataFile.open(dataFileName.c_str());
	string line;
	int fileSection=0;
	if (dataFile.is_open()) {
		while (dataFile.good()) {
			getline(dataFile,line);
			if (fileSection==1) {
				fileSection=0;
			}
			if (fileSection==2) {
				size_t pos = line.find(",");
				if (pos!=string::npos) {
					referenceDataSet.push_back(pair<double, double> (atof(line.substr(0,line.find(",")).c_str()),atof(line.substr(line.find(",")+1).c_str())));
				}
			}
			if (line.compare("Number of Cartesian coordinate pairs:")==0) {
				fileSection=1;
			}
			if (line.compare("Cartesian coordinate pairs:")==0) {
				fileSection=2;
			}
		}
	} else {
		cout << "Data file not opened\n";
	}
	dataSet multi;
	multi.push_back(pair<double, double> (referenceDataSet[0].first,referenceDataSet[0].second));
	original.push_back(pair<double, double> (referenceDataSet[0].first,referenceDataSet[0].second));
	for (int i = 0; i < referenceDataSet.size()-1; i++) {
		double p0 = referenceDataSet[i].second;
		double p1 = referenceDataSet[i+1].second;
		double m0;
		double m1;
		if (i==0) {
			m0 = referenceDataSet[i+1].second-referenceDataSet[i].second;
			m1 = 0.5*(referenceDataSet[i+2].second-referenceDataSet[i].second);
		} else if(i == referenceDataSet.size()-2){
			m0 = 0.5*(referenceDataSet[i+1].second-referenceDataSet[i-1].second);
			m1 = (referenceDataSet[i+1].second-referenceDataSet[i].second);
		} else {
			m0 = 0.5*(referenceDataSet[i+1].second-referenceDataSet[i-1].second);
			m1 = 0.5*(referenceDataSet[i+2].second-referenceDataSet[i].second);
		}
		for (int j = 1; j < fragmentNumber+1; j++) {
			double x = referenceDataSet[i].first+j*(referenceDataSet[i+1].first-referenceDataSet[i].first)/((double)fragmentNumber+1);
			double yo = referenceDataSet[i].second+j*(referenceDataSet[i+1].second-referenceDataSet[i].second)/((double)fragmentNumber+1);
			double y = cubicInterpolation(p0,p1,m0,m1,j/((double)fragmentNumber+1));
			multi.push_back(pair<double, double>(x,y));
			original.push_back(pair<double, double>(x,yo));
		}
		multi.push_back(pair<double, double> (referenceDataSet[i+1].first,referenceDataSet[i+1].second));
		original.push_back(pair<double, double>(referenceDataSet[i+1].first,referenceDataSet[i+1].second));
	}
	referenceDataSet = multi;
//	for (int i = 0; i < referenceDataSet.size(); i++) {
//		cout << referenceDataSet[i].first << "\t" << referenceDataSet[i].second << endl;
//	}
}
