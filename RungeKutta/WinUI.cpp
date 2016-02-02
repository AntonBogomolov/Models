#include "WinUI.h"
#include "resource.h"
#include "ProgMain.h"
#include "Log.h"

CUIControl::CUIControl(HWND Hwnd, const int Id, const int BlockID, const int X, const int Y, const int Width, const int Height, const string Type, const string Name, const string Value)
{
	handle = Hwnd; 
	id = Id; 

	blockID = BlockID;
	x = X;
	y = Y;
	sx = Width;
	sy = Height;

	sName  = Name;
	sType  = Type;
	sValue = Value;
}

CUIControl::~CUIControl(void)
{
	if(handle != NULL) DestroyWindow(handle);
}

const LRESULT CUIControl::SendMessageToControl(const UINT Msg,const WPARAM WParam,const LPARAM LParam) const
{
	return SendDlgItemMessage(handle,id,Msg,WParam,LParam);
}

HWND CUIControl::GetHandle() const 
{
	return handle;
}
const long int  CUIControl::GetId() const
{
	return id;
}
const bool CUIControl::GetVisibility() const
{
	return isVisible;
}
const int  CUIControl::GetXPos() const
{
	return x;
}
const int  CUIControl::GetYPos() const
{
	return y;
}
const int  CUIControl::GetWidth() const
{
	return sx;
}
const int  CUIControl::GetHeight() const
{
	return sy;
}
const int  CUIControl::GetBlockId() const
{
	return blockID;
}

const string CUIControl::GetType() const
{
	return sType;
}
const string CUIControl::GetName() const
{
	return sName;
}
const string CUIControl::GetValue() const
{
	return sValue;
}
/////////////////////////////////////////////////////////////////
CUIContainer::CUIContainer(int BlockId)
{
	isVisible = true;
	blockId = BlockId;
}
CUIContainer::~CUIContainer()
{
	for(auto it = controls.begin(); it != controls.end(); it++)
	{		
		if( (*it) )delete (*it);
	}
	controls.clear();
}

const int CUIContainer::GetUIBlockId()
{
	return blockId;
}
const CUIControl* CUIContainer::FindControl(const int ControlId) const
{
	for(auto it = controls.begin(); it != controls.end(); it++)
	{
		CUIControl* currControl = (*it);
		if(currControl->GetId() == ControlId)
		{
			return currControl;
		}
	}
	return NULL;
}
const CUIControl* CUIContainer::FindControl(const string ControlName) const
{
	for(auto it = controls.begin(); it != controls.end(); it++)
	{
		CUIControl* currControl = (*it);
		if(currControl->GetName().compare(ControlName) == 0)
		{
			return currControl;
		}
	}
	return NULL;
}

void CUIContainer::AddControl(CUIControl* Control)
{
	if(Control != NULL)controls.push_back(Control);
}
void CUIContainer::DestroyControl(const int ControlId)
{
	for(auto it = controls.begin(); it != controls.end();)
	{
		CUIControl* currControl = (*it);
		if(currControl->GetId() == ControlId)
		{
			it = controls.erase(it);
			delete currControl;
			currControl = NULL;
			return;
		}
		it++;
	}
	return;
}
void CUIContainer::DestroyControl(const string ControlName)
{
	for(auto it = controls.begin(); it != controls.end();)
	{
		CUIControl* currControl = (*it);
		if(currControl->GetName().compare(ControlName) == 0)
		{
			it = controls.erase(it);
			delete currControl;
			currControl = NULL;
			return;
		}
		it++;
	}
	return;
}
void CUIContainer::DestroyAllControls()
{
	for(auto it = controls.begin(); it != controls.end();)
	{
		CUIControl* currControl = (*it);		
		it = controls.erase(it);
		delete currControl;
		currControl = NULL;		
	}
	controls.clear();
	return;
}
/////////////////////////////////////////////////////////////////

CWinUI::CWinUI(unsigned int AvailableID)
{
	availableID = AvailableID;
	initAvailableID = AvailableID;
	mainProgPtr = NULL;
	handle = NULL;
	hInst  = NULL;
	isVisible = false;
	needRedraw = true;
	
	defaultFont = CreateFont(14, 0, 0, 0, 0, FALSE, 0, 0, RUSSIAN_CHARSET, 0, 0, 0, 0, "Courier New");	//Times New Roman
}

CWinUI::~CWinUI(void)
{
	DeleteObject(defaultFont);
	for(auto it = uiBlocks.begin(); it != uiBlocks.end(); it++)
	{		
		if( (*it) )delete (*it);
	}
}

void CWinUI::CreateUIWindow(int Width,int Height,const unsigned int DialogId)
{
	hInst  = GetModuleHandle(NULL);
	handle = CreateDialog(hInst,MAKEINTRESOURCE(DialogId),NULL,(DLGPROC)StaticWndProc);	
    SetWindowLongPtr(handle, GWLP_USERDATA,(LONG_PTR)this);

	//int style;
    //style = GetWindowLong(handle,GWL_STYLE);
    //style ^= CS_VREDRAW;
	//style ^= CS_HREDRAW;
	//style |= WS_CLIPCHILDREN;
    //SetWindowLong(handle, GWL_STYLE, style);

	width  = Width;
	height = Height;

	SetWindowPos(handle,HWND_TOP,0,0,Width,Height,NULL);	
	isVisible = true;
}

void CWinUI::DestroyUIWindow(void)
{
	if(handle)CloseWindow(handle);
	isVisible = false;
}

void CWinUI::ShowWindow()
{
	isVisible = true;
	::ShowWindow(handle,SW_SHOW);
}

void CWinUI::HideWindow()
{
	isVisible = false;
	::ShowWindow(handle,SW_HIDE);
}

const bool CWinUI::IsWindowVisible() const
{
	return isVisible;
}

void CWinUI::SetPos(const unsigned int X,const unsigned int Y) const
{
	SetWindowPos(handle,HWND_TOP,X,Y,width,height,NULL);
}

void CWinUI::SetMainProgPointer(CProgMain* MainProgPtr)
{
	mainProgPtr = MainProgPtr;
}

const LRESULT CWinUI::SendMessageToControl(const unsigned int Id,const UINT Msg,const WPARAM WParam,const LPARAM LParam) const
{
	return SendDlgItemMessage(handle,Id,Msg,WParam,LParam);
}

// Fix this 
CUIControl*  CWinUI::CreateControl(string ControlName,string ControlType,const int x,const int y,const int cx,const int cy,string Value,DWORD Styles,const int BlockID)
{	
	HWND hwnd = NULL;
	if(BlockID < (int)uiBlocks.size() )
	{
		//char* tmp = new char[Value.length()+1];
		try
		{				
			hwnd = CreateWindow((LPSTR)ControlType.c_str(),Value.c_str(),WS_CHILD|WS_VISIBLE|Styles,x,y,cx-10,cy,this->handle,(HMENU)availableID,this->hInst,NULL);		
			SendMessage(hwnd, WM_SETFONT, WPARAM (defaultFont), TRUE);				
		}
		catch(std::exception e)
		{
			std::cout<<e.what()<<endl;
			return NULL;
		}	
		//delete tmp;
		CUIControl* control = new CUIControl(hwnd,availableID,BlockID,x,y,cx-10,cy,ControlType,ControlName,Value);

		uiBlocks[BlockID]->AddControl(control);
		availableID++;

		return control;
	}
	return NULL;
}

void CWinUI::DestroyControl(string ControlName)
{
	for(auto it = uiBlocks.begin(); it != uiBlocks.end(); it++)
	{		
		(*it)->DestroyControl(ControlName);
	}
}

void CWinUI::DestroyAllControls(void)
{
	DestroyAllUIBlocks();
	availableID = initAvailableID;
}

const int CWinUI::CreateUIBlock()
{
	CUIContainer* uiBlock = new CUIContainer(uiBlocks.size());
	uiBlocks.push_back(uiBlock);
	return uiBlock->GetUIBlockId();
}
void  CWinUI::DestroyUIBlock(const int BlockId)
{
	if(BlockId > (int)uiBlocks.size() - 1)return;

	auto it = uiBlocks.begin();	
	it += BlockId;
	if((*it)) 
	{
		CUIContainer* currContainer = (*it);
		it = uiBlocks.erase(it);
		delete currContainer;
		currContainer = NULL;
	}
}
void  CWinUI::DestroyAllUIBlocks()
{
	for(auto it = uiBlocks.begin(); it != uiBlocks.end();)
	{		
		CUIContainer* currContainer = (*it);
		currContainer->DestroyAllControls();
		it = uiBlocks.erase(it);
		delete currContainer;
		currContainer = NULL;
	}
	uiBlocks.clear();
}

const CUIControl* CWinUI::FindControl(const int ControlId) const
{
	for(auto it = uiBlocks.begin(); it != uiBlocks.end(); it++)
	{		
		CUIContainer* currContainer = (*it);
		const CUIControl* control = currContainer->FindControl(ControlId);
		if(control) return control;
	}
	return NULL; 
}
const CUIControl* CWinUI::FindControl(const string ControlName) const
{
	for(auto it = uiBlocks.begin(); it != uiBlocks.end(); it++)
	{		
		CUIContainer* currContainer = (*it);
		const CUIControl* control = currContainer->FindControl(ControlName);
		if(control) return control;
	}
	return NULL; 
}
const CUIControl* CWinUI::FindControl(const int ControlId, const int BlockId) const
{
	if(BlockId > (int)uiBlocks.size() - 1)return NULL;

	auto it = uiBlocks.begin();	
	it += BlockId;
	if((*it)) 
	{
		CUIContainer* currContainer = (*it);
		const CUIControl* control = currContainer->FindControl(ControlId);
		if(control) return control;
	}
	return NULL;
}
const CUIControl* CWinUI::FindControl(const string ControlName,const int BlockId) const
{
	if(BlockId > (int)uiBlocks.size() - 1)return NULL;

	auto it = uiBlocks.begin();	
	it += BlockId;
	if((*it)) 
	{
		CUIContainer* currContainer = (*it);
		const CUIControl* control = currContainer->FindControl(ControlName);
		if(control) return control;
	}
	return NULL;
}

const int CWinUI::GetWidth() const
{
	return width;
}
const int CWinUI::GetHeight() const
{
	return height;
}
HWND CWinUI::GetHandle() const 
{
	return handle;
}

const int CWinUI::GetUIBlockId(const int ControlId) const
{
	const CUIControl *control = FindControl(ControlId);
	if(control != NULL)
	{
		return control->GetBlockId();
	}
	return -1;
}
const int CWinUI::GetUIBlockId(const string ControlName) const
{
	const CUIControl *control = FindControl(ControlName);
	if(control != NULL) 	
	{
		return control->GetBlockId();
	}
	return -1;
}

BOOL CALLBACK CWinUI::StaticWndProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    CWinUI* pThis = (CWinUI*)(GetWindowLongPtr(hDlg,GWLP_USERDATA));
    if (pThis)
	{
        return pThis->WndProc(hDlg, message, wParam, lParam);
    } 
	else 
	{
        return DefWindowProc(hDlg, message, wParam, lParam);
    }
};

BOOL CALLBACK CWinUI::WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{         
		case WM_INITDIALOG :				
        break;
		case WM_MOVE:			
			return 0;
		break;
		case WM_ERASEBKGND:
			if(!needRedraw)	return 1;
			else return 0;

		break;
		case WM_COMMAND: 
			mainProgPtr->UIEvents(hwnd,LOWORD(wParam));
		break;
	}
	return false;
}

void CWinUI::RedrawWindow() const
{
	InvalidateRect(GetHandle(),NULL,true);	
}

void CWinUI::LockWindow()
{
	needRedraw = false;
}
void CWinUI::UnlockWindow()
{
	needRedraw = true;
}