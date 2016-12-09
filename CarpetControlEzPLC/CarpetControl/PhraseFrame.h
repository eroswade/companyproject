#pragma once

#include "GridCtrl_src/GridCtrl.h"

#include "PhraseManager.h"
#include "ControlManager.h"
#include "afxwin.h"

// CPhraseFrame 窗体视图


#define ID_MAPING_TIMER 500
#define ID_GETSTAT_TIMER 501


class CPhraseFrame : public CFormView
{
	DECLARE_DYNCREATE(CPhraseFrame)

protected:
	CPhraseFrame();           // 动态创建所使用的受保护的构造函数
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	virtual void OnInitialUpdate();
//////////////////////////////////////////////////////////////////////////
// 解析相关
public:
	//! 读配置文件
	//! 读系统参数： 容差，密度，Z方向偏移量，速度，颜色量，驱动器脉冲量
	void LoadConfig();
    CGridCtrl m_Grid;

	//! 获得图片路径
	afx_msg void OnBnClickedButtonImload();

    void Load3DImage( int ScaleLayer, float m_MaxHight );

	virtual BOOL DestroyWindow();
	//CImageList m_ImageList;
	CBitmap m_bmp1;
	CBitmap m_bmp2;
	int m_DetaSpeed;

    // 把整个转换为二进制,用STRING输出
    CString BinaryRecursion(int nData);
    // npos 从0倒着数
    int BinaryRecursionPos(int nData,int nPos);
    // 用来显示DEBUG数据的列表

//////////////////////////////////////////////////////////////////////////
// 系统参数 
public:

	CString m_strImagePath;

	int m_nNumberOfColor;// 有多少种颜色
	CString m_strSysPath;
    CString m_CurrentDataConfigPath;
//////////////////////////////////////////////////////////////////////////
// 以下参数与伺服相关
public:
	//! 停止运行
	afx_msg void OnBnClickedButtonStop();
	//! 关闭伺服 
	afx_msg void OnBnClickedButtonShutdown();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

    void GettingPLCState();

	//! 当伺服电机在READY的时候,指示灯改变
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


