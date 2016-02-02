#pragma once

/**
*	Структура для описания цвета.  
*/
struct SColor
{
	float r;
	float g;
	float b;
	SColor(float R,float G,float B){r=R; g=G; b=B;};
	SColor(const SColor &Color){r=Color.r; g=Color.g; b=Color.b;};
};

/**
*	Класс реализующий цветовые схемы
*/
class CChartColorScheme
{
private:
	SColor* prayColor;						/**<цвет графика жертв			*/
	SColor* predColor;						/**<цвет графика хищников		*/
	SColor* vecFieldColor;					/**<цвет векторного поля		*/
	SColor* gridColor;						/**<цвет сетки					*/
	SColor* backColor;						/**<цвет заднего фона			*/
	SColor* borderColor;					/**<цвет границ					*/
	SColor* buttonColor;					/**<цвет кнопок					*/
	SColor* buttonTextColor;				/**<цвет текста кнопок			*/
	SColor* textColor;						/**<цвет текста					*/
public:
	
	/**
	*	Получить цвет графика жертв
	*/
	const SColor* GetPrayColor(void) const {return prayColor;};
	/**
	*	Получить цвет графика хищников
	*/
	const SColor* GetPredColor(void) const {return predColor;};
	/**
	*	Получить цвет векторного поля
	*/
	const SColor* GetVecFieldColor(void) const {return vecFieldColor;};
	/**
	*	Получить цвет сетки
	*/
	const SColor* GetGridColor(void) const {return gridColor;};
	/**
	*	Получить цвет заднего фона
	*/
	const SColor* GetBackColor(void) const {return backColor;};
	/**
	*	Получить цвет границ
	*/
	const SColor* GetBorderColor(void) const {return borderColor;};
	/**
	*	Получить цвет кнопок
	*/
	const SColor* GetButtonColor(void) const {return buttonColor;};
	/**
	*	Получить цвет текста кнопок
	*/
	const SColor* GetButtonTextColor(void) const {return buttonTextColor;};
	/**
	*	Получить цвет текста
	*/
	const SColor* GetTextColor(void) const {return textColor;};
		
	CChartColorScheme();
	CChartColorScheme(const SColor PrayColor,const SColor PredColor,const SColor VecFieldColor,const SColor GridColor,const SColor BackColor,const SColor BorderColor,const SColor ButtonColor,const SColor ButtonTextColor,const SColor TextColor);
	~CChartColorScheme(void);
};

