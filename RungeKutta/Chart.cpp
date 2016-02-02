#include "Chart.h"
#include "ChartColorScheme.h"
#include "EcoCl.h"
#include "ModelDescription.h"
#include "VideoManeger.h"
#include "VertexBuffer.h"

#define M_PI       3.14159265358979323846

CChart::CChart(const int Width,const int Height,const int X0,const int Y0,const CRungeKutta* Model)
{
	Init(Width,Height,X0,Y0,Model);	
	type = 0;

	type0DimensionsCnt = dim;
	type0Dimensions = new unsigned int[type0DimensionsCnt];
	for(unsigned int i = 0; i < type0DimensionsCnt; i++)
	{
		type0Dimensions[i] = i;
	}

	SetupChartScale();
	InitUI();
}

CChart::CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int *Dimensions,const unsigned int DimensionsCnt,const CRungeKutta* Model)
{
	Init(Width,Height,X0,Y0,Model);	
	type = 0;

	type0DimensionsCnt = DimensionsCnt;
	if(type0DimensionsCnt > dim) type0DimensionsCnt = dim;
	type0Dimensions = new unsigned int[type0DimensionsCnt];
	for(unsigned int i = 0; i < type0DimensionsCnt; i++)
	{
		type0Dimensions[i] = Dimensions[i] - 1;
	}

	SetupChartScale();
	InitUI();
}

CChart::CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int nFirstD,const unsigned int nSecD,const CRungeKutta* Model)
{
	Init(Width,Height,X0,Y0,Model);	
	type = 1;

	if(nFirstD <= dim) nD1 = nFirstD;
	else nD1 = 0;
	if(nSecD <= dim) nD2 = nSecD;
	else nD2 = 0;
	
	SetupChartScale();
	InitUI();
}

CChart::CChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int nFirstD,const unsigned int nSecD,const unsigned int nFirdD,const CRungeKutta* Model)
{
	Init(Width,Height,X0,Y0,Model);
	type = 2;

	if(nFirstD <= dim) nD1 = nFirstD;
	else nD1 = 0;
	if(nSecD <= dim) nD2 = nSecD;
	else nD2 = 0;
	if(nFirdD <= dim) nD3 = nFirdD;
	else nD3 = 0;

	SetupChartScale();

	SetAngel(30,45);
	InitUI();
}

void CChart::Init(const int Width,const int Height,const int X0,const int Y0,const CRungeKutta* Model)
{
	model = Model;
	
	width  = Width;
	height = Height;

	dim = model->GetDim();						
	dt  = model->GetDeltaTime();	

	anim = false;
	animInt = 1;
	animCurrTime = 1;
	animTime = 1;

	mouseX = 0;
	mouseY = 0;
	drawMouse = false;

	IsDrawVecField = false;
	needBuidlPointsArray = true;
	useAutoScale = false;
	visible = true;

	values = model->Rezult;					
	unsigned int  NumOfSteps = model->GetNumberOfSteps();
	lenOfValues = NumOfSteps * dim;		
	numberOfPoints = NumOfSteps;

	ecvilibrium = new double[dim];
	model->GetEcvilibrium(ecvilibrium);

	type0DimensionsCnt = 0;
	type0Dimensions = NULL;

	x = X0;
	y = Y0;

	nD1 = -1;
	nD2 = -1;
	nD3 = -1;

	////colors = new CChartColorScheme(SColor(0.0f,0.8f,0.0f),SColor(0.8f,0.0f,0.0f),SColor(0.2f,0.2f,0.2f),SColor(0.2f,0.2f,0.2f),SColor(0.0f,0.0f,0.0f),SColor(0.1f,0.1f,0.4f),SColor(0.1f,0.1f,0.1f),SColor(0.5f,0.1f,0.1f),SColor(0.1f,0.9f,0.1f));
	colors = new CChartColorScheme(SColor(0.0f,0.8f,0.0f),SColor(0.8f,0.0f,0.0f),SColor(0.14f,0.14f,0.14f),SColor(0.2f,0.2f,0.2f),SColor(0.05f,0.05f,0.05f),SColor(0.2f,0.2f,0.2f),SColor(0.24f,0.24f,0.24f),SColor(0.62f,0.18f,0.12f),SColor(0.85f,0.85f,0.85f));
	//colors = new CChartColorScheme(SColor(0.0f,0.9f,0.0f),SColor(0.9f,0.0f,0.0f),SColor(0.7f,0.7f,0.7f),SColor(0.7f,0.7f,0.7f),SColor(0.99f,0.99f,0.99f),SColor(0.5f,0.5f,0.5f),SColor(0.7f,0.7f,0.7f),SColor(0.62f,0.18f,0.12f),SColor(0.0f,0.0f,0.0f));
	//colors = new CChartColorScheme(SColor(0.0f,0.8f,0.0f),SColor(0.8f,0.0f,0.0f),SColor(0.14f,0.14f,0.14f),SColor(0.2f,0.23f,0.23f),SColor(0.29f,0.32f,0.32f),SColor(0.2f,0.2f,0.2f),SColor(0.24f,0.24f,0.24f),SColor(0.62f,0.18f,0.12f),SColor(0.85f,0.85f,0.85f));
		
	//maxValue;
	//minValue;
	blockOffsetPoints = 0;	 
	globalOffsetPoints = 0;
	maxPointsOnScreen = 60000;
	if(maxPointsOnScreen > NumOfSteps) maxPointsOnScreen = NumOfSteps;
	currPointsOnScreen= maxPointsOnScreen;
	minPointsOnScreen = 100;
	points = new double[maxPointsOnScreen*dim + 1];

	scaleOffsetX = 1.0;
	scaleOffsetY = 1.0;
	scaleOffsetZ = 1.0;

	maxValues = new double[dim];
	minValues = new double[dim];
	drawCoeff = new double[dim+1];

	vecFieldCoord = new double[dim];
	vecFieldResCoord = new double[dim];

	tbuff = new char[255];
}

void CChart::BuildPointsArray()
{	
	unsigned int begin = globalOffsetPoints*dim;
	unsigned int end = begin + maxPointsOnScreen*dim;
	unsigned int ind = 0;

	if(end >= lenOfValues) end = lenOfValues - 1;
	for(unsigned int i = begin; i <= end ; i += 1)
	{	
		points[ind] = values[i];
		ind++;
	}
	needBuidlPointsArray = false;
}

void CChart::SetupChartScale()
{	
	if(needBuidlPointsArray) BuildPointsArray();
		
	unsigned int startPointOffset = 0;
	unsigned int endPointOffset = maxPointsOnScreen;
	if(useAutoScale)
	{
		startPointOffset = blockOffsetPoints;
		endPointOffset = blockOffsetPoints + currPointsOnScreen;
	}

	if(type == CHART_TYPE_VALUES)
	{		
		maxValues[0] = ceil( GetMaxInD(startPointOffset,endPointOffset,type0Dimensions[0]));
		minValues[0] = floor(GetMinInD(startPointOffset,endPointOffset,type0Dimensions[0]));
		for(unsigned int i = 1; i < type0DimensionsCnt; i++)
		{
			double currMax = ceil( GetMaxInD(startPointOffset,endPointOffset,type0Dimensions[i]));
			double currMin = floor(GetMinInD(startPointOffset,endPointOffset,type0Dimensions[i]));
			if(currMax > maxValues[0]) maxValues[0] = currMax;
			if(currMin < minValues[0]) minValues[0] = currMin;
		}
		scaleOffsetY = (abs(minValues[0]) + abs(maxValues[0])) * 0.1;

		drawCoeff[0] = ((double)currPointsOnScreen)*dt;
		drawCoeff[1] = maxValues[0] + scaleOffsetY;
		if(maxValues[0] < 0) drawCoeff[1] = abs(minValues[0]) + scaleOffsetY;
		if(minValues[0] < 0 && maxValues[0] > 0) drawCoeff[1] = (abs(maxValues[0])+abs(minValues[0])) + 2*scaleOffsetY;
		
	}
	if(type == CHART_TYPE_PHASE)
	{	
		for(unsigned int i = 0; i < dim; i++)
		{
			maxValues[i] = ceil( GetMaxInD(startPointOffset,endPointOffset,i));	
			minValues[i] = floor(GetMinInD(startPointOffset,endPointOffset,i));			
		}

		if(nD1 == CHART_DIM_TIME)	drawCoeff[nD1] = ((double)currPointsOnScreen)*dt;
		else
		{
			scaleOffsetX = (abs(minValues[nD1-1]) + abs(maxValues[nD1-1])) * 0.1;
			drawCoeff[nD1] = abs(maxValues[nD1-1]) + scaleOffsetX;
			if(maxValues[nD1-1] <= 0) drawCoeff[nD1] = abs(minValues[nD1-1]) + scaleOffsetX;	
			if(minValues[nD1-1] < 0 && maxValues[nD1-1] > 0) drawCoeff[nD1] = (abs(maxValues[nD1-1])+abs(minValues[nD1-1])) + 2*scaleOffsetX;
		}
		if(nD2 == CHART_DIM_TIME)	drawCoeff[nD2] = ((double)currPointsOnScreen)*dt;
		else	
		{
			scaleOffsetY = (abs(minValues[nD2-1]) + abs(maxValues[nD2-1])) * 0.1;
			drawCoeff[nD2] = abs(maxValues[nD2-1]) + scaleOffsetY;
			if(maxValues[nD2-1] <= 0) drawCoeff[nD2] = abs(minValues[nD2-1]) + scaleOffsetY;		
			if(minValues[nD2-1] < 0 && maxValues[nD2-1] > 0) drawCoeff[nD2] = (abs(maxValues[nD2-1])+abs(minValues[nD2-1])) + 2*scaleOffsetY;
		}
	}
	if(type == CHART_TYPE_INTEGRAL)
	{	
		for(unsigned int i = 0; i < dim; i++)
		{
			maxValues[i] = ceil( GetMaxInD(startPointOffset,endPointOffset,i));	
			minValues[i] = floor(GetMinInD(startPointOffset,endPointOffset,i));
		}

		if(nD1 == CHART_DIM_TIME)	drawCoeff[nD1] = ((double)currPointsOnScreen)*dt;
		else
		{
			scaleOffsetX = (abs(minValues[nD1-1]) + abs(maxValues[nD1-1])) * 0.1;
			drawCoeff[nD1] = abs(maxValues[nD1-1]) + scaleOffsetX;
			if(maxValues[nD1-1] <=  0) drawCoeff[nD1] = abs(minValues[nD1-1]) + scaleOffsetX;		
		}
		if(nD2 == CHART_DIM_TIME)	drawCoeff[nD2] = ((double)currPointsOnScreen)*dt;
		else	
		{
			scaleOffsetY = (abs(minValues[nD2-1]) + abs(maxValues[nD2-1])) * 0.1;
			drawCoeff[nD2] = abs(maxValues[nD2-1]) + scaleOffsetY;
			if(maxValues[nD2-1] <=  0) drawCoeff[nD2] = abs(minValues[nD2-1]) + scaleOffsetY;		
		}
		if(nD3 == CHART_DIM_TIME)	drawCoeff[nD3] = ((double)currPointsOnScreen)*dt;
		else		
		{
			scaleOffsetZ = (abs(minValues[nD3-1]) + abs(maxValues[nD3-1])) * 0.1;
			drawCoeff[nD3] = abs(maxValues[nD3-1]);
			if(maxValues[nD3-1] <=  0) drawCoeff[nD3] = abs(minValues[nD3-1]) + scaleOffsetZ;	
		}
	}	
}

void CChart::InitUI()
{
	chartBox.x = 0;
	chartBox.y = 0;
	chartBox.width  = width;
	chartBox.height = height;

	uiButtonClose.x = width - 20;
	uiButtonClose.y = 0;
	uiButtonClose.width  = 20;
	uiButtonClose.height = 20;

	uiButtonVecField.x = width - 40;
	uiButtonVecField.y = 0;
	uiButtonVecField.width  = 20;
	uiButtonVecField.height = 20;

	uiButtonResize.x = width  - 20;
	uiButtonResize.y = height - 20;
	uiButtonResize.width  = 20;
	uiButtonResize.height = 20;
	
	uiSlider.x = 0;
	uiSlider.y = height - 20;
	uiSlider.width  = width;
	uiSlider.height = 20;

	uiSliderMover.x = (int)(((float)(globalOffsetPoints + blockOffsetPoints) / numberOfPoints)*(width-5));
	uiSliderMover.y = height - 16;
	uiSliderMover.width  = (int)(((float)currPointsOnScreen / numberOfPoints)*(width-5));
	uiSliderMover.height = 12;
	
	uiSliderMoverButtonResize.x = uiSliderMover.x + uiSliderMover.width - 10;
	uiSliderMoverButtonResize.y = uiSliderMover.y;
	uiSliderMoverButtonResize.width  = 10;
	uiSliderMoverButtonResize.height = uiSliderMover.height;
}

CChart::~CChart()
{
	if(maxValues)delete []  maxValues;
	if(minValues)delete []  minValues;
	if(drawCoeff)delete []	drawCoeff;
	if(points) delete [] points;
	if(colors) delete colors;

	delete [] vecFieldCoord;
	delete [] vecFieldResCoord;
	delete [] type0Dimensions;

	if(ecvilibrium)	delete []	ecvilibrium;
	if(tbuff) delete [] tbuff;
}

void CChart::Draw()
{	
	CVideoManeger::SetClipingRect(x-1,y-1,x + width + 1,y + height + 1);
	const SColor *bColor = colors->GetBackColor();
	glBegin(GL_POLYGON);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex3d(x,y,1000.0);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex3d(x+width,y,1000.0);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex3d(x+width,y+height,1000.0);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex3d(x,y+height,1000.0);
	glEnd();

	if(type == CHART_TYPE_VALUES)
	{
		DrawType_0();		
		DrawUI(false);
	}else
	if(type == CHART_TYPE_PHASE)
	{
		DrawType_1();
		bool HasVecButton = ((dim == 2) && ( nD1 != 0 && nD2 != 0));
		DrawUI(HasVecButton);
	}else
	if(type == CHART_TYPE_INTEGRAL)
	{	
		DrawType_2();
		bool HasVecButton = ((dim == 3) && ( nD1 != 0 && nD2 != 0 && nD3 != 0));
		DrawUI(HasVecButton);
	}

	DrawBorder();
	CVideoManeger::UnsetClipingRect();
}

void CChart::DrawType_0()
{		
	const int height = this->height - 20;
	const double blockStartTime = (blockOffsetPoints + globalOffsetPoints) * dt;
	const double blockEndTime   = (blockOffsetPoints + globalOffsetPoints + currPointsOnScreen) * dt;
	const double blockTime = currPointsOnScreen * dt;

	const SColor *gClr = colors->GetGridColor();
	const SColor *borderClr = colors->GetBorderColor();
	const SColor *prayClr = colors->GetPrayColor();
	const SColor *predClr = colors->GetPredColor();
	const SColor *textClr = colors->GetTextColor();

	const int numsTextH = 14;
	const int numsTextW = numsTextH / 2;

	const double minValScale = abs(minValues[0]) + scaleOffsetY;
	const double maxValScale = abs(maxValues[0]) + scaleOffsetY;
	double negativePartHeight = 0.0;
	double positivePartHeight = height;
	
	double zeroLevel = height;
	if(maxValues[0] < 0)
	{
		zeroLevel = 0;
		negativePartHeight = height;
		positivePartHeight = 0;
	}
	if(minValues[0] < 0 && maxValues[0] > 0)
	{
		zeroLevel = height - minValScale / (drawCoeff[1] ) * height;
		negativePartHeight = (minValScale / (minValScale + maxValScale)) * height;
		positivePartHeight = (maxValScale / (minValScale + maxValScale)) * height;
	}

	glLineWidth(2);
	glBegin(GL_LINES);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+0,y+height);		
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+width,y+height);		
	glEnd();
	glBegin(GL_LINES);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+0,y+height);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+0,y+0);		
	glEnd();
			
	/////////////////////////////////////////////////////////////////	

	double scalse = GetScaleCoeff(blockTime,width);
	const double multipler = (scalse / blockTime);
	const double offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i=0;i<=(int)scalse;i++)
	{				
		const double xLine = x + (i / scalse) * width + (offsetInTime / scalse ) * width;
		glLineWidth(1);
		glBegin(GL_LINES);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(xLine ,y+height);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(xLine ,y+0);		
		glEnd();
	}
	
	if(zeroLevel != 0 && zeroLevel != height )
	{
		glLineWidth(3);
		glBegin(GL_LINES);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(x+0,		y+zeroLevel);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(x+width, y+zeroLevel);		
		glEnd();	
		RenderNumber(0,x+3,y+zeroLevel-numsTextH/2,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);	
	}

	scalse = GetScaleCoeff(minValScale,negativePartHeight);
	for(int i=1;i<=(int)scalse;i++)
	{
		const double yLine = y+zeroLevel+(i/scalse)*(negativePartHeight);
		glLineWidth(1);
		glBegin(GL_LINES);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(x+0,		yLine);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(x+width, yLine);		
		glEnd();		
		RenderNumber(-(i/scalse)*minValScale - 0.01,x+3,yLine-numsTextH/2,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);	
	}
	scalse = GetScaleCoeff(maxValScale,positivePartHeight);
	for(int i=1;i<=(int)scalse;i++)
	{
		const double yLine = y+zeroLevel-(i/scalse)*(positivePartHeight);
		glLineWidth(1);
		glBegin(GL_LINES);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(x+0,		yLine);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex2d(x+width, yLine);		
		glEnd();		
		RenderNumber((i/scalse)*maxValScale - 0.01,x+3,yLine-numsTextH/2,0,8,16,textClr->r,textClr->g,textClr->b);	
	}
	/////////////////////////////////////////////////////////////////
	glEnable(GL_LINE_SMOOTH);

	glLineWidth(2);	
	double point[2];
	unsigned int pointsOnScreen = currPointsOnScreen - 1;
	const double pointsCnt = (double)(pointsOnScreen*dim);
	const int yh = y + (int)zeroLevel;
	const double dCoeff1 = drawCoeff[1];

	for(unsigned int i = 0; i < type0DimensionsCnt; i++)
	{
		unsigned int Dim = type0Dimensions[i];

		glBegin(GL_LINE_STRIP);
		if(Dim == 0) glColor3f(predClr->r,predClr->g  ,predClr->b);
		else
		if(Dim == 1) glColor3f(prayClr->r,prayClr->g  ,prayClr->b);
		else		 glColor3f((prayClr->r + predClr->r)/Dim,(prayClr->g + predClr->g)/Dim  ,(prayClr->b + predClr->b)/Dim);
		
		unsigned int startInd = blockOffsetPoints*dim + Dim; 
		unsigned int endInd = (unsigned int)(startInd + currPointsOnScreen*dim);

		const double widthCoeff = width / pointsCnt;
		const double heightCoeff = -height / dCoeff1;

		if(endInd > lenOfValues) endInd = lenOfValues;
		for(unsigned int i = startInd; i < endInd; i += dim)
		{			
			point[0] = x  + (i - startInd) * widthCoeff;
			point[1] = yh + points[i] * heightCoeff;

			glVertex2dv(point);	
		}	
		glEnd();		
	}			
	glDisable(GL_LINE_SMOOTH);		

	/////////////////////////////////////////////////////////////////

	SDL_Color clr = {(Uint8)(255*textClr->r),(Uint8)(255*textClr->g),(Uint8)(255*textClr->b),0};
	SDL_Rect dest = {0,0,0,0};

	const CModelDescription* desc = ((CEcoModel*)model)->GetModelDescription();	
	if(drawMouse && (mouseX > x) && (mouseX < x + width) )
	{
		glLineWidth(1);
		glBegin(GL_LINES);					
			glColor3f(borderClr->r,borderClr->g,borderClr->b);
			glVertex2d(mouseX,y);		
			glColor3f(borderClr->r,borderClr->g,borderClr->b);
			glVertex2d(mouseX,y+height);		
		glEnd();

		CVideoManeger::glEnable2D();
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);	

		sprintf(tbuff,"t: %.2f",blockStartTime + blockTime*((double)(mouseX-x)/width));			
		dest.x=mouseX+10; dest.w=100;
		dest.y=y+height+10; dest.h=20;
		SDL_GL_RenderText(tbuff,16,clr,&dest);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		CVideoManeger::glDisable2D();	
		
		for(unsigned int i = 0; i < type0DimensionsCnt; i++)
		{
			unsigned int Dim = type0Dimensions[i];
			unsigned int ind = (unsigned int)(((double)(mouseX-x)/width)*currPointsOnScreen*dim + blockOffsetPoints*dim);   
			ind -= ind%dim;
			ind += Dim;

			double val = y+zeroLevel+(-1)*((points[ind])/drawCoeff[1])*height;

			glPointSize(5);
			glBegin(GL_POINTS);	
				if(Dim == 0) glColor3f(predClr->r,predClr->g  ,predClr->b);
				else
				if(Dim == 1) glColor3f(prayClr->r,prayClr->g  ,prayClr->b);
				else		 glColor3f((prayClr->r + predClr->r)/Dim,(prayClr->g + predClr->g)/Dim  ,(prayClr->b + predClr->b)/Dim);
				glVertex2d(mouseX,val);
			glEnd();
			glLineWidth(1);
			glBegin(GL_LINES);
				if(Dim == 0) glColor3f(predClr->r,predClr->g  ,predClr->b);
				else
				if(Dim == 1) glColor3f(prayClr->r,prayClr->g  ,prayClr->b);
				else		 glColor3f((prayClr->r + predClr->r)/Dim,(prayClr->g + predClr->g)/Dim  ,(prayClr->b + predClr->b)/Dim);
				glVertex2d(x,val);
				glVertex2d(mouseX,val);
			glEnd();

			CVideoManeger::glEnable2D();
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_ONE, GL_ONE);	
						
			sprintf(tbuff,"%s: %.2f",desc->GetValueDescription()[Dim].c_str(), points[ind]);			
			dest.x=x+10;		dest.w=120;
			dest.y=(Sint16)val; dest.h=20;
			SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_DEPTH_TEST);
			CVideoManeger::glDisable2D();	
		}
	}

	CVideoManeger::glEnable2D();
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);

	sprintf(tbuff,"Время:%.1f",blockEndTime);
	dest.x=x+width-100; dest.w=120;
	dest.y=y+height+10; dest.h=20;
	SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

	sprintf(tbuff,"%s",desc->GetName().c_str());
	int strL = strlen(tbuff);
	dest.x=x+width-35 - strL*10;	dest.w=120;	
	dest.y=y+33;  dest.h=20;
	SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	CVideoManeger::glDisable2D();	
}

void CChart::DrawType_1()
{
	const int height = this->height - 20;
	const double blockStartTime = (blockOffsetPoints + globalOffsetPoints) * dt;
	const double blockEndTime   = (blockOffsetPoints + globalOffsetPoints + currPointsOnScreen) * dt;
	const double blockTime = currPointsOnScreen * dt;

	const SColor *gClr = colors->GetGridColor();
	const SColor *prayClr = colors->GetPrayColor();
	const SColor *predClr = colors->GetPredColor();
	const SColor *textClr = colors->GetTextColor();
	const SColor *vecClr = colors->GetVecFieldColor();

	const int numsTextH = 14;
	const int numsTextW = numsTextH / 2;

	const double minValScaleX = abs(minValues[nD1 - 1]) + scaleOffsetX;
	const double maxValScaleX = abs(maxValues[nD1 - 1]) + scaleOffsetX;
	double negativePartWidth = 0.0;
	double positivePartWidth = width;
	double zeroLevelX = 0;

	if(maxValues[nD1 - 1] <= 0)
	{
		zeroLevelX = width;
		negativePartWidth = width;
		positivePartWidth = 0;
	}
	if(minValues[nD1 - 1] < 0 && maxValues[nD1 - 1] > 0)
	{
		zeroLevelX = minValScaleX / (drawCoeff[nD1] ) * width;
		negativePartWidth = (minValScaleX / (minValScaleX + maxValScaleX)) * width;
		positivePartWidth = (maxValScaleX / (minValScaleX + maxValScaleX)) * width;
	}

	const double minValScaleY = abs(minValues[nD2 - 1]) + scaleOffsetY;
	const double maxValScaleY = abs(maxValues[nD2 - 1]) + scaleOffsetY;
	double negativePartHeight = 0.0;
	double positivePartHeight = height;
	double zeroLevelY = height;

	if(maxValues[nD2 - 1] <= 0)
	{
		zeroLevelY = 0;
		negativePartHeight = height;
		positivePartHeight = 0;
	}
	if(minValues[nD2 - 1] < 0 && maxValues[nD2-1] > 0)
	{
		zeroLevelY = height - minValScaleY / (drawCoeff[nD2] ) * height;
		negativePartHeight = (minValScaleY / (minValScaleY + maxValScaleY)) * height;
		positivePartHeight = (maxValScaleY / (minValScaleY + maxValScaleY)) * height;
	}

	// =========== Vec =============
	if(IsDrawVecField && nD1 != 0 && nD2 != 0 && dim == 2)
	{
		const double scaleX = drawCoeff[nD1];
		const double scaleY = drawCoeff[nD2];
		
		const int ind1 = nD1 - 1;
		const int ind2 = nD2 - 1;

		const double dt=0.1;			//Model->GetDeltaTime()*100;
		double dx;
		double dy;
		double cx;
		double cy;
		glLineWidth(1);

		for(int i = 0; i < width; i += 20)
		{
			for(int j = 0; j < height; j += 20)
			{			
				glBegin(GL_LINES);	
				
				vecFieldCoord[ind1] = ((double)i / width)*scaleX;
				vecFieldCoord[ind2] = ((double)j / height)*scaleY;

				model->GetDirect(dt,vecFieldCoord,vecFieldResCoord);
				vecFieldResCoord[ind1] *= dt;
				vecFieldResCoord[ind2] *= dt;
				dx = (vecFieldResCoord[ind1] / scaleX) * width;
				dy = (vecFieldResCoord[ind2] / scaleY) * height;
				cx = (vecFieldCoord[ind1] / scaleX) * width;
				cy = (vecFieldCoord[ind2] / scaleY) * height;

				double len = sqrt(dx*dx+dy*dy);
				if(len > 15)
				{
					dx /= len;
					dy /= len;
					dx *= 15;
					dy *= 15;
					len = 15;
				}
				
				glColor3f(vecClr->r,vecClr->g,vecClr->b);
				glVertex2d(x+cx,y+height+(-1)*cy);

				glColor3f(vecClr->r,vecClr->g,vecClr->b);
				glVertex2d(x+cx+dx,y+height+(-1)*cy-dy);

				if(len > 10)
				{
					double alpha=atan2(dy,dx);				
					double alphar=alpha+M_PI/6;
					double alphal=alpha-M_PI/6;

					glColor3f(vecClr->r,vecClr->g,vecClr->b);
					glVertex2d(x+cx+dx,y+height+(-1)*cy-dy);
					glColor3f(vecClr->r,vecClr->g,vecClr->b);
					glVertex2d(x+dx+cx-6*cos(alphar),y+height+(-1)*cy-dy+6*sin(alphar));

					glColor3f(vecClr->r,vecClr->g,vecClr->b);
					glVertex2d(x+cx+dx,y+height+(-1)*cy-dy);
					glColor3f(vecClr->r,vecClr->g,vecClr->b);
					glVertex2d(x+dx+cx-6*cos(alphal),y+height+(-1)*cy-dy+6*sin(alphal));
				}
				glEnd();	
			}
		}
	}
	//========================================

	glLineWidth(2);
	glBegin(GL_LINE_STRIP);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+0,y+height);		
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+width,y+height);		
	glEnd();
	glBegin(GL_LINE_STRIP);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+0,y+height);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex2d(x+0,y+0);		
	glEnd();

	if(nD1 == 0)
	{
		double scalse = GetScaleCoeff(drawCoeff[nD1],width);
		double multipler = (scalse / drawCoeff[nD1]);
		double offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));	
		for(int i = 0; i <= (int)scalse; i++)
		{	
			double xLine = x + (i / scalse) * width + (offsetInTime / scalse ) * width;
			double legNumber = blockStartTime + blockTime* ((xLine-x)/width) - 0.01;

			glLineWidth(1);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(xLine,y+height);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(xLine,y+0);		
			glEnd();
			RenderNumber(legNumber,xLine,y+height-18,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);
		}
	}
	else
	{
		if(zeroLevelX != 0 && zeroLevelX != width )
		{
			glLineWidth(3);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+zeroLevelX,y+height);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+zeroLevelX,y+0);		
			glEnd();
			RenderNumber(0,x+zeroLevelX,y+height-18,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);	
		}

		double scalse = GetScaleCoeff(minValScaleX,negativePartWidth);		
		for(int i = 1; i <= (int)scalse; i++)
		{	
			double xLine = x + zeroLevelX - (i / scalse) * negativePartWidth;			
			double legNumber = ((xLine-x-zeroLevelX)/negativePartWidth)*minValScaleX - 0.01;			

			glLineWidth(1);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(xLine,y+height);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(xLine,y+0);		
			glEnd();
			RenderNumber(legNumber,xLine,y+height-18,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);
		}
		scalse = GetScaleCoeff(maxValScaleX,positivePartWidth);		
		for(int i = 1; i <= (int)scalse; i++)
		{	
			double xLine = x + zeroLevelX + (i / scalse) * positivePartWidth;			
			double legNumber = ((xLine-x-zeroLevelX)/positivePartWidth)*maxValScaleX - 0.01;			

			glLineWidth(1);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(xLine,y+height);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(xLine,y+0);		
			glEnd();
			RenderNumber(legNumber,xLine,y+height-18,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);
		}
	}
	if(nD2 == 0)
	{
		double scalse = GetScaleCoeff(drawCoeff[nD2],height);
		double multipler = (scalse / drawCoeff[nD2]);	
		double offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));	
		for(int i = 0; i <= (int)scalse; i++)
		{
			double yLine = y + height - (i / scalse) * height - (offsetInTime / scalse ) * height;
			double legNumber = blockStartTime + blockTime*((height - (yLine-y))/height) - 0.01;

			glLineWidth(1);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+0,		yLine);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+width,	yLine);		
			glEnd();
			RenderNumber(legNumber,x+3,yLine + 3,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);
		}
	}
	else
	{
		if(zeroLevelY != 0 && zeroLevelY != height )
		{
			glLineWidth(3);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+0,		y+zeroLevelY);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+width,	y+zeroLevelY);		
			glEnd();		
			RenderNumber(0,x+3,y+zeroLevelY - numsTextH/2,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);	
		}

		double scalse = GetScaleCoeff(minValScaleY,negativePartHeight);		
		for(int i = 1; i <= (int)scalse; i++)
		{
			double yLine = y + zeroLevelY + (i / scalse) * negativePartHeight;		
			double legNumber = -(i / scalse)*minValScaleY - 0.01;

			glLineWidth(1);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+0,		yLine);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+width,	yLine);		
			glEnd();
			RenderNumber(legNumber,x+3,yLine - numsTextH/2,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);
		}
		scalse = GetScaleCoeff(maxValScaleY,positivePartHeight);		
		for(int i = 1; i <= (int)scalse; i++)
		{
			double yLine = y + zeroLevelY - (i / scalse) * positivePartHeight;		
			double legNumber = (i / scalse)*maxValScaleY - 0.01;

			glLineWidth(1);
			glBegin(GL_LINE_STRIP);	
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+0,		yLine);		
				glColor3f(gClr->r,gClr->g,gClr->b);
				glVertex2d(x+width,	yLine);		
			glEnd();
			RenderNumber(legNumber,x+3,yLine - numsTextH/2,0,numsTextW,numsTextH,textClr->r,textClr->g,textClr->b);
		}
	}
		
	//==================
	double sx;
	double sy;	

	double point[2];
	double color[3];

	glEnable(GL_LINE_SMOOTH);	

	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	unsigned int startInd = blockOffsetPoints*dim; 
	unsigned int endInd = (unsigned int)(startInd + currPointsOnScreen*dim*(animCurrTime/animTime));
			
	unsigned int pointsOnScreen = currPointsOnScreen - 1;
	const double pointsCnt = (double)(pointsOnScreen*dim);
	const int xh = x + (int)zeroLevelX;
	const int yh = y + (int)zeroLevelY;
	const int nd1 = nD1 - 1;
	const int nd2 = nD2 - 1;
	const double dCoeff1 = drawCoeff[nD1];
	const double dCoeff2 = drawCoeff[nD2];

	if(endInd > lenOfValues) endInd = lenOfValues;	
	if(zeroLevelX > 0 || zeroLevelY < height) color[2] = 1.0f;
	else color[2] = 0.0f;

	color[0] = abs(predClr->r - prayClr->r);
	color[1] = abs(predClr->g - prayClr->g);

	for(unsigned int i = startInd; i < endInd; i += dim)
	{ 
		if(nD2 != 0 && nD1 != 0)
		{
			sx = points[i+nd1] / dCoeff1;
			sy = points[i+nd2] / dCoeff2;
		}
		else
		if(nD2 == 0 && nD1 == 0)
		{
			sx = (i - startInd)/ pointsCnt;
			sy = (i - startInd)/ pointsCnt;
		}
		else
		if(nD2 == 0)
		{
			sx = points[i+nd1] / dCoeff1;
			sy = (i - startInd)/ pointsCnt;			
		}
		else	
		if(nD1 == 0)
		{
			sx = (i - startInd)/ pointsCnt;
			sy = points[i+nd2] / dCoeff2;
		}
		
		//color[0] = 0.2 + sx;
		//color[1] = 0.2 + sy;	
		//color[0] = 0.9f;
		//color[1] = 0.0f;	
		glColor3dv(color);

		point[0] = xh + sx * width;
		point[1] = yh + (-1) * sy * height;
		glVertex2dv(point);
	}		
	glEnd();

	glDisable(GL_LINE_SMOOTH);	

		CVideoManeger::glEnable2D();
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);	

		const CModelDescription* desc = ((CEcoModel*)model)->GetModelDescription();	
		SDL_Color clr = {(Uint8)(255*textClr->r),(Uint8)(255*textClr->g),(Uint8)(255*textClr->b),0};
		SDL_Rect dest = {0,0,0,0};

		sprintf(tbuff,"%s",desc->GetValueDescription()[nd2].c_str());			
		dest.x=x+8;		dest.w=120;
		dest.y=y+33;		dest.h=20;
		SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

		sprintf(tbuff,"%s",desc->GetValueDescription()[nd1].c_str());	
		int strL = strlen(tbuff);
		dest.x=x+width-5 - strL*10;	dest.w=120;
		dest.y=y+height+10;				dest.h=20;
		SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

		sprintf(tbuff,"%s",desc->GetName().c_str());
		strL = strlen(tbuff);
		dest.x=x+width-55 - strL*10;	dest.w=120;	
		dest.y=y+33;  dest.h=20;
		SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		CVideoManeger::glDisable2D();	
			
	glPointSize(3);
	glBegin(GL_POINTS);	
		glColor3f(0.9f,0.0,0.0);
		glVertex2d(x+(ecvilibrium[nD1-1]/drawCoeff[nD1])*width,y+height+(-1)*(ecvilibrium[nD2-1]/drawCoeff[nD2])*height);
	glEnd();
}

void CChart::DrawType_2()
{
	const int zeroLevel = 0;
	const int height = this->height - 20;
	const double blockStartTime = (blockOffsetPoints + globalOffsetPoints) * dt;
	const double blockEndTime   = (blockOffsetPoints + globalOffsetPoints + currPointsOnScreen) * dt;
	const double blockTime = currPointsOnScreen * dt;

	const SColor *gClr = colors->GetGridColor();
	const SColor *prayClr = colors->GetPrayColor();
	const SColor *predClr = colors->GetPredColor();
	const SColor *textClr = colors->GetTextColor();
	const SColor *vecClr = colors->GetVecFieldColor();

	bool hasNegValues = false;
	for(unsigned int i = 0; i < dim; i++)
	{
		if(minValues[i] < 0) 
		{
			hasNegValues = true;
			break;
		}
	}

	glPushMatrix();
	glTranslated(x+width/2,y+height/2,0);
	glRotated(angX,1,0,0);
	glRotated(angY,0,1,0);

	glLineWidth(2);
	glBegin(GL_LINE_STRIP);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex3d(0,0,0);		
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex3d(width/2,0,0);		
	glEnd();
	glBegin(GL_LINE_STRIP);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex3d(0,0,0);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex3d(0,-height/2,0);		
	glEnd();
	glBegin(GL_LINE_STRIP);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex3d(0,0,0);	
		glColor3f(gClr->r,gClr->g,gClr->b);
		glVertex3d(0,0,-width/2);		
	glEnd();

	if(hasNegValues) glDepthFunc(GL_LEQUAL);
				
	double scalse = GetScaleCoeff(drawCoeff[nD1],width);
	int scaleInd = (int)floor(scalse);
	double multipler = (scalse / drawCoeff[nD1]);
	double offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i = 0; i <= (int)scalse; i++)
	{
		double xLine = (i/scalse)*(width/2);
		if(nD1 == 0) xLine = (i/scalse)*(width/2) + (offsetInTime / scalse) * (width / 2); 
		if(xLine > (width / 2))break;
		double legNumber = (xLine)/(width/2)*drawCoeff[nD1] - 0.01;
		if(nD1 == 0) legNumber = blockStartTime + blockTime*((xLine)/(width/2)) - 0.01;
		
		glLineWidth(1);
		glBegin(GL_LINE_STRIP);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(xLine,-(height/2),0);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(xLine,0,0);		
		glEnd();
		if(i != 0)RenderNumber(legNumber,xLine-11,-19,0,8,16,textClr->r,textClr->g,textClr->b);
	}
	scalse = GetScaleCoeff(drawCoeff[nD2],height);
	scaleInd = (int)floor(scalse);
	multipler = (scalse / drawCoeff[nD2]);
	offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i = 0; i <= scaleInd; i++)
	{
		double yLine = -(i/scalse)*(height/2);
		if(nD2 == 0) yLine = -(i/scalse)*(height/2) - (offsetInTime / scalse) * (height / 2); 
		if(yLine < -(height / 2))break;
		double legNumber = (-yLine)/(height/2)*drawCoeff[nD2] - 0.01;
		if(nD2 == 0) legNumber = blockStartTime + blockTime*((-yLine)/(height/2)) - 0.01;

		glLineWidth(1);
		glBegin(GL_LINE_STRIP);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(0,		yLine,0);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d((width/2),	yLine,0);		
		glEnd();
		if(i != 0) RenderNumber(legNumber,3,yLine + 3,0,8,16,textClr->r,textClr->g,textClr->b);
	}
	scalse = GetScaleCoeff(drawCoeff[nD2],height);
	scaleInd = (int)floor(scalse);
	multipler = (scalse / drawCoeff[nD2]);
	offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i = 0; i <= scaleInd; i++)
	{
		double yLine = -(i/scalse)*(height/2);
		if(nD2 == 0) yLine = -(i/scalse)*(height/2) - (offsetInTime / scalse) * (height / 2); 
		if(yLine < -(height / 2))break;

		glLineWidth(1);
		glBegin(GL_LINE_STRIP);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(0,yLine,-(width/2));		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(0,yLine,0);		
		glEnd();
	}

	scalse = GetScaleCoeff(drawCoeff[nD3],width);
	scaleInd = (int)floor(scalse);
	multipler = (scalse / drawCoeff[nD3]);
	offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i = 0; i <= scaleInd; i++)
	{
		double zLine = -(i/scalse)*(width/2);
		if(nD3 == 0) zLine = -(i / scalse) * (width / 2) - (offsetInTime / scalse) * (width / 2); 
		if(zLine < -(width / 2))break;
		double legNumber = (-zLine)/(width/2)*drawCoeff[nD3]  - 0.01;
		if(nD3 == 0) legNumber = blockStartTime + blockTime*((-zLine)/(width/2)) - 0.01;

		glLineWidth(1);
		glBegin(GL_LINE_STRIP);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(0,-(height/2),zLine);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(0,0,zLine);		
		glEnd();
		RenderNumber(legNumber,3,-19,zLine+3,8,16,textClr->r,textClr->g,textClr->b);
	}
	scalse = GetScaleCoeff(drawCoeff[nD3],width);
	multipler = (scalse / drawCoeff[nD3]);
	offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i = 0; i <= (int)scalse; i++)
	{
		double zLine = -(i/scalse)*(width/2);
		if(nD3 == 0) zLine = -(i / scalse) * (width / 2) - (offsetInTime / scalse) * (width / 2); 
		if(zLine < -(width / 2))break;

		glLineWidth(1);
		glBegin(GL_LINE_STRIP);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d((width/2),0,zLine);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(0,0,zLine);		
		glEnd();
	}
	scalse = GetScaleCoeff(drawCoeff[nD1],width);
	multipler = (scalse / drawCoeff[nD1]);
	offsetInTime = 1.0 - (blockStartTime*multipler - (int)(blockStartTime*multipler));
	for(int i = 0; i <= (int)scalse; i++)
	{
		double xLine = (i/scalse)*(width/2);
		if(nD1 == 0) xLine = (i/scalse)*(width/2) + (offsetInTime / scalse) * (width / 2); 
		if(xLine > (width / 2))break;

		glLineWidth(1);
		glBegin(GL_LINE_STRIP);	
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(xLine,	0	,0);		
			glColor3f(gClr->r,gClr->g,gClr->b);
			glVertex3d(xLine,0	,-(width/2));		
		glEnd();
	}
		
	double sx;
	double sy;
	double sz;

	double color[3];
	double point[3];
	
	glEnable(GL_LINE_SMOOTH);	

	glLineWidth(1);	
	unsigned int startInd = blockOffsetPoints*dim; 
	unsigned int endInd = (unsigned int)(startInd + currPointsOnScreen*dim*(animCurrTime/animTime));
	
	unsigned int pointsOnScreen = currPointsOnScreen - 1;
	const double pointsCnt = (double)(pointsOnScreen*dim);
	const int nd1 = nD1 - 1;
	const int nd2 = nD2 - 1;
	const int nd3 = nD3 - 1;
	const double dCoeff1 = drawCoeff[nD1];
	const double dCoeff2 = drawCoeff[nD2];
	const double dCoeff3 = drawCoeff[nD3];
	const int negHalfH = -height/2;
	const int halfW = width/2;
	const int negHalfW = -width/2;

	if(endInd > lenOfValues) endInd = lenOfValues;

	color[0] = abs(predClr->r - prayClr->r);
	color[1] = abs(predClr->g - prayClr->g);
	color[2] = abs(predClr->b - prayClr->b);
	glColor3dv(color);

	glBegin(GL_LINE_STRIP);
	for(unsigned int i = startInd; i < endInd; i+= dim)
	{	
		sx = points[i+nd1] / dCoeff1;
		sy = points[i+nd2] / dCoeff2;
		sz = points[i+nd3] / dCoeff3;

		if(nD3 == 0) sz = (i - startInd)/ pointsCnt;
		if(nD2 == 0) sy = (i - startInd)/ pointsCnt;
		if(nD1 == 0) sx = (i - startInd)/ pointsCnt;	

		point[0] = sx * halfW;
		point[1] = sy * negHalfH;
		point[2] = sz * negHalfW;
		glVertex3dv(point);
	}		
	glEnd();	
	glDisable(GL_LINE_SMOOTH);	
	if(hasNegValues) glDepthFunc(GL_ALWAYS);

	CVideoManeger::glEnable2D();
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);	

	const CModelDescription* desc = ((CEcoModel*)model)->GetModelDescription();	
	SDL_Color clr = {(Uint8)(255*textClr->r),(Uint8)(255*textClr->g),(Uint8)(255*textClr->b),0};
	SDL_Rect dest = {0,0,0,0};

	sprintf(tbuff,"%s",desc->GetName().c_str());
	int strL = strlen(tbuff);
	dest.x=x+width-55 - strL*10;	dest.w=120;	
	dest.y=y+33;  dest.h=20;
	SDL_GL_RenderTextCyrillic(tbuff,16,clr,&dest);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	CVideoManeger::glDisable2D();	

	glPopMatrix();
}

void CChart::DrawBorder()
{
	const SColor* bColor = colors->GetBorderColor(); 

	glLineWidth(2);
	glBegin(GL_LINE_STRIP);			
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x,y+height);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+width,y+height);		
	glEnd();
	glBegin(GL_LINE_STRIP);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x,y+height);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x,y);		
	glEnd();
	glBegin(GL_LINE_STRIP);	
		glBegin(GL_LINE_STRIP);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x,y);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+width,y);		
	glEnd();
	glBegin(GL_LINE_STRIP);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+width,y+height);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+width,y);		
	glEnd();
}

void CChart::DrawUI(const bool HasVecButton)
{
	const SColor* bColor = colors->GetBorderColor();
	const SColor* backColor  = colors->GetBackColor();
	const SColor* bTextColor = colors->GetButtonTextColor();
	const SColor* buttonColor = colors->GetButtonColor();
	// Close Button
	glBegin(GL_POLYGON);			
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiButtonClose.x,y+uiButtonClose.y);		
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiButtonClose.x + uiButtonClose.width,y+uiButtonClose.y);	
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiButtonClose.x + uiButtonClose.width,y+uiButtonClose.y + uiButtonClose.height);		
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiButtonClose.x,y+uiButtonClose.y + uiButtonClose.height);	
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiButtonClose.x,y+uiButtonClose.y);	
	glEnd();
	//  Close Button Text
	glLineWidth(1);
	glBegin(GL_LINES);	
		glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
		glVertex2d(x+uiButtonClose.x+4,y+uiButtonClose.y+4);		
		glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
		glVertex2d(x+uiButtonClose.x + uiButtonClose.width-4,y+uiButtonClose.y + uiButtonClose.height-4);	
		glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
		glVertex2d(x+uiButtonClose.x + uiButtonClose.width-4,y+uiButtonClose.y+4);		
		glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
		glVertex2d(x+uiButtonClose.x+4,y+uiButtonClose.y + uiButtonClose.height-4);	
	glEnd();
	// Vec Button
	if(HasVecButton)
	{
		glBegin(GL_POLYGON);				
			glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
			glVertex2d(x+uiButtonVecField.x,y+uiButtonVecField.y);		
			glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
			glVertex2d(x+uiButtonVecField.x + uiButtonVecField.width,y+uiButtonVecField.y);	
			glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
			glVertex2d(x+uiButtonVecField.x + uiButtonVecField.width,y+uiButtonVecField.y + uiButtonVecField.height);		
			glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
			glVertex2d(x+uiButtonVecField.x,y+uiButtonVecField.y + uiButtonVecField.height);	
			glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
			glVertex2d(x+uiButtonVecField.x,y+uiButtonVecField.y);	
		glEnd();
		//  Vec Button Text
		glLineWidth(1);
		glEnable(GL_LINE_SMOOTH);
		glBegin(GL_LINES);	
			glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
			glVertex2d(x+uiButtonVecField.x+5,y+uiButtonVecField.y+4);		
			glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
			glVertex2d(x+uiButtonVecField.x + uiButtonVecField.width/2,y+uiButtonVecField.y + uiButtonVecField.height-4);	
			glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
			glVertex2d(x+uiButtonVecField.x + uiButtonVecField.width/2,y+uiButtonVecField.y + uiButtonVecField.height-4);		
			glColor3f(bTextColor->r,bTextColor->g,bTextColor->b);
			glVertex2d(x+uiButtonVecField.x + uiButtonVecField.width-5,y+uiButtonVecField.y+4);	
		glEnd();
		glDisable(GL_LINE_SMOOTH);
	}
	// Slider
	glBegin(GL_POLYGON);	
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiSlider.x,y+uiSlider.y);		
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiSlider.x + uiSlider.width,y+uiSlider.y);	
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiSlider.x + uiSlider.width,y+uiSlider.y + uiSlider.height);		
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiSlider.x,y+uiSlider.y + uiSlider.height);	
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiSlider.x,y+uiSlider.y);	
	glEnd();
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSlider.x,y+uiSlider.y);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSlider.x + uiSlider.width,y+uiSlider.y);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSlider.x + uiSlider.width,y+uiSlider.y + uiSlider.height);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSlider.x,y+uiSlider.y + uiSlider.height);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSlider.x,y+uiSlider.y);	
	glEnd();

	// Slider Mover
	glBegin(GL_POLYGON);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSliderMover.x,y+uiSliderMover.y);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSliderMover.x + uiSliderMover.width,y+uiSliderMover.y);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSliderMover.x + uiSliderMover.width,y+uiSliderMover.y + uiSliderMover.height);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSliderMover.x,y+uiSliderMover.y + uiSliderMover.height);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiSliderMover.x,y+uiSliderMover.y);	
	glEnd();
	// Slider Mover Button Resize
	glBegin(GL_POLYGON);	
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiSliderMoverButtonResize.x,y+uiSliderMoverButtonResize.y);		
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiSliderMoverButtonResize.x + uiSliderMoverButtonResize.width,y+uiSliderMoverButtonResize.y);	
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiSliderMoverButtonResize.x + uiSliderMoverButtonResize.width,y+uiSliderMoverButtonResize.y + uiSliderMoverButtonResize.height);		
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiSliderMoverButtonResize.x,y+uiSliderMoverButtonResize.y + uiSliderMoverButtonResize.height);	
		glColor3f(buttonColor->r,buttonColor->g,buttonColor->b);
		glVertex2d(x+uiSliderMoverButtonResize.x,y+uiSliderMoverButtonResize.y);	
	glEnd();

	// Resize Button
	glLineWidth(2);
	glBegin(GL_POLYGON);	
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiButtonResize.x,y+uiButtonResize.y + uiButtonResize.height);		
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiButtonResize.x+uiButtonResize.width,y+uiButtonResize.y + uiButtonResize.height);		
		glColor3f(backColor->r,backColor->g,backColor->b);
		glVertex2d(x+uiButtonResize.x+uiButtonResize.width,y+uiButtonResize.y);		
	glEnd();
	glBegin(GL_LINES);	
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiButtonResize.x,y+uiButtonResize.y + uiButtonResize.height);		
		glColor3f(bColor->r,bColor->g,bColor->b);
		glVertex2d(x+uiButtonResize.x+uiButtonResize.width,y+uiButtonResize.y);		
	glEnd();
}

const int CChart::HitTest(const int X,const int Y) const
{
	if(	(y + uiButtonClose.y + uiButtonClose.height > Y)&&
		(y + uiButtonClose.y < Y)&&
		(x + uiButtonClose.x + uiButtonClose.width > X)&&
		(x + uiButtonClose.x < X) )
	{		
		return CHART_BUTTON_CLOSE;
	}
	if(	(y + uiButtonVecField.y + uiButtonVecField.height > Y)&&
		(y + uiButtonVecField.y < Y)&&
		(x + uiButtonVecField.x + uiButtonVecField.width > X)&&
		(x + uiButtonVecField.x < X) )
	{		
		return CHART_BUTTON_VEC;
	}
	if(	(y + uiButtonResize.y + uiButtonResize.height > Y)&&
		(y + uiButtonResize.y + (x + uiButtonResize.x + uiButtonResize.width - X)< Y)&&
		(x + uiButtonResize.x + uiButtonResize.width > X)&&
		(x + uiButtonResize.x < X) )
	{		
		return CHART_BUTTON_RESIZE;
	}
	if(	(y + uiSliderMoverButtonResize.y + uiSliderMoverButtonResize.height > Y)&&
		(y + uiSliderMoverButtonResize.y < Y)&&
		(x + uiSliderMoverButtonResize.x + uiSliderMoverButtonResize.width > X)&&
		(x + uiSliderMoverButtonResize.x < X) )
	{		
		return CHART_SLIDER_MOVER_RESIZE;
	}
	if(	(y + uiSliderMover.y + uiSliderMover.height > Y)&&
		(y + uiSliderMover.y < Y)&&
		(x + uiSliderMover.x + uiSliderMover.width > X)&&
		(x + uiSliderMover.x < X) )
	{		
		return CHART_SLIDER_MOVER;
	}
	if(	(y + uiSlider.y + uiSlider.height > Y)&&
		(y + uiSlider.y < Y)&&
		(x + uiSlider.x + uiSlider.width > X)&&
		(x + uiSlider.x < X) )
	{		
		return CHART_SLIDER;
	}

	if(	(y + height - 3 > Y)&&
		(y + 3 < Y)&&
		(x + 3 < X)&&
		(x + width - 3 > X) )
	{		
		return CHART_CONTENT;
	}	

	return CHART_NONE;
}

void CChart::DrawVecField(const bool draw)
{
	IsDrawVecField = draw;
}

const bool CChart::IsVecField() const
{
	return IsDrawVecField;
}

const double CChart::GetMax(const unsigned int Begin,const unsigned int End) const
{
	unsigned int begin = Begin * dim;	
	unsigned int end = End * dim;
	if(end > numberOfPoints * dim) end = numberOfPoints * dim;
	if(begin > end) begin = end;

	double max = points[begin];
	for(unsigned int i = begin; i < end; i++)
	{	
		if(points[i] > max)max = points[i];
	}		
	return max;
}

const double CChart::GetMin(const unsigned int Begin,const unsigned int End) const
{
	unsigned int begin = Begin * dim;	
	unsigned int end = End * dim;
	if(end > numberOfPoints * dim) end = numberOfPoints * dim;
	if(begin > end) begin = end;

	double min = points[begin];
	for(unsigned int i = begin; i < end; i++)
	{	
		if(points[i] < min)min = points[i];
	}		
	return min;
}

const double CChart::GetMaxInD(const unsigned int Begin,const unsigned int End,const unsigned int Dim) const
{
	unsigned int dimension = Dim;
	if(dimension >= dim) dimension %= dim; 

	unsigned int begin = Begin * dim + dimension;	
	unsigned int end = End * dim + dimension;	
	if(end > numberOfPoints * dim) end = numberOfPoints * dim;
	if(begin > end) begin = end;

	double max = points[begin];
	for(unsigned int i = begin; i < end; i+= dim)
	{	
		if(points[i] > max)max = points[i];
	}		
	return max;
}

const double CChart::GetMinInD(const unsigned int Begin,const unsigned int End,const unsigned int Dim) const
{
	unsigned int dimension = Dim;
	if(dimension >= dim) dimension %= dim; 

	unsigned int begin = Begin * dim + dimension;	
	unsigned int end = End * dim + dimension;	
	if(end > maxPointsOnScreen * dim) end = maxPointsOnScreen * dim;
	if(begin > end) begin = end;

	double min = points[begin];
	for(unsigned int i = begin; i < end; i+= dim)
	{	
		if(points[i] < min)min = points[i];
	}		
	return min;
}

void CChart::SetPointsStartPos(const unsigned int StartPos)
{	
	if(StartPos == globalOffsetPoints + blockOffsetPoints)
	{
		return;
	}
	if(StartPos < globalOffsetPoints)
	{
		globalOffsetPoints = StartPos;
		blockOffsetPoints = 0;
		needBuidlPointsArray = true;
	}
	else
	if(StartPos + currPointsOnScreen >= numberOfPoints)
	{
		globalOffsetPoints = numberOfPoints - currPointsOnScreen;
		blockOffsetPoints = 0;
		needBuidlPointsArray = true;
	}
	else
	if((StartPos - globalOffsetPoints) + currPointsOnScreen > maxPointsOnScreen )
	{
		globalOffsetPoints = StartPos;
		blockOffsetPoints = 0;
		needBuidlPointsArray = true;		
	}
	else
	{
		blockOffsetPoints = StartPos - globalOffsetPoints;
	}	
	SetupChartScale();
	InitUI();
}
void CChart::SetChartScale(const unsigned int NumPointsOnScreen)
{
	currPointsOnScreen = NumPointsOnScreen;
	if(currPointsOnScreen < minPointsOnScreen) currPointsOnScreen = minPointsOnScreen;
	if(currPointsOnScreen > maxPointsOnScreen) currPointsOnScreen = maxPointsOnScreen;

	if(blockOffsetPoints + currPointsOnScreen > maxPointsOnScreen)
	{
		globalOffsetPoints += blockOffsetPoints;
		blockOffsetPoints = 0;
		needBuidlPointsArray = true;		
	}
	else	
	if(blockOffsetPoints + globalOffsetPoints + currPointsOnScreen > numberOfPoints)
	{
		currPointsOnScreen = numberOfPoints - globalOffsetPoints - blockOffsetPoints - 1;				
	}	
	SetupChartScale();
	InitUI();
}
const unsigned int CChart::GetPointsStartPos() const
{
	return globalOffsetPoints + blockOffsetPoints;
}
const unsigned int CChart::GetChartScale() const
{
	return currPointsOnScreen;
}

const unsigned int CChart::GetNumberOfPoints() const
{
	return numberOfPoints;
}

const CChartColorScheme* CChart::GetChartColorScheme() const
{
	return colors;
}

const int CChart::GetHeight() const
{
	return height;
}

const int CChart::GetWidth() const
{
	return width;
}

const int CChart::GetType() const
{
	return type;
}

const int CChart::GetX() const
{
	return x;
}

const int CChart::GetY() const
{
	return y;
}

const int CChart::GetD1() const
{
	return nD1;
}

const int CChart::GetD2() const
{
	return nD2;
}

const int CChart::GetD3() const
{
	return nD3;
}

const CRungeKutta* CChart::GetModel() const
{
	return model;
}

const double CChart::GetAngX() const 
{
	return angX;
}

const double CChart::GetAngY() const 
{
	return angY;
}

const bool CChart::IsVisible() const
{
	return visible;
}

const double CChart::GetScaleCoeff(const double DrCoeff,const double Size) const
{
	if(Size == 0) return 0;
	if(Size/abs(DrCoeff) >= 25)		return abs(DrCoeff);
	if(Size/abs(DrCoeff) < 0.00025)	return 1; 
	if(Size/abs(DrCoeff) < 0.0025)	return abs(DrCoeff)/10000; 
	if(Size/abs(DrCoeff) < 0.025)	return abs(DrCoeff)/10000; 
	if(Size/abs(DrCoeff) < 0.25)		return abs(DrCoeff)/1000; 
	if(Size/abs(DrCoeff) < 2.5)		return abs(DrCoeff)/100; 
	if(Size/abs(DrCoeff) <  25)		return abs(DrCoeff)/10; 

	return abs(DrCoeff);
}

const unsigned int CChart::GetType0DimsCnt() const
{
	if(type != CHART_TYPE_VALUES) return 0;
	return type0DimensionsCnt;
}
const unsigned int* CChart::GetType0Dims() const
{
	if(type != CHART_TYPE_VALUES) return NULL;
	return type0Dimensions;
}

void CChart::Resize(const int Width,const int Height)
{
	width  = Width;
	height = Height;	
	InitUI();
}

void CChart::SetChartColorScheme(CChartColorScheme* ColorScheme)
{
	//if(colors) delete colors;
	colors = ColorScheme;
}

void CChart::SetAngel(const double AngX,const double AngY)
{
	angX = AngX;
	angY = AngY;
}

void CChart::SetVisibility(const bool isVisible)
{
	visible = isVisible;
}

void CChart::SetPos(const int X0,const int Y0)
{
	x = X0;
	y = Y0;
}

void CChart::SetMouseX(const int X)
{
	mouseX = X;
}

void CChart::SetMouseY(const int Y)
{
	mouseY = Y;
}

void CChart::DrawMouseLine(const bool draw)
{
	drawMouse = draw;
}

void CChart::SetAnim(const double time)
{
	anim = true;
	animTime = time;

	if(time > 0) animInt = (lenOfValues/(double)dim)/time;
	else animInt = (lenOfValues/(double)dim);

	animCurrTime=  0;
}

void CChart::StopAnim()
{
	anim = false;
	animTime = 1;
	animInt = 1;
	animCurrTime = 1;	
}

const double CChart::GetAnimInt()
{
	return animInt;
}

const bool CChart::IsAnim()
{
	return anim;
}

void CChart::SetCurrAnimTime(const double NewTime)
{
	animCurrTime=NewTime;
}

const double CChart::GetCurrAnimTime()
{
	return animCurrTime;
}

const double CChart::GetAnimTime()
{
	return animTime;
}

