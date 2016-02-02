#pragma once

const int CHART_NONE = 0; 
const int CHART_CONTENT = 1;
const int CHART_BUTTON_RESIZE = 10; 
const int CHART_BUTTON_CLOSE  = 11; 
const int CHART_BUTTON_VEC	  = 12;
const int CHART_SLIDER		  = 20; 
const int CHART_SLIDER_MOVER  = 21; 
const int CHART_SLIDER_MOVER_RESIZE  = 22; 

const int CHART_DIM_TIME = 0;
const int CHART_DIM_VAL1 = 1;
const int CHART_DIM_VAL2 = 2;
const int CHART_DIM_VAL3 = 3;
const int CHART_DIM_VAL4 = 4;

const int CHART_TYPE_VALUES  = 0;
const int CHART_TYPE_PHASE   = 1;
const int CHART_TYPE_INTEGRAL= 2;

class CRungeKutta;
class CVertexBuffer;
class CChartColorScheme;

/**
*	��������� ��� �������� ������ ��������� ���������� ��������.  
*/
struct Box 
{
	int x;
	int y;
	int width;
	int height;
};

/**
*	����� ��� �������� ��������. ������������ ��� ������������ ������, ���������� ��� ��������� ������� �������. 
*/
class CChart
{
private:
	int width;							/**<������ �������			*/
	int height;							/**<������ �������			*/
	int x;								/**<��� ����� �  �������	*/
	int y;								/**<��� ����� Y  �������	*/

	double *maxValues;					/**<������ ������������ �������� ������  (�� ����������)	*/
	double *minValues;					/**<������ �����������  �������� ������  (�� ����������)	*/
	double *drawCoeff;					/**<������ ������������� ��������������� (�� ����������)	*/
	double *ecvilibrium;

	const CRungeKutta* model;			/**<��������� �� ������		*/

	unsigned int dim;					/**<����� ���������			*/
	double		 dt;					/**<��� �������				*/
	unsigned int lenOfValues;			/**<���������� ��������		*/

	char* tbuff;
	
	bool	anim;
	double	animInt;
	double	animTime;
	double	animCurrTime;

	unsigned int numberOfPoints;		/**<���������� ����� � ������� ����� 	*/
	int type;							/**<��� �������							*/
	int nD1;							/**<����� �������� ���������			*/
	int nD2;							/**<����� �������  ���������			*/
	int nD3;							/**<����� �������� ���������			*/
	unsigned int *type0Dimensions;		/**<������ ������� ��������� ��� ������� ������� ����	*/
	unsigned int type0DimensionsCnt;	/**<���-�� ������� ��������� ��� ������� ������� ����	*/

	double angX;						/**<���� �������� �� ��� � ������� �������� ����		*/
	double angY;						/**<���� �������� �� ��� Y ������� �������� ����		*/

	int mouseX;
	int mouseY;
	bool drawMouse;						/**<����: ������������ �� ��������� ��������� ����		*/
	bool IsDrawVecField;				/**<����: �������� �� ��������� ����	*/
 	bool needBuidlPointsArray;			/**<����: �������� �� ������ ��������	*/
	bool useAutoScale;					/**<����: ������������ �� �����������	*/
	bool visible;						/**<����: ����� �� ������				*/

	double *values;						/**<������ ��������	(����)				*/
	double *points;						/**<������ ��������	(�� ����� numberOfPoints ��������)	*/

	double *vecFieldCoord;				/**<������ ��� ��������� ���������� ����				*/
	double *vecFieldResCoord;			/**<������ ��� ��������� ���������� ����				*/

	double maxValue;					/**<������������ �������� (����� ���� ���������)		*/
	double minValue;					/**<�����������  �������� (����� ���� ���������)		*/
	unsigned int blockOffsetPoints;		/**<��������� ������ ����� � ������� ����� �����		*/
	unsigned int currPointsOnScreen;	/**<���-�� ������ ������� �� ������						*/
	unsigned int globalOffsetPoints;	/**<��������� ������ �����  �������� ����� �����		*/
	unsigned int maxPointsOnScreen;		/**<������������ ���������� ����� � ������� ����� 		*/
	unsigned int minPointsOnScreen;		/**<�����������  ���������� ����� � ������� ����� 		*/
	double scaleOffsetX;				/**<����� �� ��� � 		*/
	double scaleOffsetY;				/**<����� �� ��� Y 		*/
	double scaleOffsetZ;				/**<����� �� ��� Z 		*/

	Box chartBox;						/**<Box ��� �������						*/
	Box uiButtonClose;					/**<Box ��� ������ ��������				*/
	Box uiButtonVecField;				/**<Box ��� ������ ���������� ����		*/
	Box uiButtonResize;					/**<Box ��� ������ ��� �������			*/
	Box uiSlider;						/**<Box ��� ��������					*/
	Box uiSliderMover;					/**<Box ��� ��������					*/
	Box uiSliderMoverButtonResize;		/**<Box ��� ������ ��� ���� ��������	*/

	CChartColorScheme* colors;			/**<�������� ����� �������				*/
private:	
	/**
	*	��������� ������� ������� ���� 
	*/
	void DrawType_0();
	/**
	*	��������� ������� ������� ���� 
	*/
	void DrawType_1();
	/**
	*	��������� ������� �������� ���� 
	*/
	void DrawType_2();

	/**
	*	��������� ������ �������� 
	*/
	void BuildPointsArray();
	/**
	*	������������� ������� 
	*/
	void Init(const int Width,const int Height,const int X0,const int Y0,const CRungeKutta* Model);
	/**
	*	������������� ���������� ������� 
	*/
	void InitUI();

	/**
	*	�������� ������������ �������� �� ������� ������� (����� ���� ���������).
		@param Begin	- ��������� ������ ������� ������.
		@param End		- ��������  ������ ������� ������.
		@return			������������ �������� �� ������� �������.
	*/
	const double GetMax(const unsigned int Begin,const unsigned int End) const;
	/**
	*	�������� ����������� �������� �� ������� ������� (����� ���� ���������).
		@param Begin	- ��������� ������ ������� ������.
		@param End		- ��������  ������ ������� ������.
		@return			����������� �������� �� ������� �������.
	*/
	const double GetMin(const unsigned int Begin,const unsigned int End) const;
	/**
	*	�������� ������������ �������� �� ������� ������� (��� ������ ���������).
		@param Begin	- ��������� ������ ������� ������.
		@param End		- ��������  ������ ������� ������.
		@param dim		- ���������.
		@return			������������ �������� �� ������� �������.
	*/
	const double GetMaxInD(const unsigned int Begin,const unsigned int End,const unsigned int dim) const;
	/**
	*	�������� ����������� �������� �� ������� ������� (��� ������ ���������).
		@param Begin	- ��������� ������ ������� ������.
		@param End		- ��������  ������ ������� ������.
		@param dim		- ���������.
		@return			����������� �������� �� ������� �������.
	*/
	const double GetMinInD(const unsigned int Begin,const unsigned int End,const unsigned int dim) const;

	/**
	*	����� ����������� ������������ ��������������� ��� ������� 
	*/
	void SetupChartScale();
	/**
	*	�������� ����������� ��������  
	*/
	const double GetScaleCoeff(const double DrCoeff,const double Size) const;
	/**
	*	���������� ������� �������  
	*/
	void DrawBorder();
	/**
	*	���������� ���������������� ��������� �������  
	*/
	void DrawUI(const bool HasVecButton = false);
public:
	CChart(const int Width,const int Height,const int X0,const int Y0,const CRungeKutta* Model);
	CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int *Dimensions,const unsigned int DimensionsCnt,const CRungeKutta* Model);
	CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int nFirstD,const unsigned int nSecD,const CRungeKutta* Model);
	CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int nFirstD,const unsigned int nSecD,const unsigned int nFirdD,const CRungeKutta* Model);

	~CChart();

	/**
	*	��������� ��������  
	*/
	void SetAnim(double Time);
	/**
	*	���������� ��������  
	*/
	void StopAnim();
	/**
	*	��������� ������� ��������  
	*/
	void SetCurrAnimTime(const double NewTime);
	const bool	 IsAnim();
	const double GetAnimInt();
	const double GetCurrAnimTime();
	const double GetAnimTime();

	/**
	*	��������� �������  
	*/
	void  Draw();	

	/**
	*	������� ��� �������  
	*/
	const int GetType()	  const;
	/**
	*	������� ������ �������  
	*/
	const int GetHeight() const;
	/**
	*	������� ������ �������  
	*/
	const int GetWidth()  const; 
	/**
	*	������� ���������� � �������  
	*/
	const int GetX()  const;
	/**
	*	������� ���������� Y �������  
	*/
	const int GetY()  const;
	/**
	*	������� ����� ������� ���������  
	*/
	const int GetD1() const;
	/**
	*	������� ����� ������� ���������  
	*/
	const int GetD2() const;
	/**
	*	������� ����� �������� ���������  
	*/
	const int GetD3() const;
	/**
	*	������� ���� �������� �� ��� X  
	*/
	const double GetAngX() const;
	/**
	*	������� ���� �������� �� ��� Y  
	*/
	const double GetAngY() const;
	/**
	*	������� ������ ��������� ����� �����  
	*/
	const unsigned int GetPointsStartPos() const;
	/**
	*	������� �������� ������� �������  
	*/
	const unsigned int GetChartScale() const;
	/**
	*	������� ���������� ����� � �����  
	*/
	const unsigned int GetNumberOfPoints() const;
	/**
	*	������� �������� �����  
	*/
	const CChartColorScheme* GetChartColorScheme() const;
	/**
	*	������� ���������� ��������� ������� ������� ����  
	*/
	const unsigned int GetType0DimsCnt() const;
	/**
	*	������� ������ ��������� ������� ������� ����  
	*/
	const unsigned int* GetType0Dims() const;

	/**
	*	�������� ��������� ���� �� ��������� ���������� �������  
	*/
	const int HitTest(const int X,const int Y) const;

	/**
	*	���������� ������ ��������� ����� �����  
	*/
	void SetPointsStartPos(const unsigned int StartPos);
	/**
	*	���������� �������� ������� �������  
	*/
	void SetChartScale(const unsigned int NumPointsOnScreen);
	/**
	*	���������� �������� �����  
	*/
	void SetChartColorScheme(CChartColorScheme* ColorScheme);
	void SetMouseX(const int X);
	void SetMouseY(const int Y);
	/**
	*	���������� �������� ����� drawMouse 
	*/
	void DrawMouseLine(const bool draw);
	/**
	*	���������� �������� ����� drawVecField 
	*/
	void DrawVecField (const bool draw);
	/**
	*	���������� �������� ����� visible 
	*/
	void SetVisibility(const bool isVisible);	
	/**
	*	���������� ���� �������� ������ ���� X � Y 
	*/
	void SetAngel(const double AngX,const double AngY);	

	/**
	*	�������� �������� ����� IsDrawVecField 
	*/
	const bool IsVecField() const;
	/**
	*	������� �������� ����� visible 
	*/
	const bool IsVisible() const;

	/**
	*	�������� ������ ������� 
	*/
	void Resize(const int Width,const int Height);
	/**
	*	�������� ��������� ������� 
	*/
	void SetPos(const int X0,const int Y0);

	/**
	*	������� ��������� �� ������  
	*/
	const CRungeKutta* GetModel() const;
};