#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Log.h"

#define LEX_OP_ADD		10
#define LEX_OP_SUB		11
#define LEX_OP_MUL		12
#define LEX_OP_DIV		13
#define LEX_OP_POW		14
#define LEX_OP_ASSIGN	15

#define LEX_UNDEF		0
#define LEX_NUMBER		1
#define LEX_VAR			2
#define LEX_FUNCTION	3
#define LEX_PAR_OPEN	4
#define LEX_PAR_CLOSE	5

#define LEX_MAX_LEN		128
#define MAX_LEX_CNT		128
#define MAX_VARS_CNT	128
#define MAX_FUNC_CNT	128

#define VAR_CONSTANT	0
#define VAR_VARIABLE	1
#define VAR_UNDEF		2

#define OP_PRIORITY_LOWEST -1
#define OP_PRIORITY_LOW 0
#define OP_PRIORITY_MED	1
#define OP_PRIORITY_HI	2

#define OP_ASSOC_LEFT	0
#define OP_ASSOC_RIGHT	1

using namespace std;

/**
*	Класс лексем парсера
*/
class CLexem
{
public:
	int lexemType;
	double value;
	char*  lexem;
	int    index;
	int    varType;
	bool   optimized;
public:
	CLexem(void)
	{
		lexemType = -1;
		value = 1;
		lexem = NULL;
		index = -1;
		varType = VAR_UNDEF;
		optimized = false;
	}

	CLexem(int LexemType,double Value,char* Lexem)
	{
		lexemType = LexemType;
		value = Value;
		lexem = new char[strlen(Lexem) + 1];
		strcpy(lexem,Lexem);
		index = -1;
		varType = VAR_UNDEF;
		optimized = false;
	}

	~CLexem(void)
	{
		if(lexem) delete [] lexem;
	}
};

/**
*	Класс функций парсера
*/
class CFunction
{
public:
	char*	name;
	double (*addr)(double);					/**<указатель на функцию	*/
public:
	CFunction(void)
	{		
		name  = NULL;
		addr  = NULL;
	}
	CFunction(char* Name,double (*funcAddr)(double))
	{		
		name  = new char[strlen(Name)+1];
		strcpy(name,Name);
		addr  = funcAddr;
	}
	~CFunction(void)
	{
		if(name) delete [] name;
	}
};

/**
*	Контейнер функций парсера
*/
class CFunctionList
{
public:
	CFunction* functions[MAX_FUNC_CNT];
	int	  funcCnt;
public:
	void Clear()
	{
		for(int i = 0; i < funcCnt; i++) delete functions[i];
		funcCnt = 0;
		for(int i = 0; i < MAX_FUNC_CNT; i++) functions[i] = NULL;
	}

	int GetFuncIndex(char* Name)
	{
		for(int i = 0; i < funcCnt; i++)
		{
			if(strcmp(functions[i]->name,Name) == 0)
			{
				return i;
			}
		}
		return -1;
	}

	void SetFuncPointer(char* Name,double (*Addr)(double))
	{
		CFunction* func = FindFunc(Name);
		if(func != NULL)
		{
			func->addr = Addr;
		}
	}
	void SetFuncPointer(int funcIndex,double (*Addr)(double))
	{
		if(funcIndex >= MAX_FUNC_CNT)return;

		CFunction* func = functions[funcIndex];
		if(func != NULL)
		{
			func->addr = Addr;
		}
	}
	
	CFunction* GetFuncByName(char* Name)
	{
		return FindFunc(Name);
	}
	CFunction* GetFuncByIndex(int funcIndex)
	{
		if(funcIndex < funcCnt)
		{
			return functions[funcIndex];
		}
		else
		{
			return NULL;
		}
	}

	CFunction* FindFunc(char* Name)
	{
		for(int i = 0; i < funcCnt; i++)
		{
			if(strcmp(functions[i]->name,Name) == 0)
			{
				return functions[i];
			}
		}
		return NULL;
	}

	int AddFunc(char* Name,double (*Addr)(double))
	{
		if(funcCnt == MAX_FUNC_CNT)return -1;

		CFunction* func = FindFunc(Name);
		if(func == NULL)
		{
			functions[funcCnt] = new CFunction(Name,Addr); 
			funcCnt++;
			return (funcCnt-1);
		}
		return GetFuncIndex(Name);
	}
public:
	CFunctionList(void)
	{
		for(int i = 0; i < MAX_FUNC_CNT; i++)functions[i] = NULL;
		funcCnt = 0;
	}
	~CFunctionList(void)
	{
		if(functions)
		{
			for(int i = 0; i < funcCnt; i++) delete functions[i];
			//delete [] &(functions);
		}
	}
};

/**
*	Класс переменных парсера
*/
class CVar
{
public:
	double*	value;
	char*	name;
	int		type;
private:
	bool isDeletedValue;
public:
	CVar(void)
	{
		value = NULL;
		name  = NULL;
		type  = VAR_UNDEF;
		isDeletedValue = true;
	}
	CVar(double Value,const char* Name,int Type)
	{
		value = new double;
		*value = Value;
		name  = new char[strlen(Name)+1];
		strcpy(name,Name);
		type  = Type;
		isDeletedValue = false;
	}
	~CVar(void)
	{
		if(!isDeletedValue) delete value;
		if(name) delete [] name;
	}
	void SetPtrToValue(double* ptr)
	{		
		if(!isDeletedValue)
		{
			delete value;
			isDeletedValue = true;
		}
		value = ptr;
	}
};

/**
*	Контейнер переменных парсера
*/
class CVarList
{
public:
	CVar* variables[MAX_VARS_CNT];
	int	  varCnt;
public:
	void Clear()
	{
		for(int i = 0; i < varCnt; i++) delete variables[i];
		varCnt = 0;
		for(int i = 0; i < MAX_VARS_CNT; i++) variables[i] = NULL;
	}

	int Getindex(const char* Name)
	{
		for(int i = 0; i < varCnt; i++)
		{
			if(strcmp(variables[i]->name,Name) == 0)
			{
				return i;
			}
		}
		return -1;
	}

	void SetVarValue(const char* Name,double Value)
	{
		CVar* var = FindVar(Name);
		if(var != NULL)
		{
			*(var->value) = Value;
		}
	}
	void SetVarValue(int index,double Value)
	{
		if(index >= MAX_VARS_CNT)return;

		CVar* var = variables[index];
		if(var != NULL)
		{
			*(var->value) = Value;
		}
	}

	double* GetVarPointer(int index)
	{
		if(index >= MAX_VARS_CNT)return NULL;

		CVar* var = variables[index];
		if(var != NULL)
		{
			return var->value;
		}
		else return NULL;
	}
	double* GetVarPointer(const char* Name)
	{
		CVar* var = FindVar(Name);
		if(var != NULL)
		{
			return var->value;
		}
		else return NULL;
	}

	void SetVar(const char* Name,int Type,double Value)
	{
		CVar* var = FindVar(Name);
		if(var != NULL)
		{
			var->type = Type;
			*(var->value) = Value;
		}
	}
	void SetVar(const char* Name,int Type,double* Value)
	{
		CVar* var = FindVar(Name);
		if(var != NULL)
		{
			var->type = Type;
			var->SetPtrToValue(Value);			
		}
	}
	void SetVar(int index,int Type,double Value)
	{
		if(index >= MAX_VARS_CNT)return;
		CVar* var = variables[index];
		if(var != NULL)
		{
			var->type = Type;
			*(var->value) = Value;
		}
	}
	void SetVar(int index,int Type,double *Value)
	{
		if(index >= MAX_VARS_CNT)return;
		CVar* var = variables[index];
		if(var != NULL)
		{
			var->type = Type;
			var->SetPtrToValue(Value);			
		}
	}

	void SetVarType(const char* Name,int Type)
	{
		CVar* var = FindVar(Name);
		if(var != NULL)
		{
			var->type = Type;
		}
	}
	void SetVarType(int index,int Type)
	{
		if(index >= MAX_VARS_CNT)return;
		CVar* var = variables[index];
		if(var != NULL)
		{
			var->type = Type;
		}
	}

	CVar* GetVarByName(const char* Name)
	{
		return FindVar(Name);
	}
	CVar* GetVarByIndex(int index)
	{
		if(index < varCnt)
		{
			return variables[index];
		}
		else
		{
			return NULL;
		}
	}

	CVar* FindVar(const char* Name)
	{
		for(int i = 0; i < varCnt; i++)
		{
			if(strcmp(variables[i]->name,Name) == 0)
			{
				return variables[i];
			}
		}
		return NULL;
	}

	int CreateVar(const char* Name,double Value,int Type)
	{
		if(varCnt == MAX_VARS_CNT)return -1;

		CVar* var = FindVar(Name);
		if(var == NULL)
		{
			variables[varCnt] = new CVar(Value,Name,Type); 
			varCnt++;
			return (varCnt-1);
		}
		return Getindex(Name);
	}
public:
	CVarList(void)
	{
		for(int i = 0; i < MAX_VARS_CNT; i++)variables[i] = NULL;
		varCnt = 0;
	}
	~CVarList(void)
	{
		if(variables)
		{
			for(int i = 0; i < varCnt; i++) delete variables[i];
			//delete [] &(variables);
		}
	}
};

/**
*	Парсер алгебраических выражений
*/
class CMathParser
{
private:
	double* stack;								/**<стек для вычислений	*/
	int stackTopIndex;

	CLexem** outputLexemesInRPN;				/**<массив указателей на лексемы после оптимизации в постфиксной записи	*/
	int outputLexemesCnt;

	vector<CLexem*> lexemesInRPN;				/**<вектор указателей на лексемы в постфиксной записи	*/
	vector<CLexem*> optimizedLexemesInRPN;		/**<вектор указателей на лексемы после оптимизации в постфиксной записи	*/
	vector<CLexem*> inputLexemes;				/**<вектор указателей на лексемы						*/

	bool optimized;	
public:
	CVarList variables;
	CFunctionList functions;
public:
	CMathParser(void)
	{
		optimized = false;
		stack = NULL;
		outputLexemesInRPN = NULL;
	}

	CMathParser(const char* inputStr)
	{
		optimized = false;
		stack = NULL;
		outputLexemesInRPN = NULL;

		Parse(inputStr);		 
	}

	~CMathParser(void)
	{
		vector<CLexem*>::iterator it;
		for(it = inputLexemes.begin(); it != inputLexemes.end(); it++)
		{
			delete (*it);
		}
		if(stack) delete [] stack;
		if(outputLexemesInRPN) delete [] outputLexemesInRPN;
	}
	
	bool Parse(const char* inputStr)
	{
		optimized = false;

		int inputStrLen = strlen(inputStr);
		if(inputStrLen == 0) return false;
		
		InitFunctions();

		BuildLexemes(inputStr);
		SyntaxCheck(&inputLexemes);
		SetFuncs(&inputLexemes);
		CreateVars(&inputLexemes);
		BuildRPN(&inputLexemes);

		return true;
	}
	
	CVarList* GetVars()
	{
		return &variables;
	}

	CFunctionList* GetFunctions()
	{
		return &functions;
	}

	double _fastcall Execute()
	{	
		if(!optimized) optimizeRPN(&lexemesInRPN);
		int stackTopIndex = 0;		
		
		for(int i = 0; i < outputLexemesCnt; i++) 
		{
			CLexem* currLexem = outputLexemesInRPN[i];
			int lexType = currLexem->lexemType;
			
			if(lexType == LEX_NUMBER || currLexem->varType == VAR_CONSTANT ) 
			{			
				stack[stackTopIndex++] = currLexem->value;
				continue;	
			}
			else
			if(currLexem->varType == VAR_VARIABLE ) 
			{
				double val = currLexem->value * (*(variables.variables[currLexem->index])->value);			
				stack[stackTopIndex++] = val;
				continue;	
			}
			else
			if(lexType >= 10)
			{
				double op2 = stack[--stackTopIndex];
				double op1 = stack[--stackTopIndex];

				if (lexType == LEX_OP_ADD)
				{						
					op1 = op1 + op2;				
				}else
				if(lexType == LEX_OP_SUB)
				{						
					op1 = op1 - op2;				
				}else
				if(lexType == LEX_OP_MUL)
				{
					op1 = op1 * op2;				
				}else
				if(lexType == LEX_OP_DIV)
				{						
					op1 = op1 / op2;											
				}else
				if(lexType == LEX_OP_POW)
				{		
					op1 = pow(op1,op2);										
				}
				stack[stackTopIndex++] = op1;
				continue;					
			}else
			if(lexType == LEX_FUNCTION)
			{
				double op1 = stack[--stackTopIndex];
				op1 = functions.GetFuncByIndex(currLexem->index)->addr(op1);
				stack[stackTopIndex++] = op1;
				continue;	
			}
			//else
			//if(currLexem->varType == VAR_UNDEF)
			//{				
			//	//CLog::GetInstance()->ReportError("Execute error: undef var");
			//	currLexem->varType = VAR_CONSTANT;
			//	continue;	
			//}
		}	

		if(stackTopIndex == 1)
		{
			return  stack[0];
		}
		else
		{		
			//CLog::GetInstance()->ReportError("Execute error");
			return 0;			
		}
		return 0;
	}

	string GetInputStr()
	{
		string retStr;
		vector<CLexem*>::iterator it;
		for(it = inputLexemes.begin(); it != inputLexemes.end(); it++)
		{
			CLexem* currLex = (*it);
			retStr += currLex->lexem;
		}
		return retStr;
	}
	string GetRPNStr()
	{
		string retStr;
		vector<CLexem*>::iterator it;
		for(it = lexemesInRPN.begin(); it != lexemesInRPN.end(); it++)
		{
			CLexem* currLex = (*it);
			retStr += currLex->lexem;
			retStr += " ";			
		}
		return retStr;
	}
	string GetOptimizedRPNStr()
	{
		string retStr;
		if(!optimized)
		{
			return retStr = "RPN not yet optimized";
		}
		vector<CLexem*>::iterator it;
		for(it = optimizedLexemesInRPN.begin(); it != optimizedLexemesInRPN.end(); it++)
		{
			CLexem* currLex = (*it);
			if(currLex ->lexemType == LEX_NUMBER)
			{
				char* tmp = new char[128];
				sprintf(tmp,"%4.3f",currLex->value);
				retStr += tmp;
				retStr += " ";
				delete [] tmp;
			}
			else
			{
				retStr += currLex->lexem;
				retStr += " ";
			}
		}
		return retStr;
	}

private:
	int GetOperatorPriority(int op)
	{
		switch(op)
		{
			case LEX_OP_ADD:
			return OP_PRIORITY_LOW;
			case LEX_OP_SUB:
			return OP_PRIORITY_LOW;
			case LEX_OP_DIV:
			return OP_PRIORITY_MED;
			case LEX_OP_MUL:
			return OP_PRIORITY_MED;
			case LEX_OP_POW:
			return OP_PRIORITY_HI;
			default:
			return 0;
		}
	}
	int GetOperatorAssociativity(int op)
	{
		switch(op)
		{
			case LEX_OP_ADD:
			return OP_ASSOC_LEFT;
			case LEX_OP_SUB:
			return OP_ASSOC_LEFT;
			case LEX_OP_DIV:
			return OP_ASSOC_LEFT;
			case LEX_OP_MUL:
			return OP_ASSOC_LEFT;
			case LEX_OP_POW:
			return OP_ASSOC_RIGHT;
			default:
			return 0;
		}
	}

	void InitFunctions()
	{
		functions.Clear();
		functions.AddFunc("sin",&sin);
		functions.AddFunc("cos",&cos);
		functions.AddFunc("tg",&tan);
		functions.AddFunc("ctg",&tanh);
		functions.AddFunc("arcsin",&asin);
		functions.AddFunc("arccos",&acos);
		functions.AddFunc("arctg",&atan);		
		
		functions.AddFunc("log",&log);
		functions.AddFunc("ln",&log10);
		functions.AddFunc("exp",&exp);

		functions.AddFunc("floor",&floor);
		functions.AddFunc("ceil",&ceil);
	}

	void CreateVars(vector<CLexem*>* lexemes)
	{
		variables.Clear();
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end(); it++)
		{
			CLexem* currVar = (*it);
			if(currVar->lexemType == LEX_VAR)
			{
				int varInd = variables.CreateVar(currVar->lexem,0,VAR_UNDEF);
				currVar->index = varInd;
			}
		}
	}
	void SetFuncs(vector<CLexem*>* lexemes)
	{		
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end(); it++)
		{
			CLexem* currFunc = (*it);
			if(currFunc->lexemType == LEX_FUNCTION)
			{
				int funcInd = functions.GetFuncIndex(currFunc->lexem);
				currFunc->index = funcInd;
			}
		}
	}

	void SyntaxCheck(vector<CLexem*>* lexemes)
	{
		minusCheck(lexemes);
		parenthesisCheck(lexemes);
		operatorsCheck(lexemes);
		funcsCheck(lexemes);
		varsCheck(lexemes);
	}
	
	void optimizeRPN(vector<CLexem*>* lexemes)
	{
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end(); it++)
		{
			CLexem* currLexem = (*it); 

			if(currLexem->lexemType == LEX_VAR)
			{
				int varInd = currLexem->index;
				if( varInd == -1)
				{
					//cout<<"Var. error: undef var "<<currLexem->lexem<<endl;
					CLog::GetInstance()->ReportError("Неопределенная переменная: " + string(currLexem->lexem));
				}
				else
				{
					CVar* currVar = variables.GetVarByIndex(varInd);
					if(currVar != NULL)
					{
						if(currVar->type == VAR_CONSTANT)
						{
							currLexem->value  *= *(currVar->value);
							currLexem->varType = VAR_CONSTANT;
						}
						else
						if(currVar->type == VAR_UNDEF)
						{
							//cout<<"Var. error: undef var "<<currLexem->lexem<<endl;
							CLog::GetInstance()->ReportError("Неопределен тип переменной: " + string(currLexem->lexem));
							currLexem->varType = VAR_CONSTANT;
						}
						else
						{
							currLexem->varType = VAR_VARIABLE;
						}
					}
					else
					{
						CLog::GetInstance()->ReportError("Неопределенная переменная: " + string(currLexem->lexem));
					}
				}
			}
		}
				
		optimizedLexemesInRPN.clear();
		for(it = lexemes->begin(); it != lexemes->end(); it++)
		{
			CLexem* currLexem = (*it); 
			if(currLexem->lexemType == LEX_NUMBER || (currLexem->lexemType == LEX_VAR && currLexem->varType == VAR_CONSTANT) ) 
			{
				currLexem->optimized = true;
				optimizedLexemesInRPN.push_back(currLexem);
			}
			if( currLexem->lexemType == LEX_VAR && currLexem->varType == VAR_VARIABLE ) 
			{
				currLexem->optimized = false;
				optimizedLexemesInRPN.push_back(currLexem);
			}
			if( currLexem->lexemType >= 10 )
			{
				CLexem* op2 = optimizedLexemesInRPN.back();
				optimizedLexemesInRPN.pop_back();
				CLexem* op1 = optimizedLexemesInRPN.back();
				optimizedLexemesInRPN.pop_back();

				if(op2->optimized == false)
				{
					currLexem->optimized = false;
					optimizedLexemesInRPN.push_back(op1);
					optimizedLexemesInRPN.push_back(op2);
					optimizedLexemesInRPN.push_back(currLexem);
				}
				else
				if(op1->optimized == false && currLexem->lexemType != LEX_OP_ASSIGN)
				{
					currLexem->optimized = false;
					optimizedLexemesInRPN.push_back(op1);
					optimizedLexemesInRPN.push_back(op2);
					optimizedLexemesInRPN.push_back(currLexem);
				}
				else
				{
					switch (currLexem->lexemType)
					{
						case LEX_OP_ADD:
							op1->value = op1->value + op2->value;
						break;
						case LEX_OP_SUB:
							op1->value = op1->value - op2->value;
						break;
						case LEX_OP_MUL:
							op1->value = op1->value * op2->value;
						break;
						case LEX_OP_DIV:
							if(op2->value != 0)
							{
								op1->value = op1->value / op2->value;
							}
							else
							{
								//cout<<"Optimize error: div by zero"<<currLexem->lexem<<endl;
								CLog::GetInstance()->ReportError("Деление на 0");
							}
						break;
						case LEX_OP_POW:
							op1->value = pow(op1->value,op2->value);
						break;
						case LEX_OP_ASSIGN:
							if(op1->lexemType == LEX_VAR && op1->varType == VAR_VARIABLE)
							{
								CVar* currVar = variables.GetVarByName(op1->lexem);
								*(currVar ->value) = op2->value;
								op1->value = op2->value;
							}
							else
							{
								//cout<<"Optimize error: assign to const"<<currLexem->lexem<<endl;
							}
						break;
						default:
						break;
					}
					op1->lexemType = LEX_NUMBER;					
					op1->optimized = true;
					optimizedLexemesInRPN.push_back(op1);
				}
			}
			if(currLexem->lexemType == LEX_FUNCTION)
			{
				CLexem* op1 = optimizedLexemesInRPN.back();
				optimizedLexemesInRPN.pop_back();

				if(op1->optimized == false)
				{
					currLexem->optimized = false;
					optimizedLexemesInRPN.push_back(op1);					
					optimizedLexemesInRPN.push_back(currLexem);
				}
				else
				{
					op1->value = functions.GetFuncByIndex(currLexem->index)->addr(op1->value);

					op1->lexemType = LEX_NUMBER;					
					op1->optimized = true;
					optimizedLexemesInRPN.push_back(op1);
				}
			}
		}
		
		outputLexemesCnt = optimizedLexemesInRPN.size();
		if(outputLexemesCnt == 0)
		{
			CLog::GetInstance()->ReportError("Ошбка оптимизации");
			//cout<<"Optimize error: no lexemes after optimization"<<endl;
		}

		if(outputLexemesInRPN) delete [] outputLexemesInRPN;
		outputLexemesInRPN = new CLexem*[outputLexemesCnt];
		if(stack) delete [] stack;
		stack = new double[outputLexemesCnt];
		stackTopIndex = 0;

		int ind = 0;
		for(it = optimizedLexemesInRPN.begin(); it != optimizedLexemesInRPN.end(); it++)
		{
			outputLexemesInRPN[ind] = (*it);
			ind++;
		}

		optimized = true;
	}

	void BuildRPN(vector<CLexem*>* lexemes)
	{
		vector<CLexem*> lexemesStack;
		lexemesInRPN.clear();

		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end(); it++)
		{
			CLexem* currLexem = (*it); 

			// var. or number => lexemesInRPN
			if(currLexem->lexemType == LEX_NUMBER || currLexem->lexemType == LEX_VAR )
			{
				lexemesInRPN.push_back(currLexem);
				continue;
			}
			// func. or '(' => lexemesStack
			if(currLexem->lexemType == LEX_FUNCTION || currLexem->lexemType == LEX_PAR_OPEN )
			{
				lexemesStack.push_back(currLexem);
				continue;
			}
			// ')' => ...
			if(currLexem->lexemType == LEX_PAR_CLOSE )
			{
				bool done = false;
				while( lexemesStack.size() > 0 && !done )
				{
					CLexem* currStackLexem = lexemesStack.back();
					lexemesStack.pop_back();

					if(currStackLexem->lexemType == LEX_PAR_OPEN)
					{
						if(lexemesStack.size() > 0)
						{
							if(lexemesStack.back()->lexemType == LEX_FUNCTION)
							{
								lexemesInRPN.push_back(lexemesStack.back());
								lexemesStack.pop_back();
							}
						}
						done = true;
					}
					else
					{
						lexemesInRPN.push_back(currStackLexem);
					}
				}
				if(!done)
				{
					//cout<<"Syntax RPN Error: parenthesis error"<<endl;
					CLog::GetInstance()->ReportError("Ошибка расстановки скобок");
				}

				continue;
			}
			// operator => ...
			if(currLexem->lexemType >= 10)
			{
				bool done = false;
				while( lexemesStack.size() > 0 && !done )
				{
					CLexem* currStackLexem = lexemesStack.back();
					if(currStackLexem->lexemType >= 10)
					{
						if(GetOperatorAssociativity(currLexem->lexemType) ==  OP_ASSOC_LEFT)
						{
							if(GetOperatorPriority(currLexem->lexemType) > GetOperatorPriority(currStackLexem->lexemType))
							{
								done = true;
								continue;
							}
						}
						if(GetOperatorAssociativity(currLexem->lexemType) ==  OP_ASSOC_RIGHT)
						{
							if(GetOperatorPriority(currLexem->lexemType) >= GetOperatorPriority(currStackLexem->lexemType))
							{
								done = true;
								continue;
							}
						}
					}
					if(currStackLexem->lexemType == LEX_PAR_OPEN)
					{
						done = true;
						continue;
					}
					lexemesInRPN.push_back(currStackLexem);
					lexemesStack.pop_back();
				}
				lexemesStack.push_back(currLexem);
			}
		}
		if(lexemesStack.size() > 0)
		{
			while( lexemesStack.size() > 0 )
			{
				CLexem* currStackLexem = lexemesStack.back();
				lexemesStack.pop_back();
				if(currStackLexem->lexemType >= 10)
				{
					lexemesInRPN.push_back(currStackLexem);					
				}
				else
				{
					//cout<<"Syntax RPN Error: parenthesis error end"<<endl;	
					CLog::GetInstance()->ReportError("Ошибка расстановки скобок");
				}
			}
		}
	}
	
	void varsCheck(vector<CLexem*>* lexemes)
	{		
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end();)
		{
			CLexem* currLex = (*it);
			CLexem* prevLex = (it	  == lexemes->begin())	? NULL : (*(it-1));
			CLexem* nextLex = ((it+1) == lexemes->end())		? NULL : (*(it+1));

			if(currLex->lexemType == LEX_NUMBER || currLex->lexemType == LEX_VAR)
			{
				if(prevLex != NULL)
				{
					if(prevLex->lexemType == LEX_NUMBER || prevLex->lexemType == LEX_VAR || prevLex->lexemType == LEX_PAR_CLOSE )
					{
						delete currLex;
						currLex = NULL;
						it = lexemes->erase(it);	
						//cout<<"Syntax Error: var. error"<<endl;
						CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
						continue;
					}
				}				
			}
			it++;
		}
	}

	void funcsCheck(vector<CLexem*>* lexemes)
	{
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end();)
		{
			CLexem* currLex = (*it);
			CLexem* prevLex = (it	  == lexemes->begin())	? NULL : (*(it-1));
			CLexem* nextLex = ((it+1) == lexemes->end())	? NULL : (*(it+1));

			if(currLex->lexemType == LEX_FUNCTION)
			{				
				if(nextLex == NULL)
				{
					delete currLex;
					currLex = NULL;
					lexemes->erase(it);	
					it = lexemes->end();					
					//cout<<"Syntax Error: function error"<<endl;
					CLog::GetInstance()->ReportError(string(currLex->lexem) + " - синтаксическая ошибка");
					continue;
				}
				else
				if(nextLex->lexemType == LEX_PAR_CLOSE)
				{
					delete currLex;
					currLex = NULL;
					lexemes->erase(it);	
					it = lexemes->end();					
					//cout<<"Syntax Error: function error"<<endl;
					CLog::GetInstance()->ReportError(string(currLex->lexem) + string(nextLex->lexem) + " - синтаксическая ошибка");
					continue;
				}
				else
				if(nextLex->lexemType >= 10)
				{
					delete currLex;
					currLex = NULL;
					it = lexemes->erase(it);		
					CLog::GetInstance()->ReportError(string(currLex->lexem) + string(nextLex->lexem) + " - синтаксическая ошибка");
					//cout<<"Syntax Error: function error"<<endl;
					continue;
				}
				else
				if(prevLex != NULL && (prevLex->lexemType == LEX_NUMBER || prevLex->lexemType == LEX_VAR || prevLex->lexemType == LEX_PAR_CLOSE) )
				{
					delete currLex;
					currLex = NULL;
					it = lexemes->erase(it);					
					//cout<<"Syntax Error: function error"<<endl;
					CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
					continue;
				}				
			}
			it++;
		}
	}

	void operatorsCheck(vector<CLexem*>* lexemes)
	{	
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end();)
		{
			CLexem* currLex = (*it);
			CLexem* prevLex = (it	  == lexemes->begin())	? NULL : (*(it-1));
			CLexem* nextLex = ((it+1) == lexemes->end())	? NULL : (*(it+1));

			if(currLex->lexemType >= 10)
			{
				if(prevLex == NULL)
				{
					delete currLex;
					currLex = NULL;
					it = lexemes->erase(it);	
					it = lexemes->begin();
					//cout<<"Syntax Error: operator error"<<endl;
					CLog::GetInstance()->ReportError(string(currLex->lexem) + " - синтаксическая ошибка");
					continue;
				}
				else
				if(nextLex == NULL)
				{
					delete currLex;
					currLex = NULL;
					lexemes->erase(it);	
					it = lexemes->end();					
					//cout<<"Syntax Error: operator error"<<endl;
					CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
					continue;
				}
				else
				if(nextLex->lexemType == LEX_PAR_CLOSE)
				{
					delete currLex;
					currLex = NULL;
					lexemes->erase(it);	
					it = lexemes->end();					
					//cout<<"Syntax Error: operator error"<<endl;
					CLog::GetInstance()->ReportError(string(currLex->lexem) + string(nextLex->lexem) + " - синтаксическая ошибка");
					continue;
				}
				else
				if(prevLex->lexemType >= 10 || prevLex->lexemType == LEX_PAR_OPEN || prevLex->lexemType == LEX_FUNCTION)
				{
					delete currLex;
					currLex = NULL;
					it = lexemes->erase(it);					
					//cout<<"Syntax Error: operator error"<<endl;
					CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
					continue;
				}				
			}
			it++;
		}
	}

	void parenthesisCheck(vector<CLexem*>* lexemes)
	{
		int parCnt = 0;
		vector<CLexem*>::iterator it;		
		for(it = lexemes->begin(); it != lexemes->end();)
		{
			CLexem* currLex = (*it);
			CLexem* prevLex = (it	  == lexemes->begin())	? NULL : (*(it-1));
			CLexem* nextLex = ((it+1) == lexemes->end())		? NULL : (*(it+1));

			if(currLex->lexemType == LEX_PAR_OPEN )
			{
				parCnt++;
				if(prevLex != NULL)
				{
					if(prevLex->lexemType == LEX_VAR || prevLex->lexemType == LEX_NUMBER || prevLex->lexemType == LEX_PAR_CLOSE)
					{
						//cout<<"Syntax Error: parenthesis error"<<endl;
						CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
						delete currLex;
						currLex = NULL;
						it = lexemes->erase(it);								
						parCnt--;
						continue;
					}
				}
			}
			if(currLex->lexemType == LEX_PAR_CLOSE )
			{
				parCnt--;
				if(parCnt < 0)
				{
					//cout<<"Syntax Error: parenthesis error"<<endl;
					CLog::GetInstance()->ReportError(string(currLex->lexem) + " - синтаксическая ошибка");
					delete currLex;
					currLex = NULL;
					it = lexemes->erase(it);					
					parCnt++;
					continue;
				}
				if(prevLex != NULL)
				{
					if(prevLex->lexemType >= 10)
					{
						//cout<<"Syntax Error: parenthesis error"<<endl;
						CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
						delete currLex;
						currLex = NULL;
						it = lexemes->erase(it);							
						parCnt++;
						continue;
					}
					if(prevLex->lexemType == LEX_FUNCTION)
					{
						//cout<<"Syntax Error: parenthesis error"<<endl;
						CLog::GetInstance()->ReportError(string(prevLex->lexem) + string(currLex->lexem) + " - синтаксическая ошибка");
						delete currLex;
						currLex = NULL;
						it = lexemes->erase(it);							
						parCnt++;
						continue;
					}
				}
			}
			it++;
		}
		if(parCnt != 0)
		{
			//cout<<"Syntax Error: parenthesis error"<<endl;
			CLog::GetInstance()->ReportError("Проверьте скобки");
			for(parCnt;parCnt > 0;parCnt--)lexemes->push_back(new CLexem(LEX_PAR_CLOSE,0,")"));
		}
	}

	void minusCheck(vector<CLexem*>* lexemes)
	{
		vector<CLexem*>::iterator it;
		for(it = lexemes->begin(); it != lexemes->end(); it++)
		{
			CLexem* currLex = (*it);
			CLexem* prevLex = (it	  == lexemes->begin())	? NULL : (*(it-1));
			CLexem* nextLex = ((it+1) == lexemes->end())	? NULL : (*(it+1));
			
			if(currLex->lexemType == LEX_OP_SUB)
			{
				if(prevLex == NULL)
				{
					if(nextLex == NULL)
					{
						//cout<<"Syntax Error: '-' error"<<endl;
						CLog::GetInstance()->ReportError("Проверьте расстановку знаков '-'");
						continue;
					}
					if(nextLex->lexemType == LEX_NUMBER || nextLex->lexemType == LEX_VAR || nextLex->lexemType == LEX_FUNCTION)
					{
						nextLex->value *= -1;
					}
					else 
					if(nextLex->lexemType == LEX_PAR_OPEN)
					{
						currLex->lexemType = LEX_OP_MUL;
						strcpy(currLex ->lexem,"*");
						it = lexemes->insert(it,new CLexem(LEX_NUMBER,-1,"-1"));
						continue;
					}
					else
					if(nextLex->lexemType == LEX_OP_SUB)
					{
						delete currLex;
						currLex = NULL;
						it = lexemes->erase(it);
						delete (*it);	
						it = lexemes->erase(it);
						continue;
					}
					else
					{
						//cout<<"Syntax Error: '-' error"<<endl;
						CLog::GetInstance()->ReportError("Проверьте расстановку знаков '-'");
						continue;
					}					
					delete currLex;
					currLex = NULL;
					it = lexemes->erase(it);							
				}
				else
				{
					if(nextLex == NULL)
					{
						//cout<<"Syntax Error: '-' error"<<endl;
						CLog::GetInstance()->ReportError("Проверьте расстановку знаков '-'");
						continue;
					}
					if(	(prevLex->lexemType == LEX_PAR_OPEN || prevLex->lexemType >= 10) &&
						(nextLex->lexemType == LEX_NUMBER   || nextLex->lexemType == LEX_VAR )	)
					{
						nextLex->value *= -1;
						delete currLex;
						currLex = NULL;
						it = lexemes->erase(it);						
					}
					else
					if(	(prevLex->lexemType == LEX_NUMBER || prevLex->lexemType == LEX_VAR) &&
						(nextLex->lexemType < 10 ||  nextLex->lexemType != LEX_PAR_CLOSE) )
					{
						//OK
					}
					else
					//if(	 (prevLex->lexemType >= 10) && (nextLex->lexemType == LEX_PAR_OPEN) )
					//{
					//	currLex->lexemType = LEX_OP_MUL;
					//	strcpy(currLex ->lexem,"*");
					//	it = lexemes->insert(it,new CLexem(LEX_NUMBER,-1,"-1"));
					//	continue;
					//}
					//else
					if(	(prevLex->lexemType == LEX_PAR_OPEN || prevLex->lexemType == LEX_NUMBER || prevLex->lexemType == LEX_VAR) &&
						(nextLex->lexemType == LEX_OP_SUB )	 )
					{
						//OK
					}
					else
					if(	prevLex->lexemType == LEX_PAR_CLOSE && (nextLex->lexemType == LEX_PAR_OPEN || nextLex->lexemType == LEX_NUMBER || nextLex->lexemType == LEX_VAR || nextLex->lexemType == LEX_FUNCTION ) )
					{
						//OK
					}
					else
					{
						CLog::GetInstance()->ReportError("Проверьте расстановку знаков '-'");
						//cout<<"Syntax Error: '-' error"<<endl;
						continue;
					}
				}
				
			}
		}
	}
	
	void BuildLexemes(const char* inputStr)
	{
		inputLexemes.clear();

		int inputStrLen = strlen(inputStr);
		const char* chr = inputStr;

		char* lexStr = new char[LEX_MAX_LEN];
		lexStr[0] = '\0';
		
		int currState = LEX_UNDEF;
		bool stop = false;
		while(!stop)
		{		
			if(!(*chr))
			{
				stop = true;				
			}

			if(strlen(lexStr)==0)
			{			
				if(*chr == ' ' || *chr == 13 || *chr == 10 || *chr == '\0')
				{
					chr++;		
					continue;
				}
				else
				if(	(*chr >= '0' && *chr <= '9') )
				{
					lexStr[0] = *chr;
					lexStr[1] = '\0';
					currState = LEX_NUMBER;
					chr++;	
					continue;
				}
				else
				if(	(*chr >= 'A' && *chr <= 'Z') ||	(*chr >= 'a' && *chr <= 'z')  )
				{
					lexStr[0] = *chr;
					lexStr[1] = '\0';
					currState = LEX_VAR;
					chr++;	
					continue;
				}
				else
				{
					lexStr[0] = *chr;
					lexStr[1] = '\0';
					switch (*chr)
					{
						case '(':
							currState = LEX_PAR_OPEN;
						break;
						case ')':
							currState = LEX_PAR_CLOSE;
						break;
						case '+':
							currState = LEX_OP_ADD;
						break;
						case '-':
							currState = LEX_OP_SUB;
						break;
						case '*':
							currState = LEX_OP_MUL;
						break;
						case '/':
							currState = LEX_OP_DIV;
						break;
						case '^':
							currState = LEX_OP_POW;
						break;
						//case '=':
						//	currState = LEX_OP_ASSIGN;
						//break;
						default:
							currState = LEX_UNDEF;
							CLog::GetInstance()->ReportError("Неопознанный символ:" + string(chr));
							//cout<<"Lexem error in: "<<inputStr<<" undefined simbol:"<<chr<<endl;
						break;
					}
				}		
				
				if(currState != LEX_UNDEF)
				{					
					inputLexemes.push_back(new CLexem(currState,0,lexStr));
					delete [] lexStr;
					lexStr = new char[LEX_MAX_LEN];
					lexStr[0] = '\0';
					currState = LEX_UNDEF;
				}
				else
				{
					delete [] lexStr;
					lexStr = new char[LEX_MAX_LEN];
					lexStr[0] = '\0';
					currState = LEX_UNDEF;
				}

			}
			else 
			{
				if(	(*chr >= '0' && *chr <= '9') || *chr == ',' || *chr == '.' )
				{
					if(currState != LEX_NUMBER) currState = LEX_VAR;
					int lexLen = strlen(lexStr);
					lexStr[lexLen] = *chr;		
					lexStr[lexLen+1] = '\0';		
					chr++;	
					continue;
				}
				else
				if(	(*chr >= 'A' && *chr <= 'Z') ||	(*chr >= 'a' && *chr <= 'z')  )
				{
					if(currState != LEX_VAR)
					{
						CLog::GetInstance()->ReportError("Неопознанная лексема в: " + string(inputStr));
						//cout<<"Lexem error in: "<<inputStr<<endl;
						currState = LEX_UNDEF;
					}
					else
					{
						int lexLen = strlen(lexStr);
						lexStr[lexLen] = *chr;		
						lexStr[lexLen+1] = '\0';						
						chr++;	
						continue;
					}
				}
				else
				{					
					if(lexStr[strlen(lexStr)-1] == ',' || lexStr[strlen(lexStr)-1] == '.') lexStr[strlen(lexStr)-1] = '\0';
					lexStr[strlen(lexStr)] = '\0';
							
					if(currState == LEX_VAR)
					{
						if(functions.FindFunc(lexStr) != NULL)
						{
							currState = LEX_FUNCTION;
						}
					}

					if(currState != LEX_UNDEF)
					{						
						double val = 1;
						if(currState == LEX_NUMBER)
						{
							val = atof(lexStr);
						}
						inputLexemes.push_back(new CLexem(currState,val,lexStr));
						delete [] lexStr;
						lexStr = new char[LEX_MAX_LEN];
						lexStr[0] = '\0';
						currState = LEX_UNDEF;
						continue;
					}
				}
			}
			
			chr++;
		}

		if(lexStr) delete [] lexStr;		
	}
};