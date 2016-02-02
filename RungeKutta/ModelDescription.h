#pragma once
#include "RungeKutta.h"
#include <string>
#include <vector>

using namespace std;

/**
*	Описатель графиков модели. 
*/
class CChartDescription
{
private:
	struct SChartData
	{
		string chartName;
		int *axisX;
		unsigned int axisXValsCnt;
		int *axisY;
		unsigned int axisYValsCnt;
		int *axisZ;
		unsigned int axisZValsCnt;
	};
	unsigned int chartCnt;
	unsigned int modelDim;
	SChartData* chartsDescription;
public:
	const unsigned int GetChartCnt() const {return chartCnt;};
	const unsigned int GetModelDim() const {return modelDim;};

	const unsigned int GetChartAxisXValsCnt(const unsigned int ChartInd) const;
	const unsigned int GetChartAxisYValsCnt(const unsigned int ChartInd) const;
	const unsigned int GetChartAxisZValsCnt(const unsigned int ChartInd) const;
	const int GetChartAxisXValue(const unsigned int ChartInd,const unsigned int ValueInd) const;
	const int GetChartAxisYValue(const unsigned int ChartInd,const unsigned int ValueInd) const;
	const int GetChartAxisZValue(const unsigned int ChartInd,const unsigned int ValueInd) const;
	const int* GetChartAxisXValues(const unsigned int ChartInd) const;
	const int* GetChartAxisYValues(const unsigned int ChartInd) const;
	const int* GetChartAxisZValues(const unsigned int ChartInd) const;
	const string GetChartName(const unsigned int ChartInd) const;

	const unsigned int GetChartValuesCnt(const unsigned int ChartInd) const;
	const int GetChartDescription(const unsigned int ChartInd,const unsigned int Axis) const;

	void SetChartName(const unsigned int ChartInd,const string ChartName);
	void SetChartDescription(const string ChartName,const unsigned int ChartInd,const unsigned int AxisXValsCnt,int* ChartDescriptionX,const unsigned int AxisYValsCnt,int* ChartDescriptionY,const unsigned int AxisZValsCnt,int* ChartDescriptionZ);
	void SetChartDescriptionAxisX(const unsigned int ChartInd,const unsigned int AxisXValsCnt,const int* ChartDescription);
	void SetChartDescriptionAxisY(const unsigned int ChartInd,const unsigned int AxisYValsCnt,const int* ChartDescription);
	void SetChartDescriptionAxisZ(const unsigned int ChartInd,const unsigned int AxisZValsCnt,const int* ChartDescription);

	CChartDescription(const unsigned int ChartCnt,const unsigned int ModelDim);
	CChartDescription(const CChartDescription &ChartDescription);
	~CChartDescription();
};

/**
*	Описатель модели. 
*/
class CModelDescription
{
private:
	string  name;
	string  description;	
	string* paramDescription;
	string* valueDescription;
	string* paramNames;
	string* valueNames;
	string* mathFormula;

	int     paramCnt;
	double* params;	
	double* initCnt;
	double  initTime;	
	double  initDT;	

	double maxTime;
	double minTime;
	double maxDT;
	double minDT;
	double*maxParamValues;
	double*minParamValues;
	double*maxValues;
	double*minValues;

	bool havePositiveValues;

	CChartDescription* chartsDescription;
public:
	int	dimension;
public:
	void SetParams(const double* Params);

	void SetTimeLimits(const double MinTime,const double MaxTime);
	void SetDTLimits  (const double MinDT,	const double MaxDT);
	void SetParamsLimit(const double* MinParamValues,const double* MaxParamValues);
	void SetValuesLimit(const double* MinValues,	 const double* MaxValues);
	void SetChartsDescription(CChartDescription *ChartsDescription);
		
	const double  GetMaxTimeLimit(void) const {return maxTime;};
	const double  GetMinTimeLimit(void) const {return minTime;};
	const double  GetMaxDTLimit(void)	const {return maxDT;};
	const double  GetMinDTLimit(void)	const {return minDT;};
	const double* GetMaxParamsLimit(void) const {return maxParamValues;};	
	const double* GetMinParamsLimit(void) const {return minParamValues;};	
	const double* GetMaxValuesLimit(void) const {return maxValues;};	
	const double* GetMinValuesLimit(void) const {return minValues;};	

	const string  GetName()			const {return name;};
	const string  GetDescription()	const {return description;};
	const string* GetParamDescription() const {return paramDescription;};
	const string* GetValueDescription() const {return valueDescription;};
	const string* GetParamNames() const {return paramNames;};	
	const string* GetValueNames() const {return valueNames;};
	const string* GetMathFormula()const {return mathFormula;};

	const int	  GetParamIndex(const string ParamName) const;
	const int	  GetValueIndex(const string ValueName) const;
	const int     GetParamCnt()	 const {return paramCnt;};
	const double* GetParams()	 const {return params;};	
	const int	  GetDimension() const {return dimension;};
	const double* GetInitCnt()	 const {return initCnt;};
	const double  GetInitTime()	 const {return initTime;};	
	const double  GetInitDT()	 const {return initDT;};	

	void SetIsPositiveValues(const bool IsPositiveValues);
	const bool IsPositiveValues() const {return havePositiveValues;};

	const CChartDescription* GetChartsDescription() const {return chartsDescription;};

	static void SaveToFile(const vector<CModelDescription*>* ModelsDesc, string FileName = "models.mod");
	static vector<CModelDescription*>* LoadFromeFile(string FileName = "models.mod");
public:
	CModelDescription(const CModelDescription &ModelDescription);
	CModelDescription	(	const string Name,const string Description,const int ParamCnt,const string* ParamDescription,const string* ParamNames,const double* Params,const int Dimension,
							const string* ValueDescription,const string* ValueNames,const double* InitCnt,const double  Time,const double  DT,const string* MathFormula,const CChartDescription &ChartsDescription,const bool HavePositiveValues = true);
	~CModelDescription(void);
};