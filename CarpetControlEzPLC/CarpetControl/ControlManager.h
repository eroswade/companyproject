#pragma once
#include "PhraseManager.h"

#include "MXCompoment/ActDefine.h"
#include "MXCompoment\ActUtlType_i.h"	// For ActUtlType Control
#include "MXCompoment\ActProgType_i.h"	// For ActProgType Control


//以刺动作为基准轴 第三轴为T 第四轴为D 第五轴为V
// 4300+100n 启动编号  1-600 不同的轴为n.
#define PI 3.14159265358979323846264338327950288
#define CONTINUEMOVE 0x203
#define STOPMOVE    0x200

#define CONTINUEMOVE2 0xB03 // 二轴
#define STOPMOVE2    0xB00 //二轴

#define CONTINUEMOVE4 0x1B03
#define STOPMOVE4 0x1B00


// da.6 定位长
#define XAXIS_POS_BASE 6006 // word
#define YAXIS_POS_BASE 7006
#define TAXIS_POS_BASE 8006 // 枪
#define VAXIS_POS_BASE 10006// 送线
#define DAXIS_POS_BASE 9006 // 方向

// da.1-da.5 控制模式
#define XAXIS_CONTROL_BASE 6000
#define YAXIS_CONTROL_BASE 7000
#define TAXIS_CONTROL_BASE 8000
#define VAXIS_CONTROL_BASE 10000
#define DAXIS_CONTROL_BASE 9000

// da.4 速度
#define XAXIS_SPEED_BASE 6003
#define YAXIS_SPEED_BASE 7003
#define TAXIS_SPEED_BASE 8003
#define VAXIS_SPEED_BASE 10003
#define DAXIS_SPEED_BASE 9003

//CD.3 启动NO.
#define XAXIS_START_REG 4300
#define YAXIS_START_REG 4400
#define TAXIS_START_REG 4500
#define VAXIS_START_REG 4700
#define DAXIS_START_REG 4600


/*! \file ControlManager.h
\brief 控制运行相关
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
	int nPosNum;// 记录本次进入,第几针.
	int nTotalPosNum;// 对全图来说是第几针
	cv::Point phyMvFrom;// 图上移动, 从哪个位置开始
	cv::Point phyMvTo;// 图上移动, 到哪个位置
    MotionOutput(){
        bRoll=FALSE;//是否旋转
        rollangle=0;// 如果旋转, 旋转的角度多少
        movinc.x=0;// X移动量
        movinc.y=0;// Y移动量
        sewincTV.x=0; //T轴 枪
        sewincTV.y=0;//V轴 送线
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
//{// X,Y,Z,V 送线, D 方位,T枪刺 
//	X1=6,
//	X2=-1,
//	Y=7,
//	Z=1,
//	V=5,
//	D=0,
//	T=4
//};

enum AxisFlg
{// X,Y,Z,V 送线, D 方位,T枪刺 
	X1=0,// DMC5410  之前的第二张卡
	Y=1,// DMC5410
	V=2,// DMC5410
	D=3, //2410
	T=4 // DMC5410
};


extern int m_nXpos, m_nYpos;// X,Y的当前位置

extern float m_fNeedleAngle;// 针角度
//extern int m_nLenX,m_nLenY;// 画布大小
extern int m_nZoffset;// 最小的Z.其它的Z在Zoffset之上增加一个值
extern int m_nSpeed;//自动运行速度
extern int m_MotorPulseCount;// 伺服电机脉冲数

extern BOOL m_StopRunning;// 停止运行标记 该标记标记后，有些工作停止才算真的停止
extern int m_CurrentThreadPos; //记录当前运行状态，给恢复运行使用

extern CString m_str3DImagePath;
extern cv::Mat m_3DImageData;
extern float m_MotorGearRatio[AXIS_COUNT];//齿轮比. 电机  分母
extern IActUtlType*	mp_IUtlType;	
extern IActProgType*	mp_IProgType;	
//extern float m_MachineGearRatio[AXIS_COUNT];//齿轮比. 机器部分 分子 

//! 运行M2数据
extern void RunCalcPath(HWND hwnd, int Obj,int x=0,int y=0);

//! 减速停止
extern void Stop_decel_NO();
//
extern void Clear_Stop_Sign();
//! 初始化各轴的齿轮比
extern void InitMotorGearRatio();

//! 打开伺服
extern void OnSevOn();
//! 关闭伺服
extern void OffSevOn();
//! 回零
extern void OnReturnZero();

//! 计算偏差角度,运行角度转变
extern double GetDiffdegree( cv::Point2f pOt2, cv::Point2f pOt1 );
//! 初始化电机的转动方向
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