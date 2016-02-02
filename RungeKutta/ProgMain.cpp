#include "ProgMain.h"
#include "VideoManeger.h"
#include "Chart.h"
#include "ChartContainer.h"
#include "EcoCL.h"
#include "ModelDescription.h"
#include "WinUI.h"
#include "ModelConstructorUI.h"
#include "MathParser.h"
#include "Log.h"
#include "ChartColorScheme.h"

CProgMain::CProgMain(void)
{
	charts = new CChartCollection();

	quit = false;
	UIWindow = new CWinUI();
	VideoManeger = new CVideoManeger();

	mouseButtonDown = false;
	mouseMoveAction = MOUSE_ACTION_NONE;

	modelConstructor = NULL;
	descriptionList  = NULL;

	InitDescriptionList();
}

CProgMain::~CProgMain(void)
{
	for(auto it = models.begin(); it != models.end(); it++)
	{
		if((*it))delete (*it);
	}
	delete VideoManeger;
	delete UIWindow;	
	delete modelConstructor;

	if(descriptionList)
	{
		for(auto modelDescription = descriptionList->begin(); modelDescription != descriptionList->end(); modelDescription++)
		{
			delete (*modelDescription);
		}
		descriptionList->clear();
		delete descriptionList;
	}
	delete CLog::GetInstance();
}

void CProgMain::Draw() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, VideoManeger->width, VideoManeger->height, 0, 2000, -2000);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);	
	glDepthFunc(GL_ALWAYS);
	
	//glShadeModel(GL_SMOOTH);							// Включается плавное затенение
	//glEnable(GL_LINE_SMOOTH);							// Включается сглаживание линий
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);				// Выбирается самый качественный
														// режим сглаживания для линий
	glEnable(GL_BLEND);									// Включается смешение цветов, необходимое
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// для работы сглаживания 

	//////////////////////////////////////////////////////

	charts->DrawAllCharts();
}

double nativeCode(double c)
{
	double a = 10;
	double b = 5.5;
	double d = 100;
	return (2*a - cos(b/c)/d+42.3);
}

void testParse()
{
	int testsCnt = 20000000;

	double rezParse = 0;
	double rezNative = 0;
	double i;
	
	CMathParser* mathParser = new CMathParser("(2*a-(b+c)/d+42.3)");
	mathParser->variables.SetVar("a",VAR_CONSTANT,10);
	mathParser->variables.SetVar("b",VAR_CONSTANT,5.5);
	mathParser->variables.SetVar("d",VAR_CONSTANT,100);	
	mathParser->variables.SetVar("c",VAR_VARIABLE,&i);
	
	double parseT = GetTickCount();	
	for(i = 1; i < testsCnt; i++)
	{		
		rezParse += mathParser->Execute();
	}			

	parseT = GetTickCount() - parseT;
	cout<<"Parse time : "<<parseT<<" rezult: "<<rezParse<<endl; 

	double nativeT = GetTickCount();
	for(i = 1; i < testsCnt; i++)
	{
		rezNative += nativeCode(i);
	}	
	nativeT = GetTickCount() - nativeT;
	cout<<"Native time: "<<nativeT<<" rezult: "<<rezNative<<endl;
	
	cout<<endl;
	cout<<"Input  lexemes: "<<mathParser->GetInputStr()<<endl;
	cout<<"Lexemes in RPN: "<<mathParser->GetRPNStr()<<endl;
	cout<<"Optimized lexemes in RPN: "<<mathParser->GetOptimizedRPNStr()<<endl;
	cout<<endl;
	cout<<"Parse  per sec: "<<(double)testsCnt*1000/parseT<<endl;
	cout<<"Native per sec: "<<(double)testsCnt*1000/nativeT<<endl;
	cout<<"Parse time / Native time: "<<(double)parseT/nativeT<<endl;
}

void CProgMain::Init()
{
	srand((Uint32)time(0));
	quit = false;

	UIWindow->CreateUIWindow();
	UIWindow->SetMainProgPointer(this);

	modelConstructor = new CModelConstructorUI(descriptionList);
	modelConstructor->SetMainProgPointer(this);
	modelConstructor->ShowWindow(false);

	CreateModel(0);
	BuildUI();

	//testParse();
}

void CProgMain::BuildUI()
{
	UIWindow->LockWindow();

	UIWindow->DestroyAllUIBlocks();

	int lx      = 0;
	int ly      = 0;
	int uiWidth = UIWindow->GetWidth();
	int uHeight = UIWindow->GetHeight();
	HWND hwnd   = UIWindow->GetHandle();

	int currUIBlock = UIWindow->CreateUIBlock();

	UIWindow->CreateControl("0_st_frame","STATIC",10	,10		,uiWidth-25	,90,""			,SS_BLACKFRAME	,currUIBlock);
	UIWindow->CreateControl("0_st_text"	,"STATIC",10+10	,10+10	,60			,20	,"Модель:"	,SS_LEFT		,currUIBlock);
	
	CUIControl* comboBox = UIWindow->CreateControl("0_combox_model","COMBOBOX",10+10+60,10+7,uiWidth-25-80,400,"Модель:",CBS_DROPDOWNLIST,currUIBlock);
	SendDlgItemMessage(hwnd,comboBox->GetId(),CB_RESETCONTENT, 0, 0);

	for(auto modelDescription = descriptionList->begin(); modelDescription != descriptionList->end(); modelDescription++)
	{
		SendDlgItemMessage(hwnd,comboBox->GetId(),CB_ADDSTRING, 0, (LPARAM)(*modelDescription)->GetName().c_str());
		//comboBox->SendMessageToControl(CB_ADDSTRING, 0, (LPARAM)(*modelDescription)->GetName().c_str());
	}	
	SendDlgItemMessage(hwnd,comboBox->GetId(),CB_SETCURSEL, -1, (LPARAM)-1);
	
	UIWindow->CreateControl("0_add_model"	,"BUTTON",10+10,10+35,(uiWidth-45)/2,20,"Выбрать модель"	,BS_SOLID,currUIBlock);
	UIWindow->CreateControl("0_model_edit"	,"BUTTON",10+10+(uiWidth-45)/2,10+35,(uiWidth-45)/2+1,20,"Редактор моделей"	,BS_SOLID,currUIBlock);
	UIWindow->CreateControl("0_exit"		,"BUTTON",10+10,10+60,uiWidth-45,20,"Выход"		,BS_SOLID,currUIBlock);

	lx = 10;
	ly = 105;

	const int edH     = 18;
	const int stH     = 15;
	const int emptyH  = 3;
	const int middleX = (uiWidth - lx - 30)/2;
	const int thirdX  = (uiWidth - lx - 30)/3;
	const int paramH  = edH + emptyH;
	
	char* str_cnt = new char[255];
	char* tmp_val = new char[255];

	for(auto model = models.begin(); model != models.end(); model++)
	{
		const CModelDescription* currDesc = (*model)->GetModelDescription(); 
		CEcoModel* currModel = (*model); 
		const int dim = currDesc->GetDimension();

		currUIBlock = UIWindow->CreateUIBlock();
		currModel->SetUIBlockId(currUIBlock);

		if(currModel->GetUIState())
		{
			UIWindow->CreateControl("st_frame"			,"BUTTON",lx		,ly		,uiWidth-25,currDesc->GetParamCnt()*paramH + currDesc->GetDimension()*paramH+2*25+paramH+70	,currDesc->GetName(),BS_GROUPBOX,currUIBlock);
			UIWindow->CreateControl("frame_expand"		,"BUTTON",uiWidth-50,ly+2	,25,12,"-",BS_FLAT	,currUIBlock);
			
			ly += 20;
			for(int i = 0; i < currDesc->GetParamCnt(); i++)
			{				
				sprintf(str_cnt,"%d",i);
				sprintf(tmp_val,"%.3f",currModel->params[i]);

				string str = "st_p" + string(str_cnt);
				UIWindow->CreateControl(str,"STATIC",lx+10			,ly+2,thirdX*2	,stH,currDesc->GetParamDescription()[i]	,SS_LEFT	,currUIBlock);
				str = "ed_p" + string(str_cnt);			
				UIWindow->CreateControl(str,"EDIT"	,lx+5+thirdX*2	,ly  ,thirdX	,edH,string(tmp_val)						,WS_BORDER	,currUIBlock);
		   
				ly += paramH;			
			}
			ly += 5;
			for(int i = 0; i < dim; i++)
			{				
				sprintf(str_cnt,"%d",i);
				sprintf(tmp_val,"%.3f",currModel->Rezult[i]);

				string str = "st_val" + string(str_cnt);
				UIWindow->CreateControl(str,"STATIC",lx+10			,ly+2,thirdX*2	,stH,currDesc->GetValueDescription()[i]	,SS_LEFT				,currUIBlock);
				str =  "ed_val" + string(str_cnt);			
				UIWindow->CreateControl(str,"EDIT"	,lx+5+thirdX*2	,ly  ,thirdX	,edH,string(tmp_val)						,WS_BORDER|ES_OEMCONVERT,currUIBlock);
		   
				ly += paramH;
			}
			ly += 5;

			string str = "st_time";
			UIWindow->CreateControl(str,"STATIC",lx+10			,ly,middleX,stH,"Время моделирования",SS_LEFT,currUIBlock);
			str = "st_dt";
			UIWindow->CreateControl(str,"STATIC",lx+5+middleX	,ly,middleX,stH,"Шаг времени"		,SS_LEFT,currUIBlock);

			sprintf(tmp_val,"%.0f",currModel->GetEndTime());
			str = "ed_time";
			UIWindow->CreateControl(str,"EDIT",lx+10		,ly+stH,middleX,edH,tmp_val,WS_BORDER|ES_OEMCONVERT,currUIBlock);
			sprintf(tmp_val,"%.5f",currModel->GetDeltaTime());
			str = "ed_dt";
			UIWindow->CreateControl(str,"EDIT",lx+5+middleX	,ly+stH,middleX,edH,tmp_val,WS_BORDER|ES_OEMCONVERT,currUIBlock);

			ly += paramH + edH;
			ly += 0;

			str = "st_graph:";
			CUIControl* checkBox;		
			UIWindow->CreateControl("st_frame_g","BUTTON",lx+10,ly,uiWidth-45,40,"Графики:",BS_GROUPBOX,currUIBlock);

			const unsigned int chartCnt = currDesc->GetChartsDescription()->GetChartCnt();
			int checkSize = 100;
			if(chartCnt != 0 ) checkSize = (uiWidth - lx - 40) / chartCnt; 
			for(unsigned int i = 0; i < chartCnt; i++)
			{
				sprintf(tmp_val,"cb%d",i);
				string chartName = currDesc->GetChartsDescription()->GetChartName(i);
				
				checkBox = UIWindow->CreateControl(tmp_val,"BUTTON"	,lx+15+checkSize*i	,ly+3+stH	,checkSize-10,stH,chartName,SS_LEFT|BS_AUTOCHECKBOX,currUIBlock);
				SendDlgItemMessage(hwnd,checkBox->GetId(),BM_SETCHECK, currModel->GetCharts()->GetChartVisibility(i), 0);				
			}
						
			ly += paramH + edH;
			ly += 5;

			str = "apply";
			UIWindow->CreateControl(str,"BUTTON",lx+10				,ly,middleX,20,"Применить"	,BS_SOLID,currUIBlock);
			str = "close";
			UIWindow->CreateControl(str,"BUTTON",lx + 5 + middleX	,ly,middleX,20,"Закрыть"	,BS_SOLID,currUIBlock);		
					
			ly += 30;
		}
		else
		{
			UIWindow->CreateControl("st_frame"		,"BUTTON",lx		,ly		,uiWidth-25	,25	,currDesc->GetName(),BS_GROUPBOX,currUIBlock);
			UIWindow->CreateControl("frame_expand"	,"BUTTON",uiWidth-50,ly+2	,25			,12	,"+"				,BS_FLAT	,currUIBlock);
			ly += 30;
		}
	}

	SetWindowPos(hwnd,HWND_TOP,0,0,uiWidth,ly+50,SWP_NOMOVE);
	UIWindow->UnlockWindow();
	UIWindow->RedrawWindow();

	delete [] str_cnt;
	delete [] tmp_val;
}

void CProgMain::UIEvents(const HWND Hwnd,const long int ControlID)
{ 
	bool NeedBuildUI = false;
	HWND hwnd = UIWindow->GetHandle();

	if(Hwnd != hwnd) 
	{
		modelConstructor->UIEvents(Hwnd,ControlID);
		const CUIControl *eventControl = modelConstructor->GetUIWindow()->FindControl(ControlID);
		if(eventControl == NULL) return;
		if( eventControl->GetName().compare("0_exit") == 0 )
		{
			UIWindow->ShowWindow();
			NeedBuildUI = true;
			BuildUI();	
		}
	}

	const CUIControl *eventControl = UIWindow->FindControl(ControlID);
	if(eventControl == NULL) return;

	if( eventControl->GetName().compare("0_exit") == 0 )
	{
		quit = true;
		NeedBuildUI = true;	
	}
	else
	if( eventControl->GetName().compare("0_model_edit") == 0 )
	{
		DeleteAllModels();
		UIWindow->HideWindow();
		modelConstructor->ShowWindow(!modelConstructor->IsWindowVisible());

		int sel = SendDlgItemMessage(hwnd,UIWindow->FindControl("0_combox_model")->GetId(),CB_GETCURSEL,0,0);
		modelConstructor->SelectModel(sel);
		NeedBuildUI = true;	
	}
	else
	if( eventControl->GetName().compare("0_add_model") == 0 )
	{		
		int sel = SendDlgItemMessage(hwnd,UIWindow->FindControl("0_combox_model")->GetId(),CB_GETCURSEL,0,0);
		if( sel != -1)
		{
			for(auto model = models.begin(); model != models.end(); model++)
			{
				(*model)->SetUIState(false);						
			}
			CreateModel(sel);	
			NeedBuildUI = true;
		}
	}
	else
	{
		CEcoModel* currModel = NULL;
		int currBlockId = eventControl->GetBlockId();
		for(auto model = models.begin(); model != models.end(); model++)
		{
			if((*model)->GetUIBlockId() == eventControl->GetBlockId())
			{
				currModel = (*model);
				break;
			}		
		}
		if(currModel != NULL)
		{
			if(eventControl->GetName().compare("apply") == 0 )
			{	
				bool isFindDescription = false;
				const CModelDescription* currModelDescription = currModel->GetModelDescription();
				for(auto modelDescription = descriptionList->begin(); modelDescription != descriptionList->end(); modelDescription++)
				{
					if(currModelDescription == (*modelDescription)) isFindDescription = true;
				}
				if(isFindDescription)
				{
					UpdateModel(currModel);
					UIWindow->DestroyUIBlock(currBlockId);
					NeedBuildUI = true;
				}
			}
			else
			if(eventControl->GetName().compare("close") == 0)
			{
				DeleteModel(currModel);		
				UIWindow->DestroyUIBlock(currBlockId);
				NeedBuildUI = true;
			}
			else			
			if(eventControl->GetName().compare("frame_expand") == 0)
			{	
				currModel->SetUIState(!currModel->GetUIState());
				NeedBuildUI = true;	
			}
			else
			{
				const unsigned int chartCnt = currModel->GetModelDescription()->GetChartsDescription()->GetChartCnt();		
				char* checkId = new char[255];
				for(unsigned int i = 0; i < chartCnt; i++)
				{
					sprintf(checkId,"cb%d",i);
					if(eventControl->GetName().compare(checkId) == 0)
					{
						LRESULT res = SendDlgItemMessage(hwnd,UIWindow->FindControl(checkId,currBlockId)->GetId(),BM_GETCHECK, 0, 0 );						
						if( res == BST_CHECKED)
						{				
							currModel->GetCharts()->SetChartVisibility(true,i);
						}
						else
						{					
							currModel->GetCharts()->SetChartVisibility(false,i);
						}
						NeedBuildUI = true;	
					}
				}
				delete [] checkId;
			}			
		}
	}

	if(NeedBuildUI)
	{
		BuildUI();	
		CVideoManeger::Redraw();
	}
}

void CProgMain::InitDescriptionList()
{
	descriptionList = CModelDescription::LoadFromeFile();

	//int chartDimsX[2] = {1,2};
	//int chartDimsY[1] = {2};
	//int chartDimsZ[1] = {0};
	//CChartDescription oneChartModel(1,1);
	//oneChartModel.SetChartDescription("Перемен",0,1,chartDimsX,1,chartDimsZ,0,chartDimsZ);
	//CChartDescription threeChartModel(3,2);
	//threeChartModel.SetChartDescription("Перемен",0,2,chartDimsX,1,chartDimsZ,0,chartDimsZ);
	//threeChartModel.SetChartDescription("Фазовый",1,1,chartDimsX,1,chartDimsY,0,chartDimsZ);
	//threeChartModel.SetChartDescription("Интегр.",2,1,chartDimsX,1,chartDimsY,1,chartDimsZ);

	//string _2dsis[2]={"жертв","хищников"};
	//string _1dsis="особей";
	//string params[10] = {"a","b","c","d","e","f","k","j","h","l"};
	//string vals[2] = {"x","y"};
	//
	//string lvd[4] = {"к. рождаемости жертв","к. гибели жертв","к. гибели хищников","к. рождаемости хищников"};	
	//double lvdp[4] = {1.9	,0.6	,1.8	,1};
	//double lvdv[2] = {5	,3};
	//string formula[2] = {"(a-b*y)*x","(-c+d*x)*y"};
	//descriptionList->push_back(new CModelDescription("Лотка-Вольтерра","простейшая модель хищиник-жертва",4,lvd,params,lvdp,2,_2dsis,vals,lvdv,30,0.01,formula,threeChartModel));

	//string lvmd[6]={"к. рождаемости жертв","к. гибели жертв","к. гибели хищников","к. рождаемости хищников","к. ораничения кол-ва жертв"	,"к. ораничения кол-ва хищников"};
	//double lvmdp[6]={1.9	,0.6	,1.8	,1,0.01,0.01};
	//double lvmdv[2]={5	,3};	
	//formula[0] = "( a-b*y-e*x)*x";
	//formula[1] = "(-c+d*x-f*y)*y";
	//descriptionList->push_back(new CModelDescription("Модифицированая Лотка-Вольтерра","модифицированая модель хищиник-жертва",6,lvmd,params,lvmdp,2,_2dsis,vals,lvmdv,30,0.01,formula,threeChartModel));

	//string htd[6]={"r","w","s","J","K"	,"D"};
	//double htdp[6]={1	,1	,0.2	,0.5,7,1};
	//double htdv[2]={5	,3};	
	//formula[0] = "(a*(1-x/e))*x-b*y*x/(f+x)";
	//formula[1] = "(c*(1-d*y/x))*y";
	//descriptionList->push_back(new CModelDescription("Модель Холлинга—Тэннера","модифицированая модель хищиник-жертва",6,htd,params,htdp,2,_2dsis,vals,htdv,100,0.01,formula,threeChartModel));

	//string rmd[6]={"к. рождаемости жертв","к. гибели жертв","к. гибели хищников","к. рождаемости хищников","k"	,"к. ораничения среды"};
	//double rmdp[6]={1.7	,1.098	,2	,2,4.75,100};
	//double rmdv[2]={7	,8};
	//formula[0] = "a*x*(1-(x/f))-((b*x*y)/(x+e))";
	//formula[1] = "d*((b*x*y)/(x+e))-c*y";
	//descriptionList->push_back(new CModelDescription("Розенцвейга-Макартура","модифицированая модель хищиник-жертва",6,rmd,params,rmdp,2,_2dsis,vals,rmdv,300,0.01,formula,threeChartModel));

	//string nrd[1] = {"к. прироста"};
	//double nrdp = 0.5;
	//double nrdv = 4;
	//formula[0] = "a*x"; 
	//descriptionList->push_back(new CModelDescription("Нормального размножения","скорость прироста пропорциональна наличному количеству особей",1,nrd,params,&nrdp,1,&_1dsis,vals,&nrdv,10,0.01,formula,oneChartModel));
	//
	//string logd[2]={"к. прироста","к. убыли"};
	//double logdp[2]={5,0.5};
	//double logdv=5;
	//formula[0] = "(a-b*x)*x"; 
	//descriptionList->push_back(new CModelDescription("Размножения с учетом конкуренции","С увеличением числа особей конкуренция из-за пищи приводит к уменьшению скорости прироста",2,logd,params,logdp,1,&_1dsis,vals,&logdv,10,0.01,formula,oneChartModel));	
	//
	//nrdp=0.2;
	//nrdv=1;
	//formula[0] = "a*x*x"; 
	//descriptionList->push_back(new CModelDescription("Взрыва","скорость прироста пропорциональна количеству пар особей",1,nrd,params,&nrdp,1,&_1dsis,vals,&nrdv,5,0.01,formula,oneChartModel));
	//
	//string fcd[3]={"к. прироста","к. убыли","скорость отлова"};
	//double fcdp[3]={1,0.5,0.52};
	//double fcdv=4;
	//formula[0] = "(a-b*x)*x-c"; 
	//descriptionList->push_back(new CModelDescription("Квоты отлова","Отлавливаем часть популяции с постоянной скоростью",3,fcd,params,fcdp,1,&_1dsis,vals,&fcdv,30,0.01,formula,oneChartModel));	

	//formula[0] = "(a-b*x)*x-c*x"; 
	//descriptionList->push_back(new CModelDescription("Отлов с относительной квотой","Отлавливаем часть популяции с относительной скоростью",3,fcd,params,fcdp,1,&_1dsis,vals,&fcdv,30,0.01,formula,oneChartModel));	
}

void CProgMain::UpdateModel(CEcoModel* oldModel)
{
	CEcoModel* model;
	HWND hwnd = UIWindow->GetHandle();
	const int UIBlockId = oldModel->GetUIBlockId(); 
	const CModelDescription* modelDescription = oldModel->GetModelDescription();

	double dt=0;
	double time=0;
	char* tmp_str=new char[255];
	char* controlName = new char[255];

	double* params = new double[modelDescription->GetParamCnt()];
	double* values = new double[modelDescription->GetDimension()];

	const CUIControl* control;

	for(int i = 0; i < modelDescription->GetParamCnt(); i++)
	{
		sprintf(controlName,"%s%d",string("ed_p").c_str(),i);
		control = UIWindow->FindControl(controlName,UIBlockId);
		GetDlgItemText(hwnd,control->GetId(),tmp_str,10);
		params[i] = atof(tmp_str);	
	}
	for(int i = 0; i < modelDescription->GetDimension(); i++)
	{
		sprintf(controlName,"%s%d",string("ed_val").c_str(),i);
		control = UIWindow->FindControl(controlName,UIBlockId);
		GetDlgItemText(hwnd,control->GetId(),tmp_str,10);
		values[i] = atof(tmp_str);	
	}
	control = UIWindow->FindControl("ed_time",UIBlockId);
	GetDlgItemText(hwnd,control->GetId(),tmp_str,10);
	time = atof(tmp_str);
	control = UIWindow->FindControl("ed_dt",UIBlockId);
	GetDlgItemText(hwnd,control->GetId(),tmp_str,10);
	dt=atof(tmp_str);

	if(time <= 0) time = 1;
	if(dt <= 0)   dt = 0.001;

	if(time > modelDescription->GetMaxTimeLimit())	time =  modelDescription->GetMaxTimeLimit();
	if(dt	> modelDescription->GetMaxDTLimit())	dt =  modelDescription->GetMaxDTLimit();
	if(dt	< modelDescription->GetMinDTLimit())	dt =  modelDescription->GetMinDTLimit();

	model = new CEcoModel(params,modelDescription);
	model->SetInit(0,values);
	model->StartModel(dt,time);

	if(!model->IsBroken())
	{
		model->InitCharts(oldModel);
		DeleteModel(oldModel);	
		charts->AddCharts(model->GetCharts());
		models.push_back(model);
	}
	else
	{
		CLog::GetInstance()->ReportWarning("Проверьте значения параметров модели.");
		DeleteModel(oldModel);	
		DeleteModel(model);
	}

	delete [] tmp_str;
	delete [] controlName;
	delete [] params;
	delete [] values;
}

void CProgMain::CreateModel(const int ModelType)
{
	const double* params;
	CEcoModel* model;

	if(descriptionList->size() == 0)
	{
		CLog::GetInstance()->ReportWarning("Для начала работы создайте новую модел в редакторе.");
		return;
	}

	CModelDescription* currDescription = descriptionList->at(ModelType);

	double time = currDescription->GetInitTime();
	double dt	= currDescription->GetInitDT();

	if(time > currDescription->GetMaxTimeLimit())	time =  currDescription->GetMaxTimeLimit();
	if(dt	> currDescription->GetMaxDTLimit())		dt =  currDescription->GetMaxDTLimit();
	if(dt	< currDescription->GetMinDTLimit())		dt =  currDescription->GetMinDTLimit();

	params = currDescription->GetParams();
	model = new CEcoModel(params,currDescription);
	model->SetInit(0,currDescription->GetInitCnt());
	model->StartModel(currDescription->GetInitDT(),currDescription->GetInitTime());

	if(!model->IsBroken())
	{
		model->InitCharts();
		CModelCharts* modelCharts = model->GetCharts();

		charts->AddAndArrangeCharts(modelCharts);
		models.push_back(model);
	}
	else
	{
		DeleteModel(model);
	}
}

const bool CProgMain::DeleteModel(CEcoModel* Model)
{
	bool rez = false;

	for(auto model = models.begin(); model != models.end();)
	{	
		if((*model) == Model)
		{			
			model = models.erase(model);
			rez = true;

		}else model++;
	}
	if(Model)delete Model;
	return true;
}

void CProgMain::DeleteAllModels()
{
	for(auto it = models.begin(); it != models.end(); it++)
	{
		if((*it))delete (*it);
	}
	models.clear();
}

const bool CProgMain::IsWorking() const
{
	return !quit;
}

void CProgMain::SDLEvents()
{
	SDL_WaitEvent(&(VideoManeger->event));
	unsigned int maxId; 
	CChartColorScheme* clSh = NULL;
	
	switch (VideoManeger->event.type)
	{ 
	case SDL_QUIT:
		quit = true;
		break;
	case SDL_VIDEORESIZE:
		VideoManeger->ResizeWindow(VideoManeger->event.resize.w,VideoManeger->event.resize.h);
	break;
	case SDL_MOUSEMOTION:
		MouseMove(VideoManeger->event.motion.x,	VideoManeger->event.motion.y,VideoManeger->event.motion.xrel, VideoManeger->event.motion.yrel);
	break;
    case SDL_MOUSEBUTTONDOWN:
		MouseBDown(VideoManeger->event.button.x, VideoManeger->event.button.y,VideoManeger->event.button.button);
		mouseButtonDown=true;
    break;
	case SDL_MOUSEBUTTONUP:
		mouseButtonDown=false;
		mouseMoveAction = MOUSE_ACTION_NONE;
	break;
	case SDL_KEYDOWN:
		switch (VideoManeger->event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			quit = true;
			break;
		case SDLK_c:
			maxId = charts->GetMaxChartId();
			if(clSh)delete clSh;
			clSh = new CChartColorScheme();
			for(unsigned int i = 0; i <= maxId; i++)
			{				
				CChart* chart = charts->GetChartUnsafe(i);		
				if(chart == NULL) continue;

				chart->SetChartColorScheme(clSh);
			}
			break;
		case SDLK_F1:
			SDL_WM_ToggleFullScreen(VideoManeger->screen);
			break;
		case SDLK_SPACE:				
			break;	
		case SDLK_1:				
			break;			
		default:
			break;
		}
	}
}

void CProgMain::InitVideo() const
{
	VideoManeger->Init();
	// ----- OpenGL init ---------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, VideoManeger->width, VideoManeger->height, 0, -2000, 2000);
	glMatrixMode(GL_MODELVIEW);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	

	glClearDepth(1.0f);									// Устанавливается значение для
														// заполнения буфера глубины по умолчанию
	glEnable(GL_DEPTH_TEST);							// Включается тест глубины
	glDepthFunc(GL_LESS);								// Устанавливается значение, используемое
														// в сравнениях при использовании
														// буфера глубины

	glShadeModel(GL_SMOOTH);							// Включается плавное затенение
//	glEnable(GL_LINE_SMOOTH);							// Включается сглаживание линий
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);				// Выбирается самый качественный
														// режим сглаживания для линий
//	glEnable(GL_BLEND);									// Включается смешение цветов, необходимое
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// для работы сглаживания и задается способ смешения

	glEnable(GL_TEXTURE_2D);
	
}														

void CProgMain::MouseBDown(const int x,const int y,const int button)
{
	int hitTestResult = charts->HitTest(x,y);
	if( hitTestResult == CHART_NONE) return;
	charts->SetTopChart(charts->GetChartId(x,y));

	if(button == 1)
	{		
		if( hitTestResult == CHART_BUTTON_CLOSE )
		{				
			for(auto model = models.begin(); model != models.end(); model++)
			{
				const unsigned int chartId = (*model)->GetCharts()->GetChartId(charts->GetTopChart());
				if(chartId != MAX_CHARTS_CNT)
				{
					const int blockId = (*model)->GetUIBlockId();
					char* checkId = new char[255];
					sprintf(checkId,"cb%d",chartId);

					charts->SetChartVisibility(false,0);
					const CUIControl* checkBox =  UIWindow->FindControl(checkId,blockId);					
					if(checkBox != NULL)
					{
						SendDlgItemMessage(UIWindow->GetHandle(),checkBox->GetId(),BM_SETCHECK, 0, 0);						
					}
					delete [] checkId;
				}
			}			
		}
		else
		if( hitTestResult == CHART_BUTTON_VEC )
		{
			charts->SetChartVecField(!charts->GetChartVecField(0),0);
		}
		else
		if( hitTestResult == CHART_BUTTON_RESIZE )
		{
			mouseMoveAction = MOUSE_ACTION_RESIZE;
		}	
		else
		if( hitTestResult == CHART_SLIDER)
		{
			const unsigned int chartW = charts->GetTopChart()->GetWidth();
			unsigned int startPos = abs(x - charts->GetTopChart()->GetX());
			if(startPos > chartW) startPos = chartW - 1;
			startPos = (unsigned int)(((float)startPos/chartW)*charts->GetTopChart()->GetNumberOfPoints());
			charts->SetChartSliderPos(startPos,0);

			const CChart* topChart = charts->GetTopChart();
			for(auto model = models.begin(); model != models.end(); model++)
			{
				CModelCharts* modelCharts = (*model)->GetCharts();
				if(!modelCharts->IsContainedChart(topChart)) continue;			
				for(int i = modelCharts->GetMaxChartId(); i >= 0; i--)
				{				
					modelCharts->SetChartSliderPos(startPos,i);
				}			
			}
		}
		else
		if( hitTestResult == CHART_SLIDER_MOVER)
		{
			mouseMoveAction = MOUSE_ACTION_SLIDER_MOVE;
		}
		else
		if( hitTestResult == CHART_SLIDER_MOVER_RESIZE)
		{
			mouseMoveAction = MOUSE_ACTION_SLIDER_RESIZE;
		}
		else 
		if( hitTestResult == CHART_CONTENT)
		{
			if(charts->GetTopChart()->GetType() == 2)
			{
				mouseMoveAction = MOUSE_ACTION_ROTATE;
			}
		}
	}else
	if(button == 2)
	{	
		if( hitTestResult != CHART_NONE )
		{
			charts->MoveChart(0, 0, 0);
			charts->ResizeChart(VideoManeger->width,VideoManeger->height, 0);		
		}				
	}else
	if(button == 3)
	{
		if( hitTestResult != CHART_NONE )
		{	
			mouseMoveAction = MOUSE_ACTION_MOVE;
		}				
	}
}

void CProgMain::MouseMove(const int X,const int Y,int dx,int dy)
{
	const CChart* currChart = charts->GetTopChart();
	if(currChart == NULL) return;

	int chartX = currChart->GetX();
	int chartY = currChart->GetY();
	int chartW = currChart->GetWidth();
	int chartH = currChart->GetHeight();
	
	if(mouseMoveAction == MOUSE_ACTION_NONE)
	{
		for(unsigned  int i = 0; i <= charts->GetMaxChartId(); i++)
		{		
			charts->GetChart(i);
			if(currChart == NULL) continue;
			charts->DrawMouseLines(X,Y,false,i);
		}
		const unsigned int id = charts->GetChartId(X,Y);
		const int hitTestRes = charts->HitTest(X,Y);
		if(hitTestRes == CHART_CONTENT) charts->DrawMouseLines(X,Y,true,id);

	}
	if(mouseMoveAction == MOUSE_ACTION_ROTATE)
	{
		const double angX = currChart->GetAngX(); 
		const double angY = currChart->GetAngY(); 
		double deltaAngX =  dy / 10.0;
		double deltaAngY = -dx / 10.0;

		if(angX + deltaAngX < 0 ) deltaAngX = 0;
		if(angX + deltaAngX > 90) deltaAngX = 0;
		if(angY + deltaAngY < 0 ) deltaAngY = 0;
		if(angY + deltaAngY > 90) deltaAngY = 0;

		charts->SetChartAngel(angX + deltaAngX, angY + deltaAngY, 0);
		return;
	}
	if(mouseMoveAction == MOUSE_ACTION_MOVE)
	{
		if(chartX + dx < 0) dx =- chartX;
		if(chartY + dy < 0) dy =- chartY;
		if(chartW + chartX + dx > VideoManeger->width)	dx = VideoManeger->width  - chartW - chartX;
		if(chartH + chartY + dy > VideoManeger->height)	dy = VideoManeger->height - chartH - chartY;

		charts->MoveChart(chartX + dx, chartY + dy, 0);
		return;
	}
	if(mouseMoveAction == MOUSE_ACTION_RESIZE)
	{
		if(chartW + chartX + dx > VideoManeger->width)	dx = VideoManeger->width  - chartW - chartX;
		if(chartH + chartY + dy > VideoManeger->height)	dy = VideoManeger->height - chartH - chartY;

		if(chartW + dx < 20) dx = 20 - chartW;
		if(chartH + dy < 40) dy = 40 - chartH;

		charts->ResizeChart(chartW + dx, chartH + dy, 0);
		return;
	}
	if(mouseMoveAction == MOUSE_ACTION_SLIDER_MOVE)
	{
		const unsigned int chartW = charts->GetTopChart()->GetWidth();
		unsigned int startPos = charts->GetTopChart()->GetPointsStartPos();
		const unsigned int numOfPoints = charts->GetTopChart()->GetNumberOfPoints();
		const int dxPoints = (int)(((float)(dx)/chartW)*numOfPoints);
				
		if((startPos < (unsigned int) abs(dxPoints)) && (dxPoints < 0)) startPos = 0;
		else startPos += dxPoints;
		if(startPos >= numOfPoints - 1) startPos = numOfPoints - 2;

		const CChart* topChart = charts->GetTopChart();
		for(auto model = models.begin(); model != models.end(); model++)
		{
			CModelCharts* modelCharts = (*model)->GetCharts();
			if(!modelCharts->IsContainedChart(topChart)) continue;			
			for(int i = modelCharts->GetMaxChartId(); i >= 0; i--)
			{				
				modelCharts->SetChartSliderPos(startPos,i);
			}			
		}
		return;
	}
	if(mouseMoveAction == MOUSE_ACTION_SLIDER_MOVE)
	{
		const unsigned int chartW = charts->GetTopChart()->GetWidth();
		unsigned int startPos = charts->GetTopChart()->GetPointsStartPos();
		const unsigned int numOfPoints = charts->GetTopChart()->GetNumberOfPoints();
		const int dxPoints = (int)(((float)(dx)/chartW)*numOfPoints);
				
		if((startPos < (unsigned int) abs(dxPoints)) && (dxPoints < 0)) startPos = 0;
		else startPos += dxPoints;
		if(startPos >= numOfPoints - 1) startPos = numOfPoints - 2;

		const CChart* topChart = charts->GetTopChart();
		for(auto model = models.begin(); model != models.end(); model++)
		{
			CModelCharts* modelCharts = (*model)->GetCharts();
			if(!modelCharts->IsContainedChart(topChart)) continue;			
			for(int i = modelCharts->GetMaxChartId(); i >= 0; i--)
			{				
				modelCharts->SetChartSliderPos(startPos,i);
			}			
		}
		return;
	}	
	if(mouseMoveAction == MOUSE_ACTION_SLIDER_RESIZE)
	{
		const unsigned int chartW = charts->GetTopChart()->GetWidth();
		unsigned int sliderPoints = charts->GetTopChart()->GetChartScale();
		const unsigned int numOfPoints = charts->GetTopChart()->GetNumberOfPoints();
		const int dxPoints = (int)(((float)(dx)/chartW)*numOfPoints);
				
		if((sliderPoints < 100 + (unsigned int)abs(dxPoints)) && (dxPoints < 0)) sliderPoints = 100;
		else sliderPoints += dxPoints;
		if(sliderPoints >= numOfPoints - 1) sliderPoints = numOfPoints - 2;

		const CChart* topChart = charts->GetTopChart();
		for(auto model = models.begin(); model != models.end(); model++)
		{
			CModelCharts* modelCharts = (*model)->GetCharts();
			if(!modelCharts->IsContainedChart(topChart)) continue;			
			for(int i = modelCharts->GetMaxChartId(); i >= 0; i--)
			{				
				modelCharts->SetChartSliderScale(sliderPoints,i);
			}			
		}
		return;
	}
}