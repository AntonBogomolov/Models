#include "ChartColorScheme.h"
#include "global.h"

CChartColorScheme::CChartColorScheme(const SColor PrayColor,const SColor PredColor,const SColor VecFieldColor,const SColor GridColor,const SColor BackColor,const SColor BorderColor,const SColor ButtonColor,const SColor ButtonTextColor,const SColor TextColor)
{
	prayColor = new SColor(PrayColor);
	predColor = new SColor(PredColor);
	vecFieldColor = new SColor(VecFieldColor);
	gridColor = new SColor(GridColor);
	backColor = new SColor(BackColor);
	borderColor = new SColor(BorderColor);
	buttonColor = new SColor(ButtonColor);
	buttonTextColor = new SColor(ButtonTextColor);
	textColor = new SColor(TextColor);
}

CChartColorScheme::CChartColorScheme()
{
	prayColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	predColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	vecFieldColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	gridColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	backColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	borderColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	buttonColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	buttonTextColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
	textColor = new SColor((float)(rand()%255)/255.0,(float)(rand()%255)/255.0,(float)(rand()%255)/255.0);
}

CChartColorScheme::~CChartColorScheme(void)
{
	delete prayColor;
	delete predColor;
	delete vecFieldColor;
	delete gridColor;
	delete backColor;
	delete borderColor;
	delete buttonColor;
	delete buttonTextColor;
	delete textColor;
}
