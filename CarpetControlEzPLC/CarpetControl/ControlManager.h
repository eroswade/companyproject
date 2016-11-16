#pragma once
#include "PhraseManager.h"

#include "MXCompoment/ActDefine.h"
#include "MXCompoment\ActUtlType_i.h"	// For ActUtlType Control
#include "MXCompoment\ActProgType_i.h"	// For ActProgType Control


//�Դ̶���Ϊ��׼�� ������ΪT ������ΪD ������ΪV
// 4300+100n �������  1-600 ��ͬ����Ϊn.
#define PI 3.14159265358979323846264338327950288
#define CONTINUEMOVE 0x203
#define STOPMOVE    0x200

#define CONTINUEMOVE2 0xB03 // ����
#define STOPMOVE2    0xB00 //����

#define CONTINUEMOVE4 0x1B03
#define STOPMOVE4 0x1B00


// da.6 ��λ��
#define XAXIS_POS_BASE 6006 // word
#define YAXIS_POS_BASE 7006
#define TAXIS_POS_BASE 8006 // ǹ
#define VAXIS_POS_BASE 10006// ����
#define DAXIS_POS_BASE 9006 // ����

// da.1-da.5 ����ģʽ
#define XAXIS_CONTROL_BASE 6000
#define YAXIS_CONTROL_BASE 7000
#define TAXIS_CONTROL_BASE 8000
#define VAXIS_CONTROL_BASE 10000
#define DAXIS_CONTROL_BASE 9000

// da.4 �ٶ�
#define XAXIS_SPEED_BASE 6003
#define YAXIS_SPEED_BASE 7003
#define TAXIS_SPEED_BASE 8003
#define VAXIS_SPEED_BASE 10003
#define DAXIS_SPEED_BASE 9003

//CD.3 ����NO.
#define XAXIS_START_REG 4300
#define YAXIS_START_REG 4400
#define TAXIS_START_REG 4500
#define VAXIS_START_REG 4700
#define DAXIS_START_REG 4600


/*! \file ControlManager.h
\brief �����������
*/
extern BOOL b3DFlag;
#define  MAXCOUNT 5000

struct ct_motion {

	long dist1;
	long dist2;
};

struct s_MotionProcessPra
{
	cv::Point from;
	cv::Point to;
	BOOL bwithNeedle;
    BOOL b3DFlag;
};

struct RunProcessPra
{
	HWND hwnd;
	int x;
	int y;
	int runnum;
};

struct MotionOutput
{
    BOOL bRoll;
    int rollangle;
    cv::Point movinc;//X,Y setting by movinc
    cv::Point sewincTV;// for T and V . 
	int nPosNum;// ��¼���ν���,�ڼ���.
	int nTotalPosNum;// ��ȫͼ��˵�ǵڼ���
	cv::Point phyMvFrom;// ͼ���ƶ�, ���ĸ�λ�ÿ�ʼ
	cv::Point phyMvTo;// ͼ���ƶ�, ���ĸ�λ��
    MotionOutput(){
        bRoll=FALSE;//�Ƿ���ת
        rollangle=0;// �����ת, ��ת�ĽǶȶ���
        movinc.x=0;// X�ƶ���
        movinc.y=0;// Y�ƶ���
        sewincTV.x=0; //T�� ǹ
        sewincTV.y=0;//V�� ����
		phyMvFrom.x = 0;
		phyMvFrom.y = 0;
		phyMvTo.x = 0;
		phyMvTo.y = 0;
		nPosNum = 0;
		nTotalPosNum = 0;
    }
};

struct MotionRoll
{
    int Rollpos;
    int rollangle;
};

//enum AxisFlg
//{// X,Y,Z,V ����, D ��λ,Tǹ�� 
//	X1=6,
//	X2=-1,
//	Y=7,
//	Z=1,
//	V=5,
//	D=0,
//	T=4
//};

enum AxisFlg
{// X,Y,Z,V ����, D ��λ,Tǹ�� 
	X1=0,// DMC5410  ֮ǰ�ĵڶ��ſ�
	Y=1,// DMC5410
	V=2,// DMC5410
	D=3, //2410
	T=4 // DMC5410
};


extern int m_nXpos, m_nYpos;// X,Y�ĵ�ǰλ��

extern float m_fNeedleAngle;// ��Ƕ�
//extern int m_nLenX,m_nLenY;// ������С
extern int m_nZoffset;// ��С��Z.������Z��Zoffset֮������һ��ֵ
extern int m_nSpeed;//�Զ������ٶ�
extern int m_MotorPulseCount;// �ŷ����������

extern BOOL m_StopRunning;// ֹͣ���б�� �ñ�Ǳ�Ǻ���Щ����ֹͣ�������ֹͣ
extern int m_CurrentThreadPos; //��¼��ǰ����״̬�����ָ�����ʹ��

extern CString m_str3DImagePath;
extern cv::Mat m_3DImageData;
extern float m_MotorGearRatio[AXIS_COUNT];//���ֱ�. ���  ��ĸ
extern IActUtlType*	mp_IUtlType;	
extern IActProgType*	mp_IProgType;	
//extern float m_MachineGearRatio[AXIS_COUNT];//���ֱ�. �������� ���� 

//! ����M2����
extern void RunCalcPath(HWND hwnd, int Obj,int x=0,int y=0);

//! ����ֹͣ
extern void Stop_decel_NO();
//
extern void Clear_Stop_Sign();
//! ��ʼ������ĳ��ֱ�
extern void InitMotorGearRatio();

//! ���ŷ�
extern void OnSevOn();
//! �ر��ŷ�
extern void OffSevOn();
//! ����
extern void OnReturnZero();

//! ����ƫ��Ƕ�,���нǶ�ת��
extern double GetDiffdegree( cv::Point2f pOt2, cv::Point2f pOt1 );
//! ��ʼ�������ת������
extern void InitMotorDirection();

extern BOOL InitialCom();

extern BOOL SetDevice(CString DeviceName, int DeviceValue);
extern BOOL GetDevice(CString DeviceName, long& lValue);
extern void SetData2(CString strDVName,int nPos,int nLen);
extern void SetData(CString DeviceName,int npos, int DeviceValue);
extern void GetCurrentRunPos();


extern BOOL m_MOVEXYFLAG;
extern BOOL m_SEWFLAG;
extern int m_CurrentPos;