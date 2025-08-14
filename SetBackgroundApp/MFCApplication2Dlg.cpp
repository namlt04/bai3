
// MFCApplication2Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
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


// CMFCApplication2Dlg dialog

BEGIN_DHTML_EVENT_MAP(CMFCApplication2Dlg)
	DHTML_EVENT_ONCLICK(_T("sendLog"), OnSendLog)
	DHTML_EVENT_ONCLICK( _T("openFile"), OnOpenFile)
	DHTML_EVENT_ONCLICK( _T("setBackground"), OnSetBackground)

END_DHTML_EVENT_MAP()


CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDHtmlDialog(IDD_MFCAPPLICATION2_DIALOG, IDR_HTML_MFCAPPLICATION2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CMFCApplication2Dlg message handlers

BOOL CMFCApplication2Dlg::OnInitDialog()
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

void CMFCApplication2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCApplication2Dlg::OnPaint()
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
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CMFCApplication2Dlg::OnSendLog(IHTMLElement* /*pElement*/)
{
	CComPtr<IHTMLDocument2> spDoc2; 
	GetDHtmlDocument(&spDoc2); 

	CComQIPtr<IHTMLDocument3> spDoc3 = spDoc2;
	
	CComPtr<IHTMLElement> spEle; 
	spDoc3->getElementById(CComBSTR(L"inputLog"), &spEle);

	CComQIPtr<IHTMLInputTextElement> spInput = spEle; 
	CComBSTR bstr = nullptr; 
	spInput->get_value(&bstr); 
	CString log = CString(bstr); 
	SendToService(_T("SEND_LOG"), log); 
	return S_OK;
}

HRESULT CMFCApplication2Dlg::OnOpenFile(IHTMLElement* /*pElement*/)
{
	CFileDialog dlg(TRUE); 
	if (dlg.DoModal() == IDOK) 
	{
		CStringW path = dlg.GetPathName(); 
		// Day len man hinh ( input )
		CComPtr<IHTMLDocument2> spDoc; 
		GetDHtmlDocument(&spDoc); 


		CComQIPtr<IHTMLDocument3> spDoc3 = spDoc;

		CComPtr<IHTMLElement> spEle;
		spDoc3->getElementById(CComBSTR(L"inputPath"), &spEle);
		CComQIPtr<IHTMLInputTextElement> spInput = spEle; 
		spInput->put_value(CComBSTR(path)); 

	}
	return S_OK;
}

HRESULT CMFCApplication2Dlg::OnSetBackground(IHTMLElement* /*pElement*/)
{

	CComPtr<IHTMLDocument2> spDoc2;
	GetDHtmlDocument(&spDoc2); 

	CComQIPtr<IHTMLDocument3> spDoc3 = spDoc2; 

	CComPtr<IHTMLElement> spEle; 
	spDoc3->getElementById(CComBSTR(L"inputPath"), &spEle); 
	CComQIPtr<IHTMLInputTextElement> spInput = spEle; 
	CComBSTR bstr = nullptr; 
	spInput->get_value(&bstr);

	CString path = CString(bstr); 
	SendToService(_T("SEND_MSG"), path); 
	return S_OK; 
}

void CMFCApplication2Dlg::SendToService(CString header, CString body)
{
	// WindowApi
	HANDLE hPipe = CreateFileW(L"\\\\.\\pipe\\aatest8pipe", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); 
	if (hPipe == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("Loi ket noi toi service"));
		return; 
	}
	if (header == L"SEND_MSG")
	{
		body.Replace(L"\\", L"\\\\"); 
	}

	CString msg = header + _T("|") + body; 
	AfxMessageBox(msg);
	//CStringW msg = L"MSG_LOG|test thu";
	DWORD sizeWritten = 0;
	if (!WriteFile(hPipe, msg.GetBuffer(), (msg.GetLength() + 1) * sizeof(TCHAR), &sizeWritten, NULL))
	{
		AfxMessageBox(_T("Gui yeu cau that bai"));
	}

	wchar_t buffer[1024]; 
	DWORD sizeReceiver = 0 ;
	if (ReadFile(hPipe, buffer, sizeof(buffer), &sizeReceiver, NULL))
	{

		buffer[sizeReceiver / sizeof(wchar_t)] = L'\0';
		CString res = CString(buffer); 
		AfxMessageBox(res);
	}





}
