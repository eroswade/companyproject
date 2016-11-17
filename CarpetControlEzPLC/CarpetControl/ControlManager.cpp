#include "StdAfx.h"
#include "ControlManager.h"
#include <list>
#include "GlobalFunction.h"

using namespace std;
BOOL b3DFlag=TRUE;
BOOL NOTRUN = FALSE;


BOOL m_StopRunning;
int m_CurrentThreadPos=0; //��¼��ǰ����״̬�����ָ�����ʹ��
float m_MotorGearRatio[AXIS_COUNT];//���ֱ�. ���  ��ĸ
int m_MotorDirection[AXIS_COUNT];//����
//float m_MachineGearRatio[AXIS_COUNT];//���ֱ�. �������� ���� 
list<ct_motion> m_MotionPos;
//ct_motion m_MotionPos[BUFFER_COUNT];
int m_MotorPulseCount;// �ŷ����������
int m_nXpos, m_nYpos;// X,Y�ĵ�ǰλ��

float m_fNeedleAngle;// ��Ƕ� ������
double m_SumAngle = 0; // ִ��ʱ��ȷ��λ��
//int m_nLenX,m_nLenY;// ������С
int m_nZoffset;// ��С��Z.������Z��Zoffset֮������һ��ֵ
int m_nSpeed;//�Զ������ٶ�
CString m_str3DImagePath;
cv::Mat m_3DImageData;


BOOL m_MOVEXYFLAG;
BOOL m_SEWFLAG;
int m_CurrentPos; // ��ǰPLC��¼��λ��

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
#if DEBUG_WITHOUT_PLC
	return TRUE;
#endif

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
#if DEBUG_WITHOUT_PLC
	return TRUE;
#endif
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
	// ������������communcation setting utility���õ�
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
    // ��λΪmm
    m_MotorGearRatio[AxisFlg::X1] = 10.0/3.0;//MOTORһȦ,�ߵĺ�����
    m_MotorGearRatio[AxisFlg::Y] = 198.0/50.0;//MOTORһȦ,�ߵĺ�����
    m_MotorGearRatio[AxisFlg::V] = 13.0*PI*2.0;//һȦ���ߵĳ���   81.68mm
    m_MotorGearRatio[AxisFlg::D] = 22.0/100.0*360.0;//һȦ�ߵĶ��� 
    m_MotorGearRatio[AxisFlg::T] = 2.0/3.0;//һȦ�ߵ����� 
}
void InitMotorDirection()
{
    m_MotorDirection[AxisFlg::X1] = 1;
    m_MotorDirection[AxisFlg::Y] = 1;
    m_MotorDirection[AxisFlg::V] = 1;//
    m_MotorDirection[AxisFlg::D] = -1;// ��˳ʱ��Ϊ��
    m_MotorDirection[AxisFlg::T] = 1;//
}

DWORD MotionPhrase(s_MotionProcessPra* par,int& nCurrentSubPos, BOOL bRoll=FALSE, int nRollAngle=0);
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
        //wait for m_hThread to be over��and wait for  
        //QS_ALLINPUT��Any message is in the queue��  
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
void RunCalcPath(HWND hwnd, int Obj,int x,int y)
{
	NOTRUN = FALSE;// ��ʾ��������,��������ͼ
	if (x>0 || m_CurrentThreadPos>0)//
	{
		NOTRUN = TRUE; // ��ʾ������ͼ
	}
	int tmpnum = m_CurrentThreadPos;
	m_SumAngle = 0;
	m_fNeedleAngle = 0;
    cv::Point opOt;
    m_StopRunning = FALSE;
    m_RunCMD.clear();
	int nCurrentSubThreadPos = 0; // ��ǰ��һ��m_RunCMD������
    for (int iter=0; iter<m_ListAllPoint.size(); iter++)  
    {  
        cv::Point pOt = m_ListAllPoint[iter];
		MotionOutput PlistObj;
		if (pOt.x == MARK_M2.x && pOt.y == MARK_M2.y)//���� M2  ��������,��CUT֮ǰ,���������,��ִ������. ����ʼ��������
        {
            //CutProcess(NULL);
			RunProcessPra pra;
			pra.hwnd = hwnd;
			pra.x = x;
			pra.y = y;
			pra.runnum = tmpnum;
            while (m_RunCMD.size()>0)
            {
                HANDLE tThread =  CreateThread(NULL,0,MotionProcess,(LPVOID)&pra,0,NULL);
                WaitingForThread(tThread);
            }
            if (m_StopRunning)// �����ֹͣ, ��ֹ����Ĳ���
            {
                break;
            }
			HANDLE tThread;
			if (!NOTRUN)//��ִ��
			{
				// �����������֮��, ����, ��ǹ
				tThread =  CreateThread(NULL,0,CutProcess,NULL,0,NULL);
				WaitingForThread(tThread);

				tThread =  CreateThread(NULL,0,UpGunProcess,NULL,0,NULL);
				WaitingForThread(tThread);
			}

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
			cv::line(m_ShowPathMat, opOt, pOt, cv::Scalar(0, 0, 255));
			cv::imshow("MainDispWin", m_ShowPathMat);
			nCurrentSubThreadPos++;
			if (!NOTRUN)//��ִ��
			{
				tThread = CreateThread(NULL, 0, MoveXY, &s, 0, NULL);
				WaitingForThread(tThread);
			}
            iter++;
        }
        else if (pOt.x == MARK_M1.x && pOt.y == MARK_M1.y)//���� M1  ֻ������M1�����־
        {
			if (!NOTRUN)//��ִ��
			{
				HANDLE tThread = CreateThread(NULL, 0, DownGunProcess, 0, 0, NULL);
				WaitingForThread(tThread);
			}

            cv::Point2f pOt1 = m_ListAllPoint[iter+1];//��һ����
            cv::Point2f pOt2 = m_ListAllPoint[iter+2];// ��������
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
			cv::line(m_ShowPathMat, pOt1, pOt2, cv::Scalar(0, 255, 255));
			cv::imshow("MainDispWin", m_ShowPathMat);
			MotionPhrase(&s, nCurrentSubThreadPos, TRUE, degreepls);
            iter+=2;
        }
        else if (pOt.x == MARK_DWE.x && pOt.y == MARK_DWE.y)// ��ת DWE + XY  
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
			cv::line(m_ShowPathMat, pOt1, pOt2, cv::Scalar(0, 255, 255));
			cv::imshow("MainDispWin", m_ShowPathMat);
			MotionPhrase(&s, nCurrentSubThreadPos, TRUE, degreepls);

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
			cv::line(m_ShowPathMat, opOt, pOt, cv::Scalar(0, 255, 255));
			cv::imshow("MainDispWin", m_ShowPathMat);
			MotionPhrase(&s, nCurrentSubThreadPos);
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
    //SetDevice("U0\\G4351",0); // ������Ƶ�ʱ����
    //SetDevice("U0\\G4451",0);
    //SetDevice("U0\\G4551",0);
    //SetDevice("U0\\G4651",0);
    //SetDevice("U0\\G4751",0);
    //Sleep(20);
    SetDevice("Y1",1);
    SetDevice("Y30",1);// ����Y��
    long ret;
}

// ����ֹͣ PLC eros. 2016.11.16 �������������ֹͣ 
//��ʵ�Ŀ�����CD.18 ���������ж�����4320+100n  cd.18���Զ�����  cd.18�жϺ��޷���������
void Stop_decel_NO()
{
	// cd.180
    SetDevice("U0\\G30100",1);
    SetDevice("U0\\G30110",1);
    SetDevice("U0\\G30120",1);
    SetDevice("U0\\G30130",1);
    SetDevice("U0\\G30140",1);
}

// ���ֹͣ�ź�
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


DWORD MotionPhrase(s_MotionProcessPra* par, int &nCurrentSubPos, BOOL bRoll, int nRollAngle)
{
    s_MotionProcessPra *s = par;
    cv::Point from = s->from;
    cv::Point to = s->to;
    BOOL bwithNeedle=s->bwithNeedle; // �Ƿ�����
    //BOOL b3DFlag=s->b3DFlag;
    int m_nOffsetLen = 20;// Ĭ�����߳���
    ct_motion tmpMotion;
    tmpMotion.dist1=m_MotorDirection[AxisFlg::X1]*(to.x-from.x)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
    tmpMotion.dist2=m_MotorDirection[AxisFlg::Y]*(to.y-from.y)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];

    int mNeedle;//��������
    double stringlen;// X,Y�ߵ������ܳ�
    double needlelen;// ���ߵ������ܳ�

    MotionOutput PhraseObj;

    if (bwithNeedle)
    {
        stringlen = sqrt( double((to.x-from.x)*(to.x-from.x)) +  double((to.y-from.y)*(to.y-from.y)));

        // ���� �� ����*�ܶ� *������ / ����  ÿ����ƽ��
        mNeedle = (stringlen / (float)m_nDensity)+0.5;//����ת��Ϊ����  ��������
        //long TspeedLen =  mNeedle / m_MotorGearRatio[AxisFlg::T] * m_MotorPulseCount;//���ߵ��ܳ�

        // ���߳�
        needlelen = stringlen + mNeedle*2* m_nOffsetLen ;// ������

        if (b3DFlag)//3D
        {
            for (int nneddl=0; nneddl < mNeedle; nneddl++)//������ÿһ��������ֵ.
            {
                cv::Point2f nedDownPos;
                nedDownPos.x = float((to.x-from.x)) / float(mNeedle) * (nneddl+1);
                nedDownPos.y = float((to.y-from.y)) / float(mNeedle) * (nneddl+1);

                int nx = from.y+(nedDownPos.y+0.5);//��������
                int ny = from.x+(nedDownPos.x+0.5);
#if DEBUG_WITHOUT_PLC
				float dt = 0;
#else
                float dt= m_3DImageData.at<float>(nx,ny);// ��ȡ������λ�õ�3D����
#endif
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
				PlistObj.phyMvFrom.x = from.x + (float((to.x - from.x)) / float(mNeedle) * (nneddl) + 0.5);
				PlistObj.phyMvFrom.y = from.y + (float((to.y - from.y)) / float(mNeedle) * (nneddl) + 0.5);
				PlistObj.phyMvTo.x = ny;
				PlistObj.phyMvTo.y = nx;
				PlistObj.nTotalPosNum =  nCurrentSubPos;
				PlistObj.nPosNum = m_RunCMD.size() == 0 ? 0 : m_RunCMD.back().nPosNum + 1;
				nCurrentSubPos++;
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

				PlistObj.phyMvFrom.x = from.x + (float((to.x - from.x)) / float(mNeedle) * (i)+0.5);
				PlistObj.phyMvFrom.y = from.y + (float((to.y - from.y)) / float(mNeedle) * (i)+0.5);
				cv::Point2f nedDownPos;
				nedDownPos.x = float((to.x - from.x)) / float(mNeedle) * (i + 1);
				nedDownPos.y = float((to.y - from.y)) / float(mNeedle) * (i + 1);
				int nx = from.y + (nedDownPos.y + 0.5);//��������
				int ny = from.x + (nedDownPos.x + 0.5);
				PlistObj.phyMvTo.x = ny;
				PlistObj.phyMvTo.y = nx;
				PlistObj.nTotalPosNum = nCurrentSubPos;
				PlistObj.nPosNum = m_RunCMD.size() == 0 ? 0 : m_RunCMD.back().nPosNum + 1;
				nCurrentSubPos++;
				m_RunCMD.push_back(PlistObj);
            }
        }
    }
    return 0;

}

DWORD WINAPI MoveXY(LPVOID par)
{
    s_MotionProcessPra *s = (s_MotionProcessPra*)par;
    cv::Point from = s->from;
    cv::Point to = s->to;
    BOOL bwithNeedle=s->bwithNeedle; // �Ƿ�����
    //BOOL b3DFlag=s->b3DFlag;
    int m_nOffsetLen = 20;// Ĭ�����߳���
    ct_motion tmpMotion;
    tmpMotion.dist1=m_MotorDirection[AxisFlg::X1]*(to.x-from.x)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::X1];
    tmpMotion.dist2=m_MotorDirection[AxisFlg::Y]*(to.y-from.y)*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::Y];

    long distlist[2];
    distlist[0]=tmpMotion.dist1;
    distlist[1]=tmpMotion.dist2;

    m_MOVEXYFLAG = TRUE;

    stringstream strstrem;
    strstrem << "MovXY: X:" << distlist[0] << " Y:" << distlist[1];
    WriteDebugData(strstrem.str());

    SetDevice("U0\\G6000",STOPMOVE2);// һ�� ���� ���� NO.1
    SetData2("U0\\G",6006,distlist[0]);// ��λ��ַ
    SetData2("U0\\G",7006,distlist[1]);// ��λ��ַ
    SetData("U0\\G",6003,1);// �岹���� ��3���ֽ� 1���ֽ�Ϊ1��  ��0-15 �����ƶ�����ʾ
    //SetData2("U0\\G",,distlist[1]);
    SetData2("U0\\G",6004,4194304*4);//ָ���ٶ�

    SetDevice("U0\\G4300",1);// 4300+100n ��λ��������� 

    Sleep(200);

    SetDevice("Y10",1); // ��1 MOV 
    Sleep(10);
    SetDevice("Y10",0); // ��1 MOV 

	//cv::line(m_ShowPathMat, from, to,RGB(0,0,255));
	//cv::imshow("MainDispWin", m_ShowPathMat);

    while (m_MOVEXYFLAG) // ��ONTIME��Ŀ���λ��ֹͣ
    {
#if DEBUG_WITHOUT_PLC
        if (m_StopRunning == FALSE)//���Ե�ʱ����
        {
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
	RunProcessPra *parb = (RunProcessPra*)par;
	HWND tmpHWND = parb->hwnd;
    int roucount=0;
    int mxroucount = (int)floor(m_RunCMD.size()/50.0);
    if (m_RunCMD.size()%50 != 0)
    {
        mxroucount+=1;
    }

    stringstream strstrem;
    BOOL m_Wait100Needle = FALSE;
    int m_Runed=0;
    while (true)
    {
        {// �ֽ�·�� ÿ��50��������һ��.
            int dd = floor(roucount/2.0)*100;
            if (roucount%2==0)//1��0���� 5��2���룬��3
            {
                strstrem<<"��" <<roucount<<"�� 50����ѭ��";
                WriteDebugData(strstrem.str());
                strstrem.str("");
				if (!NOTRUN)// ����ͼ��ʱ��ִ��
				{
					for (int i = 0; i < 50; i++)//��һ�ηŽ�50����
					{
						if (dd + i + 1 >= m_RunCMD.size())
						{
							SettingMotionData4x(dd, i, TRUE);
							break;
						}
						SettingMotionData4x(dd, i, FALSE);
					}
				}
			}

            if (roucount%2==1)//3��roucount��1 ����
            {
                strstrem<<"��" <<roucount<<"�� 50-100 ���ݵ�ѭ��";
                WriteDebugData(strstrem.str());
                strstrem.str("");
				if (!NOTRUN)//����ͼ��ʱ��ִ��
				{
					for (int i = 50; i < 100; i++)//�ڶ��ηŽ�50����
					{
						if (i == 99 || dd + i + 1 >= m_RunCMD.size())
						{
							SettingMotionData4x(dd, i, TRUE);
							break;
						}
						else
						{
							SettingMotionData4x(dd, i, FALSE);
						}
					}
				}
			}

            // 
			if( roucount%2 == 0)
			{
                if (roucount == 0)
                {
					if (!NOTRUN)//����ͼ��ʱ��ִ��
					{
						WriteDebugData("��ʼ����");
						SetDevice("U0\\G4500", 1);// 4300+100n ��λ���������  
						Sleep(200);
						SetDevice("Y12", 1); // ��1 MOV 
						Sleep(10);
						SetDevice("Y12", 0); // ��1 MOV 
					}
				}
                else
                {
                    m_Wait100Needle = TRUE;//���õȴ�, 
                }
			}
			m_SEWFLAG=TRUE;
            roucount++;
        }
        while (m_SEWFLAG)
        {

			MotionOutput s = m_RunCMD[m_CurrentPos + m_Runed];
			cv::line(m_ShowPathMat, s.phyMvFrom, s.phyMvTo, cv::Scalar(0, 255, 0));
			cv::imshow("MainDispWin", m_ShowPathMat);
			if (s.bRoll == TRUE)// ����ת������ͳ������
			{
				//*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::D] * m_MotorDirection[AxisFlg::D]
				m_SumAngle += (s.rollangle /(float)m_MotorPulseCount)*m_MotorGearRatio[AxisFlg::D] * m_MotorDirection[AxisFlg::D];
				if (abs(m_SumAngle)>360)
				{
					if (m_SumAngle < 0)
						m_SumAngle+=360.0;
					else
						m_SumAngle-=360.0;
				}
			}

			if (NOTRUN)
			{
				if (parb->x >0)
				{
					float dx = (parb->x - s.phyMvTo.x);
					float dy = parb->y - s.phyMvTo.y;
					if (sqrt(dx*dx + dy*dy) < 3.0)
					{
						m_StopRunning = TRUE;
						break;
					}
				}
				else if (parb->runnum == s.nTotalPosNum)
				{
					NOTRUN = FALSE;
					//m_StopRunning = TRUE;
					//break;

					//CreateThread(NULL, 0, RoteBackProcess, 0, 0, NULL);
					//HANDLE tThread = CreateThread(NULL, 0, MoveXY, &s, 0, NULL);
					//WaitingForThread(tThread);
					//CreateThread(NULL, 0, UpGunProcess, 0, 0, NULL);

				}
				
			}

			CString strTmp;
			strTmp.Format("%d,%d/%d/%f", m_CurrentPos + m_Runed,s.nTotalPosNum, s.nPosNum,m_SumAngle);//��¼��ǰ������ ȫͼ��������
			::SetDlgItemText(tmpHWND, 1068, strTmp);
			m_CurrentThreadPos = s.nTotalPosNum;
			if (NOTRUN)//��ͼʱִ��
			{
				if (m_CurrentPos < 100)
				{
					m_CurrentPos++;
				}
				else
				{
					m_CurrentPos = 0;
				}
			}
#if DEBUG_WITHOUT_PLC
			if (!NOTRUN)//��ͼʱִ��
			{
				if (m_CurrentPos < 100)
				{
					m_CurrentPos++;
					strstrem << "m_CurrentPos" << m_CurrentPos;
					WriteDebugData(strstrem.str());
					strstrem.str("");
				}
				else
				{
					m_CurrentPos = 0;
				}
			}
#endif
            // BUG ���mxroucount <= roucount? �ᷢ��ʲô?
            if (m_CurrentPos > 50 && roucount%2==0 && roucount<mxroucount)//4��roucount=2 ����m_CurrentPos>50 break; 
            {
                strstrem<<"��" <<roucount<<"�� step 4, m_CurrentPos>50 ׼������һ������";
                WriteDebugData(strstrem.str());
                strstrem.str("");
                break;
            }
            else if (m_CurrentPos<10 && roucount%2==1 && roucount<mxroucount)//2���ж���Ч�� break�� 6��roucount��3��������50��100���꣬����
            {
                strstrem<<"��" <<roucount<<"�� step2, m_CurrentPos<10  ׼������һ������";
                WriteDebugData(strstrem.str());
                strstrem.str("");
                break;
            }
            if (m_StopRunning)
            {
                WriteDebugData("stop running ");
                break;
            }
            if (m_Wait100Needle)//������ڵ�50-100
            {
                if (m_CurrentPos == 100)//����ܵ�100, Ҫ���¿�ʼ������һ��.
                {
                    strstrem<<"��" <<roucount<<"�� ��ʼ���� m_Wait100Needle";
                    WriteDebugData(strstrem.str());
                    strstrem.str("");
                    while (!NOTRUN)//�е�ʱ����ܵ�̫�� ����ͼ��ʱ��ִ��
                    {
                        long nGetting=0;
                        GetDevice("U0\\G2609",nGetting);//�ȴ���״̬��� ����ᵼ��ϵͳ����.
                        if (nGetting == 0 || nGetting ==1)
                        {
                            break;
                        }
						Sleep(50);
                    }
					//��ʼ������һ��100��
					if (!NOTRUN)//����ͼ��ʱ��ִ��
					{
						SetDevice("U0\\G4500", 1);// 4300+100n ��λ���������  
						Sleep(200);
						SetDevice("Y12", 1); // ��1 MOV 
						Sleep(10);
						SetDevice("Y12", 0); // ��1 MOV 
					}

                    m_Wait100Needle = FALSE;
                    m_Runed+=100;
					m_CurrentPos = 0;// ���������OK. ���������е�ʱ����ܻ������ 2016.11.16
				}
            }
            if(m_Runed+m_CurrentPos == m_RunCMD.size())// һ������ȫ����, ����   ���������Kout == m_RunCMD.size()
            {
                strstrem<<"��" <<roucount<<"�� һ�����н���";
                WriteDebugData(strstrem.str());
                strstrem.str("");
                break;
            }
			if (!NOTRUN)//����ͼ��ʱ��ִ��
				Sleep(50);
        }
		if(m_CurrentPos+m_Runed == m_RunCMD.size())
		{
            strstrem<<"��" <<roucount<<"�� һ�����н��� ȷ��!";
            WriteDebugData(strstrem.str());
            strstrem.str("");
			m_CurrentPos = 0;
            break;
		}
        if (m_StopRunning)
        {
            WriteDebugData("stop running ");
			m_Wait100Needle = FALSE;
			m_CurrentPos = 0;// ���������OK. ���������е�ʱ����ܻ������ 2016.11.16
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

    SetDevice("U0\\G9000",STOPMOVE);// 5�� ���� ���� NO.1  NO.2 6010       6000+1000n n��
    //SetDevice("U0\\G6003",0x203);// �岹��������
    SetData2("U0\\G",9004,4194304);// ָ���ٶ�
    SetData2("U0\\G",9006,nPluse);// ��λ��ַ
    SetDevice("U0\\G4600",1);// 4300+100n ��λ��������� 

    Sleep(200);
    SetDevice("Y13",1); // ��5 MOV 
    Sleep(10);
    SetDevice("Y13",0); // ��5 MOV 

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
    //m_SumAngle += diffdegree;

    int nPluse = diffdegree*m_MotorPulseCount/m_MotorGearRatio[AxisFlg::D] * m_MotorDirection[AxisFlg::D];

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
    SetDevice("U0\\G9000",STOPMOVE);// 5�� ���� ���� NO.1  NO.2 6010       6000+1000n n��
    //SetDevice("U0\\G6003",0x203);// �岹��������
    SetData2("U0\\G",9004,4194304);// ָ���ٶ�
    SetData2("U0\\G",9006,nPluse);// ��λ��ַ
    SetDevice("U0\\G4600",1);// 4300+100n ��λ��������� 

    Sleep(200);
    SetDevice("Y13",1); // ��5 MOV 
    Sleep(10);
    SetDevice("Y13",0); // ��5 MOV 
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

// ����� �޸�ΪPLC
void OnReturnZero()
{
    CreateThread(NULL,0,CutProcess,0,0,NULL);
	CreateThread(NULL, 0, UpGunProcess, 0, 0, NULL);

    CreateThread(NULL,0,RoteBackProcess,0,0,NULL);

	//MD.20
	long pox, poy;
	GetDevice("UO\\G2400", pox);
	GetDevice("UO\\G2500", poy);
    long x = pox ;
    long y = poy;

	long distlist[2]={-x,-y};

    SetDevice("U0\\G6000",STOPMOVE2);// һ�� ���� ���� NO.1
    SetData2("U0\\G",6006,distlist[0]);// ��λ��ַ
    SetData2("U0\\G",7006,distlist[1]);// ��λ��ַ
    SetData("U0\\G",6003,1);// �岹���� ��3���ֽ� 1���ֽ�Ϊ1��  ��0-15 �����ƶ�����ʾ
    //SetData2("U0\\G",,distlist[1]);
    SetData2("U0\\G",6004,4194304*4);//ָ���ٶ�

    SetDevice("U0\\G4300",1);// 4300+100n ��λ��������� 

    Sleep(200);

    SetDevice("Y10",1); // ��1 MOV 
    Sleep(10);
    SetDevice("Y10",0); // ��1 MOV 

}

void SettingMotionData4x( int dd, int i , BOOL RUNMET)
{

    stringstream strstrem;

    MotionOutput s=m_RunCMD[dd+i];
    if (s.bRoll)//����ת
    {
        SetData2("U0\\G",9006+10*i,s.rollangle); //��ת��
        SetData("U0\\G",9000+10*i,STOPMOVE);// ����ģʽ
        SetData2("U0\\G",9004+10*i,4194304*3);// ָ���ٶ�

        SetData("U0\\G",8001+10*i,i+1); //T�� MCODE

        strstrem << "SettingMotionData4x: Roll:" << s.rollangle ;
        WriteDebugData(strstrem.str());
        strstrem.str("");
    }


    SetData2("U0\\G",6006+10*i,s.movinc.x);//X��
    SetData2("U0\\G",7006+10*i,s.movinc.y);//Y��
    SetData2("U0\\G",8006+10*i,s.sewincTV.x); //T�� //��׼��
    SetData2("U0\\G",10006+10*i,s.sewincTV.y);// V��
    if(!RUNMET)
    {
        strstrem << "SettingMotionData4x: continue count:" <<dd+i <<" X:" << s.movinc.x << " Y:" << s.movinc.y\
            << " T(master):" << s.sewincTV.x << " V:"<<s.sewincTV.y;
        WriteDebugData(strstrem.str());
        strstrem.str("");
        SetData("U0\\G",8000+10*i,CONTINUEMOVE4);// ����ģʽ
    }
    else
    {
        strstrem << "SettingMotionData4x: stop count"<< dd+i <<" X:" << s.movinc.x << " Y:" << s.movinc.y\
            << " T(master):" << s.sewincTV.x << " V:"<<s.sewincTV.y;
        WriteDebugData(strstrem.str());
        strstrem.str("");
        SetData("U0\\G",8000+10*i,STOPMOVE4);// ����ģʽ
    }
    SetData2("U0\\G",8004+10*i,4194304*3);// ָ���ٶ�

    SetData("U0\\G",8003+10*i,0x014);// �岹���� ��3���ֽ� 1���ֽ�Ϊ1��  ��0-15 �����ƶ�����ʾ

    // M�������ʱ�� 27+150n
    // M���� 2408+100n
}


void GetCurrentRunPos()
{
    long nGetting=0;
    GetDevice("U0\\G2437",nGetting); // X�� ���ִ�е�NO.

}