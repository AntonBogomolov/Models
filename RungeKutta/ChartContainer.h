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
*	��������� ��������. ������������ ��� �������� �������� � ���������� ���.
*/
class CChartContainer
{
protected:
	CChart*** charts;							/**<������ ���������� �� ��������� ��������	*/						
	unsigned int maxChartId;					/**<������������ Id ������� � ����������	*/		
protected:
	/**
	*	������� ��������� Id 
	*/
    const unsigned int FindFreeId() const;
	/**
	*	������� ������������ Id 
	*/
	const unsigned int FindMaxChartId();
	/**
	*	������� Id ������� ������� 
	*/
	const unsigned int FindTopChartId() const;

	/**
	*	�������� ������ � ���������
	*/
	const unsigned int AddChart(CChart **Chart);
	/**
	*	��������� ����� ���������
	*/
	Box FillScreen(const int maxId);

	/**
	*	������� ��������� �� ��������� �� ������ �� ��� Id  
	*/
	CChart** GetChartPrt(const unsigned int Id) const;
	/**
	*	������� ��������� �� ��������� �� ������ ������ 
	*/
	CChart** GetTopChartPtr() const;
	/**
	*	������� ��������� �� ��������� �� ������ ��� ���������� ����  
	*/
	CChart** GetChartPtr(const int MouseX,const int MouseY) const;
public:
	/**
	*	������� ������������ Id ������� � ���������� 
	*/
	const unsigned int GetMaxChartId() const;

	/**
	*	������� ��� ������� 
	*/
    void DeleteAllCharts();
	/**
	*	������� ������ �� ��� Id
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
	*	�������� ��������� ���� �� ��������� ���������� ��������  
	*/
	virtual const int HitTest(const int X,const int Y);
	
	/**
	*	�������� ��� ������� ���������� Charts  
	*/
	const unsigned int AddCharts(CChartContainer *Charts);
	/**
	*	�������� ��� ������� ���������� Charts � ���������� �� ��������.  
	*/
	void AddAndArrangeCharts(CChartContainer *Charts);

	/**
	*	������� ��������� �� ������ �� ��� Id  
	*/
	const CChart*	GetChart(const unsigned int Id) const;
	/**
	*	������� ��������� �� ������ �� ��� Id  
	*/
	CChart*			GetChartUnsafe(const unsigned int Id) const;
	/**
	*	������� ��������� �� ������ ������ 
	*/
	const CChart*	GetTopChart() const;
	/**
	*	������� ��������� �� ������ ������ 
	*/
	CChart*			GetTopChartUnsafe() const;
	/**
	*	������� ��������� �� ������ ��� ���������� ����  
	*/
	const CChart*	GetChart(const int MouseX,const int MouseY) const;
	/**
	*	������� ��������� �� ������ ��� ���������� ����  
	*/
	CChart*			GetChartUnsafe(const int MouseX,const int MouseY) const;
	/**
	*	������� Id ������� ��� ���������� ����  
	*/
	const unsigned int GetChartId(const int MouseX,const int MouseY) const;
	/**
	*	������� Id ������� Chart  
	*/
	const unsigned int GetChartId(const CChart* Chart) const;
	const double GetChartAngelX(const unsigned int Id) const;
	const double GetChartAngelY(const unsigned int Id) const;

	/**
	*	������� true, ���� ��������� �������� ������ Chart
	*/
	const bool IsContainedChart(const CChart* Chart) const;

	CChartContainer();
	~CChartContainer();
};

/**
*	��������� �������� ������. ������������ ��� �������� �������� ������ � ���������� ���.
*/
class CModelCharts : public CChartContainer
{
private:
	const CRungeKutta* model;					/**<��������� �� ������	*/			
private:
	/**
	*	������� ��������� �� ������
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
	*	�������� ��������� ���� �� ��������� ���������� ��������  
	*/
	virtual const int HitTest(const int X,const int Y);

	/**
	*	������� id ������� �� ��� ����  
	*/
	const unsigned int GetChartIdByType(const int chartType) const;

	CModelCharts(const CRungeKutta* Model);
	CModelCharts(const CModelCharts &ModelCharts,const CRungeKutta* Model);
	~CModelCharts();
};

/**
*	��������� ��������. ������������ ��� �������� �������� ��������� ������ � ���������� ���.
*/
class CChartCollection : public CChartContainer
{
public:
	/**
	*	���������� ������� ������  
	*/
	void SetTopChart(const unsigned int Id);
	/**
	*	���������� ��� �������  
	*/
    void DrawAllCharts() const;
	
	/**
	*	�������� ��������� ���� �� ��������� ���������� ��������  
	*/
	virtual const int HitTest(const int X,const int Y);	

	CChartCollection();	
	~CChartCollection();
};