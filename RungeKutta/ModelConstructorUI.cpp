#include "ModelConstructorUI.h"
#include "WinUI.h"
#include "ModelDescription.h"
#include "ProgMain.h"
#include "MathParser.h"
#include "Log.h"

CModelConstructorUI::CModelConstructorUI(vector<CModelDescription*> *ModelsDescription)
{
	UIWindow = new CWinUI(18000);
	UIWindow->CreateUIWindow(500,450,102);
	modelsDescription = ModelsDescription;
	activeDescription = NULL;
	activeModelIndex = -1;
	haveParseErrors = false;
}


CModelConstructorUI::~CModelConstructorUI(void)
{
	paramsNames.clear();
	valuesNames.clear();
	delete activeDescription;
	delete UIWindow;
}

void CModelConstructorUI::UIEvents(const HWND Hwnd,const long int ControlID)
{
	if(UIWindow->GetHandle() != Hwnd) return;

	const CUIControl *eventControl = UIWindow->FindControl(ControlID);
	if(eventControl == NULL) return;
	
	bool NeedBuildUI = false;
	bool NeedBuildUITwiseAndParse = false;

	if( eventControl->GetName().compare("0_exit") == 0 )
	{
		UIWindow->HideWindow();
	}
	else
	if( eventControl->GetName().compare("0_add_model") == 0 )
	{		
		int sel = SendDlgItemMessage(UIWindow->GetHandle(),UIWindow->FindControl("0_combox_model")->GetId(),CB_GETCURSEL,0,0);
		if( sel != -1)
		{			
			delete activeDescription;
			activeDescription = new CModelDescription((*modelsDescription->at(sel)));	
			activeModelIndex = sel;
			NeedBuildUI = true;
			UIWindow->RedrawWindow();
		}
	}
	else
	if( eventControl->GetName().compare("0_add_new_model") == 0 )
	{				
		delete activeDescription;
		activeDescription = new CModelDescription("","",0,NULL,NULL,NULL,0,NULL,NULL,NULL,30,0.01,NULL,(* new CChartDescription(0,0)));		
		NeedBuildUI = true;	
		UIWindow->RedrawWindow();
	}	
	else
	if( eventControl->GetName().compare("b_save") == 0 )
	{			
		ParseParams();
		UpdateActiveDescription();

		string modelName = activeDescription->GetName();
		const CChartDescription* activeChartDesc = activeDescription->GetChartsDescription();
		const int activeDescDim = activeDescription->GetDimension();

		if(haveParseErrors) 
		{
			CLog::GetInstance()->ReportWarning("Не удалось сохранить модель: проверьте правильность уравнений");
			return;
		}
		if(modelName.length() < 1) 
		{
			CLog::GetInstance()->ReportWarning("Не удалось сохранить модель: проверьте имя модели");
			return;
		}
		if(activeDescDim == 0) 
		{
			CLog::GetInstance()->ReportWarning("Не удалось сохранить модель: у модели нет ни одного уравнения");
			return;
		}
		if(activeChartDesc->GetChartCnt() == 0) 
		{
			CLog::GetInstance()->ReportWarning("Не удалось сохранить модель: у модели нет ни одного графика");
			return;
		}

		DeleteBrokenCharts(activeDescription->GetChartsDescription());

		const int pos = FindPosInModelsDescription(modelName);
		if(pos != -1)
		{			
			delete modelsDescription->at(pos);
			modelsDescription->at(pos) = activeDescription;
		}
		else
		{
			modelsDescription->push_back(activeDescription);
		}
		activeDescription = new CModelDescription((*activeDescription));
		CModelDescription::SaveToFile(modelsDescription);
		
		NeedBuildUI = true;
	}	
	else
	if(eventControl->GetName().compare("b_del") == 0)
	{
		MessageBox(UIWindow->GetHandle(),"Удалить модель?","Подтверждение",MB_YESNO);
		string modelName = activeDescription->GetName();
		const int pos = FindPosInModelsDescription(modelName);
		if(pos != -1)
		{			
			delete modelsDescription->at(pos);		
			modelsDescription->erase(modelsDescription->begin()+pos);
			CModelDescription::SaveToFile(modelsDescription);
			delete activeDescription;
			activeDescription = new CModelDescription("","",0,NULL,NULL,NULL,0,NULL,NULL,NULL,30,0.01,NULL,(* new CChartDescription(0,0)));		
			activeModelIndex = 0;
			UIWindow->RedrawWindow();
		}						
		NeedBuildUI = true;
	}
	else
	if( eventControl->GetName().compare("b_parse_params") == 0 )
	{
		ParseParams();
		UpdateActiveDescription();
		NeedBuildUI = true;
		NeedBuildUITwiseAndParse = true;
	}	
	else
	if( eventControl->GetName().compare("b_c_add") == 0 )
	{
		ParseParams();
		UpdateActiveDescription();
		DeleteBrokenCharts(activeDescription->GetChartsDescription());
		AddChart(activeDescription->GetChartsDescription());
		NeedBuildUI = true;
	}
	else
	if( eventControl->GetName().compare("ch_positive") == 0 )
	{
		LRESULT res = SendDlgItemMessage(Hwnd,UIWindow->FindControl("ch_positive")->GetId(),BM_GETCHECK, 0, 0 );
		if(res == BST_CHECKED) activeDescription->SetIsPositiveValues(true);
		else activeDescription->SetIsPositiveValues(false);
	}					
	else
	{
		if(activeDescription != NULL)
		{
			char* chartIndStr = new char[16];
		
			for(unsigned int i = 0; i < activeDescription->GetChartsDescription()->GetChartCnt(); i++)
			{			
				sprintf(chartIndStr,"b_c_del%d",i);
				if( eventControl->GetName().compare(chartIndStr) == 0 )
				{
					ParseParams();
					UpdateActiveDescription();
					DeleteChart(i,activeDescription->GetChartsDescription());
					NeedBuildUI = true;
				}			
			}
			delete [] chartIndStr;
		}
	}

	if(NeedBuildUI) BuildUI();
	if(NeedBuildUITwiseAndParse)
	{
		ParseParams();
		UpdateActiveDescription();
		BuildUI();
	}
}

void CModelConstructorUI::SelectModel(const int modelInd)
{
	int sel = modelInd;
	if( sel != -1)
	{
		delete activeDescription;
		activeDescription = new CModelDescription((*modelsDescription->at(sel)));	
		activeModelIndex = sel;
		BuildUI();
	}
	else
	{
		delete activeDescription;
		activeDescription = new CModelDescription("","",0,NULL,NULL,NULL,0,NULL,NULL,NULL,30,0.01,NULL,(* new CChartDescription(0,0)));		
		BuildUI();	
	}
}

const CWinUI* CModelConstructorUI::GetUIWindow() const
{
	return UIWindow;
}

void CModelConstructorUI::AddChart(const CChartDescription* ChartDescription)
{
	const CChartDescription* oldChartDescription = ChartDescription;//activeDescription->GetChartsDescription();
	unsigned int chartCnt = oldChartDescription->GetChartCnt();

	CChartDescription* newChartDescr = new CChartDescription(chartCnt + 1,oldChartDescription->GetModelDim());
	for(unsigned int i = 0; i < chartCnt; i++)
	{
		newChartDescr->SetChartDescriptionAxisX(i,oldChartDescription->GetChartAxisXValsCnt(i),oldChartDescription->GetChartAxisXValues(i));
		newChartDescr->SetChartDescriptionAxisY(i,oldChartDescription->GetChartAxisYValsCnt(i),oldChartDescription->GetChartAxisYValues(i));
		newChartDescr->SetChartDescriptionAxisZ(i,oldChartDescription->GetChartAxisZValsCnt(i),oldChartDescription->GetChartAxisZValues(i));
		newChartDescr->SetChartName(i,oldChartDescription->GetChartName(i));
	}
	newChartDescr->SetChartDescriptionAxisX(chartCnt,0,NULL);
	newChartDescr->SetChartDescriptionAxisY(chartCnt,0,NULL);
	newChartDescr->SetChartDescriptionAxisZ(chartCnt,0,NULL);

	activeDescription->SetChartsDescription(newChartDescr);
}

void CModelConstructorUI::DeleteChart(const int ChartInd,const CChartDescription* ChartDescription)
{
	const CChartDescription* oldChartDescription = ChartDescription;//activeDescription->GetChartsDescription();
	unsigned int chartCnt = oldChartDescription->GetChartCnt();
	unsigned int newChartInd = 0;

	CChartDescription* newChartDescr = new CChartDescription(chartCnt - 1,oldChartDescription->GetModelDim());
	for(unsigned int i = 0; i < chartCnt; i++)
	{
		if(i != ChartInd)
		{
			newChartDescr->SetChartDescriptionAxisX(newChartInd,oldChartDescription->GetChartAxisXValsCnt(i),oldChartDescription->GetChartAxisXValues(i));
			newChartDescr->SetChartDescriptionAxisY(newChartInd,oldChartDescription->GetChartAxisYValsCnt(i),oldChartDescription->GetChartAxisYValues(i));
			newChartDescr->SetChartDescriptionAxisZ(newChartInd,oldChartDescription->GetChartAxisZValsCnt(i),oldChartDescription->GetChartAxisZValues(i));
			newChartDescr->SetChartName(newChartInd,oldChartDescription->GetChartName(i));
			newChartInd++;
		}
	}
	activeDescription->SetChartsDescription(newChartDescr);
}
void CModelConstructorUI::DeleteChart(const int ChartInd,CChartDescription* ChartDescription)
{
	CChartDescription* oldChartDescription = ChartDescription;
	unsigned int chartCnt = oldChartDescription->GetChartCnt();
	unsigned int newChartInd = 0;

	CChartDescription* newChartDescr = new CChartDescription(chartCnt - 1,oldChartDescription->GetModelDim());
	for(unsigned int i = 0; i < chartCnt; i++)
	{
		if(i != ChartInd)
		{
			newChartDescr->SetChartDescriptionAxisX(newChartInd,oldChartDescription->GetChartAxisXValsCnt(i),oldChartDescription->GetChartAxisXValues(i));
			newChartDescr->SetChartDescriptionAxisY(newChartInd,oldChartDescription->GetChartAxisYValsCnt(i),oldChartDescription->GetChartAxisYValues(i));
			newChartDescr->SetChartDescriptionAxisZ(newChartInd,oldChartDescription->GetChartAxisZValsCnt(i),oldChartDescription->GetChartAxisZValues(i));
			newChartDescr->SetChartName(newChartInd,oldChartDescription->GetChartName(i));
			newChartInd++;
		}
	}
	delete ChartDescription;
	ChartDescription = newChartDescr;
}

void CModelConstructorUI::DeleteBrokenCharts(const CChartDescription* ChartDescription)
{
	const CChartDescription* oldChartDescription = ChartDescription;
	unsigned int chartCnt = oldChartDescription->GetChartCnt();
	unsigned int newChartInd = 0;
	
	unsigned int* delIndex = new unsigned int[chartCnt];
	unsigned int delCnt = 0;
	for(unsigned int i = 0; i < chartCnt; i++)
	{
		const unsigned int valsCntX = oldChartDescription->GetChartAxisXValsCnt(i);
		const unsigned int valsCntY = oldChartDescription->GetChartAxisYValsCnt(i);
		const unsigned int valsCntZ = oldChartDescription->GetChartAxisZValsCnt(i);

		if(valsCntX == 0 || valsCntX > 1)
		{
			delIndex[delCnt] = i;
			delCnt++;
			continue;
		}
		if(valsCntY == 0 )
		{
			delIndex[delCnt] = i;
			delCnt++;
			continue;
		}
		if(valsCntZ > 1) 
		{
			delIndex[delCnt] = i;
			delCnt++;			
			continue;	
		}
		if(valsCntZ == 1 && valsCntY > 1) 
		{
			delIndex[delCnt] = i;
			delCnt++;			
			continue;	
		}
	}

	if(delCnt > 0) CLog::GetInstance()->ReportWarning("Неверно заданы некоторые графики");

	CChartDescription* newChartDescr = new CChartDescription(chartCnt - delCnt,oldChartDescription->GetModelDim());
	for(unsigned int i = 0; i < chartCnt; i++)
	{
		bool needDelete = false;
		for(unsigned int j = 0; j < delCnt; j++) 
		{
			if(i == delIndex[j]) 
				needDelete = true;
		}
		if(!needDelete)
		{
			newChartDescr->SetChartDescriptionAxisX(newChartInd,oldChartDescription->GetChartAxisXValsCnt(i),oldChartDescription->GetChartAxisXValues(i));
			newChartDescr->SetChartDescriptionAxisY(newChartInd,oldChartDescription->GetChartAxisYValsCnt(i),oldChartDescription->GetChartAxisYValues(i));
			newChartDescr->SetChartDescriptionAxisZ(newChartInd,oldChartDescription->GetChartAxisZValsCnt(i),oldChartDescription->GetChartAxisZValues(i));
			newChartDescr->SetChartName(newChartInd,oldChartDescription->GetChartName(i));
			newChartInd++;
		}
	}

	delete [] delIndex;
	//delete ChartDescription;
	//ChartDescription = newChartDescr;
	activeDescription->SetChartsDescription(newChartDescr);
}

const int CModelConstructorUI::FindPosInModelsDescription(string ModelName) const
{
	int pos = -1;
	for(auto modelDesc = modelsDescription->begin(); modelDesc != modelsDescription->end(); modelDesc++)
	{
		if(ModelName.compare((*modelDesc)->GetName()) == 0) return (modelDesc - modelsDescription->begin());
	}
	return pos;
}

const int* CModelConstructorUI::ParseChartStrings(const char* AxisStr,unsigned int &RetValCnt)
{
	char* varName = new char[128];
	const char* tmpStr = AxisStr;

	RetValCnt = 0;
	const unsigned int maxRetValCnt = activeDescription->GetDimension(); 
	int* retValues = new int[maxRetValCnt];

	const char* currChar = tmpStr;
	unsigned int ind = 0;
	strcpy(varName,"");
	while(true)
	{
		if(RetValCnt >= maxRetValCnt) break;
		if(*currChar == '\0')
		{
			if(strlen(varName) > 0)	
			{
				varName[ind] = '\0';					
				for(auto param = valuesNames.begin(); param != valuesNames.end(); param++ )
				{
					if((*param).compare(varName) == 0)
					{
						retValues[RetValCnt] = param - valuesNames.begin() + 1;
						RetValCnt++;
					}
				}
				if(string("t").compare(varName) == 0)
				{
					retValues[RetValCnt] = 0;
					RetValCnt++;
				}
			}
			break;
		}
	  	if(*currChar == ',')
		{
			if(strlen(varName) > 0)
			{
				varName[ind] = '\0';					
				for(auto param = valuesNames.begin(); param != valuesNames.end(); param++ )
				{
					if((*param).compare(varName) == 0)
					{
						retValues[RetValCnt] = param - valuesNames.begin() + 1;
						RetValCnt++;
					}
				}
				if(string("t").compare(varName) == 0)
				{
					retValues[RetValCnt] = 0;
					RetValCnt++;
				}
				ind = 0;
				varName[ind] = '\0';
			}
		}
		if((*currChar >= 'A' && *currChar <= 'Z') || (*currChar >= 'a' && *currChar <= 'z'))
		{			
			varName[ind] = (*currChar);
			if(ind < 127) ind++;
		}
		currChar++;
	}
	delete [] varName;

	return retValues;
}

CChartDescription* CModelConstructorUI::ParseCharts()
{
	char* edId = new char[256];
	char* tmpStr = new char[256];
	
	const CChartDescription* oldChartDescription = activeDescription->GetChartsDescription();
	unsigned int chartCnt = oldChartDescription->GetChartCnt();
		
	CChartDescription* newChartDescription = new CChartDescription(chartCnt,oldChartDescription->GetModelDim());

	for(unsigned int i = 0; i < chartCnt; i++)
	{
		sprintf(edId,"ed_c_desc%d",i);
		const CUIControl* edChartDesc = UIWindow->FindControl(edId);
		sprintf(edId,"ed_c_x%d",i);
		const CUIControl* edAxisX = UIWindow->FindControl(edId);
		sprintf(edId,"ed_c_y%d",i);
		const CUIControl* edAxisY = UIWindow->FindControl(edId);
		sprintf(edId,"ed_c_z%d",i);
		const CUIControl* edAxisZ = UIWindow->FindControl(edId);

		GetDlgItemText(UIWindow->GetHandle(),edAxisX->GetId(),tmpStr,255);
		unsigned int valsCntX;		
		const int* valsX = ParseChartStrings(tmpStr,valsCntX);

		GetDlgItemText(UIWindow->GetHandle(),edAxisY->GetId(),tmpStr,255);
		unsigned int valsCntY;
		const int* valsY = ParseChartStrings(tmpStr,valsCntY);

		GetDlgItemText(UIWindow->GetHandle(),edAxisZ->GetId(),tmpStr,255);		
		unsigned int valsCntZ;
		const int* valsZ = ParseChartStrings(tmpStr,valsCntZ);
			
		if(valsCntX > 0) newChartDescription->SetChartDescriptionAxisX(i,valsCntX,valsX);
		else			 newChartDescription->SetChartDescriptionAxisX(i,0,NULL);
		
		if(valsCntY > 0) newChartDescription->SetChartDescriptionAxisY(i,valsCntY,valsY);
		else			 newChartDescription->SetChartDescriptionAxisY(i,0,NULL);
		
		if(valsCntZ > 0) newChartDescription->SetChartDescriptionAxisZ(i,valsCntZ,valsZ);
		else			 newChartDescription->SetChartDescriptionAxisZ(i,0,NULL);
				
		delete [] valsX;
		delete [] valsY;
		delete [] valsZ;		

		GetDlgItemText(UIWindow->GetHandle(),edChartDesc->GetId(),tmpStr,512);	
		newChartDescription->SetChartName(i,tmpStr);				
	}

	delete [] edId;
	delete [] tmpStr;
		
	return newChartDescription;
}

void CModelConstructorUI::ParseParams()
{
	char* edMathId = new char[256];
	char* tmpStr = new char[2048];

	const unsigned int oldDim = activeDescription->GetDimension();

	const CUIControl* edVars = UIWindow->FindControl("ed_model_vars");
	GetDlgItemText(UIWindow->GetHandle(),edVars->GetId(),tmpStr,255);
	valuesNames.clear();

	char* varName = new char[128];
	char* currChar = tmpStr;
	unsigned int ind = 0;
	strcpy(varName,"");
	while(true)
	{
		if(*currChar == '\0')
		{
			if(strlen(varName) > 0)	
			{
				varName[ind] = '\0';				
				bool isNewVal = true;
				for(auto param = valuesNames.begin(); param != valuesNames.end(); param++ )
				{
					if((*param).compare(varName) == 0) isNewVal = false;
				}
				if(isNewVal) valuesNames.push_back(varName);
			}
			break;
		}
	  	if(*currChar == ',')
		{
			if(strlen(varName) > 0)
			{
				varName[ind] = '\0';
				bool isNewVal = true;
				for(auto param = valuesNames.begin(); param != valuesNames.end(); param++ )
				{
					if((*param).compare(varName) == 0) isNewVal = false;
				}
				if(isNewVal) valuesNames.push_back(varName);
				ind = 0;
				varName[ind] = '\0';
			}
		}
		if((*currChar >= 'A' && *currChar <= 'Z') || (*currChar >= 'a' && *currChar <= 'z'))
		{			
			varName[ind] = (*currChar);
			if(ind < 127) ind++;
		}
		currChar++;
	}
	delete [] varName;

	const unsigned int newDim = valuesNames.size();	
	CMathParser** modelFormula = new CMathParser*[newDim];
	paramsNames.clear();

	haveParseErrors = false;
	for(unsigned int i = 0; i < newDim; i++ )
	{
		sprintf(edMathId,"ed_model_formula%d",i);
		const CUIControl* edMath = UIWindow->FindControl(edMathId);
		if(edMath != NULL)
		{
			GetDlgItemText(UIWindow->GetHandle(),edMath->GetId(),tmpStr,1024);
		}
		else
		{
			strcpy(tmpStr,"0");
		}

		int errorCnt = CLog::GetInstance()->GetErrorsCnt();
		modelFormula[i] = new CMathParser(tmpStr);
		errorCnt = CLog::GetInstance()->GetErrorsCnt() - errorCnt;
		if(errorCnt > 0) haveParseErrors = true;

		for(unsigned int j = 0; j < newDim; j++)
		{
			modelFormula[i]->variables.SetVarType(valuesNames.at(j).c_str(),VAR_VARIABLE);
		}
	}
	for(unsigned int i = 0; i < newDim; i++ )
	{
		for(int j = 0; j < modelFormula[i]->variables.varCnt; j++)
		{
			CVar* currVar = modelFormula[i]->variables.GetVarByIndex(j);
			if(currVar->type == VAR_UNDEF)
			{
				bool isNewParam = true;
				for(auto param = paramsNames.begin(); param != paramsNames.end(); param++ )
				{
					if((*param).compare(currVar->name) == 0) isNewParam = false;
				}
				if(isNewParam) paramsNames.push_back(currVar->name);
			}
		}
	}

	for(unsigned int i = 0; i < newDim; i++ )
	{
		delete modelFormula[i];
	}
	delete [] modelFormula;
	delete [] edMathId;	
	delete [] tmpStr;
}

void CModelConstructorUI::UpdateActiveDescription()
{
	char* tmpStr = new char[2048];
	char* idStr  = new char[256];

	bool isEditExist = true;

	const int paramCnt = paramsNames.size();
	const int valCnt = valuesNames.size();

	int count = (paramCnt > 0)? paramCnt : 1;
	double* paramValues = new double[count]; 
	double* paramMinValues = new double[count]; 
	double* paramMaxValues = new double[count]; 
	string* paramNames  = new string[count]; 
	string* paramDescr  = new string[count]; 

	count = (valCnt > 0)? valCnt : 1;
	string* valNames  = new string[count]; 
	string* valDescr  = new string[count]; 
	double* values = new double[count]; 
	double* minValues = new double[count]; 
	double* maxValues = new double[count]; 

	string* modelFormula = new string[count]; 
		
	const CUIControl* edName = UIWindow->FindControl("ed_model_name");
	GetDlgItemText(UIWindow->GetHandle(),edName->GetId(),tmpStr,256);
	string name = tmpStr; 

	const CUIControl* edDesc = UIWindow->FindControl("ed_model_desc");
	GetDlgItemText(UIWindow->GetHandle(),edDesc->GetId(),tmpStr,2048);
	string description = tmpStr; 

	for(int i = 0; i < valCnt; i++)
	{
		const int paramIndex = activeDescription->GetValueIndex(valuesNames.at(i));
		if(paramIndex == -1) isEditExist = false;
		else isEditExist = true;

		sprintf(idStr,"ed_model_formula%d",paramIndex);
		const CUIControl* edMath = UIWindow->FindControl(idStr);

		sprintf(idStr,"st_v_name%d",paramIndex);
		const CUIControl* editVName = UIWindow->FindControl(idStr);
	
		sprintf(idStr,"ed_v_desc%d",paramIndex);
		const CUIControl* editVDesc = UIWindow->FindControl(idStr);
	
		sprintf(idStr,"ed_v_init%d",paramIndex);
		const CUIControl* editVInit = UIWindow->FindControl(idStr);

		sprintf(idStr,"ed_v_min%d",paramIndex);
		const CUIControl* editVMinInit = UIWindow->FindControl(idStr);

		sprintf(idStr,"ed_v_max%d",paramIndex);
		const CUIControl* editVMaxInit = UIWindow->FindControl(idStr);
	
		if(isEditExist)
		{
			GetDlgItemText(UIWindow->GetHandle(),edMath->GetId(),tmpStr,2048);
			modelFormula[i] = tmpStr;
			GetDlgItemText(UIWindow->GetHandle(),editVName->GetId(),tmpStr,256);
			valNames[i] = tmpStr;
			GetDlgItemText(UIWindow->GetHandle(),editVDesc->GetId(),tmpStr,256);
			valDescr[i] = tmpStr;
			GetDlgItemText(UIWindow->GetHandle(),editVInit->GetId(),tmpStr,16);
			values[i] = atof(tmpStr);
			GetDlgItemText(UIWindow->GetHandle(),editVMinInit->GetId(),tmpStr,16);
			minValues[i] = atof(tmpStr);
			GetDlgItemText(UIWindow->GetHandle(),editVMaxInit->GetId(),tmpStr,16);
			maxValues[i] = atof(tmpStr);
		}
		else
		{
			strcpy(tmpStr,"0");
			modelFormula[i] = tmpStr;
			strcpy(tmpStr,valuesNames.at(i).c_str());
			valNames[i] = tmpStr;
			strcpy(tmpStr,"");
			valDescr[i] = tmpStr;
			strcpy(tmpStr,"0");
			values[i] = atof(tmpStr);
			strcpy(tmpStr,"0");
			minValues[i] = atof(tmpStr);
			strcpy(tmpStr,"200");
			maxValues[i] = atof(tmpStr);
		}
	}

	for(int i = 0; i < paramCnt; i++)
	{		
		const int paramIndex = activeDescription->GetParamIndex(paramsNames.at(i));
		if(paramIndex == -1) isEditExist = false;
		else isEditExist = true;

		sprintf(idStr,"st_p_name%d",paramIndex);
		const CUIControl* editVName = UIWindow->FindControl(idStr);

		sprintf(idStr,"ed_p_desc%d",paramIndex);
		const CUIControl* editVDesc = UIWindow->FindControl(idStr);

		sprintf(idStr,"ed_p_init%d",paramIndex);
		const CUIControl* editVInit = UIWindow->FindControl(idStr);

		sprintf(idStr,"ed_p_min%d",paramIndex);
		const CUIControl* editVMinInit = UIWindow->FindControl(idStr);

		sprintf(idStr,"ed_p_max%d",paramIndex);
		const CUIControl* editVMaxInit = UIWindow->FindControl(idStr);

		if(isEditExist)
		{
			GetDlgItemText(UIWindow->GetHandle(),editVName->GetId(),tmpStr,256);
			paramNames[i] = tmpStr;
			GetDlgItemText(UIWindow->GetHandle(),editVDesc->GetId(),tmpStr,256);
			paramDescr[i] = tmpStr;
			GetDlgItemText(UIWindow->GetHandle(),editVInit->GetId(),tmpStr,16);
			paramValues[i] = atof(tmpStr);
			GetDlgItemText(UIWindow->GetHandle(),editVMinInit->GetId(),tmpStr,16);
			paramMinValues[i] = atof(tmpStr);
			GetDlgItemText(UIWindow->GetHandle(),editVMaxInit->GetId(),tmpStr,16);
			paramMaxValues[i] = atof(tmpStr);
		}
		else
		{
			strcpy(tmpStr,paramsNames.at(i).c_str());
			paramNames[i] = tmpStr;
			strcpy(tmpStr,"");
			paramDescr[i] = tmpStr;
			strcpy(tmpStr,"0");
			paramValues[i] = atof(tmpStr);
			strcpy(tmpStr,"0");
			paramMinValues[i] = atof(tmpStr);
			strcpy(tmpStr,"100");
			paramMaxValues[i] = atof(tmpStr);
		}
	}

	const CChartDescription* chartDescription = ParseCharts();

	const CUIControl* edTime = UIWindow->FindControl("ed_t_init");
	GetDlgItemText(UIWindow->GetHandle(),edTime->GetId(),tmpStr,16);
	const double time = atof(tmpStr);

	const CUIControl* edMinTime = UIWindow->FindControl("ed_t_min");
	GetDlgItemText(UIWindow->GetHandle(),edMinTime->GetId(),tmpStr,16);
	const double minTime = atof(tmpStr);

	const CUIControl* edMaxTime = UIWindow->FindControl("ed_t_max");
	GetDlgItemText(UIWindow->GetHandle(),edMaxTime->GetId(),tmpStr,16);
	const double maxTime = atof(tmpStr);

	const CUIControl* edDTime = UIWindow->FindControl("ed_dt_init");
	GetDlgItemText(UIWindow->GetHandle(),edDTime->GetId(),tmpStr,16);
	const double dtime = atof(tmpStr);

	const CUIControl* edMinDTime = UIWindow->FindControl("ed_dt_min");
	GetDlgItemText(UIWindow->GetHandle(),edMinDTime->GetId(),tmpStr,16);
	const double minDTime = atof(tmpStr);

	const CUIControl* edMaxDTime = UIWindow->FindControl("ed_dt_max");
	GetDlgItemText(UIWindow->GetHandle(),edMaxDTime->GetId(),tmpStr,16);
	const double maxDTime = atof(tmpStr);

	bool havePositiveValues = true;
	LRESULT res = SendDlgItemMessage(UIWindow->GetHandle(),UIWindow->FindControl("ch_positive")->GetId(),BM_GETCHECK, 0, 0 );
	if(res == BST_CHECKED) havePositiveValues = true;
	else havePositiveValues = false;

	CModelDescription *oldDescription = activeDescription;
	activeDescription = new CModelDescription(name,description,paramCnt,paramDescr,paramNames,paramValues,valCnt,valDescr,valNames,values,time,dtime,modelFormula,(*chartDescription),havePositiveValues);
	activeDescription->SetValuesLimit(minValues,maxValues);
	activeDescription->SetParamsLimit(paramMinValues,paramMaxValues);
	activeDescription->SetTimeLimits(minTime,maxTime);
	activeDescription->SetDTLimits(minDTime,maxDTime);

	delete [] paramValues; 
	delete [] paramMinValues; 
	delete [] paramMaxValues; 
	delete [] paramNames; 
	delete [] paramDescr; 
	delete [] valNames; 
	delete [] valDescr; 
	delete [] values; 
	delete [] minValues; 
	delete [] maxValues; 
	delete [] modelFormula; 

	delete chartDescription;
	delete oldDescription;
	delete [] tmpStr;
	delete [] idStr;
}

void CModelConstructorUI::BuildUI()
{
	UIWindow->LockWindow();

	UIWindow->DestroyAllUIBlocks();	

	int lx = 0;
	int ly = 0;
	int uiWidth = UIWindow->GetWidth();
	int uHeight = UIWindow->GetHeight();
	HWND hwnd   = UIWindow->GetHandle();

	int currUIBlock = UIWindow->CreateUIBlock();
	
	UIWindow->CreateControl("0_st_frame","STATIC",10	,10	,uiWidth-25	,90,""			,SS_BLACKFRAME	,currUIBlock);
	UIWindow->CreateControl("0_st_text"	,"STATIC",20	,20	,60			,20	,"Модель:"	,SS_LEFT		,currUIBlock);
	
	CUIControl* comboBox = UIWindow->CreateControl("0_combox_model","COMBOBOX",80,20,uiWidth-105,400,"Модель:",CBS_DROPDOWNLIST,currUIBlock);
	SendDlgItemMessage(hwnd,comboBox->GetId(),CB_RESETCONTENT, 0, 0);
	for(auto modelDescription = modelsDescription->begin(); modelDescription != modelsDescription->end(); modelDescription++)
	{		
		SendDlgItemMessage(hwnd,comboBox->GetId(),CB_ADDSTRING, 0, (LPARAM)(*modelDescription)->GetName().c_str());
	}	
	SendDlgItemMessage(hwnd,comboBox->GetId(),CB_SETCURSEL, -1, (LPARAM)-1);
	UIWindow->CreateControl("0_add_model","BUTTON",20,45,(uiWidth-45)/2,20,"Выбрать модель"	,BS_SOLID,currUIBlock);
	UIWindow->CreateControl("0_add_new_model","BUTTON",20+(uiWidth-45)/2,45,(uiWidth-45)/2,20,"Новая модель"	,BS_SOLID,currUIBlock);
	UIWindow->CreateControl("0_exit","BUTTON",20,68,uiWidth-45,20,"Закрыть"	,BS_SOLID,currUIBlock);

	lx = 10;
	ly = 105;

	const int edH = 18;
	const int stH = 15;
	const int emptyH = 3;
	const int middleX = (uiWidth - lx - 20 )/2;
	const int thirdX  = (uiWidth - lx - 20 )/3;
	const int paramH  = edH + emptyH;
	
	if(activeDescription == NULL)
	{
		SetWindowPos(hwnd,HWND_TOP,0,0,uiWidth,ly+50,SWP_NOMOVE);
		UIWindow->UnlockWindow();
		UIWindow->RedrawWindow();
		return;
	}

	char* str_cnt = new char[255];
	char* tmp_val = new char[255];
	
	UIWindow->CreateControl("0_st_text","STATIC",lx,ly+2,thirdX,edH,"Название модели:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("ed_model_name","EDIT",lx+thirdX,ly,uiWidth-15-(lx+thirdX),edH,activeDescription->GetName(),WS_BORDER,currUIBlock);
	ly+=paramH;
	UIWindow->CreateControl("0_st_text","STATIC",lx,ly+2,thirdX,edH,"Описание модели:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("ed_model_desc","EDIT",lx+thirdX,ly,uiWidth-15-(lx+thirdX),edH*3,activeDescription->GetDescription(),WS_BORDER|ES_MULTILINE|ES_AUTOVSCROLL,currUIBlock);
	ly+=paramH + 2*edH + 5;


	const int oldLy = ly;
	string varsList = "";
	for(int i = 0; i < activeDescription->dimension; i++)
	{
		if(i != 0) varsList += ",";
		varsList += activeDescription->GetValueNames()[i];
	}
	UIWindow->CreateControl("0_st_text","STATIC",lx,ly+2,thirdX,edH,"Список переменных:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("ed_model_vars","EDIT",lx+thirdX,ly,uiWidth-105-(lx+thirdX+70),edH,varsList,WS_BORDER,currUIBlock);
	CUIControl* checkBox = UIWindow->CreateControl("ch_positive","BUTTON"	,lx+thirdX+uiWidth-105-(lx+thirdX+70),ly	,70,edH," >= 0",SS_LEFT|BS_AUTOCHECKBOX,currUIBlock);
	SendDlgItemMessage(hwnd,checkBox->GetId(),BM_SETCHECK, activeDescription->IsPositiveValues(), 0);				
	ly+=paramH;
	
	for(int i = 0; i < activeDescription->GetDimension(); i++)
	{
		sprintf(str_cnt,"%d",i);
		string str = "ed_model_formula" + string(str_cnt);
		UIWindow->CreateControl("st_formula","STATIC",lx,ly+2,35, edH,string(activeDescription->GetValueNames()[i]+"'="), SS_LEFT, currUIBlock);	
		UIWindow->CreateControl(str,"EDIT",lx+25,ly,uiWidth-105-(lx+25),edH,activeDescription->GetMathFormula()[i],WS_BORDER,currUIBlock);
		ly+=paramH;
	}
	UIWindow->CreateControl("b_parse_params","BUTTON",uiWidth-105,oldLy,90,ly-oldLy-3,"Применить"	,BS_SOLID,currUIBlock);

	ly+=10;

	UIWindow->CreateControl("0_st_text","STATIC",lx+25,ly,thirdX+50,edH,"Описание парметра:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("0_st_text","STATIC",lx+80+thirdX,ly, thirdX/2,edH,"Начальное значение:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("0_st_text","STATIC",lx+80+thirdX+thirdX/2,ly,thirdX/2,edH,"Min значение:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("0_st_text","STATIC",lx+80+2*thirdX,ly,thirdX/2,edH,"Max значение:",SS_LEFT,currUIBlock);	
	ly+=edH;

	const double* params = activeDescription->GetParams();
	for(int i = 0; i < activeDescription->GetParamCnt(); i++)
	{				
		sprintf(str_cnt,"%d",i);
		sprintf(tmp_val,"%.3f",params[i]);
	
		string str = "st_p_name" + string(str_cnt);
		UIWindow->CreateControl(str,"STATIC",lx,ly+2,25, edH,activeDescription->GetParamNames()[i], SS_LEFT, currUIBlock);
		str = "ed_p_desc" + string(str_cnt);
		UIWindow->CreateControl(str,"EDIT",lx+25,ly,thirdX+50, edH,activeDescription->GetParamDescription()[i], WS_BORDER, currUIBlock);
		str = "ed_p_init" + string(str_cnt);		
		UIWindow->CreateControl(str,"EDIT",lx+80+thirdX	,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);

		str = "ed_p_min" + string(str_cnt);	
		sprintf(tmp_val,"%.3f",activeDescription->GetMinParamsLimit()[i]);	
		UIWindow->CreateControl(str,"EDIT",lx+80+thirdX+thirdX/2 ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
		str = "ed_p_max" + string(str_cnt);	
		sprintf(tmp_val,"%.3f",activeDescription->GetMaxParamsLimit()[i]);	
		UIWindow->CreateControl(str,"EDIT",lx+80+2*thirdX ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
		   
		ly += paramH;			
	}
	ly += 5;

	const double* values = activeDescription->GetInitCnt();
	for(int i = 0; i < activeDescription->GetDimension(); i++)
	{				
		sprintf(str_cnt,"%d",i);
		sprintf(tmp_val,"%.3f",values[i]);

		string str = "st_v_name" + string(str_cnt);
		UIWindow->CreateControl(str,"STATIC",lx,ly+2,25, edH,activeDescription->GetValueNames()[i], SS_LEFT, currUIBlock);
		str = "ed_v_desc" + string(str_cnt);
		UIWindow->CreateControl(str,"EDIT",lx+25,ly,thirdX+50, edH,activeDescription->GetValueDescription()[i], WS_BORDER, currUIBlock);
		str = "ed_v_init" + string(str_cnt);		
		UIWindow->CreateControl(str,"EDIT",lx+80+thirdX	,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);

		str = "ed_v_min" + string(str_cnt);	
		sprintf(tmp_val,"%.3f",activeDescription->GetMinValuesLimit()[i]);	
		UIWindow->CreateControl(str,"EDIT",lx+80+thirdX+thirdX/2 ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
		str = "ed_v_max" + string(str_cnt);	
		sprintf(tmp_val,"%.3f",activeDescription->GetMaxValuesLimit()[i]);	
		UIWindow->CreateControl(str,"EDIT",lx+80+2*thirdX ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
		   
		ly += paramH;			
	}
	ly += 5;

	string str = "st_time";
	UIWindow->CreateControl(str,"STATIC",lx+25,ly,thirdX+50, edH,"Время моделирования:", SS_LEFT, currUIBlock);
	str = "ed_t_init";		
	sprintf(tmp_val,"%.0f",activeDescription->GetInitTime());

	UIWindow->CreateControl(str,"EDIT",lx+80+thirdX	,ly, thirdX/2 ,edH,tmp_val ,WS_BORDER,currUIBlock);

	str = "ed_t_min";
	sprintf(tmp_val,"%.0f",activeDescription->GetMinTimeLimit());	
	UIWindow->CreateControl(str,"EDIT",lx+80+thirdX+thirdX/2 ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
	str = "ed_t_max";
	sprintf(tmp_val,"%.0f",activeDescription->GetMaxTimeLimit());	
	UIWindow->CreateControl(str,"EDIT",lx+80+2*thirdX ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
	ly += paramH;			

	str = "st_dt";	
	UIWindow->CreateControl(str,"STATIC",lx+25,ly,thirdX+50, edH,"Шаг времени:", SS_LEFT, currUIBlock);
	str = "ed_dt_init";		
	sprintf(tmp_val,"%.5f",activeDescription->GetInitDT());
	UIWindow->CreateControl(str,"EDIT",lx+80+thirdX	,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
	str = "ed_dt_min";
	sprintf(tmp_val,"%.5f",activeDescription->GetMinDTLimit());	
	UIWindow->CreateControl(str,"EDIT",lx+80+thirdX+thirdX/2 ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
	str = "ed_dt_max";	
	sprintf(tmp_val,"%.5f",activeDescription->GetMaxDTLimit());	
	UIWindow->CreateControl(str,"EDIT",lx+80+2*thirdX ,ly, thirdX/2 ,edH, tmp_val, WS_BORDER, currUIBlock);
	ly += paramH;			
	ly += 10;

	UIWindow->CreateControl("0_st_text","STATIC",lx,ly,thirdX,edH,"График:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("0_st_text","STATIC",lx+thirdX,ly, thirdX/2,edH,"Ось Х:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("0_st_text","STATIC",lx+thirdX+thirdX/2,ly,thirdX/2,edH,"Ось Y:",SS_LEFT,currUIBlock);	
	UIWindow->CreateControl("0_st_text","STATIC",lx+2*thirdX,ly,thirdX/2,edH,"Ось Z:",SS_LEFT,currUIBlock);	
	ly+=edH;

	const CChartDescription* charts = activeDescription->GetChartsDescription();
	for(unsigned int i = 0; i < charts->GetChartCnt(); i++)
	{
		sprintf(str_cnt,"%d",i);
		string axisVals = "";
				
		str = "ed_c_desc" + string(str_cnt);
		UIWindow->CreateControl(str,"EDIT",lx,ly,thirdX, edH,activeDescription->GetChartsDescription()->GetChartName(i), WS_BORDER, currUIBlock);
			
		axisVals = "";
		for(unsigned int j = 0; j < charts->GetChartAxisXValsCnt(i); j++)
		{
			if(j != 0) axisVals += ",";
			int valId = charts->GetChartAxisXValue(i,j);
			if(valId == 0 ) axisVals += 't';
			else axisVals += activeDescription->GetValueNames()[valId - 1];
		}
		str = "ed_c_x" + string(str_cnt);		
		UIWindow->CreateControl(str,"EDIT",lx+thirdX	,ly, thirdX/2 ,edH, axisVals, WS_BORDER, currUIBlock);
			
		axisVals = "";
		for(unsigned int j = 0; j < charts->GetChartAxisYValsCnt(i); j++)
		{
			if(j != 0) axisVals += ",";
			int valId = charts->GetChartAxisYValue(i,j);
			if(valId == 0 ) axisVals += 't';
			else axisVals += activeDescription->GetValueNames()[valId - 1];
		}
		str = "ed_c_y" + string(str_cnt);
		UIWindow->CreateControl(str,"EDIT",lx+thirdX+thirdX/2 ,ly, thirdX/2 ,edH, axisVals, WS_BORDER, currUIBlock);
		
		axisVals = "";
		for(unsigned int j = 0; j < charts->GetChartAxisZValsCnt(i); j++)
		{
			if(j != 0) axisVals += ",";
			int valId = charts->GetChartAxisZValue(i,j);
			if(valId == 0 ) axisVals += 't';
			else axisVals += activeDescription->GetValueNames()[valId - 1];
		}
		str = "ed_c_z" + string(str_cnt);	
		UIWindow->CreateControl(str,"EDIT",lx+2*thirdX ,ly, thirdX/2 ,edH, axisVals, WS_BORDER, currUIBlock);

		str = "b_c_del" + string(str_cnt);	
		UIWindow->CreateControl(str,"BUTTON",lx+2*thirdX + thirdX/2 ,ly, thirdX/2 ,edH, "Удалить", BS_SOLID, currUIBlock);
		   
		ly += paramH;	
	}
	str = "b_c_add";	
	UIWindow->CreateControl(str,"BUTTON",lx+2*thirdX ,ly, thirdX ,edH, "Добавить график", BS_SOLID, currUIBlock);
	ly += edH;	
	ly += 5;	

	str = "b_save";
	UIWindow->CreateControl(str,"BUTTON",lx ,ly, thirdX*3 ,edH, "Сохранить модель", BS_SOLID, currUIBlock);
	ly += edH;	
	ly += 5;	

	str = "b_del";
	UIWindow->CreateControl(str,"BUTTON",lx ,ly, thirdX*3 ,edH, "Удалить модель", BS_SOLID, currUIBlock);
	ly += edH;	
	ly += 5;

	delete [] str_cnt;
	delete [] tmp_val;

	SetWindowPos(hwnd,HWND_TOP,0,0,uiWidth,ly+50,SWP_NOMOVE);
	UIWindow->UnlockWindow();
	UIWindow->RedrawWindow();
}

void CModelConstructorUI::ShowWindow(const bool IsVisible)
{	
	if(IsVisible == true)
	{
		UIWindow->ShowWindow();
		BuildUI();
	}
	else
	{
		UIWindow->HideWindow();
	}
}

const bool CModelConstructorUI::IsWindowVisible() const
{
	return UIWindow->IsWindowVisible();
}

void CModelConstructorUI::SetMainProgPointer(CProgMain* MainProgPtr)
{
	UIWindow->SetMainProgPointer(MainProgPtr);
}