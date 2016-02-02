#pragma once
#include "RungeKutta.h"
#include <string>

using namespace std;

class CMathParser;
class CChartContainer;
class CModelCharts;
class CModelDescription;

/**
*	Класс служит для представления различных моделей в программе. 
*/
class CEcoModel : public CRungeKutta
{
private:
	CMathParser				**mathExpressions;			/**<массив указателей на уравнения модели	*/
	const CModelDescription	*modelDescription;			/**<указатель на описание модели			*/
	CModelCharts			*charts; 					/**<графики модели							*/

	mutable double* values;		

	int  uiBlockId;										/**<id блока элементов управлени			*/
	bool isActive;										/**<флаг: активна ли модель					*/
public:
	CEcoModel(const double* Params,const CModelDescription* ModelDescription);
	~CEcoModel();

	/**
	*	Инициализация графиков модели 
	*/
	void InitCharts();
	/**
	*	Инициализация графиков модели параметрами из Model
	*/
	void InitCharts(const CEcoModel* Model);

	/**
	*	Установка начальных значений и начального времени
	*/
	void			SetInit(const double t0,const double* Values);
	/**
	*	Установка параметров
	*/
	void			SetParam(const double* Params);	
	virtual void	GetEcvilibrium(double* epoint) const;

	/**
	*	Вычисление следующего значения 
	*/
	virtual void _fastcall 	F(const double t,const double* Y,double* FY) const;
	
	void  SetUIBlockId(const int  BlockId);
	void  SetUIState(const bool IsActive);
	const int  GetUIBlockId() const;	
	const bool GetUIState(void) const;

	/**
	*	Возвращение указателя на контейнер графиков модели 
	*/
	CModelCharts* GetCharts() const;

	/**
	*	Возвращение указателя на описание модели 
	*/
	const CModelDescription *GetModelDescription() const;
	/**
	*	Возвращение массива указателей на уравнения модели 
	*/	
	CMathParser **GetMathExpressions()  const;
};
