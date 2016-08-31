#pragma once

#include "infill.h"
#include "pathOptimizer.h"
#include "polygonOptimizer.h"
#include <math.h>
#include "clipper.hpp"
#include "BinaryTree.h"
/*! \file PhraseManager.h
\brief 图像解析相关
*/


//#define SHOW_DEBUG_IMAGE

#define PHRASE_PROCESS_MSG 2012
#define AXIS_COUNT 5
#define BUFFER_COUNT 20
#define  MARK_M2	cv::Point2f(-20,-20)// 切线
#define  MARK_M1	cv::Point2f(-10,-10)// 下针
#define  MARK_DWE	cv::Point2f(-30,-30)// 转角
struct SThreadMsg
{
	HWND nHwnd;
	CString strPath;
};


extern int m_ResolutionX,m_ResolutionY;// X Y resolution的倍数 
extern int m_nDensity;// 密度
extern vector<cv::Point2f> m_ListAllPoint;
extern ClipperLib::Polygons m_PG;
extern CBinaryTree<CInt, int, int, int> m_BST;
extern cv::Mat m_ShowPathMat;
extern int m_ImageSizeX, m_ImageSizeY;
/**
判断是否有在转弯的地方(下针的角度改变)
在某些情况下还需要插入点.
@param vctPoint 输出,判断之后的数据输出
@param poly 输入,原始坐标点的输入
@param count 判断ploy中的第count个点
*/
extern BOOL JudgeAngleChange(vector<cv::Point2f> &vctPoint, ClipperLib::Polygon& poly, int count);
/**
一个几何形状有可能包涵内外圈,以及多个子几何形状.
这里是处理内外圈及子几何图形关系的函数
@param contours counter 生成的数组
@param hierarchy counter的逻辑关系数组
@param  i hierarchy序号 
@param  outPoly 输出
@param  isback  输出的数据要不要逆转
@param  lines 输出图
*/
extern void addChildContour(vector< vector< cv::Point > > contours,vector< cv::Vec4i > hierarchy,int i, ClipperLib::Polygons& outPoly,bool isback,cv::Mat lines = cv::Mat());
//DWORD WINAPI RoteProcess(LPVOID fDegree);

//! PhraseImage 
extern DWORD __stdcall PhraseImage(LPVOID strOPath);
void TreePlot( vector< cv::Vec4i > &hierarchy, vector< vector< cv::Point > > &contours, cv::Mat &lines );
// 
void TestFindContours( vector< cv::Vec4i > hierarchy, cv::Mat dst, vector< vector< cv::Point > > contours, double nmin, double nmax, cv::Mat bkphrone );
/**
写M2文件
*/
extern void WriteM2File( CString strTxtOutPath ) ;
/**
读M2文件
*/
extern void ReadM2File( CString strPath ) ;
/**
画M2路径
*/
extern void DrawM2Path();

