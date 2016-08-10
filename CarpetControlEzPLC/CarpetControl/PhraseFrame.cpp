// PhraseFrame.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CarpetControl.h"
#include "PhraseFrame.h"

// Y0 ����ģ��׼��  X0����׼�����
// Y1 ��1��λ����

// Y30 Y32 Y34 ��Y�� ���ߵ� Z������ 

// ��λ
// X806 ������� 
// X807 �ᱨ�����
// X1502 �����λ

// X3F ָֹͣ��

// G1500 ��λ���������  �ӵڼ���NO.1-NO.600  9001 ��еԭ�㸴λ  9002 ���ٸ�λ 7000-7004��ָ�� 9003 ��ǰֵ����  9004 ����ͬʱ����
// G15001 ֻ���ڸ߼���λ�������

// G835 ��ȡִ���ж�λ����

// G1534 ��Ŀ��λ��  G1538 ��Ŀ��λ������  �������Ŀ��λ��֮����ҪG1538ȷ��
// G2004 ��ָ���ٶ�  G2006 �ٶ�ȷ��  


// G35200 ���������������



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


// CPhraseFrame ���

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


// CPhraseFrame ��Ϣ�������


void CPhraseFrame::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetDocument()->SetTitle("����");

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

	//if( d2410_board_init() <= 0 )//���ƿ��ĳ�ʼ������
	//	MessageBox("��ʼ��DMC2410��ʧ�ܣ�","����");

 //   if (dmc_board_init() <=0 )
 //   {
 //       MessageBox("��ʼ��DMC8410��ʧ�ܣ�","����");
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

	// ��ͼ���Ƕ����ҳ��
	HWND hWnd_video = (HWND)cvGetWindowHandle("MainDispWin");
	HWND hParent = ::GetParent(hWnd_video); 
	::SetParent(hWnd_video,GetDlgItem(IDC_STATIC_PATHIMAGE)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);  

	//m_ImageList.Create(32,32,ILC_COLOR24|ILC_MASK,0,0);
	m_bmp1.LoadBitmap(IDB_BITMAP1);
	
	m_bmp2.LoadBitmap(IDB_BITMAP2);
	//m_bmp1.

	// ���ûص�����
	cv::setMouseCallback("MainDispWin",(CPhraseFrame::CV_On_Mouse),0);

	//SetTimer( ID_GETSTAT_TIMER, 100, NULL );
	//2410_variety_speed_range( AxisFlg::V, 1, 50000);


	//ClearDMPosition();
}

void CPhraseFrame::OnBnClickedButtonImload()
{
	char szFilters[]= _T("ͼ���ļ� (*.bmp;*.jpg;*.png;*.tiff)|*.bmp;*.jpg;*.png;*.tiff|All Files (*.*)|*.*||");
	CFileDialog fdlg(TRUE, _T("ͼ���ļ�"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fdlg.DoModal()==IDOK)
	{
		m_strImagePath = fdlg.GetPathName();//ȫ·�������ļ���
        CString strPT = fdlg.GetFileName();//�ļ���
        CString strPP = fdlg.GetFileExt();//�ļ���չ��
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
			//wait for m_hThread to be over��and wait for  
			//QS_ALLINPUT��Any message is in the queue��  
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



// ��·��
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
	char szFilters[]= _T("�����ļ� (*.txt)|*.txt|All Files (*.*)|*.*||");
	CFileDialog fdlg(TRUE, _T("�����ļ�"), _T(""),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);
	if (fdlg.DoModal()==IDOK)
	{
		strPath = fdlg.GetPathName();
        CString strPT = fdlg.GetFileName();//�ļ���
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
// SRV ON ������4351(CD.100) Ϊ0 ������Y1Ϊ1
// SRV OFF ������4351Ϊ1 Ȼ����Y1����Ϊ0
void CPhraseFrame::OnBnClickedButtonSrvoff()
{
    //// ������Ƶ�ʱ��ʹ��
    //// �ؼ���SRV OFF��, ����ͨ��Y1����ȫ��SRV ON. ����������Ϊ0 ������Y1Ϊ1
    //SetDevice("U0\\G4351",1);// ����SRV OFF 4351+100n ������������״̬. X0�źŲ���ΪOFF  
    //SetDevice("U0\\G4451",1);// ����SRV OFF 4351+100n ������������״̬. X0�źŲ���ΪOFF  
    //SetDevice("U0\\G4551",1);// ����SRV OFF 4351+100n ������������״̬. X0�źŲ���ΪOFF  
    //SetDevice("U0\\G4651",1);// ����SRV OFF 4351+100n ������������״̬. X0�źŲ���ΪOFF  
    //SetDevice("U0\\G4751",1);// ����SRV OFF 4351+100n ������������״̬. X0�źŲ���ΪOFF  

    SetDevice("Y1",0); // ms16�ķ���������MD.31��. MD.31  2417+100n
    SetDevice("Y30",0);// ��Y��

}



void CPhraseFrame::OnBnClickedButtonMovex()
{
    // X806 ������� 
    // X807 �ᱨ�����
    // X1502 �����λ
    CString strROLL;
    ((CEdit*)GetDlgItem(IDC_EDIT_MOVECOUNT))->GetWindowText(strROLL);
    int nRoll;
    if (strROLL=="")
    {
        nRoll = 1;
    }
    else
        nRoll = StrToInt(strROLL);



    SetDevice("U0\\G2406",0);// ����  2406+100n
    SetDevice("U0\\G2407",0);// ������Ÿ�λ 2407+100n
    SetDevice("U0\\G4302",0);// �����λ 4302+100n

    SetDevice("U0\\G6000",STOPMOVE);// һ�� ���� ���� NO.1
    //SetDevice("U0\\G6003",0x203);// �岹��������
    SetData2("U0\\G",6004,4194304);// ָ���ٶ�
    SetData2("U0\\G",6006,4194304*nRoll);// ��λ��ַ

    // G1500 ��λ���������  �ӵڼ���NO.1-NO.600  9001 ��еԭ�㸴λ  9002 ���ٸ�λ 7000-7004��ָ�� 9003 ��ǰֵ����  9004 ����ͬʱ����
    SetDevice("U0\\G4300",1);// 4300+100n ��λ��������� 
    // 4300 �������   4301 ���������  4302 ����λ  4303 ����  ���к�3�����Զ�����Ϊ0
    Sleep(200);

    SetDevice("Y10",1); // ��1 MOV 
    Sleep(10);
    SetDevice("Y10",0); // ��1 MOV 
}

void CPhraseFrame::OnBnClickedButtonMovey()
{
    // X806 ������� 
    // X807 �ᱨ�����
    // X1502 �����λ
    CString strROLL;
    ((CEdit*)GetDlgItem(IDC_EDIT_MOVECOUNT))->GetWindowText(strROLL);
    int nRoll;
    if (strROLL=="")
    {
        nRoll = 1;
    }
    else
        nRoll = StrToInt(strROLL);


    SetDevice("U0\\G2506",0);// ����  2406+100n
    SetDevice("U0\\G2507",0);// ������Ÿ�λ 2407+100n
    SetDevice("U0\\G4402",0);// �����λ 4302+100n

    SetDevice("U0\\G7000",STOPMOVE);// һ�� ���� ���� NO.1
    //SetDevice("U0\\G6003",0x203);// �岹��������
    SetData2("U0\\G",7004,4194304);// ָ���ٶ�
    SetData2("U0\\G",7006,4194304*nRoll);// ��λ��ַ

    // G1500 ��λ���������  �ӵڼ���NO.1-NO.600  9001 ��еԭ�㸴λ  9002 ���ٸ�λ 7000-7004��ָ�� 9003 ��ǰֵ����  9004 ����ͬʱ����
    SetDevice("U0\\G4400",1);// 4300+100n ��λ��������� 
    // 4300 �������   4301 ���������  4302 ����λ  4303 ����  ���к�3�����Զ�����Ϊ0
    Sleep(200);

    SetDevice("Y11",1); // ��1 MOV 
    Sleep(10);
    SetDevice("Y11",0); // ��1 MOV 
}


void CPhraseFrame::OnBnClickedButtonTestdata()
{
	//SetDevice("U0\\G2606",0);// ����  2406+100n
	//SetDevice("U0\\G2607",0);// ������Ÿ�λ 2407+100n
	//SetDevice("U0\\G4502",0);// �����λ 4302+100n

    SetData2("U0\\G",8006,4194304);//X��
    SetDevice("U0\\G8000",STOPMOVE);// ����ģʽ
    SetData2("U0\\G",8004,4194304/2);// ָ���ٶ�

	SetDevice("U0\\G8001",1); //R�� MCODE

	SetDevice("U0\\G4500",1);// 4300+100n ��λ��������� 

	SetData2("U0\\G",9006,4194304);//X��
	SetDevice("U0\\G9000",STOPMOVE);// ����ģʽ
	SetData2("U0\\G",9004,4194304/2);// ָ���ٶ�

    Sleep(200);

    SetDevice("Y12",1); // ��1 MOV 
    Sleep(10);
    SetDevice("Y12",0); // ��1 MOV 

}

void CPhraseFrame::GettingPLCState()
{
    long nGetting=0;
    CString strRetHex;
    CString str;
    GetDevice("U0\\G2417",nGetting); // X��״̬  md.31
    strRetHex = BinaryRecursion(nGetting); // 
    // B0 �ٶȿ����б�־
    // b1 �ٶ�.λ���л������־
    // b2 ָ�����λ�ñ�־
    // b3 ԭ�㸴λ�����־
    // b4 ԭ�㸴λ��ϱ�־ 
    // b5 λ���ٶ��л������־   b9 �ᱨ�����  b10 �ٶȸ���0��־  
    // MS16 ר��b12 M-code ON  b13 ������ b14 ������� b15 ��λ���
    int nstrLen = strRetHex.GetLength();
    for(int i=0; i<nstrLen; i++)
    {
        str = strRetHex.GetAt(nstrLen-i-1);//��ǰ����ȡ
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

	GetDevice("U0\\G2517",nGetting); // ��Y��״̬  md.31
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

	GetDevice("U0\\G2617",nGetting); // ��T��״̬  md.31
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

	GetDevice("U0\\G2717",nGetting); // ��R��״̬  md.31
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

    // TODO ������Ҫ��ȡMD.26  2409+100n �����˶�״̬ ������, ֹͣ��, �岹��, JOG������, ....
    //m_CurrentPos MD.44 ִ���еĶ�λ���� 2435+100n
    GetDevice("U0\\G2435",nGetting); // ��һ��״̬  md.44
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
    SetDevice("U0\\G4303",1);// 4303+100n ������  ��������Զ���Ϊ0
    SetDevice("U0\\G4403",1);// 4303+100n ������
    SetDevice("U0\\G4503",1);// 4303+100n ������
    SetDevice("U0\\G4603",1);// 4303+100n ������
    SetDevice("U0\\G4703",1);// 4303+100n ������
}




void CPhraseFrame::OnBnClickedButtonRstmcode()
{
	SetDevice("U0\\G4504",1);// 
	Sleep(50);
	SetDevice("U0\\G4504",0);// 
}
