#include "Function.h"
#include <cmath>
#include <sstream>

Function::Function(){
	functionType = rand()%8;
	double u1 = randomDouble();
	double u2 = randomDouble();
	terminalValue = numericalMutationStandardDeviation*sqrt(log(1.0/(u1*u1)))*cos(2*3.14159265359*u2);
	xAsTerminal = (randomDouble()) < xAsTerminalRate;
	p = NULL;
	l = NULL;
	r = NULL;
	rChild = false;
}

Function::Function(Function const &original){
	functionType=original.functionType;
	terminalValue=original.terminalValue;
	xAsTerminal=original.xAsTerminal;
	p = NULL;
	rChild = original.rChild;
	if (original.l!=NULL) {
		l = new Function(*original.l);
		l->p = this;
	} else {
		l = NULL;
	}
	if (original.r!=NULL) {
		r = new Function(*original.r);
		r->p = this;
	} else {
		r = NULL;
	}
}

Function & Function::operator= (Function const &original){
	if (this!=&original) {
		// 1: allocate new memory and copy the elements
		Function * newLeftNode = new Function(*original.l);
		Function * newRightNode = new Function(*original.r);
		// 2: deallocate old memory
		delete p;
		delete l;
		delete r;
		// 3: assign the new memory to the object
		p = NULL;
		l = newLeftNode;
		r = newRightNode;
		functionType = original.functionType;
		terminalValue = original.terminalValue;
		xAsTerminal = original.xAsTerminal;
		rChild = original.rChild;
	}
	return *this;
}

Function::~Function(){
	if (hasParent()) {
		if (rChild) p->r = NULL;
		else p->l = NULL;
		p = NULL;
	}
	if (hasLChild()) {
		l->p = NULL;
		delete l;
	}
	if (hasRChild()){
		r->p = NULL;
		delete r;
	}
}

double Function::getValue(double x){
	if (hasLChild()) {
		double operandOne = l->getValue(x);
		double operandTwo = hasRChild() ? r->getValue(x) : 0;
		switch (functionType) {
			default:
				return operandOne + operandTwo;
				break;
			case 1:
				return operandOne - operandTwo;
				break;
			case 2:
				return operandOne * operandTwo;
				break;
			case 3:
				return operandOne / operandTwo;
				break;
			case 4:
				return pow(operandOne, operandTwo);
				break;
			case 5:
				return sin(2*M_PI*operandOne);
				break;
			case 6:
				return cos(2*M_PI*operandOne);
				break;
			case 7:
				return log(operandOne);
				break;
		}
	} else return xAsTerminal?x:terminalValue;
}

int Function::getDepth(){
	if (hasParent()) {
		return p->getDepth()+1;
	} else {
		return 1;
	}
}

string Function::getFunction(){
	string function;
//	if (hasParent()) functionStream << "(";
	if(hasLChild()){
		switch (functionType) {
			default:
				function = l->getFunction() + " + " + r->getFunction();
				break;
			case 1:
				function = l->getFunction() + " - " + r->getFunction();
				break;
			case 2:
				function = l->getFunction() + " * " + r->getFunction();
				break;
			case 3:
				function = l->getFunction() + " / " + r->getFunction();
				break;
			case 4:
				function = l->getFunction() + " ^ " + r->getFunction();
				break;
			case 5:
				function = "sin(2π" + l->getFunction() + ")";
				break;
			case 6:
				function = "cos(2π" + l->getFunction() + ")";
				break;
			case 7:
				function = "ln(" + l->getFunction() + ")";
				break;
		}
		if (hasParent()) function = "(" + function + ")";
	} else {
		if (xAsTerminal) {
			function = "x";
		} else {
			stringstream terminalString;
			terminalString << terminalValue;
			function = terminalString.str();
		}
	}
	return function;
}

void Function::trim(){
	if (hasLChild()) l->trim();
	if (hasRChild()) r->trim();
	terminalValue = round(terminalValue*100)/100;
	if (getDepth()>maxTreeDepth) {
		delete this;
	}
}

void Function::reduce(){
	if (hasLChild()) l->reduce();
	if (hasRChild()) r->reduce();
	// Simplify "<constant> <operator> <constant>"
	if (hasLChild()) {
		double operandOne = l->getValue(0);
		double operandTwo = r != NULL ? r->getValue(0) : 0;
		if ((l->l == NULL)&&!(l->xAsTerminal)) {
			if (functionType<5) {
				if ((hasRChild())&&(r->l==NULL)&&(!(r->xAsTerminal))) {
					xAsTerminal = false;
					switch (functionType) {
						default:
							terminalValue = operandOne + operandTwo;
							break;
						case 1:
							terminalValue = operandOne - operandTwo;
							break;
						case 2:
							terminalValue = operandOne * operandTwo;
							break;
						case 3:
							terminalValue = operandOne / operandTwo;
							break;
						case 4:
							terminalValue = pow(operandOne, operandTwo);
							break;
					}
					delete l;
					delete r;
				}
			} else {
				xAsTerminal = false;
				switch (functionType) {
					case 5:
						terminalValue = sin(2*M_PI*operandOne);
						break;
					case 6:
						terminalValue = cos(2*M_PI*operandOne);
						break;
					case 7:
						terminalValue = log(operandOne);
						break;
				}
				delete l;
			}
		}
	}
	
	//	(c (+,*) (ƒ,x)) = ((ƒ,x) (+,*) c)
	if (((functionType==0)||(functionType==2))&&(hasLChild())&&(hasRChild())&&(l->l == NULL)&&(!(l->xAsTerminal))&&(((r->l == NULL)&&(r->xAsTerminal))||(r->l != NULL))) {
		swap(l, r);
		l->rChild = false;
		r->rChild = true;
	}
	
	//	(c - (ƒ,x)) = ((-1 * (ƒ,x)) + c)
	if ((functionType==1)&&(hasLChild())&&(hasRChild())&&(l->l == NULL)&&(!(l->xAsTerminal))&&(((r->l == NULL)&&(r->xAsTerminal))||(r->l != NULL))) {
		swap(l, r);
		l->rChild = false;
		r->rChild = true;
		functionType = 0;
		Function * temp = l;
		l = new Function;
		l->l = temp;
		l->l->rChild = false;
		l->l->p = l;
		l->r = new Function;
		l->r->rChild = true;
		l->r->p = l;
		l->r->xAsTerminal = false;
		l->r->terminalValue = -1;
		l->functionType = 2;
		l->reduce();
		l->p = this;
		// (ƒ,x) + c
	}
	
	
	//	((ƒ,x) - c) = ((ƒ,x) + (-c))
	//	((ƒ,x) / c) = ((ƒ,x) * (1/c))
	if ((hasRChild())&&(r->l==NULL)&&(!(r->xAsTerminal))) {
		if (functionType==1) {
			functionType = 0;
			r->terminalValue *= -1;
		} else if (functionType==3) {
			functionType = 2;
			r->terminalValue = 1.0/(r->terminalValue);
		}
		
	}
	
	if ((hasLChild())&&(hasRChild())) {
		// ((ƒ,x)+c1) * c2
		if ((functionType==2)&&(l->functionType==0)&&(l->hasRChild())&&(l->r->l==NULL)&&(!(l->r->xAsTerminal))&&(!(r->hasLChild()))) {
			double c1 = l->r->terminalValue;
			double c2 = r->terminalValue;
			l->functionType=2;
			l->r->terminalValue=c2;
			terminalValue=0;
			r->terminalValue = c1*c2;
		}
		
		//	((ƒ,x) + c1) + c2 = (c2 + c1) + (ƒ,x)
		//	((ƒ,x) * c1) * c2 = (c2 * c1) * (ƒ,x)
		if (((functionType==0)||(functionType==2))&&(l->functionType==functionType)&&(l->hasRChild())&&(!(r->hasLChild()))&&(l->r->l==NULL)&&(!(l->r->xAsTerminal))) {
			if (functionType==0) {
				l->r->terminalValue+=r->terminalValue;
				r->terminalValue = 0;
			} else {
				l->r->terminalValue*=r->terminalValue;
				r->terminalValue = 1;
			}
		}
		
		//	((ƒ1,x) + c1) (+,-) ((ƒ2,x)  + c2) = ((ƒ1,x) (+,-) (ƒ2,x)) + (c1 (+,-) c2)
		if ((l->functionType==0)&&(r->functionType==0)&&((functionType==0)||(functionType==1))&&(r->hasRChild())&&(r->r->l==NULL)&&(!(r->r->xAsTerminal))&&(l->hasRChild())&&(l->r->l==NULL)&&(!(l->r->xAsTerminal))) {
			swap(l->r,r->l);
			l->r->rChild = true;
			l->r->p = l;
			r->l->rChild = false;
			r->l->p = r;
			r->functionType = functionType;
			l->functionType = functionType;
			functionType = 0;
			r->reduce();
			l->reduce();
		}
		//	((ƒ1,x) * c1) (*,/) ((ƒ2,x) * c2) = ((ƒ1,x) (*,/) (ƒ2,x)) * (c1 (*,/) c2)
		if ((l->functionType==2)&&(r->functionType==2)&&((functionType==2)||(functionType==3))&&(r->hasRChild())&&(r->r->l==NULL)&&(!(r->r->xAsTerminal))&&(l->hasRChild())&&(l->r->l==NULL)&&(!(l->r->xAsTerminal))) {
			swap(l->r,r->l);
			l->r->rChild = true;
			l->r->p = l;
			r->l->rChild = false;
			r->l->p = r;
			r->functionType = functionType;
			l->functionType = functionType;
			functionType = 2;
			r->reduce();
			l->reduce();
		}
		//	(x  * c1) (+,-) (x  * c2) = (x (+,-) 0) * (c1 (+,-) c2)
		if ((l->functionType==2)&&(r->functionType==2)&&((functionType==0)||(functionType==1))&&(r->hasRChild())&&(r->r->l==NULL)&&(!(r->r->xAsTerminal))&&(l->hasRChild())&&(l->r->l==NULL)&&(!(l->r->xAsTerminal))&&(r->hasLChild())&&(r->l->l==NULL)&&(r->l->xAsTerminal)&&(l->hasLChild())&&(l->l->l==NULL)&&(l->l->xAsTerminal)) {
			r->l->terminalValue = l->r->terminalValue;
			r->l->xAsTerminal = false;
			l->r->terminalValue = 0;
			l->r->xAsTerminal = false;
			r->functionType = functionType;
			l->functionType = functionType;
			functionType = 2;
			r->reduce();
			l->reduce();
		}
	}
	
	// Simplify "x <operator> x"
	if ((hasLChild())&&(l->l==NULL)&&(l->xAsTerminal)&&(hasRChild())&&(r->l==NULL)&&(r->xAsTerminal)&&(functionType<4)) {
		switch (functionType) {
			default:					// x+x
				l->xAsTerminal = false;	// c+x
				l->terminalValue = 2;	// 2+x
				functionType = 2;		// 2*x
				break;
			case 1:						// x-x
				xAsTerminal=false;		// x-x
				terminalValue = 0;		// x-x
				delete l;				// 0
				delete r;				// 0
				break;
			case 2:						// x*x
				r->xAsTerminal = false;	// x*c
				r->terminalValue = 2;	// x*2
				functionType = 4;		// x^2
				break;
			case 3:						// x/x
				xAsTerminal=false;		// x/x
				terminalValue = 1;		// x/x
				delete l;				// 1
				delete r;				// 1
				break;
		}
	}
	
	// Simplify NaN
	
	if (hasLChild()) {
		
	}
	
	// Simplify 0s and 1s
	if (hasLChild()) {	// Implies current node is not a terminal
		bool lConst = (l->l==NULL)&&(!(l->xAsTerminal));
		bool rConst = (hasRChild())&&(r->l==NULL)&&(!(r->xAsTerminal));
		if (((!lConst)&&rConst)||((lConst)&&!rConst)) {
			switch (functionType) {
				default:
					if (lConst && (l->terminalValue == 0)) {
						delete l;
					} else if(rConst && (r->terminalValue == 0)){
						delete r;
						functionType = -1;
					}
					break;
				case 1:
					if (rConst && (r->terminalValue == 0)) {
						delete r;
						functionType = -1;
					}
					break;
				case 2:
					if (lConst) {
						if (l->terminalValue == 0) {
							delete l;
							delete r;
							terminalValue = 0;
							xAsTerminal = false;
						} else if (l->terminalValue == 1) {
							delete l;
						}
					} else if (rConst){
						if (r->terminalValue == 0) {
							delete l;
							delete r;
							terminalValue = 0;
							xAsTerminal = false;
						} else if (r->terminalValue == 1) {
							delete r;
							functionType = -1;
						}
					}
					break;
				case 3:
					if (rConst) {
						if (r->terminalValue==0) {
							delete l;
							delete r;
							terminalValue = NAN;
							xAsTerminal = false;
						} else if(r->terminalValue==1){
							delete r;
							functionType = -1;
						}
					}
					break;
				case 4:
					if (lConst && (l->terminalValue == 1)) {
						delete l;
						delete r;
						terminalValue = 1;
						xAsTerminal = false;
					} else if(rConst){
						if (r->terminalValue == 0) {
							delete l;
							delete r;
							terminalValue = 1;
							xAsTerminal = false;
						} else if(r->terminalValue == 1){
							delete r;
							functionType = -1;
						}
					}
					break;
				case 5:
					if (lConst && (l->terminalValue == 0)) {
						delete l;
						delete r;
						terminalValue = 0;
						xAsTerminal = false;
					}
					break;
				case 6:
					if (lConst && (l->terminalValue == 1)) {
						delete l;
						delete r;
						terminalValue = 0;
						xAsTerminal = false;
					}
					break;
			}
		}
	}
	
	// Assume child identity if needed
	if ((l==NULL)&&(hasRChild())) {	// Assume identity of right child
		Function *temp = r;
		if (temp->hasLChild()) {
			l = temp->l;
			l->p = this;
		} else {
			l = NULL;
		}
		if (temp->hasRChild()) {
			r = temp->r;
			r->p = this;
		} else {
			r = NULL;
		}
		functionType = temp->functionType;
		terminalValue = temp->terminalValue;
		xAsTerminal = temp-> xAsTerminal;
		temp->p = NULL;	// <
		temp->r = NULL;	// < Contain destructor recursion
		temp->l = NULL;	// <
		delete temp;
	}
	if (functionType== -1) { // Assume identity of left child
		Function *temp = l;
		if (temp->hasLChild()) {
			l = temp->l;
			l->p = this;
		} else {
			l = NULL;
		}
		
		if (temp->hasRChild()) {
			r = temp->r;
			r->p = this;
		} else {
			r = NULL;
		}
		functionType = temp->functionType;
		terminalValue = temp->terminalValue;
		xAsTerminal = temp-> xAsTerminal;
		temp->p = NULL;	// <
		temp->r = NULL;	// < Contain destructor recursion
		temp->l = NULL;	// <
		delete temp;
	}
}

void Function::checkNode(bool isRootNode){
	try {
		if (isRootNode) {
			if (hasParent()) throw 0;
		} else {
			if (!hasParent()) throw 1;
		}
	} catch (int e) {
		if (e==0) cout << "\tERROR: Root node has a parent\n";
		if (e==1) cout << "\tERROR: Node " << this << " has no parent\n";
	}
	try {
		if ((this->hasLChild())&&(this->l->p != this)) throw 0;
	} catch (...){
		cout << "\tERROR: Node " << this << " has a LEFT child " << this->l << " with incorrect parent "<< this->l->p << "\n";
	}
	try {
		if ((this->hasRChild())&&(this->r->p != this)) throw 1;
	} catch (...) {
		cout << "\tERROR: Node " << this << " has a RIGHT child " << this->r << " with incorrect parent "<< this->r->p << "\n";
	}
	if (hasLChild()) this->l->checkNode(false);
	if (hasRChild()) this->r->checkNode(false);
}

bool Function::hasRChild(){
	return r!=NULL;
}

bool Function::hasLChild(){
	return l!=NULL;
}

bool Function::hasParent(){
	return p!=NULL;
}

bool Function::isLChild(){
	return hasParent()&&(p->hasLChild())&&(p->l==this);
}

bool Function::isRChild(){
	return hasParent()&&(p->hasRChild())&&(p->r==this);
}
