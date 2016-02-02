#pragma once
#include "RungeKutta.h"
#include <string>

using namespace std;

class CMathParser;
class CChartContainer;
class CModelCharts;
class CModelDescription;

/**
*	����� ������ ��� ������������� ��������� ������� � ���������. 
*/
class CEcoModel : public CRungeKutta
{
private:
	CMathParser				**mathExpressions;			/**<������ ���������� �� ��������� ������	*/
	const CModelDescription	*modelDescription;			/**<��������� �� �������� ������			*/
	CModelCharts			*charts; 					/**<������� ������							*/

	mutable double* values;		

	int  uiBlockId;										/**<id ����� ��������� ���������			*/
	bool isActive;										/**<����: ������� �� ������					*/
public:
	CEcoModel(const double* Params,const CModelDescription* ModelDescription);
	~CEcoModel();

	/**
	*	������������� �������� ������ 
	*/
	void InitCharts();
	/**
	*	������������� �������� ������ ����������� �� Model
	*/
	void InitCharts(const CEcoModel* Model);

	/**
	*	��������� ��������� �������� � ���������� �������
	*/
	void			SetInit(const double t0,const double* Values);
	/**
	*	��������� ����������
	*/
	void			SetParam(const double* Params);	
	virtual void	GetEcvilibrium(double* epoint) const;

	/**
	*	���������� ���������� �������� 
	*/
	virtual void _fastcall 	F(const double t,const double* Y,double* FY) const;
	
	void  SetUIBlockId(const int  BlockId);
	void  SetUIState(const bool IsActive);
	const int  GetUIBlockId() const;	
	const bool GetUIState(void) const;

	/**
	*	����������� ��������� �� ��������� �������� ������ 
	*/
	CModelCharts* GetCharts() const;

	/**
	*	����������� ��������� �� �������� ������ 
	*/
	const CModelDescription *GetModelDescription() const;
	/**
	*	����������� ������� ���������� �� ��������� ������ 
	*/	
	CMathParser **GetMathExpressions()  const;
};
