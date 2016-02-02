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
*	Структура для описания границ элементов управления графиков.  
*/
struct Box 
{
	int x;
	int y;
	int width;
	int height;
};

/**
*	Класс для создания графиков. Предназначен для визуализации данных, полученных при численном решении моделей. 
*/
class CChart
{
private:
	int width;							/**<ширина графика			*/
	int height;							/**<высота графика			*/
	int x;								/**<нач коорд Х  графика	*/
	int y;								/**<нач коорд Y  графика	*/

	double *maxValues;					/**<массив максимальных значений данных  (по измерениям)	*/
	double *minValues;					/**<массив минимальных  значений данных  (по измерениям)	*/
	double *drawCoeff;					/**<массив коэффициентов масштабирования (по измерениям)	*/
	double *ecvilibrium;

	const CRungeKutta* model;			/**<указатель на модель		*/

	unsigned int dim;					/**<число измерений			*/
	double		 dt;					/**<шаг времени				*/
	unsigned int lenOfValues;			/**<количество значений		*/

	char* tbuff;
	
	bool	anim;
	double	animInt;
	double	animTime;
	double	animCurrTime;

	unsigned int numberOfPoints;		/**<количество точек в текущем блоке 	*/
	int type;							/**<тип графика							*/
	int nD1;							/**<номер первного измерения			*/
	int nD2;							/**<номер второго  измерения			*/
	int nD3;							/**<номер третьего измерения			*/
	unsigned int *type0Dimensions;		/**<массив номеров измерений для графика первого типа	*/
	unsigned int type0DimensionsCnt;	/**<кол-во номеров измерений для графика первого типа	*/

	double angX;						/**<угол поворота по оси Х графика третьего типа		*/
	double angY;						/**<угол поворота по оси Y графика третьего типа		*/

	int mouseX;
	int mouseY;
	bool drawMouse;						/**<флаг: обрабатывать ли изменения положения мыши		*/
	bool IsDrawVecField;				/**<флаг: рисовать ли векторное поле	*/
 	bool needBuidlPointsArray;			/**<флаг: обновить ли массив значений	*/
	bool useAutoScale;					/**<флаг: использовать ли автомасштаб	*/
	bool visible;						/**<флаг: виден ли график				*/

	double *values;						/**<массив значений	(всех)				*/
	double *points;						/**<массив значений	(не более numberOfPoints значений)	*/

	double *vecFieldCoord;				/**<массив для отрисовки векторного поля				*/
	double *vecFieldResCoord;			/**<массив для отрисовки векторного поля				*/

	double maxValue;					/**<максимальное значение (среди всех измерений)		*/
	double minValue;					/**<минимальное  значение (среди всех измерений)		*/
	unsigned int blockOffsetPoints;		/**<начальный нормер точки в текущем блоке точек		*/
	unsigned int currPointsOnScreen;	/**<кол-во точкек графика на экране						*/
	unsigned int globalOffsetPoints;	/**<начальный нормер точки  текущего блока точек		*/
	unsigned int maxPointsOnScreen;		/**<максимальное количество точек в текущем блоке 		*/
	unsigned int minPointsOnScreen;		/**<минимальное  количество точек в текущем блоке 		*/
	double scaleOffsetX;				/**<сдвиг по оси Х 		*/
	double scaleOffsetY;				/**<сдвиг по оси Y 		*/
	double scaleOffsetZ;				/**<сдвиг по оси Z 		*/

	Box chartBox;						/**<Box для графика						*/
	Box uiButtonClose;					/**<Box для кнопки закрытия				*/
	Box uiButtonVecField;				/**<Box для кнопки векторного поля		*/
	Box uiButtonResize;					/**<Box для кнопки изм размера			*/
	Box uiSlider;						/**<Box для слайдера					*/
	Box uiSliderMover;					/**<Box для слайдера					*/
	Box uiSliderMoverButtonResize;		/**<Box для кнопки изм разм слайдера	*/

	CChartColorScheme* colors;			/**<цветовая схема графика				*/
private:	
	/**
	*	Отрисовка графика первого типа 
	*/
	void DrawType_0();
	/**
	*	Отрисовка графика второго типа 
	*/
	void DrawType_1();
	/**
	*	Отрисовка графика третьего типа 
	*/
	void DrawType_2();

	/**
	*	Заполнить массив значений 
	*/
	void BuildPointsArray();
	/**
	*	Инициализация графика 
	*/
	void Init(const int Width,const int Height,const int X0,const int Y0,const CRungeKutta* Model);
	/**
	*	Инициализация интерфейса графика 
	*/
	void InitUI();

	/**
	*	Получить максимальное значение на заданом отрезке (среди всех измерений).
		@param Begin	- начальный индекс массива значий.
		@param End		- конечный  индекс массива значий.
		@return			максимальное значение на заданом отрезке.
	*/
	const double GetMax(const unsigned int Begin,const unsigned int End) const;
	/**
	*	Получить минимальное значение на заданом отрезке (среди всех измерений).
		@param Begin	- начальный индекс массива значий.
		@param End		- конечный  индекс массива значий.
		@return			минимальное значение на заданом отрезке.
	*/
	const double GetMin(const unsigned int Begin,const unsigned int End) const;
	/**
	*	Получить максимальное значение на заданом отрезке (для одного измерения).
		@param Begin	- начальный индекс массива значий.
		@param End		- конечный  индекс массива значий.
		@param dim		- измерение.
		@return			максимальное значение на заданом отрезке.
	*/
	const double GetMaxInD(const unsigned int Begin,const unsigned int End,const unsigned int dim) const;
	/**
	*	Получить минимальное значение на заданом отрезке (для одного измерения).
		@param Begin	- начальный индекс массива значий.
		@param End		- конечный  индекс массива значий.
		@param dim		- измерение.
		@return			минимальное значение на заданом отрезке.
	*/
	const double GetMinInD(const unsigned int Begin,const unsigned int End,const unsigned int dim) const;

	/**
	*	Найти оптимальные коэффициенты масштаборования для графика 
	*/
	void SetupChartScale();
	/**
	*	Получить коэффициент масштаба  
	*/
	const double GetScaleCoeff(const double DrCoeff,const double Size) const;
	/**
	*	Нарисовать границу графика  
	*/
	void DrawBorder();
	/**
	*	Нарисовать пользовательский интерфейс графика  
	*/
	void DrawUI(const bool HasVecButton = false);
public:
	CChart(const int Width,const int Height,const int X0,const int Y0,const CRungeKutta* Model);
	CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int *Dimensions,const unsigned int DimensionsCnt,const CRungeKutta* Model);
	CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int nFirstD,const unsigned int nSecD,const CRungeKutta* Model);
	CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int nFirstD,const unsigned int nSecD,const unsigned int nFirdD,const CRungeKutta* Model);

	~CChart();

	/**
	*	Запустить анимацию  
	*/
	void SetAnim(double Time);
	/**
	*	Остановить анимацию  
	*/
	void StopAnim();
	/**
	*	Установка времени анимации  
	*/
	void SetCurrAnimTime(const double NewTime);
	const bool	 IsAnim();
	const double GetAnimInt();
	const double GetCurrAnimTime();
	const double GetAnimTime();

	/**
	*	Отрисовка графика  
	*/
	void  Draw();	

	/**
	*	Вернуть тип графика  
	*/
	const int GetType()	  const;
	/**
	*	Вернуть высоту графика  
	*/
	const int GetHeight() const;
	/**
	*	Вернуть ширину графика  
	*/
	const int GetWidth()  const; 
	/**
	*	Вернуть координату Х графика  
	*/
	const int GetX()  const;
	/**
	*	Вернуть координату Y графика  
	*/
	const int GetY()  const;
	/**
	*	Вернуть номер первого изменерия  
	*/
	const int GetD1() const;
	/**
	*	Вернуть номер второго изменерия  
	*/
	const int GetD2() const;
	/**
	*	Вернуть номер третьего изменерия  
	*/
	const int GetD3() const;
	/**
	*	Вернуть угол поворота по оси X  
	*/
	const double GetAngX() const;
	/**
	*	Вернуть угол поворота по оси Y  
	*/
	const double GetAngY() const;
	/**
	*	Вернуть индекс начальной точки блока  
	*/
	const unsigned int GetPointsStartPos() const;
	/**
	*	Вернуть временой масштаб графика  
	*/
	const unsigned int GetChartScale() const;
	/**
	*	Вернуть количество точек в блоке  
	*/
	const unsigned int GetNumberOfPoints() const;
	/**
	*	Вернуть цветовую схему  
	*/
	const CChartColorScheme* GetChartColorScheme() const;
	/**
	*	Вернуть количество измерений графика первого типа  
	*/
	const unsigned int GetType0DimsCnt() const;
	/**
	*	Вернуть массив измерений графика первого типа  
	*/
	const unsigned int* GetType0Dims() const;

	/**
	*	Проверка попадания мыши по элементам управления графика  
	*/
	const int HitTest(const int X,const int Y) const;

	/**
	*	Установить индекс начальной точки блока  
	*/
	void SetPointsStartPos(const unsigned int StartPos);
	/**
	*	Установить временой масштаб графика  
	*/
	void SetChartScale(const unsigned int NumPointsOnScreen);
	/**
	*	Установить цветовую схему  
	*/
	void SetChartColorScheme(CChartColorScheme* ColorScheme);
	void SetMouseX(const int X);
	void SetMouseY(const int Y);
	/**
	*	Установить значение флага drawMouse 
	*/
	void DrawMouseLine(const bool draw);
	/**
	*	Установить значение флага drawVecField 
	*/
	void DrawVecField (const bool draw);
	/**
	*	Установить значение флага visible 
	*/
	void SetVisibility(const bool isVisible);	
	/**
	*	Установить углы поворота вокруг осей X и Y 
	*/
	void SetAngel(const double AngX,const double AngY);	

	/**
	*	Изменить значение флага IsDrawVecField 
	*/
	const bool IsVecField() const;
	/**
	*	Вернуть значение флага visible 
	*/
	const bool IsVisible() const;

	/**
	*	Изменить размер графика 
	*/
	void Resize(const int Width,const int Height);
	/**
	*	Изменить положение графика 
	*/
	void SetPos(const int X0,const int Y0);

	/**
	*	Вернуть указатель на модель  
	*/
	const CRungeKutta* GetModel() const;
};