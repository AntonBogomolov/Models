#pragma once

/**
*	Класс служит для численного решения дифференциальных уравнений
*/
class CRungeKutta
{
private:
	int N;						/**<размерность системы		*/
    
    double* YY;					/**< внутренние переменные	*/
	double* Y1;					/**< внутренние переменные	*/
	double* Y2;					/**< внутренние переменные	*/
	double* Y3;					/**< внутренние переменные	*/
	double* Y4;					/**< внутренние переменные	*/
	double t;					/**< текущее время			*/
	double dt;
	double et;
	unsigned int NumberOfSteps;

	bool isBroken;
protected:
	bool isPositive;
public:
    double* Y;					/**< Y[0] - само решение, Y[i] - i-тая производная решения */
	double* Rezult;

	double* params;
	int		cntParams;

	CRungeKutta(int N);
	~CRungeKutta();

	virtual void	SetInit(const double t0,const double* Y0);
	virtual void	GetEcvilibrium(double* epoint)const = 0;
	const double	GetCurrent() const;
	const double	GetEndTime() const;
	virtual void _fastcall 	F(const double t,const double* Y,double* FY)const = 0;
	void			NextStep(double dt);
	const int		GetDim() const;
	virtual void	GetDirect(const double dt,const double* Coords,double* Result) const;

	const bool IsBroken() const;
	const bool IsPositive() const;
	void SetValLimit(const bool IsPositive);

	virtual void	StartModel(const double deltaTime,const double endTime);
	void FreeRezult();
	void SetModelSTime(const double t0);

	const unsigned int	GetNumberOfSteps() const;
	const double		GetDeltaTime() const;
};


 