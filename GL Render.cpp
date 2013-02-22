#include <iostream>
#include <stdlib.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
#include <fstream>

#include "GL Render.h"
#include "Population.h"
#include "Globals.h"

dataSet secondGraph;
double minX, maxX, minY, maxY;
int eval, mainWindow, subWindow;
Population myPopulation;

bool init = true;

int window_width = 512;
int window_height = 512;


void renderScene(){
	minX = referenceDataSet[0].first;
	maxX = referenceDataSet[0].first;
	minY = referenceDataSet[0].second;
	maxY = referenceDataSet[0].second;
	for (int i = 0; i < referenceDataSet.size(); i++) {
		minX = min(minX, referenceDataSet[i].first);
		maxX = max(maxX, referenceDataSet[i].first);
		minY = min(minY, referenceDataSet[i].second);
		maxY = max(maxY, referenceDataSet[i].second);
	}
	glLoadIdentity ();
	glViewport(0, 0, 512, 512);
	if(init) glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUAD_STRIP);
	glColor4f(0.0, 0.0, 0.0, 0.1);
	glVertex2d( 1,  1);
	glVertex2d( 1, -1);
	glVertex2d(-1,  1);
	glVertex2d(-1, -1);
	glEnd();
	glScaled(2/(maxX-minX), 2/(maxY-minY), 1);
	glTranslated(-(maxX+minX)/2, -(maxY+minY)/2, 0);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glBegin(GL_TRIANGLE_STRIP);
//	glBegin(GL_LINE_STRIP);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	for (int i = 0; i < secondGraph.size(); i++) {
		glVertex2f(secondGraph[i].first, secondGraph[i].second);
		glVertex2f(referenceDataSet[i].first, referenceDataSet[i].second);
		if ((i<secondGraph.size()-1)&&(((secondGraph[i].second-referenceDataSet[i].second)*(secondGraph[i+1].second-referenceDataSet[i+1].second))<0)) {
			double x = (secondGraph[i].second-referenceDataSet[i].second)/(referenceDataSet[i+1].second-referenceDataSet[i].second-secondGraph[i+1].second+secondGraph[i].second);
			double y = x*(referenceDataSet[i+1].second-referenceDataSet[i].second)+referenceDataSet[i].second;
			glVertex2f((x*(referenceDataSet[i+1].first-referenceDataSet[i].first)+referenceDataSet[i].first),y);
			glVertex2f((x*(referenceDataSet[i+1].first-referenceDataSet[i].first)+referenceDataSet[i].first),y);
		}
	}

	glEnd();
	
	glBegin(GL_LINE_STRIP);
	glColor4f(0, 0, 0, 1.0);
	for (int i = 0; i < secondGraph.size(); i++) {
		glVertex2f(secondGraph[i].first, secondGraph[i].second);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	glColor4f(0, 0, 0, 1.0);
	for(int i = 0; i < referenceDataSet.size(); i++) {
		glVertex2f(referenceDataSet[i].first, referenceDataSet[i].second);
	}
	glEnd();
	glFlush();
}

void doIteration(){
//	glutSetWindow(mainWindow);
	init = false;
	myPopulation.doIteration();
	eval++;
	secondGraph = myPopulation.getBestIndividual()->getGraph();
//	for (int i = 0; i < secondGraph.size(); i++) {
//		secondGraph[i].second = 0;
//	}
//	secondGraph = original;
	cout << eval << "\t" << myPopulation.getAverageFitness()<< "\t" <<myPopulation.getBestIndividual()->getStandardFitness() << "\t" <<  myPopulation.getBestIndividual()->getFunction() << endl;
	glutPostRedisplay();
}

void renderInit(int argc, char **argv){
	eval = 0;
	myPopulation = *new Population();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("");
	glutDisplayFunc(renderScene);
	glutIdleFunc(doIteration);
	glutMainLoop();
}
