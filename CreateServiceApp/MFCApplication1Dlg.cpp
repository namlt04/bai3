
// MFCApplication1Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMFCApplication1Dlg dialog

BEGIN_DHTML_EVENT_MAP(CMFCApplication1Dlg)
	DHTML_EVENT_ONCLICK(_T("fileButton"),OnFileButton)
	DHTML_EVENT_ONCLICK(_T("folderButton"), OnFolderButton)
	DHTML_EVENT_ONCLICK(_T("createButton"), OnCreateButton)

END_DHTML_EVENT_MAP()


CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDHtmlDialog(IDD_MFCAPPLICATION1_DIALOG, IDR_HTML_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CMFCApplication1Dlg message handlers

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CMFCApplication1Dlg::OnFileButton(IHTMLElement* /*pElement*/)
{
	CFileDialog dlg(TRUE, _T("exe"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("PE  (*.exe)|*.exe|All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName(); 
		//AfxMessageBox(path); 


		CComPtr<IHTMLDocument2> spDoc; 
		GetDHtmlDocument(&spDoc); 
		
		CComQIPtr<IHTMLDocument3> spDoc3 = spDoc;
		CComPtr<IHTMLElement> spElem;
		spDoc3->getElementById(CComBSTR(L"inputFile"), &spElem);

		CComQIPtr<IHTMLInputTextElement> spInput = spElem;
		spInput->put_value(CComBSTR(path));
		
	}
	return S_OK;
}

HRESULT CMFCApplication1Dlg::OnFolderButton(IHTMLElement* /*pElement*/)
{
	CFolderPickerDialog dlg; 
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName(); 
		CComPtr<IHTMLDocument2> spDoc; 
		GetDHtmlDocument(&spDoc); 
		CComQIPtr<IHTMLDocument3> spDoc3 = spDoc; 
		CComPtr<IHTMLElement> spElem; 
		spDoc3->getElementById(CComBSTR(L"inputFolder"), &spElem);
		CComQIPtr<IHTMLInputTextElement> spInput = spElem;
		spInput->put_value(::SysAllocString(path));
	}
	return S_OK; 
}
HRESULT CMFCApplication1Dlg::OnCreateButton(IHTMLElement* /*pElement*/)
{
	CString name, pathLog, pathBin; 
	CComPtr<IHTMLDocument2> spDoc; 
	GetDHtmlDocument(&spDoc); 
	CComQIPtr<IHTMLDocument3> spDoc3 = spDoc; 
	CComPtr<IHTMLElement> spElemName; 
	spDoc3->getElementById(CComBSTR(L"inputName"), &spElemName); 
	CComQIPtr<IHTMLInputTextElement> spInputName = spElemName; 
	BSTR bstrVal = NULL; 
	spInputName->get_value(&bstrVal); 
	name = CString(bstrVal); 
	SysFreeString(bstrVal); 


	// -------
	CComPtr<IHTMLElement> spElemBin;
	spDoc3->getElementById(CComBSTR(L"inputFile"), &spElemBin); 

	CComQIPtr<IHTMLInputTextElement> spInputBin = spElemBin; 
	CComBSTR bstrValBin;
	spInputBin->get_value(&bstrValBin); 
	pathBin = CString(bstrValBin);

	//---------------


	CComPtr<IHTMLElement> spElemLog;
	spDoc3->getElementById(CComBSTR(L"inputFolder"), &spElemLog);

	CComQIPtr<IHTMLInputTextElement> spInputLog = spElemLog;
	BSTR bstrValLog = NULL;
	spInputLog->get_value(&bstrValLog);
	pathLog = CString(bstrValLog);
	SysFreeString(bstrValLog);
	// --------- khoi tao, chay servicies
	CStringW binPath;
	binPath.Format(_T("/C sc create %s binPath= \"%s %sLogServiceBKAV.log\" start= auto && sc start %s"), name, pathBin, pathLog, name);

	AfxMessageBox(binPath); 
	ShellExecute(
		NULL,
		_T("runas"),
		_T("cmd.exe"),
		binPath, 	
		NULL,
		SW_HIDE	
		
	); 
	AfxMessageBox(name); 
	AfxMessageBox(pathBin); 
	AfxMessageBox(pathLog); 
	return S_OK; 
} 