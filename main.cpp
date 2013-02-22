#include "GL Render.h"
#include "Function Tree.h"
int main(int argc, char **argv){
//	cout << "Dataset Choice:\n\t";
//	int datasetChoice=0;
//	cin >> datasetChoice;
//	switch (datasetChoice) {
//		default:
//			dataFileName="a2datasetE.cfg";
//			break;
//		case 1:
//			dataFileName="a2dataset1.cfg";
//			break;
//		case 2:
//			dataFileName="a2dataset2.cfg";
//			break;
//	}
	bool continueAlgorithm = true;
	readDataSet(argc, argv);
	if (!continueAlgorithm){
		cout << "Algorithm cannot run without configuration file. Exiting program.\n";
		return 0;
	}
	renderInit(argc, argv);
}
