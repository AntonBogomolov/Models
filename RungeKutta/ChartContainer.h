#pragma once

class CRungeKutta;
class CChart;
struct Box;

const int MAX_CHARTS_CNT = 100; 

const int MOUSE_ACTION_NONE = 0;
const int MOUSE_ACTION_MOVE = 1;
const int MOUSE_ACTION_RESIZE = 2;
const int MOUSE_ACTION_ROTATE = 3;
const int MOUSE_ACTION_SLIDER_SETPOS = 10;
const int MOUSE_ACTION_SLIDER_MOVE	 = 11;
const int MOUSE_ACTION_SLIDER_RESIZE = 12;

/**
*	Контейнер графиков. Предназначен для хранения графиков и управления ими.
*/
class CChartContainer
{
protected:
	CChart*** charts;							/**<массив указателей на указатели графиков	*/						
	unsigned int maxChartId;					/**<максимальный Id графика в контейнере	*/		
protected:
	/**
	*	Вернуть свободный Id 
	*/
    const unsigned int FindFreeId() const;
	/**
	*	Вернуть максимальный Id 
	*/
	const unsigned int FindMaxChartId();
	/**
	*	Вернуть Id первого графика 
	*/
	const unsigned int FindTopChartId() const;

	/**
	*	Добавить график в контейнер
	*/
	const unsigned int AddChart(CChart **Chart);
	/**
	*	Заполнить экран графиками
	*/
	Box FillScreen(const int maxId);

	/**
	*	Вернуть указатель на указатель на график по его Id  
	*/
	CChart** GetChartPrt(const unsigned int Id) const;
	/**
	*	Вернуть указатель на указатель на первый график 
	*/
	CChart** GetTopChartPtr() const;
	/**
	*	Вернуть указатель на указатель на график под указателем мыши  
	*/
	CChart** GetChartPtr(const int MouseX,const int MouseY) const;
public:
	/**
	*	Вернуть максимальный Id графика в контейнере 
	*/
	const unsigned int GetMaxChartId() const;

	/**
	*	Удалить все графики 
	*/
    void DeleteAllCharts();
	/**
	*	Удалить график по его Id
	*/
    void DeleteChart(const unsigned int ChartId);
	
	void ResizeChart(const int Width,const int Height,const unsigned int Id);
	void MoveChart(const int X,const int Y,const unsigned int Id);
	void DrawMouseLines(const int X,const int Y,const bool IsDrawMouse,const unsigned int Id);
	void SetChartVecField(const bool IsDrawVecField,const unsigned int Id);
	void SetChartVisibility(const bool IsChartVisible,const unsigned int Id);
	void SetChartSliderPos(const unsigned int StartPos,const unsigned int Id);
	void SetChartSliderScale(const unsigned int PointsNum,const unsigned int Id);
	void SetChartAngel(const double AngX,const double AngY,const unsigned int Id);

	const bool GetChartVecField(const unsigned int Id)   const;
	const bool GetChartVisibility(const unsigned int Id) const;

	/**
	*	Проверка попадания мыши по элементам управления графиков  
	*/
	virtual const int HitTest(const int X,const int Y);
	
	/**
	*	Добавить все графики контейнера Charts  
	*/
	const unsigned int AddCharts(CChartContainer *Charts);
	/**
	*	Добавить все графики контейнера Charts с изменением их размеров.  
	*/
	void AddAndArrangeCharts(CChartContainer *Charts);

	/**
	*	Вернуть указатель на график по его Id  
	*/
	const CChart*	GetChart(const unsigned int Id) const;
	/**
	*	Вернуть указатель на график по его Id  
	*/
	CChart*			GetChartUnsafe(const unsigned int Id) const;
	/**
	*	Вернуть указатель на первый график 
	*/
	const CChart*	GetTopChart() const;
	/**
	*	Вернуть указатель на первый график 
	*/
	CChart*			GetTopChartUnsafe() const;
	/**
	*	Вернуть указатель на график под указателем мыши  
	*/
	const CChart*	GetChart(const int MouseX,const int MouseY) const;
	/**
	*	Вернуть указатель на график под указателем мыши  
	*/
	CChart*			GetChartUnsafe(const int MouseX,const int MouseY) const;
	/**
	*	Вернуть Id графика под указателем мыши  
	*/
	const unsigned int GetChartId(const int MouseX,const int MouseY) const;
	/**
	*	Вернуть Id графика Chart  
	*/
	const unsigned int GetChartId(const CChart* Chart) const;
	const double GetChartAngelX(const unsigned int Id) const;
	const double GetChartAngelY(const unsigned int Id) const;

	/**
	*	Вернуть true, если контейнер содержит график Chart
	*/
	const bool IsContainedChart(const CChart* Chart) const;

	CChartContainer();
	~CChartContainer();
};

/**
*	Контейнер графиков модели. Предназначен для хранения графиков модели и управления ими.
*/
class CModelCharts : public CChartContainer
{
private:
	const CRungeKutta* model;					/**<указатель на модель	*/			
private:
	/**
	*	Вернуть указатель на модель
	*/
	const CRungeKutta* GetModel() const;
public:
    const unsigned int CreateChart(const CChart *Chart);
	const unsigned int CreateChart(const int Width,const int Height,const int X0,const int Y0);
	const unsigned int CreateChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int DimCnt, const unsigned int* Dims);
	const unsigned int CreateChart(const int Width,const int Height,const int X0,const int Y0,const int nFirstD,const int nSecD);
	const unsigned int CreateChart(const int Width,const int Height,const int X0,const int Y0,const int nFirstD,const int nSecD,const int nFirdD);
	const unsigned int CreateChart(void);
	const unsigned int CreateChart(const unsigned int *Dimensions,const unsigned int DimensionsCnt);
	const unsigned int CreateChart(const int nFirstD,const int nSecD);
	const unsigned int CreateChart(const int nFirstD,const int nSecD,const int nFirdD);

	/**
	*	Проверка попадания мыши по элементам управления графиков  
	*/
	virtual const int HitTest(const int X,const int Y);

	/**
	*	Вернуть id графика по его типу  
	*/
	const unsigned int GetChartIdByType(const int chartType) const;

	CModelCharts(const CRungeKutta* Model);
	CModelCharts(const CModelCharts &ModelCharts,const CRungeKutta* Model);
	~CModelCharts();
};

/**
*	Коллекция графиков. Предназначен для хранения графиков различных модели и управления ими.
*/
class CChartCollection : public CChartContainer
{
public:
	/**
	*	Установить верхний график  
	*/
	void SetTopChart(const unsigned int Id);
	/**
	*	Отрисовать все графики  
	*/
    void DrawAllCharts() const;
	
	/**
	*	Проверка попадания мыши по элементам управления графиков  
	*/
	virtual const int HitTest(const int X,const int Y);	

	CChartCollection();	
	~CChartCollection();
};