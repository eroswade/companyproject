#pragma once

#include "GridCtrl_src/GridCtrl.h"

#include "PhraseManager.h"
#include "ControlManager.h"
#include "afxwin.h"

// CPhraseFrame ������ͼ


#define ID_MAPING_TIMER 500
#define ID_GETSTAT_TIMER 501


class CPhraseFrame : public CFormView
{
	DECLARE_DYNCREATE(CPhraseFrame)

protected:
	CPhraseFrame();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CPhraseFrame();

public:
	enum { IDD = IDD_FORMVIEW_PHRASE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	
	virtual void OnInitialUpdate();
//////////////////////////////////////////////////////////////////////////
// �������
public:
	//! �������ļ�
	//! ��ϵͳ������ �ݲ�ܶȣ�Z����ƫ�������ٶȣ���ɫ����������������
	void LoadConfig();
    CGridCtrl m_Grid;

	//! ���ͼƬ·��
	afx_msg void OnBnClickedButtonImload();

    void Load3DImage( int ScaleLayer, float m_MaxHight );

	virtual BOOL DestroyWindow();
	//CImageList m_ImageList;
	CBitmap m_bmp1;
	CBitmap m_bmp2;
	int m_DetaSpeed;

    // ������ת��Ϊ������,��STRING���
    CString BinaryRecursion(int nData);
    // npos ��0������
    int BinaryRecursionPos(int nData,int nPos);
    // ������ʾDEBUG���ݵ��б�

//////////////////////////////////////////////////////////////////////////
// ϵͳ���� 
public:

	CString m_strImagePath;

	int m_nNumberOfColor;// �ж�������ɫ
	CString m_strSysPath;
    CString m_CurrentDataConfigPath;
//////////////////////////////////////////////////////////////////////////
// ���²������ŷ����
public:
	//! ֹͣ����
	afx_msg void OnBnClickedButtonStop();
	//! �ر��ŷ� 
	afx_msg void OnBnClickedButtonShutdown();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

    void GettingPLCState();

	//! ���ŷ������READY��ʱ��,ָʾ�Ƹı�
	void SetStatusPicture(AxisFlg flg, int nIDSrvOn, int nIDRDY);
	void SetStatusPicture5410(AxisFlg flg, int nIDSrvOn, int nIDRDY);
	void WriteRunFileConfigFile();


	afx_msg void OnBnClickedButtonTest();


	afx_msg void OnBnClickedRuncalc();

	static void CV_On_Mouse(int nevent,int x,int y,int flags,void *ustc);
	void redirectPos(int x, int y);


	afx_msg void OnBnClickedButtonLoaddata();

	void AxisRunTest(int nMaxPID, int nAccessID, int nMovID, int nCheckID, AxisFlg flg);
	void AxisRunTest5410(int nMaxPID, int nAccessID, int nMovID, int nCheckID, AxisFlg flg);

	afx_msg void OnBnClickedButtonTestdlgOn();
	afx_msg void OnBnClickedButtonReloadpos();
    afx_msg void OnBnClickedButton8();
    afx_msg void OnBnClickedButtonOpenplc();
    afx_msg void OnBnClickedButtonSrvon();
    afx_msg void OnBnClickedButtonSrvoff();
	afx_msg void OnBnClickedButtonMovex();
	afx_msg void OnBnClickedButtonTestdata();
    afx_msg void OnBnClickedButtonPause();
    afx_msg void OnBnClickedButtonContinue();
	afx_msg void OnBnClickedButtonMovey();
	afx_msg void OnBnClickedButtonRstmcode();
	afx_msg void OnBnClickedButtonGetxPos();
	afx_msg void OnBnClickedButtonRoutetest();
};


