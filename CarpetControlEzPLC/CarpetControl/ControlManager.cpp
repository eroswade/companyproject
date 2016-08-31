#include "StdAfx.h"
#include "ControlManager.h"
#include <list>


using namespace std;
BOOL b3DFlag=TRUE;

BOOL m_StopRunning;
int m_CurrentThreadPos=0; //记录当前运行状态，给恢复运行使用
float m_MotorGearRatio[AXIS_COUNT];//齿轮比. 电机  分母
int m_MotorDirection[AXIS_COUNT];//方向
//float m_MachineGearRatio[AXIS_COUNT];//齿轮比. 机器部分 分子 
list<ct_motion> m_MotionPos;
//ct_motion m_MotionPos[BUFFER_COUNT];
int m_MotorPulseCount;// 伺服电机脉冲数
int m_nXpos, m_nYpos;// X,Y的当前位置

float m_fNeedleAngle;// 针角度
//int m_nLenX,m_nLenY;// 画布大小
int m_nZoffset;// 最小的Z.其它的Z在Zoffset之上增加一个值
int m_nSpeed;//自动运行速度
double m_SumAngle=0;
static long m_SumPosX=0;
static long m_SumPosY=0;
CString m_str3DImagePath;
cv::Mat m_3DImageData;


BOOL m_MOVEXYFLAG;
BOOL m_SEWFLAG;
int m_CurrentPos;

vector<MotionOutput> m_RunCMD;
//vector<MotionRoll> m_RollPos;


IActUtlType*	mp_IUtlType;	
IActProgType*	mp_IProgType;

void SetData2(CString strDVName,int nPos,int nLen)
{
    int lwd = LOWORD(nLen);
    int hwd = HIWORD(nLen);
    CString strBuff;
    strBuff.Format("%s%d", strDVName, nPos);
    SetDevice(strBuff,lwd);
    strBuff.Format("%s%d", strDVName, nPos+1);
    SetDevice(strBuff,hwd);
}

void SetData(CString DeviceName,int npos, int DeviceValue)
{
    CString strBuff;
    strBuff.Format("%s%d", DeviceName, npos);
    SetDevice(strBuff,DeviceValue);
}


BOOL SetDevice(CString DeviceName, int DeviceValue)
{
    long lValue;
    long lRet;
    CString	MsgStr;
    HRESULT	hr;
    BSTR szDev = NULL;


    lValue = DeviceValue;
    szDev = DeviceName.AllocSysString();	// Allocate the BSTR-Type String area. 
    hr = mp_IUtlType->SetDevice(DeviceName.AllocSysString(),lValue,&lRet);	// Exec GetDevice Method
    if(SUCCEEDED(hr)){	// Compornent Communication is succeeded?
        // Renew ReturnValue
        lRet = TRUE;
    }
    else{

        AfxMessageBox("Compornent Communication Failed.", MB_ICONINFORMATION);
        lRet = FALSE;
    }
    // Free the allocated area.
    ::SysFreeString(szDev);
    return lRet;
}

BOOL GetDevice(CString DeviceName, long& lValue)
{
    long lRet;
    CString	MsgStr;
    BSTR szDev = NULL;
    HRESULT	hr;
    BOOL ret;
    ret = TRUE;

    szDev = DeviceName.AllocSysString();	// Allocate the BSTR-Type String area. 
    // (After use, you have to free it.)
    hr = mp_IUtlType->GetDevice(DeviceName.AllocSysString(),&lValue,&lRet);	// Exec GetDevice Method
    if(!SUCCEEDED(hr))// Compornent Communication is succeeded?
    {
        AfxMessageBox("Compornent Communication Failed.", MB_ICONINFORMATION);
        lValue = 0;
        ret = FALSE;
    }
    // Free the allocated area.
    ::SysFreeString(szDev);
    return ret;
}

BOOL InitialCom()
{
    long	lRet;
    HRESULT	hr;
    BSTR szAdr = NULL;

    //CWnd::UpdateData(TRUE);
	// 这个是由三菱的communcation setting utility设置的
    hr = mp_IUtlType->put_ActLogicalStationNumber(1);	// Exec set-property method
    if(SUCCEEDED(hr)){	// Compornent Communication is succeeded?
        hr = mp_IUtlType->Open(&lRet);	// Exec Open Method
    }
    if(SUCCEEDED(hr))// Compornent Communication is succeeded?
    {	
        return TRUE;
    }
    else// Failed Compornent Communication
    {	
        AfxMessageBox("Compornent Communication Failed.", MB_ICONINFORMATION);
        return FALSE;
    }
}



void InitMotorGearRatio()
{
    // 单位为mm
    m_MotorGearRatio[AxisFlg::X1] = 10.0/3.0;//MOTOR一圈,走的毫米数
    m_MotorGearRatio[AxisFlg::Y] = 198.0/50.0;//MOTOR一圈,走的毫米数
    m_MotorGearRatio[AxisFlg::V] = 13.0*PI*2.0;//一圈送线的长度   81.68mm
    m_MotorGearRatio[AxisFlg::D] = 22.0/100.0*360.0;//一圈走的度数 
    m_MotorGearRatio[AxisFlg::T] = 2.0/3.0;//一圈走的针数 
}
void InitMotorDirection()
{
    m_MotorDirection[AxisFlg::X1] = 1;
    m_MotorDirection[AxisFlg::Y] = 1;
    m_MotorDirection[AxisFlg::V] = 1;//
    m_MotorDirection[AxisFlg::D] = -1;// 以顺时针为正
    m_MotorDirection[AxisFlg::T] = 1;//
}

DWORD MotionPhrase(s_MotionProcessPra* par, BOOL bRoll=FALSE, int nRollAngle=0);
DWORD WINAPI MoveXY(LPVOID par);
DWORD WINAPI MotionProcess(LPVOID par);// cv::Point from, cv::Point to, BOOL bwithNeedle,BOOL b3DFlag=FALSE
DWORD WINAPI RoteProcess(LPVOID fDegree);
DWORD WINAPI UpGunProcess(LPVOID lpParamter);
DWORD WINAPI DownGunProcess(LPVOID lpParamter);
int RotePhrase(float fDegree);
DWORD WINAPI CutProcess(LPVOID lpParamter);
void WaitingForThread(HANDLE hnd)
{
    DWORD dwRet = 0;  
    MSG msg;  
    while (TRUE)  
    {  
        //wait for m_hThread to be over，and wait for  
        //QS_ALLINPUT（Any message is in the queue）  
        dwRet = MsgWaitForMultipleObjects (1, &hnd,   FALSE, INFINITE, QS_ALLINPUT);  
        if (dwRet == WAIT_OBJECT_0+1)
        {
            while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            break;
        } 
    } 
}
void RunCalcPath(HWND hwnd, int Obj)
{
    //OnSevOn();
    cv::Point opOt;
    m_StopRunning = FALSE;
    m_RunCMD.clear();

    //m_CurrentThreadPos = 0; // 恢复用 默认初始化时为0， TODO 这个数据最好保存
    for (int iter=m_CurrentThreadPos; iter<m_ListAllPoint.size(); iter++)  
    {  
        cv::Point pOt = m_ListAllPoint[iter];
        m_CurrentThreadPos=iter;
        CString strTmp;
        strTmp.Format("%d",m_CurrentThreadPos);
        MotionOutput PlistObj;
        ::SetDlgItemText(hwnd,Obj,strTmp);
        if (pOt.x == MARK_M2.x && pOt.y == MARK_M2.y)//切线 M2  程序流程,在CUT之前,如果有数据,就执行数据. 否则开始分析数据
        {
            //CutProcess(NULL);
            while (m_RunCMD.size()>0)
            {
                HANDLE tThread =  CreateThread(NULL,0,MotionProcess,NULL,0,NULL);
                WaitingForThread(tThread);
            }
            HANDLE tThread =  CreateThread(NULL,0,CutProcess,NULL,0,NULL);
            WaitingForThread(tThread);

            tThread =  CreateThread(NULL,0,UpGunProcess,NULL,0,NULL);
            WaitingForThread(tThread);

            if (iter == 0)
            {
                opOt = cv::Point(0,0);
            }
            else
            {
                opOt = m_ListAllPoint[iter-1];
            }
            pOt = m_ListAllPoint[iter+1];
            TRACE0("START WITHOUT NEEDLE\n");
            s_MotionProcessPra s;
            s.from = opOt;
            s.to = pOt;
            s.bwithNeedle = FALSE;
            s.b3DFlag = FALSE;
            //MotionProcess(&s);
            tThread =  CreateThread(NULL,0,MoveXY,&s,0,NULL);
            WaitingForThread(tThread);
            iter++;
        }
        else if (pOt.x == MARK_M1.x && pOt.y == MARK_M1.y)//下针 M1  只是跳过M1这个标志
        {
            HANDLE tThread =  CreateThread(NULL,0,DownGunProcess,0,0,NULL);
            WaitingForThread(tThread);

            cv::Point2f pOt1 = m_ListAllPoint[iter+1];//下一个点
            cv::Point2f pOt2 = m_ListAllPoint[iter+2];// 下两个点
            cv::Point2f pOt0 = m_ListAllPoint[iter-1];// 上一个点
            float diffdegree;
            diffdegree = GetDiffdegree(pOt2, pOt1);

            int degreepls = RotePhrase( diffdegree);

            //PlistObj.bRoll = TRUE;
            //PlistObj.rollangle = degreepls;
            //m_RunCMD.push_back(PlistObj);

            s_MotionProcessPra s;
            s.from = pOt1;
            s.to = pOt2;
            s.bwithNeedle = TRUE;
            s.b3DFlag = TRUE;
            MotionPhrase(&s, TRUE, degreepls);
            iter+=2;
        }
        else if (pOt.x == MARK_DWE.x && pOt.y == MARK_DWE.y)// 旋转 DWE + XY  
        {
            cv::Point2f pOt1 = m_ListAllPoint[iter-1];
            cv::Point2f pOt2 = m_ListAllPoint[iter+1];
            float diffdegree;
            diffdegree = GetDiffdegree(pOt2, pOt1);
            int degreepls = RotePhrase( diffdegree);

            //PlistObj.bRoll = TRUE;
            //PlistObj.rollangle = degreepls;
            //m_RunCMD.push_back(PlistObj);

            s_MotionProcessPra s;
            s.from = pOt1;
            s.to = pOt2;
            s.bwithNeedle = TRUE;
            s.b3DFlag = TRUE;
            MotionPhrase(&s, TRUE, degreepls);

            iter++;
        }
        else
        {
            opOt = m_ListAllPoint[iter-1];

            s_MotionProcessPra s;
            s.from = opOt;
            s.to = pOt;
            s.bwithNeedle = TRUE;
            s.b3DFlag = TRUE;
            MotionPhrase(&s);
        }

        if (m_StopRunning)
        {
            break;
        }
    }
}




// SRV ON PLC
void OnSevOn()
{
    //SetDevice("U0\\G4351",0); // 单轴控制的时候用
    //SetDevice("U0\\G4451",0);
    //SetDevice("U0\\G4551",0);
    //SetDevice("U0\\G4651",0);
    //SetDevice("U0\\G4751",0);
    //Sleep(20);
    SetDevice("Y1",1);
    SetDevice("Y30",1);// 解锁Y轴
    long ret;
}

// 减速停止 PLC
void Stop_decel_NO()
{
    SetDevice("U0\\G30100",1);
    SetDevice("U0\\G30110",1);
    SetDevice("U0\\G30120",1);
    SetDevice("U0\\G30130",1);
    SetDevice("U0\\G30140",1);
}

// 清除停止信号
void Clear_Stop_Sign()
{
    SetDevice("U0\\G30100",0);
    SetDevice("U0\\G30110",0);
    SetDevice("U0\\G30120",0);
    SetDevice("U0\\G30130",0);
    SetDevice("U0\\G30140",0);
}



double GetDiffdegree( cv::Point2f pOt2, cv::Point2f pOt1)
{
    double diffdegree;
    double degree = atan2(pOt2.y-pOt1.y,pOt2.x-pOt1.x)*180/PI;
    double tmp = m_fNeedleAngle;
    if (degree < 0)
    {
        m_fNeedleAngle=degree+360;
    }
    else
    {
        m_fNeedleAngle = degree;
    }
    diffdegree = tmp - m_fNeedleAngle;	
    return diffdegree;
}


DWORD MotionPhrase(s_MotionProcessPra* par, BOOL bRoll, int nRollAngle)
{
    s_MotionProcessPra *s = par;
    cv::Point from = s->from;
    cv::Point to = s->to;
    BOOL bwithNeedle=s->bwithNeedle; // 是否下针
    //BOOL b3DFlag=s->b3DFlag;
    int m_nOffsetLen = 20;// 默认送线长度
    ct_motion tmpMotion;
    tmpMotion.dist1=m_MotorDirection[AxisFlg::X1]*(to.x-from.x)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
    tmpMotion.dist2=m_MotorDirection[AxisFlg::Y]*(to.y-from.y)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];
    m_SumPosX += tmpMotion.dist1;
    m_SumPosY += tmpMotion.dist2;

    int mNeedle;//总下针数
    double stringlen;// X,Y走的物理总长
    double needlelen;// 针走的脉冲总长

    MotionOutput PhraseObj;

    if (bwithNeedle)
    {
        stringlen = sqrt( double((to.x-from.x)*(to.x-from.x)) +  double((to.y-from.y)*(to.y-from.y)));

        // 针数 ＝ 长度*密度 *脉冲数 / 针速  每针走平均
        mNeedle = (stringlen / (float)m_nDensity)+0.5;//必须转换为整型  四舍五入
        //long TspeedLen =  mNeedle / m_MotorGearRatio[AxisFlg::T] * m_MotorPulseCount;//针走的总长

        // 送线长
        needlelen = stringlen + mNeedle*2* m_nOffsetLen ;// 物理长度

        if (b3DFlag)//3D
        {
            for (int nneddl=0; nneddl < mNeedle; nneddl++)//解析成每一个点做插值.
            {
                cv::Point2f nedDownPos;
                nedDownPos.x = float((to.x-from.x)) / float(mNeedle) * (nneddl+1);
                nedDownPos.y = float((to.y-from.y)) / float(mNeedle) * (nneddl+1);

                int nx = from.y+(nedDownPos.y+0.5);//四舍五入
                int ny = from.x+(nedDownPos.x+0.5);
                float dt= m_3DImageData.at<float>(nx,ny);// 获取针所在位置的3D长度
                MotionOutput PlistObj;
                if (bRoll)
                {
                    PlistObj.bRoll = TRUE;
                    PlistObj.rollangle = nRollAngle;
                    bRoll = FALSE;
                }
                PlistObj.movinc.x = m_MotorDirection[AxisFlg::X1]*(to.x-from.x)/mNeedle*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
                PlistObj.movinc.y = m_MotorDirection[AxisFlg::Y]*(to.y-from.y)/mNeedle *m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];
                PlistObj.sewincTV.x = 1.0/m_MotorGearRatio[AxisFlg::T] * m_MotorPulseCount;
                PlistObj.sewincTV.y = (needlelen/mNeedle+dt) / m_MotorGearRatio[AxisFlg::V] *m_MotorPulseCount;
                m_RunCMD.push_back(PlistObj);
            }
        }
        else
        {
            for (int i=0; i<mNeedle; i++)
            {
                MotionOutput PlistObj;
                if (bRoll)
                {
                    PlistObj.bRoll = TRUE;
                    PlistObj.rollangle = nRollAngle;
                    bRoll = FALSE;
                }
                PlistObj.movinc.x = m_MotorDirection[AxisFlg::X1]*(to.x-from.x)/mNeedle*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
                PlistObj.movinc.y = m_MotorDirection[AxisFlg::Y]*(to.y-from.y)/mNeedle *m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];
                PlistObj.sewincTV.x = 1.0/m_MotorGearRatio[AxisFlg::T] * m_MotorPulseCount;
                PlistObj.sewincTV.y = needlelen/mNeedle / m_MotorGearRatio[AxisFlg::V] *m_MotorPulseCount;
                m_RunCMD.push_back(PlistObj);
            }
        }
    }
    else
    {
		//// 因为这里暂时只有M1用到， 所以暂时不需要 
        //// TODO 这里要进一个点. 4*4. 但问题是如果针不动, 这个指令会出错. 
        //// 但这里又涉及到旋转轴
        //MotionOutput PlistObj;
        //if (bRoll)
        //{
        //    PlistObj.bRoll = TRUE;
        //    PlistObj.rollangle = nRollAngle;
        //    bRoll = FALSE;
        //}
        //PlistObj.movinc.x = m_MotorDirection[AxisFlg::X1]*(to.x-from.x)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
        //PlistObj.movinc.y = m_MotorDirection[AxisFlg::Y]*(to.y-from.y) *m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];
        //PlistObj.sewincTV.x = 1.0/m_MotorGearRatio[AxisFlg::T] * m_MotorPulseCount;
        //PlistObj.sewincTV.y = 0;

        //m_RunCMD.push_back(PlistObj);
    }
    return 0;

}

DWORD WINAPI MoveXY(LPVOID par)
{
    s_MotionProcessPra *s = (s_MotionProcessPra*)par;
    cv::Point from = s->from;
    cv::Point to = s->to;
    BOOL bwithNeedle=s->bwithNeedle; // 是否下针
    //BOOL b3DFlag=s->b3DFlag;
    int m_nOffsetLen = 20;// 默认送线长度
    ct_motion tmpMotion;
    tmpMotion.dist1=m_MotorDirection[AxisFlg::X1]*(to.x-from.x)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
    tmpMotion.dist2=m_MotorDirection[AxisFlg::Y]*(to.y-from.y)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];
    m_SumPosX += tmpMotion.dist1;
    m_SumPosY += tmpMotion.dist2;

    long distlist[2];
    distlist[0]=tmpMotion.dist1;
    distlist[1]=tmpMotion.dist2;

    m_MOVEXYFLAG = TRUE;

    SetDevice("U0\\G6000",STOPMOVE2);// 一轴 增量 连续 NO.1
    SetData2("U0\\G",6006,distlist[0]);// 定位地址
    SetData2("U0\\G",7006,distlist[1]);// 定位地址
    SetData("U0\\G",6003,1);// 插补对象 后3个字节 1个字节为1轴  从0-15 二进制度数表示
    //SetData2("U0\\G",,distlist[1]);
    SetData2("U0\\G",6004,4194304*4);//指令速度

    SetDevice("U0\\G4300",1);// 4300+100n 定位轴启动编号 

    Sleep(200);

    SetDevice("Y10",1); // 轴1 MOV 
    Sleep(10);
    SetDevice("Y10",0); // 轴1 MOV 
    while (m_MOVEXYFLAG) // 由ONTIME里的控制位来停止
    {
#if DEBUG_WITHOUT_PLC==1
        if (m_StopRunning == FALSE)//测试的时候用
        {
            //m_SumPosX -= tmpMotion.dist1;
            //m_SumPosY -= tmpMotion.dist2;
            break;
        }
#endif
		if (m_StopRunning)
		{
			break;
		}
        Sleep(200);
    }
    return 0;

}

void SettingMotionData4x( int dd, int i, BOOL RUNMET );

DWORD WINAPI MotionProcess(LPVOID par)
{
    int Kout=0; 
    //for (int i=0; i<m_RunCMD.size(); i++)
    //{
    //    if (m_RunCMD[i].sewincTV.x==0)
    //    {
    //        Sleep(200);

    //    }
    //    
    //}
    //return 0;
    int roucount=0;
    int mxroucount = floor(m_RunCMD.size()/50)-1;
    if (m_RunCMD.size()%50 != 0)
    {
        mxroucount+=1;
    }
    while (true)
    {
        if (roucount+1 >= mxroucount)// 最后50个
        {
            int i=0;
            if (roucount%2 == 1)
            {
                i=50;
            }
            for (; i<m_RunCMD.size()-Kout; i++)
            {
                if(i!=m_RunCMD.size()-Kout-1)
                {
                    SettingMotionData4x(Kout, i, FALSE);
                }else{
                    SettingMotionData4x(Kout, i, TRUE);
                }
            }
			Kout= m_RunCMD.size();

            if (roucount == 0)
            {
                SetDevice("U0\\G4500",1);// 4300+100n 定位轴启动编号  
                Sleep(200);
                SetDevice("Y12",1); // 轴1 MOV 
                Sleep(10);
                SetDevice("Y12",0); // 轴1 MOV 
            }
			m_SEWFLAG=TRUE;
        }
        else
        {
            int dd = Kout;
            if (roucount%2==0)//1、0进入 5、2进入，变3
            {
                for (int i=0; i<50; i++,Kout++)//第一次放进50个点
                {
                    SettingMotionData4x(dd, i,FALSE);
                }
            }

            if (roucount%2==1)//3、roucount＝1 进入
            {
                for (int i=50; i<100; i++,Kout++)//第二次放进50个点
                {
                    SettingMotionData4x(dd, i,FALSE);
                }
            }
            

            // TODO 启动编号未必是从1开始
			if (roucount == 0)
			{
				SetDevice("U0\\G4500",1);// 4300+100n 定位轴启动编号  
                Sleep(200);
                SetDevice("Y12",1); // 轴1 MOV 
                Sleep(10);
                SetDevice("Y12",0); // 轴1 MOV 
			}
			m_SEWFLAG=TRUE;
            roucount++;

        }
        int dreegree ;
        while (m_SEWFLAG)
        {
            //if (m_RollPos[0].Rollpos <= m_CurrentPos)
            //{
            //    dreegree = m_RollPos[0].rollangle;
            //    CreateThread(NULL,0,RoteProcess,(LPVOID)dreegree,0,NULL);
            //    m_RollPos.erase(m_RollPos.begin());
            //}

            if (m_CurrentPos > 50 && roucount%2==0 && roucount<mxroucount)//4、roucount=2 并且m_CurrentPos>50 break; 
            {
                break;
            }
            else if (m_CurrentPos<10 && roucount%2==1 && roucount<mxroucount)//2、判断生效果 break。 6、roucount＝3，当后面50－100跑完，进入
            {
                break;
            }
            if (m_StopRunning)
            {
                break;
            }

            Sleep(50);
        }
		if(Kout == m_RunCMD.size())
		{
			break;
		}
        if (m_StopRunning)
        {
            break;
        }
    }

    m_RunCMD.clear();
    //m_RollPos.clear();

    return 0;

}
 
// rote 172ds
DWORD WINAPI RoteProcess(LPVOID fDegree)
{
    int nPluse = (int)fDegree;

    SetDevice("U0\\G9000",STOPMOVE);// 5轴 增量 连续 NO.1  NO.2 6010       6000+1000n n轴
    //SetDevice("U0\\G6003",0x203);// 插补对象轴编号
    SetData2("U0\\G",9004,4194304);// 指令速度
    SetData2("U0\\G",9006,nPluse);// 定位地址
    SetDevice("U0\\G4600",1);// 4300+100n 定位轴启动编号 

    Sleep(200);
    SetDevice("Y13",1); // 轴5 MOV 
    Sleep(10);
    SetDevice("Y13",0); // 轴5 MOV 

    return 0;
}

int RotePhrase(float fDegree)
{
    float diffdegree = fDegree;
    if (abs(diffdegree)>180.0)
    {
        if(diffdegree>0.0)
        {
            diffdegree = diffdegree - 360 ;
        }
        else
        {
            diffdegree = 360 + diffdegree;
        }
    }
    m_SumAngle += diffdegree;

    int nPluse = fDegree*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::D] * m_MotorDirection[AxisFlg::D];

    return nPluse;
}
// rote 172ds
DWORD WINAPI RoteBackProcess(LPVOID fDegree)
{
    int intang = m_SumAngle;
    int fold = intang / 360;
    m_SumAngle = m_SumAngle - fold * 360;
    if (abs(m_SumAngle)>180.0)
    {
        if(m_SumAngle>0.0)
        {
            m_SumAngle = m_SumAngle - 360 ;
        }
        else
        {
            m_SumAngle = 360 + m_SumAngle;
        }
    }
	else
		m_SumAngle = -m_SumAngle;

    int nPluse = m_SumAngle*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::D] * m_MotorDirection[AxisFlg::D];
    SetDevice("U0\\G9000",STOPMOVE);// 5轴 增量 连续 NO.1  NO.2 6010       6000+1000n n轴
    //SetDevice("U0\\G6003",0x203);// 插补对象轴编号
    SetData2("U0\\G",9004,4194304);// 指令速度
    SetData2("U0\\G",9006,nPluse);// 定位地址
    SetDevice("U0\\G4600",1);// 4300+100n 定位轴启动编号 

    Sleep(200);
    SetDevice("Y13",1); // 轴5 MOV 
    Sleep(10);
    SetDevice("Y13",0); // 轴5 MOV 
    m_SumAngle = 0;
    return 0;
}

// CUT 5410
DWORD WINAPI CutProcess(LPVOID lpParamter)
{
    SetDevice("Y32",1);
    Sleep(1000);
    SetDevice("Y32",0);
    Sleep(1000);
    return 0;
}


DWORD WINAPI DownGunProcess(LPVOID lpParamter)
{
    SetDevice("Y34",1);
    Sleep(1000);
    return 0;
}

DWORD WINAPI UpGunProcess(LPVOID lpParamter)
{
    SetDevice("Y34",0);
    Sleep(1000);
    return 0;
}

// 回零点 修改为PLC
void OnReturnZero()
{
    CreateThread(NULL,0,CutProcess,0,0,NULL);
    CreateThread(NULL,0,RoteBackProcess,0,0,NULL);

    long x = m_SumPosX ;
    long y = m_SumPosY;

    //dmc_set_vector_profile_multicoor(0,0,500,20000,1,1);
	WORD axislist[2]={AxisFlg::X1,AxisFlg::Y};//,-x ,-y
	long distlist[2]={-x,-y};

    SetDevice("U0\\G6000",STOPMOVE2);// 一轴 增量 连续 NO.1
    SetData2("U0\\G",6006,distlist[0]);// 定位地址
    SetData2("U0\\G",7006,distlist[1]);// 定位地址
    SetData("U0\\G",6003,1);// 插补对象 后3个字节 1个字节为1轴  从0-15 二进制度数表示
    //SetData2("U0\\G",,distlist[1]);
    SetData2("U0\\G",6004,4194304*4);//指令速度

    SetDevice("U0\\G4300",1);// 4300+100n 定位轴启动编号 

    Sleep(200);

    SetDevice("Y10",1); // 轴1 MOV 
    Sleep(10);
    SetDevice("Y10",0); // 轴1 MOV 

}

void SettingMotionData4x( int dd, int i , BOOL RUNMET)
{
    MotionOutput s=m_RunCMD[dd+i];
    if (s.bRoll)
    {
        SetData2("U0\\G",9006+10*i,s.rollangle); //旋转轴
        SetData("U0\\G",9000+10*i,STOPMOVE);// 运行模式
        SetData2("U0\\G",9004+10*i,4194304);// 指令速度

        SetData("U0\\G",8001+10*i,i+1); //T轴 MCODE
    }


    SetData2("U0\\G",6006+10*i,s.movinc.x);//X轴
    SetData2("U0\\G",7006+10*i,s.movinc.y);//Y轴
    SetData2("U0\\G",8006+10*i,s.sewincTV.x); //T轴 //基准轴
    SetData2("U0\\G",10006+10*i,s.sewincTV.y);// V轴
    if(!RUNMET)
    {
        SetData("U0\\G",8000+10*i,CONTINUEMOVE4);// 运行模式
    }
    else
    {
        SetData("U0\\G",8000+10*i,STOPMOVE4);// 运行模式
    }
    SetData2("U0\\G",8004+10*i,4194304*3);// 指令速度

    SetData("U0\\G",8003+10*i,0x014);// 插补对象 后3个字节 1个字节为1轴  从0-15 二进制度数表示

    // M代码输出时机 27+150n
    // M代码 2408+100n
}


void GetCurrentRunPos()
{
    long nGetting=0;
    GetDevice("U0\\G2437",nGetting); // X轴 最后执行的NO.

}