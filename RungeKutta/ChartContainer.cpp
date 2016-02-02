#include "ChartContainer.h"
#include "Chart.h"
#include "RungeKutta.h"
#include "VideoManeger.h"

CChartContainer::CChartContainer()
{
	maxChartId = 0;	
	charts = new CChart**[MAX_CHARTS_CNT];
	for(int i = 0; i < MAX_CHARTS_CNT; i++)
	{		
		charts[i] = new CChart*;
		(*charts[i]) = NULL; 
		//if(charts[i] != NULL)
		//{
		//	(*charts[i]) = NULL;
		//}
		//charts[i] = NULL;
		//charts[i] = new CChart*;
	}
}
CChartContainer::~CChartContainer()
{
	DeleteAllCharts();
}

const unsigned int CChartContainer::GetMaxChartId() const
{
	return maxChartId;
}

const unsigned int CChartContainer::FindMaxChartId()
{
	for(unsigned int i = 0; i < MAX_CHARTS_CNT; i++)
	{
		if(charts[i]) 
		{
			if((*charts[i]))
			{
				if( i > maxChartId ) maxChartId = i;
			}
		}		
	}
	return maxChartId;
}

void CChartContainer::DeleteAllCharts()
{
	FindMaxChartId();
	for(unsigned int i = 0; i <= maxChartId; i++)
	{
		if(charts[i]) 
		{
			if((*charts[i])) delete (*charts[i]);
			(*charts[i]) = NULL;
		}
	//	delete charts[i];
		charts[i] = NULL;		
	}
	delete [] charts;
	maxChartId = 0;
}
void CChartContainer::DeleteChart(const unsigned int ChartId)
{
	if(ChartId > maxChartId) return;
	if(charts[ChartId])
	{
		if((*charts[ChartId])) delete (*charts[ChartId]);
		(*charts[ChartId]) = NULL;
		//delete charts[ChartId];
	}
	charts[ChartId] = NULL;	

	FindMaxChartId();
}

void CChartContainer::ResizeChart(const int Width,const int Height,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id])
	{
		if((*charts[Id])) (*charts[Id])->Resize(Width,Height);
	}
}
void CChartContainer::MoveChart(const int X,const int Y,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id])
	{
		if((*charts[Id])) (*charts[Id])->SetPos(X,Y);
	}
}	

void CChartContainer::SetChartSliderPos(const unsigned int StartPos,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id])
	{
		if((*charts[Id])) (*charts[Id])->SetPointsStartPos(StartPos);
	}
}
void CChartContainer::SetChartSliderScale(const unsigned int PointsNum,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id])
	{
		if((*charts[Id])) (*charts[Id])->SetChartScale(PointsNum);
	}
}

void CChartContainer::SetChartVecField(const bool IsDrawVecField,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id])
	{
		if((*charts[Id])) (*charts[Id])->DrawVecField(IsDrawVecField);
	}
}

void CChartContainer::SetChartAngel(const double AngX,const double AngY,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id])
	{
		if((*charts[Id]))
		{
			(*charts[Id])->SetAngel(AngX,AngY);
		}
	}
}

const bool CChartContainer::GetChartVecField(const unsigned int Id) const
{
	if(Id > maxChartId)
	{
	//	exit(-1);
		return false;
	}
	if(charts[Id])
	{
		if((*charts[Id])) return (*charts[Id])->IsVecField();
	}
	//exit(-1);
	return false;
}
void CChartContainer::DrawMouseLines(const int X,const int Y,const bool IsDrawMouse,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id] && (*charts[Id])) 
	{
		(*charts[Id])->DrawMouseLine(IsDrawMouse);
		(*charts[Id])->SetMouseX(X);
		(*charts[Id])->SetMouseY(Y);
	}
}
void CChartContainer::SetChartVisibility(const bool IsChartVisible,const unsigned int Id)
{
	if(Id > maxChartId) return;
	if(charts[Id] && (*charts[Id])) 
	{
		(*charts[Id])->SetVisibility(IsChartVisible);
	}
}
const bool CChartContainer::GetChartVisibility(const unsigned int Id) const
{
	if(Id > maxChartId)
	{
	//	exit(-1);
		return false;
	}
	if(charts[Id])
	{
		if((*charts[Id])) return (*charts[Id])->IsVisible();
	}
	//exit(-1);
	return false;
}

const int CChartContainer::HitTest(const int X,const int Y)
{	
	const unsigned int ind = GetChartId(X,Y);

	if(ind > maxChartId)	return CHART_NONE;
	if(!charts[ind])		return CHART_NONE;
	if(!(*charts[ind]))		return CHART_NONE;

	CChart* currChart = (*charts[ind]);
	if(!currChart) return CHART_NONE;

	return currChart->HitTest(X,Y);
}

const unsigned int CChartContainer::FindFreeId() const
{
	for(int i = 0; i < MAX_CHARTS_CNT; i++)
	{
		if(charts[i] == NULL) return i;
		if((*charts[i]) == NULL ) return i;
	}
	return MAX_CHARTS_CNT;
}
const unsigned int CChartContainer::FindTopChartId() const
{
	for(unsigned int i = 0; i <= maxChartId; i++)
	{
		if(charts[i]) 
		{
			if((*charts[i]))
			{
				return i;
			}
		}		
	}
	return MAX_CHARTS_CNT;
}

const unsigned int CChartContainer::AddChart(CChart **Chart)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		if(charts[id]) delete charts[id];
		charts[id] = NULL;
		
		charts[id] = Chart;
		FindMaxChartId();		
	}
	return id;
}
const unsigned int CChartContainer::AddCharts(CChartContainer *Charts)
{
	for(unsigned int i = 0; i <= Charts->GetMaxChartId(); i++)
	{
		CChart** chart = Charts->GetChartPrt(i);
		if((chart != NULL) && ((*chart) != NULL))
		{
			AddChart(chart);
		}
	}
	return 0;
}
void CChartContainer::AddAndArrangeCharts(CChartContainer *Charts)
{
	for(unsigned int i = 0; i <= Charts->GetMaxChartId(); i++)
	{
		CChart** chart = Charts->GetChartPrt(i);
		if((chart != NULL) && ((*chart) != NULL))
		{
			const unsigned int id = FindFreeId();
			if(id < MAX_CHARTS_CNT)
			{
				if(charts[id]) delete charts[id];
				charts[id] = NULL;

				Box cBox = FillScreen(maxChartId);
				(*chart)->Resize(cBox.width,cBox.height);
				(*chart)->SetPos(cBox.x,cBox.y);	
		
				charts[id] = chart;
				FindMaxChartId();		
			}
		}
	}
}

const CChart* CChartContainer::GetTopChart() const
{
	unsigned int topChartInd = FindTopChartId();
	if(topChartInd > maxChartId ) return NULL;
	return (*charts[topChartInd]);
}
const CChart* CChartContainer::GetChart(const unsigned int Id) const
{
	if(Id <= maxChartId)
	{
		if(!charts[Id]) return NULL;
		return (*charts[Id]);
	}
	return NULL;
}
const CChart* CChartContainer::GetChart(const int MouseX,const int MouseY) const
{
	const unsigned int ind = GetChartId(MouseX,MouseY);
	if(ind > maxChartId) return NULL;

	return (*charts[ind]);
}
CChart* CChartContainer::GetTopChartUnsafe() const
{
	unsigned int topChartInd = FindTopChartId();
	if(topChartInd > maxChartId ) return NULL;
	return (*charts[topChartInd]);
}
CChart* CChartContainer::GetChartUnsafe(const unsigned int Id) const
{
	if(Id <= maxChartId)
	{
		if(!charts[Id]) return NULL;
		return (*charts[Id]);
	}
	return NULL;
}
CChart* CChartContainer::GetChartUnsafe(const int MouseX,const int MouseY) const
{
	const unsigned int ind = GetChartId(MouseX,MouseY);
	if(ind > maxChartId) return NULL;

	return (*charts[ind]);
}
CChart** CChartContainer::GetChartPrt(const unsigned int Id) const
{
	if(Id <= maxChartId)
	{
		return charts[Id];
	}
	return NULL;
}
CChart** CChartContainer::GetTopChartPtr() const
{
	return charts[0];
}
CChart** CChartContainer::GetChartPtr(const int MouseX,const int MouseY) const
{
	const unsigned int ind = GetChartId(MouseX,MouseY);
	if(ind > maxChartId) return NULL;

	return charts[ind];
}

const unsigned int CChartContainer::GetChartId(const int MouseX,const int MouseY) const
{
	for(unsigned int i = 0; i <= maxChartId; i++)
	{
		if(charts[i] && (*charts[i]) && (*charts[i])->IsVisible())
		{
			int chartX = (*charts[i])->GetX();
			int chartY = (*charts[i])->GetY();
			int chartW = (*charts[i])->GetWidth();
			int chartH = (*charts[i])->GetHeight();

			if(MouseX < chartX) continue;
			if(MouseY < chartY) continue;
			if(MouseX > chartX + chartW) continue;
			if(MouseY > chartY + chartH) continue;

			return i;
		}
	}
	return MAX_CHARTS_CNT;
}
const unsigned int CChartContainer::GetChartId(const CChart* Chart) const
{
	for(int i = maxChartId; i >= 0; i--)
	{
		if(charts[i] && (*charts[i])) 
		{
			if((*charts[i]) == Chart)return i;
		}
	}	
	return MAX_CHARTS_CNT;
}

const double CChartContainer::GetChartAngelX(const unsigned int Id) const 
{
	if(Id > maxChartId) return 0;
	if(charts[Id])
	{
		if((*charts[Id])) return (*charts[Id])->GetAngX();
	}
	return 0;
}

const double CChartContainer::GetChartAngelY(const unsigned int Id) const 
{
	if(Id > maxChartId) return 0;
	if(charts[Id])
	{
		if((*charts[Id])) return (*charts[Id])->GetAngY();
	} 
	return 0;
}

const bool CChartContainer::IsContainedChart(const CChart* Chart) const
{
	for(int i = maxChartId; i >= 0; i--)
	{
		if(charts[i] && (*charts[i])) 
		{
			if((*charts[i]) == Chart)return true;
		}
	}
	
	return false;
}

Box CChartContainer::FillScreen(const int maxId)
{
	const int screenW = CVideoManeger::width;
	const int screenH = CVideoManeger::height;
	int minX = screenW;
	int minY = screenH;
	int maxX = 0;
	int maxY = 0;
	int maxS = 0;
	int maxSInd = -1;
	CChart* currChart = NULL;
	Box chartBox;

	chartBox.x = 0;
	chartBox.y = 0;
	chartBox.width  = 0;
	chartBox.height = 0;

	for(int i = 0; i <= maxId ; i++)
	{
		if((charts[i]) && (*charts[i])) 
		{
			if((*charts[i])->IsVisible())
			{
				currChart = (*charts[i]);
				const int chartX = currChart->GetX(); 
				const int chartY = currChart->GetY(); 
				const int chartW = currChart->GetWidth(); 
				const int chartH = currChart->GetHeight(); 

				if(maxS < chartW * chartH)
				{
					maxS = chartW * chartH;
					maxSInd = i;
				}

				if(chartX + chartW > maxX) maxX = chartX + chartW;
				if(chartY + chartH > maxY) maxY = chartY + chartH;
				if(chartX < minX) minX = chartX;
				if(chartY < minY) minY = chartY;
			}
		}
	}
	if((minX > 100) || ( minY > 100))
	{		
		chartBox.x = 0;
		chartBox.y = 0;
		chartBox.width  = minX;
		chartBox.height = minY;
		return chartBox;
	}

	if(screenW - maxX > 100)
	{		
		chartBox.x = maxX;
		chartBox.y = 0;
		chartBox.width  = screenW - maxX;
		chartBox.height = screenH;
		return chartBox;
	}
	if(screenH - maxY > 100)
	{
		chartBox.x = 0;
		chartBox.y = maxY;
		chartBox.width  = screenW;
		chartBox.height = screenH - maxY;
		return chartBox;
	}

	if(maxSInd == -1)
	{	
		chartBox.x = 0;
		chartBox.y = 0;
		chartBox.width  = screenW;
		chartBox.height = screenH;
		return chartBox;
	}
	currChart = (*charts[maxSInd]);

	if(currChart->GetType() == 0)
	{
		int chartW = currChart->GetWidth(); 
		int chartH = currChart->GetHeight(); 
		double aspectRatio = (double)chartW / chartH;

		chartBox.x = currChart->GetX();
		chartBox.y = currChart->GetY();
		chartBox.width  = chartW;
		chartBox.height = chartH;

		if(aspectRatio < 4)
		{
			chartH = chartW / 4;
			if(chartH > screenH / 3) chartH = screenH / 3;

			currChart->Resize(chartW,chartH);
			chartBox.y = chartH + chartBox.y;
			chartBox.height -= chartH;
			return chartBox;
		}
		if(aspectRatio >= 4)
		{
			chartW = chartW / 2;
			currChart->Resize(chartW,chartH);
			chartBox.x = chartW + chartBox.x;
			chartBox.width -= chartW;
			return chartBox;
		}
	}
	if(currChart->GetType() == 1 || currChart->GetType() == 2)
	{
		int chartW = currChart->GetWidth(); 
		int chartH = currChart->GetHeight(); 
		double aspectRatio = (double)chartW / chartH;

		chartBox.x = currChart->GetX();
		chartBox.y = currChart->GetY();
		chartBox.width  = chartW;
		chartBox.height = chartH;

		if(aspectRatio < 1)
		{
			chartH = chartH / 2;
			currChart->Resize(chartW,chartH);
			chartBox.y = chartH + chartBox.y;
			chartBox.height -= chartH;
			return chartBox;
		}
		if(aspectRatio >= 1)
		{
			chartW = chartW / 2;
			currChart->Resize(chartW,chartH);
			chartBox.x = chartW + chartBox.x;
			chartBox.width -= chartW;
			return chartBox;
		}
	}
	return chartBox;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

CModelCharts::CModelCharts(const CRungeKutta* Model) : CChartContainer()
{
	model = Model;
}
CModelCharts::CModelCharts(const CModelCharts &Container,const CRungeKutta* Model) : CChartContainer()
{
	model = Model;
	for(unsigned int i = 0; i <= Container.GetMaxChartId(); i++)
	{
		const CChart* currChart = Container.GetChart(i);
		if(currChart)
		{
			CreateChart(currChart);
		}
	}
}
CModelCharts::~CModelCharts()
{

}

const CRungeKutta* CModelCharts::GetModel() const
{
	return model;
}

const int CModelCharts::HitTest(const int X,const int Y)
{	
	const unsigned int ind = GetChartId(X,Y);

	if(ind > maxChartId)	return CHART_NONE;
	if(!charts[ind])		return CHART_NONE;
	if(!(*charts[ind]))		return CHART_NONE;

	CChart* currChart = (*charts[ind]);
	if(!currChart) return CHART_NONE;
	
	return currChart->HitTest(X,Y);
}

const unsigned int CModelCharts::CreateChart(const CChart *Chart)
{
	if(Chart == NULL)return MAX_CHARTS_CNT;
	
	const int chartX = Chart->GetX();
	const int chartY = Chart->GetY();
	const int chartWidth  = Chart->GetWidth();
	const int chartHeight = Chart->GetHeight();

	const int nD1 = Chart->GetD1();
	const int nD2 = Chart->GetD2();
	const int nD3 = Chart->GetD3();

	const bool isVecField = Chart->IsVecField();
	const bool isVisible  = Chart->IsVisible();

	const double angX = Chart->GetAngX();
	const double angY = Chart->GetAngY();

	const unsigned int startPos = Chart->GetPointsStartPos();
	const unsigned int numPoints = Chart->GetChartScale();

	unsigned int id = MAX_CHARTS_CNT;
	const int chartType = Chart->GetType();

	if(chartType == 0)
	{
		const unsigned int dimCnt = Chart->GetType0DimsCnt();
		const unsigned int* oldDims = Chart->GetType0Dims();
		unsigned int *dims = new unsigned int[dimCnt];
		for(unsigned int i = 0; i < dimCnt; i++ ) dims[i] = oldDims[i] + 1;
		id = CreateChart(chartWidth,chartHeight,chartX,chartY,dimCnt,dims); 
		delete [] dims;
	}
	if(chartType == 1) id = CreateChart(chartWidth,chartHeight,chartX,chartY,nD1,nD2);
	if(chartType == 2) id = CreateChart(chartWidth,chartHeight,chartX,chartY,nD1,nD2,nD3); 

	if(id >= MAX_CHARTS_CNT) return MAX_CHARTS_CNT;
	(*charts[id])->DrawVecField(isVecField);
	(*charts[id])->SetAngel(angX,angY);
	(*charts[id])->SetVisibility(isVisible);
//	(*charts[id])->SetChartScale(numPoints);
//	(*charts[id])->SetPointsStartPos(startPos);

	maxChartId++;
	return id;
}

const unsigned int CModelCharts::CreateChart(const int Width,const int Height,const int X0,const int Y0)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(Width,Height,X0,Y0,model);
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart(const int Width,const int Height,const int X0,const int Y0,const unsigned int DimCnt, const unsigned int* Dims)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(Width,Height,X0,Y0,Dims,DimCnt,model);
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart(const int Width,const int Height,const int X0,const int Y0,const int nFirstD,const int nSecD)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(Width,Height,X0,Y0,nFirstD,nSecD,model);
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart(const int Width,const int Height,const int X0,const int Y0,const int nFirstD,const int nSecD,const int nFirdD)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(Width,Height,X0,Y0,nFirstD,nSecD,nFirdD,model);
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart()
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(CVideoManeger::width, CVideoManeger::height/3,0,0,model);
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart(const unsigned int *Dimensions,const unsigned int DimensionsCnt)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(CVideoManeger::width, CVideoManeger::height/3,0,0,Dimensions,DimensionsCnt,model);		
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart(const int nFirstD,const int nSecD)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(CVideoManeger::width/2, 2*CVideoManeger::height/3,0,CVideoManeger::height/3,nFirstD,nSecD,model);
		(*charts[id]) = chart;
		FindMaxChartId();
	}
	return id;
}
const unsigned int CModelCharts::CreateChart(const int nFirstD,const int nSecD,const int nFirdD)
{
	const unsigned int id = FindFreeId();
	if(id < MAX_CHARTS_CNT)
	{
		CChart* chart = new CChart(CVideoManeger::width/2, 2*CVideoManeger::height/3,CVideoManeger::width/2, CVideoManeger::height/3,nFirstD,nSecD,nFirdD,model);
		(*charts[id]) = chart;	
		FindMaxChartId();
	}
	return id;
}

const unsigned int CModelCharts::GetChartIdByType(const int chartType) const
{
	for(int i = maxChartId; i >= 0; i--)
	{
		if(charts[i] && (*charts[i])) 
		{
			if((*charts[i])->GetType() == chartType)  return i;
		}
	}
	return MAX_CHARTS_CNT;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
CChartCollection::CChartCollection() : CChartContainer()
{
}

CChartCollection::~CChartCollection()
{
}

void CChartCollection::DrawAllCharts() const
{
	for(int i = maxChartId; i >= 0; i--)
	{
		if(charts[i] && (*charts[i])) 
		{
			if((*charts[i])->IsVisible()) (*charts[i])->Draw();
		}
	}
}

void CChartCollection::SetTopChart(const unsigned int Id)
{
	if(Id > maxChartId) return;
	CChart** oldTopChart = charts[0];
	charts[0]  = charts[Id];
	charts[Id] = oldTopChart;
}

const int CChartCollection::HitTest(const int X,const int Y)
{	
	const unsigned int ind = GetChartId(X,Y);

	if(ind > maxChartId)	return CHART_NONE;
	if(!charts[ind])		return CHART_NONE;
	if(!(*charts[ind]))		return CHART_NONE;

	CChart* currChart = (*charts[ind]);
	if(!currChart) return CHART_NONE;

	return currChart->HitTest(X,Y);
}

