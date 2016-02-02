#pragma once

#include <vector>
#include <string>

/**
*	 ласс дл€ обработки, сохранени€ и вывода пользователю сообщений об ошибках и предупреждени€х.  
*/
class CLog
{
private:
	bool showErrors;
	bool showWarnings;

	std::vector<std::string> errors;
	std::vector<std::string> warnings;

	static CLog* instance;
public:
	void ShowWarnings(const bool Show);
	void ShowWErrors(const bool Show);

	void ClearErrors();
	void ClearWarnings();

	unsigned int GetErrorsCnt() const;
	unsigned int GetWarningsCnt() const;

	void ReportError(const std::string ErrorText);
	void ReportWarning(const std::string WarningText);

	static CLog* GetInstance();

	CLog(void);
	~CLog(void);
};

