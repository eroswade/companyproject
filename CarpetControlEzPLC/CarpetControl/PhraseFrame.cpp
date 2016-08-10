// PhraseFrame.cpp : 实现文件
//

#include "stdafx.h"
#include "CarpetControl.h"
#include "PhraseFrame.h"

// Y0 请求模块准备  X0返回准备完成
// Y1 轴1定位启动

// Y30 Y32 Y34 解Y轴 切线刀 Z轴气缸 

// 复位
// X806 轴出错编号 
// X807 轴报警编号
// X1502 轴出错复位

// X3F 停止指令

// G1500 定位轴启动编号  从第几个NO.1-NO.600  9001 机械原点复位  9002 高速复位 7000-7004块指定 9003 当前值更改  9004 多轴同时启动
// G15001 只有在高级定位情况下用

// G835 获取执行中定位数据

// G1534 改目标位置  G1538 改目标位置请求  这里改完目标位置之后需要G1538确认
// G2004 改指令速度  G2006 速度确认  


// G35200 编码器轴监视数据



// CPhraseFrame
IMPLEMENT_DYNCREATE(CPhraseFrame, CFormView)

CPhraseFrame::CPhraseFrame()
	: CFormView(CPhraseFrame::IDD)
{
	m_fNeedleAngle=0;
	m_DetaSpeed=0;
}

CPhraseFrame::~CPhraseFrame()
{
}

void CPhraseFrame::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_GRID, m_Grid); 
    CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPhraseFrame, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_IMLOAD, &CPhraseFrame::OnBnClickedButtonImload)
	ON_BN_CLICKED(IDC_RUNCALC, &CPhraseFrame::OnBnClickedRuncalc)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CPhraseFrame::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_LOADDATA, &CPhraseFrame::OnBnClickedButtonLoaddata)
	ON_BN_CLICKED(IDC_BUTTON_RELOADPOS, &CPhraseFrame::OnBnClickedButtonReloadpos)
    ON_BN_CLICKED(IDC_BUTTON8, &CPhraseFrame::OnBnClickedButton8)
    ON_BN_CLICKED(IDC_BUTTON_OPENPLC, &CPhraseFrame::OnBnClickedButtonOpenplc)
    ON_BN_CLICKED(IDC_BUTTON_SRVON, &CPhraseFrame::OnBnClickedButtonSrvon)
    ON_BN_CLICKED(IDC_BUTTON_SRVOFF, &CPhraseFrame::OnBnClickedButtonSrvoff)
	ON_BN_CLICKED(IDC_BUTTON_MOVEX, &CPhraseFrame::OnBnClickedButtonMovex)
	ON_BN_CLICKED(IDC_BUTTON_TESTDATA, &CPhraseFrame::OnBnClickedButtonTestdata)
    ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CPhraseFrame::OnBnClickedButtonPause)
    ON_BN_CLICKED(IDC_BUTTON_CONTINUE, &CPhraseFrame::OnBnClickedButtonContinue)
	ON_BN_CLICKED(IDC_BUTTON_MOVEY, &CPhraseFrame::OnBnClickedButtonMovey)
	ON_BN_CLICKED(IDC_BUTTON_RSTMCODE, &CPhraseFrame::OnBnClickedButtonRstmcode)
END_MESSAGE_MAP()


// CPhraseFrame 诊断

#ifdef _DEBUG
void CPhraseFrame::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CPhraseFrame::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CPhraseFrame 消息处理程序


void CPhraseFrame::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetDocument()->SetTitle("解析");

    //m_Grid.EnableDragAndDrop(TRUE);
    //m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

    m_Grid.SetRowCount(5);
    m_Grid.SetColumnCount(16);
    m_Grid.SetFixedRowCount(1);
    for (int i=0; i<16; i++)
    {
        GV_ITEM Item;
        Item.mask = GVIF_TEXT;
        Item.row = 0;
        Item.col = 15-i;
        int nword = 48+i;
        if (nword > 57)
        {
            nword += 7;
        }
        char z = nword;
        Item.strText = z;
        m_Grid.SetItem(&Item);
    }
    for (int j=0; j<4; j++)
    {
        for (int i=0; i<16; i++)
        {
            GV_ITEM Item;
            Item.mask = GVIF_TEXT;
            Item.row = j+1;
            Item.col = i;
            Item.strText = "0";
            Item.crBkClr=RGB(255,0,0);
            m_Grid.SetItem(&Item);
        }
    }
    
    m_Grid.AutoSize();

	CString strModelPath;
	GetModuleFileName(0,strModelPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	strModelPath.ReleaseBuffer();
	int npos = strModelPath.ReverseFind('\\');
	strModelPath=strModelPath.Left(npos);
	m_strSysPath.Format("%s\\%s",strModelPath,"Config.ini");
	LoadConfig();




    //HRESULT	hr = CoCreateInstance(	CLSID_ActProgType,
    //    NULL,
    //    CLSCTX_INPROC_SERVER,
    //    IID_IActProgType,
    //    (LPVOID*)&mp_IProgType);
    //if(!SUCCEEDED(hr)){
    //    AfxMessageBox("CoCrateInstance() Failed.");
    //}
    // ActUtlType Control
    HRESULT hr = CoCreateInstance(	CLSID_ActUtlType,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IActUtlType,
        (LPVOID*)&mp_IUtlType);
    if(!SUCCEEDED(hr)){
        AfxMessageBox("CoCrateInstance() Failed.");
    }

	//if( d2410_board_init() <= 0 )//控制卡的初始化操作
	//	MessageBox("初始化DMC2410卡失败！","出错");

 //   if (dmc_board_init() <=0 )
 //   {
 //       MessageBox("初始化DMC8410卡失败！","出错");
 //   }

	//for (int i=0; i<AXIS_COUNT; i++)
	//{
	//	//d2410_variety_speed_range( i, 1, 9000);
	//	d2410_set_pulse_outmode(i,5);
	//	dmc_set_pulse_outmode(0,i,5);
	//}
	cvNamedWindow("MainDispWin",CV_WINDOW_NORMAL);// CV_WINDOW_AUTOSIZE

	CWnd* cImageWnd = GetDlgItem(IDC_STATIC_PATHIMAGE);
	CRect rectCtl;
	cImageWnd->GetWindowRect(&rectCtl);
	cvResizeWindow("MainDispWin",rectCtl.right-rectCtl.left,rectCtl.bottom-rectCtl.top);

	// 把图像框嵌入主页面
	HWND hWnd_video = (HWND)cvGetWindowHandle("MainDispWin");
	HWND hParent = ::GetParent(hWnd_video); 
	::SetParent(hWnd_video,GetDlgItem(IDC_STATIC_PATHIMAGE)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);  

	//m_ImageList.Create(32,32,ILC_COLOR24|ILC_MASK,0,0);
	m_bmp1.LoadBitmap(IDB_BITMAP1);
	
	m_bmp2.LoadBitmap(IDB_BITMAP2);
	//m_bmp1.

	// 设置回调函数
	cv::setMouseCallback("MainDispWin",(CPhraseFrame::CV_On_Mouse),0);

	//SetTimer( ID_GETSTAT_TIMER, 100, NULL );
	//2410_variety_speed_range( AxisFlg::V, 1, 50000);


	//ClearDMPosition();
}

void CPhraseFrame::OnBnClickedButtonImload()
{
	char szFilters[]= _T("图像文件 (*.bmp;*.jpg;*.png;*.tiff)|*.bmp;*.jpg;*.png;*.tiff|All Files (*.*)|*.*||");
	CFileDialog fdlg(TRUE, _T("图像文件"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fdlg.DoModal()==IDOK)
	{
		m_strImagePath = fdlg.GetPathName();//全路径，带文件名
        CString strPT = fdlg.GetFileName();//文件名
        CString strPP = fdlg.GetFileExt();//文件扩展名
        CString strPPath = m_strImagePath.Left(m_strImagePath.GetLength() - strPT.GetLength() -1);
        m_str3DImagePath = strPPath + "\\" + strPT.Left(strPT.GetLength() - strPP.GetLength() -1) + "-3D." + strPP ; 
        CString strConfigFilePath = strPPath+"\\config.ini";
        m_CurrentDataConfigPath = strConfigFilePath;
		if (b3DFlag)
		{
			int ScaleLayer = 16;
			float m_MaxHight = 20;
            Load3DImage(ScaleLayer, m_MaxHight);
		}
        


		//AfxBeginThread(PhraseImage,(LPVOID)&strFilePath);
		HANDLE hnd = CreateThread(NULL,0,PhraseImage,&m_strImagePath,0,NULL);
		//WaitForSingleObject(hnd, INFINITE);
		DWORD dwRet = 0;  
		MSG msg;  
		while (TRUE)  
		{  
			//wait for m_hThread to be over，and wait for  
			//QS_ALLINPUT（Any message is in the queue）  
			dwRet = MsgWaitForMultipleObjects (1, &hnd,   FALSE, INFINITE, QS_ALLINPUT);  
			switch(dwRet)  
			{  
			case WAIT_OBJECT_0:   
				break; //break the loop  
			case WAIT_OBJECT_0 + 1:  
				//get the message from Queue  
				//and dispatch it to specific window  
				PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);  
				//TranslateMessage(&msg);
				DispatchMessage(&msg);   
				continue;  
			default:  
				break; // unexpected failure  
			}  
			break;  
		} 
        CString strTmp;
        strTmp.Format("%d",m_ImageSizeX);
        WritePrivateProfileString("CONFIG","MapSizeX",strTmp,strConfigFilePath);
        strTmp.Format("%d",m_ImageSizeY);
        WritePrivateProfileString("CONFIG","MapSizeY",strTmp,strConfigFilePath);
        WritePrivateProfileString("CONFIG","3DIMAGEFILEPATH", m_str3DImagePath, strConfigFilePath);
        strTmp.Format("%d",m_CurrentThreadPos);
        WritePrivateProfileString("CONFIG","CURRENTRUNPOS", strTmp, strConfigFilePath);
	}


	//cv::imshow("MainDispWin",m_ShowPathMat);
}



// 跑路径
void CPhraseFrame::OnBnClickedRuncalc()
{
	//SetTimer( ID_MAPING_TIMER, 1000, NULL );
	RunCalcPath(this->m_hWnd,IDC_EDIT_CURRENTRUN);
}


BOOL CPhraseFrame::DestroyWindow()
{
	//d2410_board_close();
	//dmc_board_close();
	KillTimer(ID_MAPING_TIMER);
	KillTimer(ID_GETSTAT_TIMER);
	return CFormView::DestroyWindow();
}



void CPhraseFrame::LoadConfig()
{
	CString strTmp;
	m_ResolutionX=GetPrivateProfileInt("CONFIG","RESOLUTIONX",1,m_strSysPath);
	m_ResolutionY=GetPrivateProfileInt("CONFIG","RESOLUTIONY",1,m_strSysPath);
	m_nDensity = GetPrivateProfileInt("CONFIG","DENSITY",1,m_strSysPath);
	m_nZoffset = GetPrivateProfileInt("CONFIG","ZOFFSET",10,m_strSysPath);
	m_nSpeed = GetPrivateProfileInt("CONFIG","SPEED",100,m_strSysPath);
	m_nNumberOfColor = GetPrivateProfileInt("CONFIG","NOCOLOR",16,m_strSysPath);
	m_MotorPulseCount = GetPrivateProfileInt("CONFIG","MOTORPULSECOUNT",4194304,m_strSysPath);
	InitMotorGearRatio();
	InitMotorDirection();
	//for (int i=0; i<AXIS_COUNT; i++)
	//{
	//	strTmp.Format("Motor%d",i);
	//	m_MotorGearRatio[i] = GetPrivateProfileInt("GEARRATIO",strTmp,1,m_strSysPath);
	//	strTmp.Format("Machine%d",i);
	//	m_MachineGearRatio[i] = GetPrivateProfileInt("GEARRATIO",strTmp,1,m_strSysPath);
	//}
}

void CPhraseFrame::OnBnClickedButtonStop()
{
    Stop_decel_NO();
    m_StopRunning=TRUE;
}

CString CPhraseFrame::BinaryRecursion(int nData) 
{ 
    int a; 
    a= nData%2; 
    nData = nData >> 1; 
    CString str;
    CString strRet;
    if (nData!=0)
    {
        str = BinaryRecursion(nData);
    }
    strRet.Format("%s%d",str,a);
    return strRet;
}  

int CPhraseFrame::BinaryRecursionPos(int nData,int nPos) 
{ 
    int a; 
    nData = nData >> nPos;  
    a = nData % 2;
    return a;
}  

void CPhraseFrame::OnTimer(UINT_PTR nIDEvent)
{
    /*
    BOOL m_MOVEXYFLAG;
    BOOL m_SEWFLAG;
    int m_CurrentPos;
    */

	switch(nIDEvent) {
	case ID_MAPING_TIMER:
        GettingPLCState();


        //m_CurrentPos=(m_nXpos-100)/8;
		break;
	}
	CFormView::OnTimer(nIDEvent);
}


void CPhraseFrame::OnBnClickedButtonLoaddata()
{
	m_ListAllPoint.clear();
	CString strPath;
	char szFilters[]= _T("数据文件 (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog fdlg(TRUE, _T("数据文件"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fdlg.DoModal()==IDOK)
	{
		strPath = fdlg.GetPathName();
        CString strPT = fdlg.GetFileName();//文件名
        CString strPPath = strPath.Left(strPath.GetLength() - strPT.GetLength() -1);
        CString strConfigFilePath = strPPath+"\\config.ini";
        m_CurrentDataConfigPath = strConfigFilePath;
        m_ImageSizeX = GetPrivateProfileInt("CONFIG", "MapSizeX",0,strConfigFilePath);
        m_ImageSizeY = GetPrivateProfileInt("CONFIG", "MapSizeY",0,strConfigFilePath);
        m_CurrentThreadPos = GetPrivateProfileInt("CONFIG", "CURRENTRUNPOS",0,strConfigFilePath);
        CString  strTmp;
        strTmp.Format("%d",m_CurrentThreadPos);
        SetDlgItemText(IDC_EDIT_CURRENTRUN,strTmp);
        if (b3DFlag)
        {
            GetPrivateProfileString("CONFIG", "3DIMAGEFILEPATH","", m_str3DImagePath.GetBuffer(MAX_PATH), MAX_PATH,strConfigFilePath);
            //m_3DImageData = cv::imread(m_str3DImagePath.GetBuffer(m_str3DImagePath.GetLength()));

            int ScaleLayer = 16;
            float m_MaxHight = 20;
            Load3DImage(ScaleLayer, m_MaxHight);
        }
        
		ReadM2File(strPath);
		DrawM2Path();
		cv::imshow("MainDispWin",m_ShowPathMat);
		//WriteM2File("e:\\work\\test.txt");
	}
	
}

void CPhraseFrame::CV_On_Mouse(int nevent,int x,int y,int flags,void *ustc) 
{
	// CV_EVENT_LBUTTONUP  CV_EVENT_FLAG_LBUTTON CV_EVENT_MOUSEMOVE  CV_EVENT_LBUTTONUP
	if (nevent == CV_EVENT_LBUTTONDOWN)
	{

	}
}




void CPhraseFrame::OnBnClickedButtonReloadpos()
{
	//SetTimer( ID_MAPING_TIMER, 1000, NULL );
	((CButton*)(GetDlgItem(IDC_BUTTON_RELOADPOS)))->EnableWindow(FALSE);
	OnReturnZero();
	((CButton*)(GetDlgItem(IDC_BUTTON_RELOADPOS)))->EnableWindow(TRUE);
}


void CPhraseFrame::Load3DImage( int ScaleLayer, float m_MaxHight )
{
    cv::Mat origin = cv::imread(m_str3DImagePath.GetBuffer(m_str3DImagePath.GetLength()));
    cv::Mat gray;
    double nmin;
    double nmax;
    int nChannel=origin.channels();
    if (nChannel!=1)
    {
        cv::cvtColor(origin,gray,CV_BGR2GRAY);
    }
    else
    {
        origin.copyTo(gray);
    }
    origin.release();

    cv::minMaxLoc(gray,&nmin,&nmax);
    {
        gray = (gray - nmin) * ScaleLayer / (nmax-nmin);
        //gray=gray-nmin;
    }

    gray.convertTo(m_3DImageData,CV_32F);
    gray.release();

    m_3DImageData = m_3DImageData * m_MaxHight / (float)ScaleLayer;
}


void CPhraseFrame::OnBnClickedButton8()
{
    m_CurrentThreadPos=0;
    CString  strTmp;
    strTmp.Format("%d",m_CurrentThreadPos);
    SetDlgItemText(IDC_EDIT_CURRENTRUN,strTmp);
}


void CPhraseFrame::OnBnClickedButtonOpenplc()
{
    InitialCom();
    SetTimer( ID_MAPING_TIMER, 1000, NULL );

}




void CPhraseFrame::OnBnClickedButtonSrvon()
{
    OnSevOn();
}


//////////////////////////////////////////////////////////////////////////
// SRV ON 先设置4351(CD.100) 为0 再设置Y1为1
// SRV OFF 先设置4351为1 然后再Y1设置为0
void CPhraseFrame::OnBnClickedButtonSrvoff()
{
    //// 单轴控制的时候使用
    //// 关键是SRV OFF后, 不能通过Y1设置全轴SRV ON. 必须先设置为0 再设置Y1为1
    //SetDevice("U0\\G4351",1);// 单轴SRV OFF 4351+100n 不可以在运行状态. X0信号不会为OFF  
    //SetDevice("U0\\G4451",1);// 单轴SRV OFF 4351+100n 不可以在运行状态. X0信号不会为OFF  
    //SetDevice("U0\\G4551",1);// 单轴SRV OFF 4351+100n 不可以在运行状态. X0信号不会为OFF  
    //SetDevice("U0\\G4651",1);// 单轴SRV OFF 4351+100n 不可以在运行状态. X0信号不会为OFF  
    //SetDevice("U0\\G4751",1);// 单轴SRV OFF 4351+100n 不可以在运行状态. X0信号不会为OFF  

    SetDevice("Y1",0); // ms16的返回数据在MD.31中. MD.31  2417+100n
    SetDevice("Y30",0);// 锁Y轴

}



void CPhraseFrame::OnBnClickedButtonMovex()
{
    // X806 轴出错编号 
    // X807 轴报警编号
    // X1502 轴出错复位
    CString strROLL;
    ((CEdit*)GetDlgItem(IDC_EDIT_MOVECOUNT))->GetWindowText(strROLL);
    int nRoll;
    if (strROLL=="")
    {
        nRoll = 1;
    }
    else
        nRoll = StrToInt(strROLL);



    SetDevice("U0\\G2406",0);// 出错  2406+100n
    SetDevice("U0\\G2407",0);// 报警编号复位 2407+100n
    SetDevice("U0\\G4302",0);// 轴出错复位 4302+100n

    SetDevice("U0\\G6000",STOPMOVE);// 一轴 增量 连续 NO.1
    //SetDevice("U0\\G6003",0x203);// 插补对象轴编号
    SetData2("U0\\G",6004,4194304);// 指令速度
    SetData2("U0\\G",6006,4194304*nRoll);// 定位地址

    // G1500 定位轴启动编号  从第几个NO.1-NO.600  9001 机械原点复位  9002 高速复位 7000-7004块指定 9003 当前值更改  9004 多轴同时启动
    SetDevice("U0\\G4300",1);// 4300+100n 定位轴启动编号 
    // 4300 启动编号   4301 块启动编号  4302 出错复位  4303 重启  其中后3个会自动重置为0
    Sleep(200);

    SetDevice("Y10",1); // 轴1 MOV 
    Sleep(10);
    SetDevice("Y10",0); // 轴1 MOV 
}

void CPhraseFrame::OnBnClickedButtonMovey()
{
    // X806 轴出错编号 
    // X807 轴报警编号
    // X1502 轴出错复位
    CString strROLL;
    ((CEdit*)GetDlgItem(IDC_EDIT_MOVECOUNT))->GetWindowText(strROLL);
    int nRoll;
    if (strROLL=="")
    {
        nRoll = 1;
    }
    else
        nRoll = StrToInt(strROLL);


    SetDevice("U0\\G2506",0);// 出错  2406+100n
    SetDevice("U0\\G2507",0);// 报警编号复位 2407+100n
    SetDevice("U0\\G4402",0);// 轴出错复位 4302+100n

    SetDevice("U0\\G7000",STOPMOVE);// 一轴 增量 连续 NO.1
    //SetDevice("U0\\G6003",0x203);// 插补对象轴编号
    SetData2("U0\\G",7004,4194304);// 指令速度
    SetData2("U0\\G",7006,4194304*nRoll);// 定位地址

    // G1500 定位轴启动编号  从第几个NO.1-NO.600  9001 机械原点复位  9002 高速复位 7000-7004块指定 9003 当前值更改  9004 多轴同时启动
    SetDevice("U0\\G4400",1);// 4300+100n 定位轴启动编号 
    // 4300 启动编号   4301 块启动编号  4302 出错复位  4303 重启  其中后3个会自动重置为0
    Sleep(200);

    SetDevice("Y11",1); // 轴1 MOV 
    Sleep(10);
    SetDevice("Y11",0); // 轴1 MOV 
}


void CPhraseFrame::OnBnClickedButtonTestdata()
{
	//SetDevice("U0\\G2606",0);// 出错  2406+100n
	//SetDevice("U0\\G2607",0);// 报警编号复位 2407+100n
	//SetDevice("U0\\G4502",0);// 轴出错复位 4302+100n

    SetData2("U0\\G",8006,4194304);//X轴
    SetDevice("U0\\G8000",STOPMOVE);// 运行模式
    SetData2("U0\\G",8004,4194304/2);// 指令速度

	SetDevice("U0\\G8001",1); //R轴 MCODE

	SetDevice("U0\\G4500",1);// 4300+100n 定位轴启动编号 

	SetData2("U0\\G",9006,4194304);//X轴
	SetDevice("U0\\G9000",STOPMOVE);// 运行模式
	SetData2("U0\\G",9004,4194304/2);// 指令速度

    Sleep(200);

    SetDevice("Y12",1); // 轴1 MOV 
    Sleep(10);
    SetDevice("Y12",0); // 轴1 MOV 

}

void CPhraseFrame::GettingPLCState()
{
    long nGetting=0;
    CString strRetHex;
    CString str;
    GetDevice("U0\\G2417",nGetting); // X轴状态  md.31
    strRetHex = BinaryRecursion(nGetting); // 
    // B0 速度控制中标志
    // b1 速度.位置切换锁存标志
    // b2 指令进入位置标志
    // b3 原点复位请求标志
    // b4 原点复位完毕标志 
    // b5 位置速度切换锁存标志   b9 轴报警检测  b10 速度更改0标志  
    // MS16 专有b12 M-code ON  b13 出错检测 b14 启动完毕 b15 定位完毕
    int nstrLen = strRetHex.GetLength();
    for(int i=0; i<nstrLen; i++)
    {
        str = strRetHex.GetAt(nstrLen-i-1);//从前往后取
        GV_ITEM Item;
        Item.mask = GVIF_TEXT|GVIF_BKCLR;
        Item.row = 1;
        Item.col = 15-i;
        Item.strText = str;
        if (str == '1')
        {
            Item.crBkClr=RGB(255,0,0);
        }
        else
        {
            Item.crBkClr=RGB(255,255,255);
        }
		if (i==2 && str=='1')
		{
			m_MOVEXYFLAG=FALSE;
		}
        m_Grid.SetItem(&Item);
    }

	GetDevice("U0\\G2517",nGetting); // 第Y轴状态  md.31
	strRetHex = BinaryRecursion(nGetting); // 
	nstrLen = strRetHex.GetLength();
	for(int i=0; i<nstrLen; i++)
	{
		str = strRetHex.GetAt(nstrLen-i-1);
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_BKCLR;
		Item.row = 2;
		Item.col = 15-i;
		Item.strText = str;
		if (str == '1')
		{
			Item.crBkClr=RGB(255,0,0);
		}
		else
		{
			Item.crBkClr=RGB(255,255,255);
		}
		m_Grid.SetItem(&Item);
	}

	GetDevice("U0\\G2617",nGetting); // 第T轴状态  md.31
	strRetHex = BinaryRecursion(nGetting); // 
	nstrLen = strRetHex.GetLength();
	for(int i=0; i<nstrLen; i++)
	{
		str = strRetHex.GetAt(nstrLen-i-1);
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_BKCLR;
		Item.row = 3;
		Item.col = 15-i;
		Item.strText = str;
		if (str == '1')
		{
			Item.crBkClr=RGB(255,0,0);
		}
		else
		{
			Item.crBkClr=RGB(255,255,255);
		}
		m_Grid.SetItem(&Item);

	}

	GetDevice("U0\\G2717",nGetting); // 第R轴状态  md.31
	strRetHex = BinaryRecursion(nGetting); // 
	nstrLen = strRetHex.GetLength();
	for(int i=0; i<nstrLen; i++)
	{
		str = strRetHex.GetAt(nstrLen-i-1);
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_BKCLR;
		Item.row = 4;
		Item.col = 15-i;
		Item.strText = str;
		if (str == '1')
		{
			Item.crBkClr=RGB(255,0,0);
		}
		else
		{
			Item.crBkClr=RGB(255,255,255);
		}
		m_Grid.SetItem(&Item);
	}
    m_Grid.Refresh();

    // TODO 这里需要获取MD.26  2409+100n 包含运动状态 待机中, 停止中, 插补中, JOG运行中, ....
    //m_CurrentPos MD.44 执行中的定位数据 2435+100n
    GetDevice("U0\\G2435",nGetting); // 第一轴状态  md.44
    m_CurrentPos=nGetting;


    
}


void CPhraseFrame::OnBnClickedButtonPause()
{
    //SetDevice("U0\\G30100",1);
    Stop_decel_NO();
}

//// STOP
//SetDevice("U0\\G30100",1);
//SetDevice("U0\\G30110",1);
//SetDevice("U0\\G30120",1);
//SetDevice("U0\\G30130",1);
//SetDevice("U0\\G30140",1);

void CPhraseFrame::OnBnClickedButtonContinue()
{
    SetDevice("U0\\G30100",0);
    SetDevice("U0\\G30110",0);
    SetDevice("U0\\G30120",0);
    SetDevice("U0\\G30130",0);
    SetDevice("U0\\G30140",0);

    Sleep(100);
    SetDevice("U0\\G4303",1);// 4303+100n 重启动  重启后会自动设为0
    SetDevice("U0\\G4403",1);// 4303+100n 重启动
    SetDevice("U0\\G4503",1);// 4303+100n 重启动
    SetDevice("U0\\G4603",1);// 4303+100n 重启动
    SetDevice("U0\\G4703",1);// 4303+100n 重启动
}




void CPhraseFrame::OnBnClickedButtonRstmcode()
{
	SetDevice("U0\\G4504",1);// 
	Sleep(50);
	SetDevice("U0\\G4504",0);// 
}
