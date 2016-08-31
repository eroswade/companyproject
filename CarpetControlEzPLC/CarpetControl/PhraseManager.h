#pragma once

#include "infill.h"
#include "pathOptimizer.h"
#include "polygonOptimizer.h"
#include <math.h>
#include "clipper.hpp"
#include "BinaryTree.h"
/*! \file PhraseManager.h
\brief ͼ��������
*/


//#define SHOW_DEBUG_IMAGE

#define PHRASE_PROCESS_MSG 2012
#define AXIS_COUNT 5
#define BUFFER_COUNT 20
#define  MARK_M2	cv::Point2f(-20,-20)// ����
#define  MARK_M1	cv::Point2f(-10,-10)// ����
#define  MARK_DWE	cv::Point2f(-30,-30)// ת��
struct SThreadMsg
{
	HWND nHwnd;
	CString strPath;
};


extern int m_ResolutionX,m_ResolutionY;// X Y resolution�ı��� 
extern int m_nDensity;// �ܶ�
extern vector<cv::Point2f> m_ListAllPoint;
extern ClipperLib::Polygons m_PG;
extern CBinaryTree<CInt, int, int, int> m_BST;
extern cv::Mat m_ShowPathMat;
extern int m_ImageSizeX, m_ImageSizeY;
/**
�ж��Ƿ�����ת��ĵط�(����ĽǶȸı�)
��ĳЩ����»���Ҫ�����.
@param vctPoint ���,�ж�֮����������
@param poly ����,ԭʼ����������
@param count �ж�ploy�еĵ�count����
*/
extern BOOL JudgeAngleChange(vector<cv::Point2f> &vctPoint, ClipperLib::Polygon& poly, int count);
/**
һ��������״�п��ܰ�������Ȧ,�Լ�����Ӽ�����״.
�����Ǵ�������Ȧ���Ӽ���ͼ�ι�ϵ�ĺ���
@param contours counter ���ɵ�����
@param hierarchy counter���߼���ϵ����
@param  i hierarchy��� 
@param  outPoly ���
@param  isback  ���������Ҫ��Ҫ��ת
@param  lines ���ͼ
*/
extern void addChildContour(vector< vector< cv::Point > > contours,vector< cv::Vec4i > hierarchy,int i, ClipperLib::Polygons& outPoly,bool isback,cv::Mat lines = cv::Mat());
//DWORD WINAPI RoteProcess(LPVOID fDegree);

//! PhraseImage 
extern DWORD __stdcall PhraseImage(LPVOID strOPath);
void TreePlot( vector< cv::Vec4i > &hierarchy, vector< vector< cv::Point > > &contours, cv::Mat &lines );
// 
void TestFindContours( vector< cv::Vec4i > hierarchy, cv::Mat dst, vector< vector< cv::Point > > contours, double nmin, double nmax, cv::Mat bkphrone );
/**
дM2�ļ�
*/
extern void WriteM2File( CString strTxtOutPath ) ;
/**
��M2�ļ�
*/
extern void ReadM2File( CString strPath ) ;
/**
��M2·��
*/
extern void DrawM2Path();

