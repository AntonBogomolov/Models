#pragma once
#include "global.h"

class CWinUI;
class CProgMain;
class CModelDescription;
class CChartDescription;
class CEcoModel;

using namespace std;

/**
*	 ласс предназначен дл€ построени€ пользовательского интерфейса редактора моделей
*/
class CModelConstructorUI
{
private:
	CWinUI *UIWindow;		
	vector<CModelDescription*> *modelsDescription;
	CModelDescription* activeDescription;	

	vector<string> paramsNames;
	vector<string> valuesNames;

	int activeModelIndex;
	bool haveParseErrors;
private:	
	void ParseParams();
	CChartDescription* ParseCharts();
	const int* ParseChartStrings(const char* AxisStr,unsigned int &RetValCnt);

	void UpdateActiveDescription();
	void AddChart(const CChartDescription* ChartDescription);
	void DeleteChart(const int ChartInd,const CChartDescription* ChartDescription);
	void DeleteChart(const int ChartInd,CChartDescription* ChartDescription);
	void DeleteBrokenCharts(const CChartDescription* ChartDescription);
	void BuildUI();

	const int FindPosInModelsDescription(string ModelName) const;
public:
	const CWinUI* GetUIWindow() const;
	void ShowWindow(const bool IsVisible);
	void SelectModel(const int modelInd); 
	void UIEvents(const HWND Hwnd,const long int ControlID);
	void SetMainProgPointer(CProgMain* MainProgPtr);

	const bool IsWindowVisible() const;

	CModelConstructorUI(vector<CModelDescription*> *ModelsDescription);
	~CModelConstructorUI(void);
};

