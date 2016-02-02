#pragma once
#include "global.h"

const int max_elem = 50;

class CProgMain;

using namespace std;

/**
*	Класс элемента управления
*/
class CUIControl
{
private:
	HWND handle; 
	long int id;
	bool isVisible;

	int blockID;
	int x;
	int y;
	int sx;
	int sy;
	string sType;
	string sName;
	string sValue;
public:
	CUIControl(HWND Hwnd, const int Id, const int BlockID, const int X, const int Y, const int Width, const int Height, const string Type, const string Name, const string Value);
	~CUIControl(void);

	const LRESULT SendMessageToControl(const UINT Msg,const WPARAM WParam,const LPARAM LParam) const;

	HWND GetHandle() const;
	const long int  GetId() const;
	const bool GetVisibility() const; 
	const int  GetXPos() const; 
	const int  GetYPos() const; 
	const int  GetWidth() const; 
	const int  GetHeight() const; 
	const int  GetBlockId() const; 

	const string GetType()  const; 
	const string GetName()  const; 
	const string GetValue() const; 
};

/**
*	Класс контейнера элементов управления
*/
class CUIContainer
{
private:
	vector<CUIControl*> controls;
	bool isVisible;
	int blockId;
public:
	CUIContainer(int BlockId);
	~CUIContainer();

	const int GetUIBlockId();

	const CUIControl* FindControl(const int ControlId) const;
	const CUIControl* FindControl(const string ControlName) const;

	void AddControl(CUIControl* Control);
	void DestroyControl(const int ControlId);
	void DestroyControl(const string ControlName);
	void DestroyAllControls();
};

/**
*	Класс позволяет  создавать окно и динамически добавлять, удалять и изменять различные элементы управления
*/
class CWinUI
{
private:
	HWND		handle; 
	HINSTANCE	hInst;
	HFONT  defaultFont;

	int width;
	int height;
	bool isVisible;
	bool needRedraw;

	CProgMain* mainProgPtr;
	unsigned int availableID;
	unsigned int initAvailableID;

	vector<CUIContainer*> uiBlocks;
public:
	void CreateUIWindow(const int Width = 310,const int Height = 450,const unsigned int DialogId = IDD_GUI);
	void DestroyUIWindow(void);

	CUIControl* CreateControl(string ControlName,string ControlType,const int x,const int y,const int cx,const int cy,string Value="",DWORD Styles=0,const int BlockID=0);
	void DestroyControl(const string ControlName);
	void DestroyAllControls(void);

	const int CreateUIBlock(); 
	void  DestroyUIBlock(const int BlockId); 
	void  DestroyAllUIBlocks();

	const LRESULT SendMessageToControl(const unsigned int Id,const UINT Msg,const WPARAM WParam,const LPARAM LParam) const;
	void SetMainProgPointer(CProgMain* MainProgPtr);
	void SetPos(const unsigned int X,const unsigned int Y) const;

	void LockWindow();
	void UnlockWindow();
	void RedrawWindow() const;

	void ShowWindow();
	void HideWindow();
	const bool IsWindowVisible() const;

	const int GetWidth() const;
	const int GetHeight() const;
	HWND GetHandle() const;

	const int GetUIBlockId(const int ControlId) const;
	const int GetUIBlockId(const string ControlName) const;

	const CUIControl* FindControl(const int ControlId) const;
	const CUIControl* FindControl(const string ControlName) const;
	const CUIControl* FindControl(const int ControlId, const int BlockId) const;
	const CUIControl* FindControl(const string ControlName,const int BlockId) const;

    static BOOL CALLBACK StaticWndProc(HWND, UINT, WPARAM, LPARAM);
    BOOL CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

	CWinUI(unsigned int AvailableID = 16000);
	~CWinUI(void);
};
