#include "Log.h"
#include <Windows.h>

CLog* CLog::instance = NULL;

CLog::CLog(void)
{
	showErrors = false;
	showWarnings = true;
}

CLog::~CLog(void)
{
	ClearErrors();
	ClearWarnings();	
}

CLog* CLog::GetInstance()
{
	if(instance == NULL) return instance = new CLog();
	else return instance;
}

void CLog::ShowWarnings(const bool Show)
{
	showWarnings = Show;
}
void CLog::ShowWErrors(const bool Show)
{
	showErrors = Show;
}

void CLog::ClearErrors()
{
	errors.clear();
}
void CLog::ClearWarnings()
{
	warnings.clear();
}

unsigned int CLog::GetErrorsCnt() const
{
	return errors.size();
}
unsigned int CLog::GetWarningsCnt() const
{
	return warnings.size();
}

void CLog::ReportError(const std::string ErrorText)
{
	errors.push_back(ErrorText);
	if(showErrors) MessageBox(NULL,ErrorText.c_str(),"Ошибка",MB_ICONERROR);
}
void CLog::ReportWarning(const std::string WarningText)
{
	warnings.push_back(WarningText);
	if(showWarnings) MessageBox(NULL,WarningText.c_str(),"Предупреждение",MB_ICONWARNING);
}