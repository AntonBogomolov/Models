#include "ModelDescription.h"
#include <fstream>
#include "Log.h"

CModelDescription::CModelDescription	(	const string Name,const string Description,const int ParamCnt,const string* ParamDescription,const string* ParamNames,const double* Params,const int Dimension,
											const string* ValueDescription,const string* ValueNames,const double* InitCnt,const double  InitTime, const double DT,const string* MathFormula,const CChartDescription &ChartsDescription,const bool HavePositiveValues)
{
	name = Name;
	description = Description;
	paramCnt = ParamCnt;
	havePositiveValues = HavePositiveValues;

	int count = (paramCnt > 0)? paramCnt : 1;
	params = new double[count];
	paramDescription = new string[count];
	paramNames = new string[count];
	minParamValues = new double[count];
	maxParamValues = new double[count];
	
	for(int i = 0; i < paramCnt; i++)
	{
		paramDescription[i] = ParamDescription[i];
		paramNames[i] = ParamNames[i];
 		params[i] = Params[i];
		minParamValues[i] = 0;
		maxParamValues[i] = 100;
	}

	dimension = Dimension;
	count = (dimension > 0)? dimension : 1;
	initCnt = new double[count];
	valueDescription = new string[count];
	valueNames = new string[count];
	mathFormula = new string[count];
	maxValues = new double[count];
	minValues = new double[count];

	for(int i = 0; i < dimension; i++)
	{
		valueDescription[i] = ValueDescription[i];
		valueNames[i] = ValueNames[i];
		mathFormula[i] = MathFormula[i];
		initCnt[i] = InitCnt[i];
		maxValues[i] = 200;
		minValues[i] = 0;
	}

	initTime = InitTime;
	initDT = DT;

	maxTime = 10000;
	minTime = 1;
	minDT = 0.00001;
	maxDT = 1.0;

	chartsDescription = new CChartDescription(ChartsDescription);
}

CModelDescription::CModelDescription(const CModelDescription &ModelDescription)
{
	name = ModelDescription.GetName();
	description = ModelDescription.GetDescription();
	paramCnt = ModelDescription.GetParamCnt();
	havePositiveValues = ModelDescription.IsPositiveValues();

	int count = (paramCnt > 0)? paramCnt : 1;
	params = new double[count];
	paramDescription = new string[count];
	paramNames = new string[count];
	minParamValues = new double[count];
	maxParamValues = new double[count];
	
	for(int i = 0; i < paramCnt; i++)
	{
		paramDescription[i] = ModelDescription.GetParamDescription()[i];
		paramNames[i] = ModelDescription.GetParamNames()[i];
 		params[i] = ModelDescription.GetParams()[i];
		minParamValues[i] = ModelDescription.GetMinParamsLimit()[i];
		maxParamValues[i] = ModelDescription.GetMaxParamsLimit()[i];
	}

	dimension = ModelDescription.GetDimension();
	count = (dimension > 0)? dimension : 1;
	initCnt = new double[count];
	valueDescription = new string[count];
	valueNames = new string[count];
	mathFormula = new string[count];
	maxValues = new double[count];
	minValues = new double[count];

	for(int i = 0; i < dimension; i++)
	{
		valueDescription[i] = ModelDescription.GetValueDescription()[i];
		valueNames[i] = ModelDescription.GetValueNames()[i];
		mathFormula[i] = ModelDescription.GetMathFormula()[i];
		initCnt[i] = ModelDescription.GetInitCnt()[i];
		maxValues[i] = ModelDescription.GetMaxValuesLimit()[i];
		minValues[i] = ModelDescription.GetMinValuesLimit()[i];
	}

	initTime = ModelDescription.GetInitTime();
	initDT = ModelDescription.GetInitDT();

	maxTime = ModelDescription.GetMaxTimeLimit();
	minTime = ModelDescription.GetMinTimeLimit();
	minDT = ModelDescription.GetMinDTLimit();
	maxDT = ModelDescription.GetMaxDTLimit();

	chartsDescription = new CChartDescription((*ModelDescription.GetChartsDescription()));
}

CModelDescription::~CModelDescription(void)
{
	if(paramDescription)delete[] paramDescription;
	if(valueDescription) delete [] valueDescription;
	if(paramNames)delete[] paramNames;
	if(valueNames) delete [] valueNames;

	if(params)delete[] params;
	if(initCnt)delete [] initCnt;
	if(mathFormula) delete [] mathFormula;
	if(chartsDescription)delete chartsDescription;

	delete [] maxValues;
	delete [] minValues;
	delete [] maxParamValues;
	delete [] minParamValues;
}

void CModelDescription::SetChartsDescription(CChartDescription *ChartsDescription)
{
	if(ChartsDescription == NULL) return;
	delete chartsDescription;
	chartsDescription = ChartsDescription;
}

void CModelDescription::SetIsPositiveValues(const bool IsPositiveValues)
{
	havePositiveValues = IsPositiveValues;
}

void CModelDescription::SetParams(const double *Params)
{
	for(int i = 0; i < paramCnt; i++)
	{
		params[i] = Params[i];
	}
}

void CModelDescription::SetTimeLimits(const double MinTime,const double MaxTime)
{
	minTime = MinTime;
	if(minTime < 0) minTime = 1;
	maxTime = MaxTime;
	if(MaxTime > 100000) maxTime = 10000;
}
void CModelDescription::SetDTLimits  (const double MinDT,const double MaxDT)
{
	minDT = MinDT;
	if(minDT < 0.000001) minDT = 0.000001;
	maxDT = MaxDT;
	if(MaxDT > 1) maxDT = 1;
}
void CModelDescription::SetParamsLimit(const double* MinParamValues, const double* MaxParamValues)
{
	for(int i = 0; i < paramCnt; i++)
	{
		minParamValues[i] = MinParamValues[i];
		maxParamValues[i] = MaxParamValues[i];
	}
}
void CModelDescription::SetValuesLimit(const double* MinValues, const double* MaxValues)
{
	for(int i = 0; i < dimension; i++)
	{
		minValues[i] = (MinValues[i] > 0)? MinValues[i] : 0;
		maxValues[i] = MaxValues[i];
	}
}

const int CModelDescription::GetParamIndex(const string ParamName) const
{
	for(int i = 0; i < paramCnt; i++)
	{
		if(paramNames[i].compare(ParamName) == 0)
		{
			return i;
		}
	}
	return -1;
}

const int CModelDescription::GetValueIndex(const string ValueName) const
{
	for(int i = 0; i < dimension; i++)
	{
		if(valueNames[i].compare(ValueName) == 0)
		{
			return i;
		}
	}
	return -1;
}

void CModelDescription::SaveToFile(const vector<CModelDescription*>* ModelsDesc, string FileName)
{
	ofstream ofile(FileName.c_str(),ios_base::out|ios_base::binary);
	if(ofile.fail()) 
	{
		CLog::GetInstance()->ReportWarning("Не удаётся сохранить файл с описанием моделей");
		return;
	}

	int strLen = 0;

	ofile.write("mod",sizeof(char)*3);
	int modelsCnt = ModelsDesc->size();
	ofile.write((char *)&modelsCnt,sizeof(int));

	for(auto it = ModelsDesc->begin(); it != ModelsDesc->end(); it++)
	{
		CModelDescription* currDesc = (*it);
		const string name				= currDesc->GetName();
		const string description		= currDesc->GetDescription();
		const string* paramDescription	= currDesc->GetParamDescription();
		const string* valueDescription	= currDesc->GetValueDescription();
		const string* paramNames		= currDesc->GetParamNames();
		const string* valueNames		= currDesc->GetValueNames();
		const string* mathFormula		= currDesc->GetMathFormula();

		const int dim = currDesc->GetDimension();
		const int paramCnt = currDesc->GetParamCnt(); 
		const double initTime = currDesc->GetInitTime();	
		const double initDT = currDesc->GetInitDT();		

		const CChartDescription* chartsDescription = currDesc->GetChartsDescription();


		ofile.write((char *)&dim,sizeof(int));
		ofile.write((char *)&paramCnt,sizeof(int));

		strLen = name.length();
		ofile.write((char *)&strLen,sizeof(int));
		ofile.write((char *)name.c_str(),sizeof(char)*strLen);

		strLen = description.length();
		ofile.write((char *)&strLen,sizeof(int));
		ofile.write((char *)description.c_str(),sizeof(char)*strLen);

		for(int i = 0; i < dim; i++)
		{
			double initCnt = currDesc->GetInitCnt()[i]; 
			double minValue = currDesc->GetMinValuesLimit()[i]; 
			double maxValue = currDesc->GetMaxValuesLimit()[i]; 

			ofile.write((char *) &initCnt,sizeof(double));
			ofile.write((char *) &minValue,sizeof(double));
			ofile.write((char *) &maxValue,sizeof(double));
						
			strLen = valueDescription[i].length();
			ofile.write((char *)&strLen,sizeof(int));
			ofile.write((char *)valueDescription[i].c_str(),sizeof(char)*strLen);

			strLen = valueNames[i].length();
			ofile.write((char *)&strLen,sizeof(int));
			ofile.write((char *)valueNames[i].c_str(),sizeof(char)*strLen);

			strLen = mathFormula[i].length();
			ofile.write((char *)&strLen,sizeof(int));
			ofile.write((char *)mathFormula[i].c_str(),sizeof(char)*strLen);
		}
		for(int i = 0; i < paramCnt; i++)
		{
			double params = currDesc->GetParams()[i];
			double minParamValues = currDesc->GetMinParamsLimit()[i]; 
			double maxParamValues = currDesc->GetMaxParamsLimit()[i]; 

			ofile.write((char *) &params,sizeof(double));
			ofile.write((char *) &minParamValues,sizeof(double));
			ofile.write((char *) &maxParamValues,sizeof(double));

			strLen = paramDescription[i].length();
			ofile.write((char *)&strLen,sizeof(int));
			ofile.write((char *)paramDescription[i].c_str(),sizeof(char)*strLen);

			strLen = paramNames[i].length();
			ofile.write((char *)&strLen,sizeof(int));
			ofile.write((char *)paramNames[i].c_str(),sizeof(char)*strLen);
		}
		
		double maxTime = currDesc->GetMaxTimeLimit();
		double minTime = currDesc->GetMinTimeLimit();
		double maxDTime = currDesc->GetMaxDTLimit();
		double minDTime = currDesc->GetMinDTLimit();

		ofile.write((char *) &maxTime,sizeof(double));
		ofile.write((char *) &minTime,sizeof(double));
		ofile.write((char *) &maxDTime,sizeof(double));
		ofile.write((char *) &minDTime,sizeof(double));
		ofile.write((char *) &initTime,sizeof(double));
		ofile.write((char *) &initDT,sizeof(double));

		bool havePositiveValues = currDesc->IsPositiveValues();
		ofile.write((char *) &havePositiveValues,sizeof(bool));

		unsigned int chartCnt = chartsDescription->GetChartCnt();	
		ofile.write((char *) &chartCnt,sizeof(int));
		for(unsigned int i = 0; i < chartCnt; i++)
		{
			unsigned int valXCnt = chartsDescription->GetChartAxisXValsCnt(i);
			unsigned int valYCnt = chartsDescription->GetChartAxisYValsCnt(i);
			unsigned int valZCnt = chartsDescription->GetChartAxisZValsCnt(i);

			const int* valX = chartsDescription->GetChartAxisXValues(i); 
			const int* valY = chartsDescription->GetChartAxisYValues(i); 
			const int* valZ = chartsDescription->GetChartAxisZValues(i); 

			ofile.write((char *)&valXCnt,sizeof(int));
			ofile.write((char *)valX,sizeof(int)*valXCnt);

			ofile.write((char *)&valYCnt,sizeof(int));
			ofile.write((char *)valY,sizeof(int)*valYCnt);

			ofile.write((char *)&valZCnt,sizeof(int));
			ofile.write((char *)valZ,sizeof(int)*valZCnt);

			strLen = chartsDescription->GetChartName(i).length();
			ofile.write((char *)&strLen,sizeof(int));
			ofile.write((char *)chartsDescription->GetChartName(i).c_str(),sizeof(char)*strLen);
		}
	}
	ofile.close();
}

vector<CModelDescription*>* CModelDescription::LoadFromeFile(string FileName)
{
	vector<CModelDescription*>* modelsDesc = new vector<CModelDescription*>;
	
	ifstream ifile(FileName.c_str(),ios_base::in|ios_base::binary);
	if(ifile.fail()) 
	{
		CLog::GetInstance()->ReportWarning("Не удаётся открыть файл с описанием моделей");
		return modelsDesc;
	}
	char* tmpStr = new char[1024];

	ifile.read(tmpStr,sizeof(char)*3);
	tmpStr[3] = '\0';
	if(strcmp(tmpStr,"mod") != 0) 
	{
		delete [] tmpStr;
		CLog::GetInstance()->ReportWarning("Не удаётся открыть файл с описанием моделей");
		return modelsDesc;
	}

	int modelsCnt = 0;
	int strLen = 0;

	ifile.read((char *)&modelsCnt,sizeof(int));
	for(int i = 0; i < modelsCnt; i++)
	{	
		int dim = 0;
		int paramCnt = 0;

		ifile.read((char *)&dim,sizeof(int));
		ifile.read((char *)&paramCnt,sizeof(int));

		string name;
		string description;
		string* paramDescription = new string[paramCnt];	
		string* valueDescription = new string[dim];	
		string* paramNames =  new string[paramCnt];	
		string* valueNames =  new string[dim];	
		string* mathFormula = new string[dim];	

		double* initCnt   = new double[dim];
		double* minValues = new double[dim];
		double* maxValues = new double[dim];
		double* params =  new double[paramCnt];	
		double* minParamValues = new double[paramCnt];	
		double* maxParamValues = new double[paramCnt];	
		
		double maxTime  = 0;
		double minTime  = 0;
		double maxDTime = 0;
		double minDTime = 0;
		double initTime = 0;
		double initDT = 0;
		bool havePositiveValues = true;

		ifile.read((char *)&strLen,sizeof(int));
		ifile.read((char *)tmpStr,sizeof(char)*(strLen));
		tmpStr[strLen] = '\0';
		name = tmpStr;

		ifile.read((char *)&strLen,sizeof(int));
		ifile.read((char *)tmpStr,sizeof(char)*(strLen));
		tmpStr[strLen] = '\0';
		description = tmpStr;

		for(int i = 0; i < dim; i++)
		{			
			ifile.read((char *) &initCnt[i],sizeof(double));
			ifile.read((char *) &minValues[i],sizeof(double));
			ifile.read((char *) &maxValues[i],sizeof(double));

			ifile.read((char *)&strLen,sizeof(int));
			ifile.read((char *)tmpStr,sizeof(char)*strLen);
			tmpStr[strLen] = '\0';
			valueDescription[i] = tmpStr;

			ifile.read((char *)&strLen,sizeof(int));
			ifile.read((char *)tmpStr,sizeof(char)*strLen);
			tmpStr[strLen] = '\0';
			valueNames[i] = tmpStr;

			ifile.read((char *)&strLen,sizeof(int));
			ifile.read((char *)tmpStr,sizeof(char)*strLen);
			tmpStr[strLen] = '\0';
			mathFormula[i] = tmpStr;
		}
		for(int i = 0; i < paramCnt; i++)
		{			
			ifile.read((char *) &params[i],sizeof(double));
			ifile.read((char *) &minParamValues[i],sizeof(double));
			ifile.read((char *) &maxParamValues[i],sizeof(double));
						
			ifile.read((char *)&strLen,sizeof(int));
			ifile.read((char *)tmpStr,sizeof(char)*strLen);
			tmpStr[strLen] = '\0';
			paramDescription[i] = tmpStr;

			ifile.read((char *)&strLen,sizeof(int));
			ifile.read((char *)tmpStr,sizeof(char)*strLen);
			tmpStr[strLen] = '\0';
			paramNames[i] = tmpStr;
		}

		ifile.read((char *) &maxTime, sizeof(double));
		ifile.read((char *) &minTime, sizeof(double));
		ifile.read((char *) &maxDTime,sizeof(double));
		ifile.read((char *) &minDTime,sizeof(double));
		ifile.read((char *) &initTime,sizeof(double));
		ifile.read((char *) &initDT,sizeof(double));
		
		ifile.read((char *) &havePositiveValues,sizeof(bool));

		unsigned int chartCnt = 0;	
		ifile.read((char *) &chartCnt,sizeof(int));
		CChartDescription* chartDesc = new CChartDescription(chartCnt,dim);

		for(unsigned int i = 0; i < chartCnt; i++)
		{
			string chartName;
			unsigned int valXCnt = 0;
			unsigned int valYCnt = 0;
			unsigned int valZCnt = 0;
						
			ifile.read((char *)&valXCnt,sizeof(int));
			int* valX = new int[valXCnt]; 
			ifile.read((char *)valX,sizeof(int)*valXCnt);

			ifile.read((char *)&valYCnt,sizeof(int));
			int* valY = new int[valYCnt]; 
			ifile.read((char *)valY,sizeof(int)*valYCnt);

			ifile.read((char *)&valZCnt,sizeof(int));
			int* valZ = new int[valZCnt]; 
			ifile.read((char *)valZ,sizeof(int)*valZCnt);

			ifile.read((char *)&strLen,sizeof(int));
			ifile.read((char *)tmpStr,sizeof(char)*strLen);
			tmpStr[strLen] = '\0';
			chartName = tmpStr;

			chartDesc->SetChartName(i,chartName);
			chartDesc->SetChartDescriptionAxisX(i,valXCnt,valX);
			chartDesc->SetChartDescriptionAxisY(i,valYCnt,valY);
			chartDesc->SetChartDescriptionAxisZ(i,valZCnt,valZ);
		}

		CModelDescription* modelDesc = new CModelDescription(name,description,paramCnt,paramDescription,paramNames,params,dim,valueDescription,valueNames,initCnt,initTime,initDT,mathFormula,(*chartDesc),havePositiveValues);
		modelDesc->SetParamsLimit(minParamValues,maxParamValues);
		modelDesc->SetValuesLimit(minValues,maxValues);
		modelDesc->SetTimeLimits(minTime,maxTime);
		modelDesc->SetDTLimits(minDTime,maxDTime);

		modelsDesc->push_back(modelDesc);
	}

	ifile.close();
	delete [] tmpStr;

	return modelsDesc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

CChartDescription::CChartDescription(const unsigned int ChartCnt,const unsigned int ModelDim)
{
	chartCnt = ChartCnt;	
	chartsDescription = new SChartData[chartCnt];
	modelDim = ModelDim + 1;
	for(unsigned int i = 0 ; i < chartCnt; i++ )
	{		
		chartsDescription[i].axisXValsCnt = modelDim;
		chartsDescription[i].axisYValsCnt = modelDim;
		chartsDescription[i].axisZValsCnt = modelDim;
		chartsDescription[i].axisX = new int[modelDim];
		chartsDescription[i].axisY = new int[modelDim];
		chartsDescription[i].axisZ = new int[modelDim];

		for(unsigned int j = 0; j < modelDim; j++)
		{
			chartsDescription[i].axisX[j] = -1;
			chartsDescription[i].axisY[j] = -1;
			chartsDescription[i].axisZ[j] = -1;
		}
	}
}

CChartDescription::CChartDescription(const CChartDescription &ChartDescription)
{
	chartCnt = ChartDescription.GetChartCnt();
	modelDim = ChartDescription.GetModelDim();
	chartsDescription = new SChartData[chartCnt];
	for(unsigned int i = 0 ; i < chartCnt; i++ )
	{
		chartsDescription[i].chartName = ChartDescription.GetChartName(i);
		chartsDescription[i].axisXValsCnt = modelDim;
		chartsDescription[i].axisYValsCnt = modelDim;
		chartsDescription[i].axisZValsCnt = modelDim;
		chartsDescription[i].axisX = new int[modelDim];
		chartsDescription[i].axisY = new int[modelDim];
		chartsDescription[i].axisZ = new int[modelDim];
		for(unsigned int j = 0; j < modelDim; j++)
		{
			chartsDescription[i].axisX[j] = ChartDescription.GetChartAxisXValue(i,j);
			chartsDescription[i].axisY[j] = ChartDescription.GetChartAxisYValue(i,j);
			chartsDescription[i].axisZ[j] = ChartDescription.GetChartAxisZValue(i,j);
		}
	}
}

CChartDescription::~CChartDescription()
{
	for(unsigned int i = 0 ; i < chartCnt; i++ )
	{
		delete [] chartsDescription[i].axisX;
		delete [] chartsDescription[i].axisY;
		delete [] chartsDescription[i].axisZ;
	}
	delete [] chartsDescription;
}

const unsigned int CChartDescription::GetChartAxisXValsCnt(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt ) return 0;			
	for(unsigned int i = 0; i < chartsDescription[ChartInd].axisXValsCnt; i++)
	{		
		if(chartsDescription[ChartInd].axisX[i] == -1) return i;		
	}
	return modelDim;
}
const unsigned int CChartDescription::GetChartAxisYValsCnt(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt ) return 0;			
	for(unsigned int i = 0; i < chartsDescription[ChartInd].axisYValsCnt; i++)
	{		
		if(chartsDescription[ChartInd].axisY[i] == -1) return i;		
	}
	return modelDim;
}
const unsigned int CChartDescription::GetChartAxisZValsCnt(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt ) return 0;			
	for(unsigned int i = 0; i < chartsDescription[ChartInd].axisZValsCnt; i++)
	{		
		if(chartsDescription[ChartInd].axisZ[i] == -1) return i;		
	}
	return modelDim;
}

const int CChartDescription::GetChartAxisXValue(const unsigned int ChartInd,const unsigned int ValueInd) const
{
	if(ChartInd >= chartCnt ) return -1;
	if(ValueInd > chartsDescription[ChartInd].axisXValsCnt) return -1;
	return chartsDescription[ChartInd].axisX[ValueInd];
}
const int CChartDescription::GetChartAxisYValue(const unsigned int ChartInd,const unsigned int ValueInd) const
{
	if(ChartInd >= chartCnt ) return -1;
	if(ValueInd > chartsDescription[ChartInd].axisYValsCnt) return -1;
	return chartsDescription[ChartInd].axisY[ValueInd];
}
const int CChartDescription::GetChartAxisZValue(const unsigned int ChartInd,const unsigned int ValueInd) const
{
	if(ChartInd >= chartCnt ) return -1;
	if(ValueInd > chartsDescription[ChartInd].axisZValsCnt) return -1;
	return chartsDescription[ChartInd].axisZ[ValueInd];
}

const int* CChartDescription::GetChartAxisXValues(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt ) return NULL;
	return chartsDescription[ChartInd].axisX;
}
const int* CChartDescription::GetChartAxisYValues(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt ) return NULL;
	return chartsDescription[ChartInd].axisY;
}
const int* CChartDescription::GetChartAxisZValues(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt ) return NULL;
	return chartsDescription[ChartInd].axisZ;
}

const string CChartDescription::GetChartName(const unsigned int ChartInd) const
{
	if(ChartInd >= chartCnt )	return "";
	return 	chartsDescription[ChartInd].chartName;
}
void CChartDescription::SetChartName(const unsigned int ChartInd,const string ChartName)
{
	if(ChartInd >= chartCnt )	return;
	chartsDescription[ChartInd].chartName = ChartName;
}

void CChartDescription::SetChartDescriptionAxisX(const unsigned int ChartInd,const unsigned int AxisXValsCnt,const int* ChartDescription)
{
	if(ChartInd >= chartCnt ) return;	
	if(AxisXValsCnt > chartsDescription[ChartInd].axisXValsCnt) return;
	if(AxisXValsCnt > 0 && ChartDescription == NULL) return;
	
	for(unsigned int i = 0; i < AxisXValsCnt; i++)
	{				
		chartsDescription[ChartInd].axisX[i] = ChartDescription[i];		
	}
}
void CChartDescription::SetChartDescriptionAxisY(const unsigned int ChartInd,const unsigned int AxisYValsCnt,const int* ChartDescription)
{
	if(ChartInd >= chartCnt ) return;	
	if(AxisYValsCnt > chartsDescription[ChartInd].axisYValsCnt) return;
	if(AxisYValsCnt > 0 && ChartDescription == NULL) return;
	
	for(unsigned int i = 0; i < AxisYValsCnt; i++)
	{				
		chartsDescription[ChartInd].axisY[i] = ChartDescription[i];		
	}
}
void CChartDescription::SetChartDescriptionAxisZ(const unsigned int ChartInd,const unsigned int AxisZValsCnt,const int* ChartDescription)
{
	if(ChartInd >= chartCnt ) return;	
	if(AxisZValsCnt > chartsDescription[ChartInd].axisZValsCnt) return;
	if(AxisZValsCnt > 0 && ChartDescription == NULL) return;
	
	for(unsigned int i = 0; i < AxisZValsCnt; i++)
	{				
		chartsDescription[ChartInd].axisZ[i] = ChartDescription[i];		
	}
}

void CChartDescription::SetChartDescription(const string ChartName,const unsigned int ChartInd,const unsigned int AxisXValsCnt,int* ChartDescriptionX,const unsigned int AxisYValsCnt,int* ChartDescriptionY,const unsigned int AxisZValsCnt,int* ChartDescriptionZ)
{
	SetChartDescriptionAxisX(ChartInd,AxisXValsCnt,ChartDescriptionX);
	SetChartDescriptionAxisY(ChartInd,AxisYValsCnt,ChartDescriptionY);
	SetChartDescriptionAxisZ(ChartInd,AxisZValsCnt,ChartDescriptionZ);
	SetChartName(ChartInd,ChartName);
}

const int CChartDescription::GetChartDescription(const unsigned int ChartInd,const unsigned int DimInd) const
{
	if(ChartInd >= chartCnt ) return -1;
	if(DimInd > modelDim) return -1;

	if(DimInd == 0) return chartsDescription[ChartInd].axisX[0];
	if(DimInd == 1) return chartsDescription[ChartInd].axisY[0];
	if(DimInd == 2) return chartsDescription[ChartInd].axisZ[0];
	return -1;
}

const unsigned int CChartDescription::GetChartValuesCnt(const unsigned int ChartInd) const
{
	return GetChartAxisXValsCnt(ChartInd) + GetChartAxisYValsCnt(ChartInd) + GetChartAxisZValsCnt(ChartInd);
}