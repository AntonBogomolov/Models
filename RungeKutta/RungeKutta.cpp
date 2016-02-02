#include "RungeKutta.h"
#include <float.h>

CRungeKutta::CRungeKutta(int N)
{
	this->N = N;				// ��������� ����������� �������
 
    if (N < 1)
    {
        this->N = -1;			// ���� ����������� ������ �������, �� ���������� ���� ������
        return;					// � ����� �� ������������
    }

    Y	= new double[N];		// ������� ������ �������
    YY	= new double[N];		// � ���������� �������
    Y1	= new double[N];
    Y2	= new double[N];
    Y3	= new double[N];
    Y4	= new double[N];

	cntParams = 0;		
	params = 0;

	Rezult=0;
	isBroken = false;
	isPositive = true;
}

CRungeKutta::~CRungeKutta()
{
	this->N = N;				// ��������� ����������� �������

	if(Y)  delete [] Y;
	if(YY) delete [] YY;
	if(Y1) delete [] Y1;
	if(Y2) delete [] Y2;
	if(Y3) delete [] Y3;
	if(Y4) delete [] Y4;

	if(params) delete [] params;

	FreeRezult();
}

void CRungeKutta::SetInit(const double t0,const double* Y0)	// ���������� ��������� �������.
{													// t0 - ��������� �����, Y0 - ��������� �������
    t = t0;
    int i;
    for (i = 0; i < N; i++)
    {
        Y[i] = Y0[i];
    }
}

const double CRungeKutta::GetCurrent()	const 			// ������� ������� �����
{
    return t;
}

const double CRungeKutta::GetEndTime() const 
{
	return et;
}

const int CRungeKutta::GetDim() const 
{
	return N;	
}

const bool CRungeKutta::IsBroken() const
{
	return isBroken;
}

const bool CRungeKutta::IsPositive() const
{
	return isPositive;
}

void CRungeKutta::SetValLimit(const bool IsPositive)
{
	isPositive = IsPositive;
}

void CRungeKutta::FreeRezult()
{
	if(Rezult)	delete [] Rezult;	
}

void CRungeKutta::SetModelSTime(const double t0)
{
	t = t0;
}

void CRungeKutta::StartModel(const double deltaTime,const double endTime)
{	
	dt = deltaTime;
	et = endTime;
			
	if(et - t > 0)
	{
		NumberOfSteps =  (unsigned int)((et-t)/dt);
	}
	else NumberOfSteps =  (unsigned int)((t-et)/dt);

	if(Rezult) delete [] Rezult;
	Rezult = new double[(NumberOfSteps*GetDim())];

	int dimCnt = GetDim();
	unsigned int lengthOfValues = NumberOfSteps * dimCnt;

	for(unsigned int i = 0; i < lengthOfValues; i += dimCnt )
	{
		for(int j = 0; j < dimCnt; j++)
		{
			if(Y[j] < 0 && isPositive)
			{
				Y[j] = 0.0f;
			}
			if(Y[j] >= DBL_MAX || Y[j] <= -DBL_MAX)	
			{
				 Y[j] = 0.0f;
				 isBroken = true;
			}			

			Rezult[i+j]	= Y[j];			
		}
		NextStep(dt);
	}
}

const unsigned int CRungeKutta::GetNumberOfSteps() const 
{
	return NumberOfSteps;
}

const double CRungeKutta::GetDeltaTime() const 
{
	return dt;
}

void CRungeKutta::GetDirect(const double dt,const double* Coords,double* Result)const 
{
	F(dt,Coords,Result);
}

void CRungeKutta::NextStep(double dt)	// ��������� ��� ������ �����-�����, dt - ��� �� ������� (����� ���� ����������)
{
	if(dt<0)return;
	int i;
	F(t,Y,Y1);							// ���������� Y1
	for (i = 0; i < N; i++)
	{
		YY[i] = Y[i] + Y1[i] * (dt / 2.0);
	}
	F(t + dt / 2.0, YY,Y2);				// ���������� Y2
	for (i = 0; i < N; i++)
	{
		YY[i] = Y[i] + Y2[i] * (dt / 2.0);
	}
	F(t + dt / 2.0, YY,Y3);				// ���������� Y3

	for (i = 0; i < N; i++)
	{
		YY[i] = Y[i] + Y3[i] * dt;
	}
	F(t + dt, YY,Y4);					 // ���������� Y4

	for (i = 0; i < N; i++)
	{
		Y[i] = Y[i] + dt / 6.0 * (Y1[i] + 2.0 * Y2[i] + 2.0 * Y3[i] + Y4[i]); // ���������� ������� �� ����� ����	
	}

	t = t + dt;							// ��������� ���
}

      