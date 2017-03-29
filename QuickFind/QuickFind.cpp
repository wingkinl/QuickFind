
// QuickFind.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "QuickFind.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "QuickFindRichEditDoc.h"
#include "QuickFindView.h"
#include "QuickFindRichEditView.h"
#include "QuickFindScintillaDoc.h"
#include "QuickFindScintillaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

_QUICKFIND_STATE::_QUICKFIND_STATE()
{
	pQuickFindWnd = nullptr;
}

_QUICKFIND_STATE _quickfindState;


// CQuickFindApp

BEGIN_MESSAGE_MAP(CQuickFindApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CQuickFindApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()


// CQuickFindApp construction

CQuickFindApp::CQuickFindApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("QuickFind.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CQuickFindApp object

CQuickFindApp theApp;


// CQuickFindApp initialization

#ifdef _ENABLE_SCINTILLA_BUILD
HMODULE LoadLibraryFromApplicationDirectory(LPCTSTR lpFileName)
{
	//Get the Application diretory
	TCHAR szFullPath[_MAX_PATH];
	szFullPath[0] = _T('\0');
	if (GetModuleFileName(nullptr, szFullPath, _countof(szFullPath)) == 0)
		return nullptr;

	//Form the new path
	TCHAR szDrive[_MAX_DRIVE];
	szDrive[0] = _T('\0');
	TCHAR szDir[_MAX_DIR];
	szDir[0] = _T('\0');
	_tsplitpath_s(szFullPath, szDrive, sizeof(szDrive) / sizeof(TCHAR), szDir, sizeof(szDir) / sizeof(TCHAR), nullptr, 0, nullptr, 0);
	TCHAR szFname[_MAX_FNAME];
	szFname[0] = _T('\0');
	TCHAR szExt[_MAX_EXT];
	szExt[0] = _T('\0');
	_tsplitpath_s(lpFileName, nullptr, 0, nullptr, 0, szFname, sizeof(szFname) / sizeof(TCHAR), szExt, sizeof(szExt) / sizeof(TCHAR));
	_tmakepath_s(szFullPath, sizeof(szFullPath) / sizeof(TCHAR), szDrive, szDir, szFname, szExt);

	//Delegate to LoadLibrary    
	return LoadLibrary(szFullPath);
}
#endif // _ENABLE_SCINTILLA_BUILD

BOOL CQuickFindApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_QuickFindRichEditTYPE,
		RUNTIME_CLASS(CQuickFindRichEditDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CQuickFindRichEditView));
	if (!pDocTemplate)
		return FALSE;
	pDocTemplate->SetContainerInfo(IDR_QuickFindTYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

#ifdef _ENABLE_SCINTILLA_BUILD
	m_hSciDLL = LoadLibraryFromApplicationDirectory(_T("SciLexer.dll"));
	if (m_hSciDLL == nullptr)
	{
		AfxMessageBox(_T("Scintilla DLL is not installed, Please download the SciTE editor and copy the SciLexer.dll into this application's directory"));
		return FALSE;
	}

	pDocTemplate = new CMultiDocTemplate(IDR_QuickFindScintillaTYPE,
		RUNTIME_CLASS(CQuickFindScintillaDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CQuickFindScintillaView));
	if (!pDocTemplate)
		return FALSE;
	pDocTemplate->SetContainerInfo(IDR_QuickFindTYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);
#else
#pragma message("scintilla-based view not enabled, put scintilla's header files to scintilla folder to enable building it.")
#endif // _ENABLE_SCINTILLA_BUILD

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CQuickFindApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);
#ifdef _ENABLE_SCINTILLA_BUILD
	if (m_hSciDLL)
		FreeLibrary(m_hSciDLL);
#endif // _ENABLE_SCINTILLA_BUILD
	return CWinAppEx::ExitInstance();
}

// CQuickFindApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CQuickFindApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CQuickFindApp customization load/save methods

void CQuickFindApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CQuickFindApp::LoadCustomState()
{
}

void CQuickFindApp::SaveCustomState()
{
}

// CQuickFindApp message handlers



