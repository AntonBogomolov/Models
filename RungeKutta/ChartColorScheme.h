#pragma once

/**
*	��������� ��� �������� �����.  
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
*	����� ����������� �������� �����
*/
class CChartColorScheme
{
private:
	SColor* prayColor;						/**<���� ������� �����			*/
	SColor* predColor;						/**<���� ������� ��������		*/
	SColor* vecFieldColor;					/**<���� ���������� ����		*/
	SColor* gridColor;						/**<���� �����					*/
	SColor* backColor;						/**<���� ������� ����			*/
	SColor* borderColor;					/**<���� ������					*/
	SColor* buttonColor;					/**<���� ������					*/
	SColor* buttonTextColor;				/**<���� ������ ������			*/
	SColor* textColor;						/**<���� ������					*/
public:
	
	/**
	*	�������� ���� ������� �����
	*/
	const SColor* GetPrayColor(void) const {return prayColor;};
	/**
	*	�������� ���� ������� ��������
	*/
	const SColor* GetPredColor(void) const {return predColor;};
	/**
	*	�������� ���� ���������� ����
	*/
	const SColor* GetVecFieldColor(void) const {return vecFieldColor;};
	/**
	*	�������� ���� �����
	*/
	const SColor* GetGridColor(void) const {return gridColor;};
	/**
	*	�������� ���� ������� ����
	*/
	const SColor* GetBackColor(void) const {return backColor;};
	/**
	*	�������� ���� ������
	*/
	const SColor* GetBorderColor(void) const {return borderColor;};
	/**
	*	�������� ���� ������
	*/
	const SColor* GetButtonColor(void) const {return buttonColor;};
	/**
	*	�������� ���� ������ ������
	*/
	const SColor* GetButtonTextColor(void) const {return buttonTextColor;};
	/**
	*	�������� ���� ������
	*/
	const SColor* GetTextColor(void) const {return textColor;};
		
	CChartColorScheme();
	CChartColorScheme(const SColor PrayColor,const SColor PredColor,const SColor VecFieldColor,const SColor GridColor,const SColor BackColor,const SColor BorderColor,const SColor ButtonColor,const SColor ButtonTextColor,const SColor TextColor);
	~CChartColorScheme(void);
};

