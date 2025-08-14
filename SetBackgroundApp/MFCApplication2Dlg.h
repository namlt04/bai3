
// MFCApplication2Dlg.h : header file
//

#pragma once


// CMFCApplication2Dlg dialog
class CMFCApplication2Dlg : public CDHtmlDialog
{
// Construction
public:
	CMFCApplication2Dlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION2_DIALOG, IDH = IDR_HTML_MFCAPPLICATION2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HRESULT OnSendLog(IHTMLElement *pElement);
	HRESULT OnOpenFile(IHTMLElement *pElement);
	HRESULT OnSetBackground(IHTMLElement* pElement);

	void SendToService(CString header, CString body); 
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
};
