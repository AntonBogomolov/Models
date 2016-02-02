#pragma once
#include "global.h"

class CVideoManeger;
class CChart;
class CRungeKutta;
class CWinUI;
class CModelDescription;
class CMathParser;
class CEcoModel;
class CChartCollection;
class CModelConstructorUI;

using namespace std;

/**
*	����������� ����� ����������. �������� ����������� ������� ����������. 
*/
class CProgMain
{
private:
	CVideoManeger	*VideoManeger;

	vector<CEcoModel*>	models;						/**<������ �������� ������� */
	CChartCollection*	charts;						/**<��������� ��������		*/
	vector<CModelDescription*> *descriptionList;	/**<������ ���������� ������� */
	CWinUI* UIWindow;
	CModelConstructorUI* modelConstructor;
	
	unsigned int mouseMoveAction;
	bool mouseButtonDown;

	bool quit;
private:
	/**
	*	�������� ���������� ������� �� ����� 
	*/
	void InitDescriptionList(void);		
	/**
	*	�������� �������� ������.
	*	@param ModelType - ����� ��������� ������ � descriptionList.
	*/
	void CreateModel(const int ModelType);
	/**
	*	���������� ������.
	*	@param oldModel - ������, ������� ����������� ��������.
	*/
	void UpdateModel(CEcoModel* oldModel);
	/**
	*	�������� ������.
	*	@param Model - ������, ������� ����������� �������.
	*/
	const bool DeleteModel(CEcoModel* Model);
	/**
	*	�������� ���� �������.
	*/
	void DeleteAllModels(); 

	/**
	*	���������� ����������������� ����������.
	*/
	void BuildUI();

	/**
	*	���������� ������� �� ������ ����.
	*/
	void MouseBDown(const int x,const int y,const int button);
	/**
	*	���������� ����������� ����.
	*/
	void MouseMove(const int x,const int y,int dx,int dy);
public:
	/**
	*	��������� �������� �������.
	*/
	void Draw() const;
	/**
	*	������������� ����������.
	*/
	void Init();	
	/**
	*	���������� ������� ����������������� ����������.
	*/
	void UIEvents(const HWND Hwnd,const long int ControlID);
	/**
	*	���������� ������� SDL.
	*/
	void SDLEvents();
	/**
	*	������������� SDL � OpenGL.
	*/
	void InitVideo() const;

	const bool IsWorking() const;

	CProgMain(void);
	~CProgMain(void);
};
