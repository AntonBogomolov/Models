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
*	Управляющий класс приложения. Является управляющим классом приложения. 
*/
class CProgMain
{
private:
	CVideoManeger	*VideoManeger;

	vector<CEcoModel*>	models;						/**<вектор активных моделей */
	CChartCollection*	charts;						/**<контейнер графиков		*/
	vector<CModelDescription*> *descriptionList;	/**<вектор описателей моделей */
	CWinUI* UIWindow;
	CModelConstructorUI* modelConstructor;
	
	unsigned int mouseMoveAction;
	bool mouseButtonDown;

	bool quit;
private:
	/**
	*	Загрузка описателей моделей из файла 
	*/
	void InitDescriptionList(void);		
	/**
	*	Создание активной модели.
	*	@param ModelType - номер описателя модели в descriptionList.
	*/
	void CreateModel(const int ModelType);
	/**
	*	Обновление модели.
	*	@param oldModel - модель, которую неообходимо обновить.
	*/
	void UpdateModel(CEcoModel* oldModel);
	/**
	*	Удаление модели.
	*	@param Model - модель, которую неообходимо удалить.
	*/
	const bool DeleteModel(CEcoModel* Model);
	/**
	*	Удаление всех моделей.
	*/
	void DeleteAllModels(); 

	/**
	*	Построение пользовательского интерфейса.
	*/
	void BuildUI();

	/**
	*	Обработчик нажатий на кнопки мыши.
	*/
	void MouseBDown(const int x,const int y,const int button);
	/**
	*	Обработчик перемещения мыши.
	*/
	void MouseMove(const int x,const int y,int dx,int dy);
public:
	/**
	*	Отрисовка активных моделей.
	*/
	void Draw() const;
	/**
	*	Инициализация приложения.
	*/
	void Init();	
	/**
	*	Обработчик событий пользовательского интерфейса.
	*/
	void UIEvents(const HWND Hwnd,const long int ControlID);
	/**
	*	Обработчик событий SDL.
	*/
	void SDLEvents();
	/**
	*	Инициализация SDL и OpenGL.
	*/
	void InitVideo() const;

	const bool IsWorking() const;

	CProgMain(void);
	~CProgMain(void);
};
