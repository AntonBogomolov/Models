#include "EcoCl.h"
#include "global.h"
#include "MathParser.h"
#include "ChartContainer.h"
#include "ModelDescription.h"

/*
void CAttLor::F(double t, double* Y, double* FY) 
{
	FY[0]=-a*Y[0]+a*Y[1];  
	FY[1]=-Y[0]*Y[2]+c*Y[0]-Y[1];
	FY[2]=Y[0]*Y[1]-b*Y[2];
} 
*/

CEcoModel::CEcoModel(const double* Params,const CModelDescription* ModelDescription): CRungeKutta(ModelDescription->GetDimension())
{
	modelDescription = ModelDescription;	
	const int dim = ModelDescription->GetDimension();
	uiBlockId = -1;
	isActive = true;

	values = new double[dim];
	
	mathExpressions = new CMathParser*[dim];
	for(int i = 0; i < dim; i++)
	{
		mathExpressions[i] = new CMathParser(modelDescription->GetMathFormula()[i].c_str());
	}

	cntParams = modelDescription->GetParamCnt();
	params = new double[cntParams];	
	for(int i = 0; i < cntParams; i++)
	{
		params[i] = Params[i];
		double maxParamVal = modelDescription->GetMaxParamsLimit()[i];
		double minParamVal = modelDescription->GetMinParamsLimit()[i];
		if(maxParamVal < params[i]) params[i] = maxParamVal;
		if(minParamVal > params[i]) params[i] = minParamVal;

		for(int j = 0; j < dim; j++)
		{
			mathExpressions[j]->variables.SetVar(modelDescription->GetParamNames()[i].c_str(),VAR_CONSTANT,params[i]);
		}
	}
	for(int i = 0; i < dim; i++)
	{
		for(int j = 0; j < dim; j++)
		{
			values[j] = 0;	
			mathExpressions[i]->variables.SetVar(modelDescription->GetValueNames()[j].c_str(),VAR_VARIABLE,&values[j]);
		}	
	}
	SetValLimit(modelDescription->IsPositiveValues());

	charts = NULL;	
}

CEcoModel::~CEcoModel()
{		
	if(charts) delete charts;

	if(values) delete [] values;
	for(int i = 0; i < modelDescription->GetDimension(); i++)
	{
		delete mathExpressions[i];
	}
	if(mathExpressions) delete mathExpressions;	
}

void CEcoModel::InitCharts()
{
	if(!charts) charts = new CModelCharts(this);

	const CChartDescription* chartsDescription = modelDescription->GetChartsDescription();
	const unsigned int chartsCnt = chartsDescription->GetChartCnt();
	for(unsigned int i = 0; i < chartsCnt; i++)
	{
		const unsigned int axisXValsCnt = chartsDescription->GetChartAxisXValsCnt(i);
		const unsigned int axisYValsCnt = chartsDescription->GetChartAxisYValsCnt(i);
		const unsigned int axisZValsCnt = chartsDescription->GetChartAxisZValsCnt(i);
		if(axisXValsCnt > 1)continue;
		if(axisZValsCnt > 1)continue;

		const int axisXVal = chartsDescription->GetChartAxisXValue(i,0);
		const int axisZVal = chartsDescription->GetChartAxisZValue(i,0);

		if(axisYValsCnt == 1 && axisXVal > 0 && axisZValsCnt == 0)
		{
			const int axisYVal = chartsDescription->GetChartAxisYValue(i,0);
			if(axisYVal < 0)continue;		

			charts->CreateChart(axisXVal,axisYVal);
			continue;
		}
		if(axisXValsCnt == 1 && axisYValsCnt == 1 && axisZValsCnt == 1)
		{
			const int axisYVal = chartsDescription->GetChartAxisYValue(i,0);
			if(axisXVal < 0)continue;
			if(axisYVal < 0)continue;
			if(axisZVal < 0)continue;

			charts->CreateChart(axisXVal,axisYVal,axisZVal);
			continue;
		}
		if(axisYValsCnt > 0 && axisXVal == 0 && axisZValsCnt == 0)
		{
			bool isValsOk = true;
			unsigned int* yVals = new unsigned int[axisYValsCnt];
			for(unsigned int j = 0; j < axisYValsCnt; j++)
			{
				yVals[j] = chartsDescription->GetChartAxisYValue(i,j);
				if(yVals[j] == 0) isValsOk = false;
			}
			if(!isValsOk)
			{
				delete [] yVals;
				continue;			
			}

			charts->CreateChart(yVals,axisYValsCnt);
			delete [] yVals;
			continue;
		}
	}
}
void CEcoModel::InitCharts(const CEcoModel* Model)
{
	if(!charts) charts = new CModelCharts(*(Model->GetCharts()),this);
}

void CEcoModel::SetInit(const double t0,const double* Values)
{
	SetModelSTime(t0);    

	for(int i = 0; i < modelDescription->GetDimension(); i++)
	{
		double val = Values[i];
		double maxVal = modelDescription->GetMaxValuesLimit()[i];
		double minVal = modelDescription->GetMinValuesLimit()[i];
		if(maxVal < val) val = maxVal;
		if(minVal > val) val = minVal;

		Y[i]		= val;
		values[i]	= val;		
	}
}

void CEcoModel::SetParam(const double* Params)
{
	for(int i = 0; i < modelDescription->GetParamCnt(); i++)
	{
		if(Params[i])
		{
			params[i] = Params[i];
			double maxParamVal = modelDescription->GetMaxParamsLimit()[i];
			double minParamVal = modelDescription->GetMinParamsLimit()[i];
			if(maxParamVal < params[i]) params[i] = maxParamVal;
			if(minParamVal > params[i]) params[i] = minParamVal;			
		}
		else
		{
			params[i] = 0;
		}
	}
}

void  _fastcall CEcoModel::F(const double t,const double* Y,double* FY) const 
{
	int dim  = modelDescription->dimension;
	for(int i = 0; i < dim; i++)
	{
		values[i] = Y[i];
	}
	for(int i = 0; i < dim; i++)
	{
		FY[i] = mathExpressions[i]->Execute();		
	}
}

void CEcoModel::GetEcvilibrium(double* epoint) const
{
	for(int i = 0; i < modelDescription->GetDimension(); i++)
	{
		epoint[i] = 0;
	}
}

CModelCharts* CEcoModel::GetCharts()  const
{
	return charts;
}

const CModelDescription *CEcoModel::GetModelDescription() const
{
	return modelDescription;
}

CMathParser** CEcoModel::GetMathExpressions() const
{
	return mathExpressions;
}

void  CEcoModel::SetUIBlockId(const int  BlockId)
{
	uiBlockId = BlockId;
}
const int CEcoModel::GetUIBlockId() const 
{
	return uiBlockId;
}
void  CEcoModel::SetUIState(const bool IsActive)
{
	isActive = IsActive;
}
const bool CEcoModel::GetUIState(void) const
{
	return isActive;
}	