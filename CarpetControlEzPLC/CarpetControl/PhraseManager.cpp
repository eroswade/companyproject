#include "StdAfx.h"
#include "PhraseManager.h"
#include "resource.h"
//#include "PhraseFrame.h"

int m_ResolutionX,m_ResolutionY;// X Y resolution的倍数 
int m_nDensity;// 密度
vector<cv::Point2f> m_ListAllPoint;
ClipperLib::Polygons m_PG;
CBinaryTree<CInt, int, int, int> m_BST;
cv::Mat m_ShowPathMat;
int m_ImageSizeX, m_ImageSizeY;

void addChildContour(vector< vector< cv::Point > > contours,
        vector< cv::Vec4i > hierarchy,int i, ClipperLib::Polygons& outPoly,bool isback,cv::Mat lines)
{

    for( ; i >= 0; i = hierarchy[i][0] )
    {
        ClipperLib::Polygon PGtmpplus;
        for (int pt=0; pt<contours[i].size(); pt++)
        {
            cv::Point d = contours[i][pt];
            //lines.ptr<char>(d.y)[d.x] = 100;
            if (lines.dims!=0)
            {
                cv::circle(lines,d,1,cv::Scalar(0,0,255));
            }
            ClipperLib::IntPoint ipt;
            ipt.X=d.x*100;
            ipt.Y=d.y*100;
            PGtmpplus.push_back(ipt);
        }
        outPoly.push_back(PGtmpplus);
        int nsize = outPoly.size();
        bool mback = ClipperLib::Orientation(outPoly[nsize-1]);
        if (mback!=isback)
        {
            ReversePolygon(outPoly[nsize-1]);
        }

        int v_next = hierarchy[i][2];
        if( v_next >= 0 )
        {
            addChildContour(contours, hierarchy, i, outPoly,!isback,lines);
        }
    }
}
void DrawM2Path()
{
    m_ShowPathMat = cv::Mat::zeros(m_ImageSizeX,m_ImageSizeY,CV_8UC3);
    cv::Point2f pLast;
    for (int iter=0; iter<m_ListAllPoint.size(); iter++)  
    {
        cv::Point2f pOt = m_ListAllPoint[iter];
        if (pOt.x == MARK_M2.x && pOt.y == MARK_M2.y)
        {
            iter++;
            continue;
        }
        else if (pOt.x == MARK_M1.x && pOt.y == MARK_M1.y)
        {
            iter++;
            pLast =  m_ListAllPoint[iter];
            iter++;
            pOt = m_ListAllPoint[iter];
            cv::line(m_ShowPathMat,pLast,pOt,cv::Scalar(128,128,128));
            pLast = pOt;
            continue;
        }
        else if (pOt.x == MARK_DWE.x && pOt.y == MARK_DWE.y)
        {
            iter++;
            pOt = m_ListAllPoint[iter];
            cv::line(m_ShowPathMat,pLast,pOt,cv::Scalar(128,128,128));
            pLast = pOt;
        }
        else
        {
            cv::line(m_ShowPathMat,pLast,pOt,cv::Scalar(128,128,128));
            pLast = pOt;
        }
    }
}
void WriteM2File( CString strTxtOutPath ) 
{
    CFile file(strTxtOutPath, CFile::modeWrite|CFile::modeCreate); //打开文件
    for (int iter=0; iter<m_ListAllPoint.size(); iter++)  
    {  
        cv::Point2f pOt = m_ListAllPoint[iter];
        if (pOt.x == MARK_M2.x && pOt.y == MARK_M2.y)
        {
            CString strtmp = "M2\n";
            file.Write(strtmp,strtmp.GetLength());
            iter++;
            if (iter>=m_ListAllPoint.size())
            {
                continue;
            }
            pOt = m_ListAllPoint[iter];
            strtmp.Format("G0 X%.02f Y%.02f\n",pOt.x,pOt.y);
            file.Write(strtmp,strtmp.GetLength());
        }
        else if (pOt.x == MARK_M1.x && pOt.y == MARK_M1.y)
        {
            CString strtmp = "M1\n";
            file.Write(strtmp,strtmp.GetLength());
        }
        else if (pOt.x == MARK_DWE.x && pOt.y == MARK_DWE.y)
        {
            CString strtmp = "DWE0\n";
            file.Write(strtmp,strtmp.GetLength());
        }
        else
        {
            CString strtmp;
            strtmp.Format("G1 X%.02f Y%.02f\n",pOt.x,pOt.y);
            file.Write(strtmp,strtmp.GetLength());
        }
    }
    file.Close();
}

//void PhraseImage(CString strPath)
DWORD __stdcall PhraseImage(LPVOID strOPath)
{
    CString strPath = *(CString*)strOPath;
    string str = strPath.GetBuffer(strPath.GetLength());
    cv::Mat origin = cv::imread(str);
    m_ImageSizeX=origin.rows;
    m_ImageSizeY=origin.cols;
    //imshow("test",origin);
#ifdef SHOW_DEBUG_IMAGE
    cv::namedWindow("test",CV_WINDOW_NORMAL|CV_WINDOW_FREERATIO|CV_GUI_NORMAL);
#endif
    double nmin;
    double nmax;
    //cv::minMaxLoc(origin,&nmin,&nmax);
    cv::Mat gray;
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
    if (nmin>0)
    {
        gray=gray-nmin;
    }
    //imshow("test",gray);
    vector<int> p;
    p=cv::Mat_<int>(gray.reshape(1,gray.rows*gray.cols));
    sort(p.begin(),p.end());
    p.erase(unique(p.begin(),p.end()),p.end());
    int nColorCount=p.size();
    //((CProgressCtrl*)hwnd)->SetRange(0,nColorCount);
    //分层 不同的颜色
    for (int i=0; i<nColorCount; i++)
    {
        //PostMessage(hwnd,PHRASE_PROCESS_MSG,(i+1)/nColorCount,0);

        CString strTxtOutPath;
        strTxtOutPath.Format("%s-Phrase%d.txt",strPath.Left(strPath.ReverseFind('.')),i);

        m_ListAllPoint.clear();
        cv::Mat phrone = gray==p[i];
        cv::minMaxLoc(phrone,&nmin,&nmax);

        //imshow("test",phrone);
        //cvvResizeWindow("test",800,600);
        //cv::waitKey();
        cv::Mat bkphrone;
        phrone.copyTo(bkphrone);
        vector< vector< cv::Point > > contours;
        vector< cv::Vec4i > hierarchy;

        //hierarchy  next and previous, first child contour and the parent contour
        cv::findContours(phrone ,contours ,hierarchy, CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
        cv::Mat dst = cv::Mat::zeros(phrone.rows,phrone.cols,CV_8UC3);
        if (contours.size()==0)
        {
            continue;
        }
#ifdef SHOW_DEBUG_IMAGE
        cv::drawContours(bkphrone,contours,0,cv::Scalar(255,0,0));
        imshow("test",bkphrone);
        cv::waitKey(0);
#endif
        // 遍历外框排序
        PathOptimizer pathOptimize(ClipperLib::IntPoint(0,0));
        ClipperLib::Polygons plogs;
        vector<int> orderid;
        for( int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
        {
            if (contours[idx].size() < 3)
            {
                continue;
            }
            ClipperLib::Polygon plog;
            for (int i=0; i<contours[idx].size(); i++)
            {
                plog.push_back(ClipperLib::IntPoint(contours[idx][i].x,contours[idx][i].y));
            }
            plogs.push_back(plog);
            orderid.push_back(idx);
        }
        pathOptimize.addPolygons(plogs);
        pathOptimize.optimize();

        plogs.clear();

        ClipperLib::IntPoint IPStart=ClipperLib::IntPoint(0,0);
        //for( int idx = 0; idx >= 0; idx = hierarchy[idx][0] )
        for( int idx = 0; idx <pathOptimize.polygons.size(); idx ++)
        {

            vector<int>::iterator result = find( pathOptimize.polyOrder.begin( ), pathOptimize.polyOrder.end( ), idx );
            int nID = orderid[pathOptimize.polyOrder[*result]];
            //int nID = orderid[pathOptimize.polyOrder[idx]];
            //int nID = idx;
            CString strdbgmsg;
            strdbgmsg.Format("%d \n",nID);
            TRACE0(strdbgmsg);
#ifdef SHOW_DEBUG_IMAGE
            cv::Mat lines = cv::Mat::zeros(phrone.rows,phrone.cols,CV_8UC3);
#endif
            ClipperLib::Polygons PGtmp;	
            PGtmp.resize(1);

            for (int pt=0; pt<contours[nID].size(); pt++)
            {
                cv::Point d = contours[nID][pt];
                ClipperLib::IntPoint ipt;
                ipt.X=d.x*100;
                ipt.Y=d.y*100;
                PGtmp[0].push_back(ipt);
            }
            // 判断内外圈
            bool mback = ClipperLib::Orientation(PGtmp[0]);
            // 如果是判断出是内圈,修改为外圈
            if (mback)
            {
                ReversePolygon(PGtmp[0]);
            }
            if (hierarchy[nID][2]!=-1)
            {
                int m = hierarchy[nID][2];
                // 填充内圈数据,一样到PGtmp
                addChildContour(contours,hierarchy,m,PGtmp,!mback);
            }


            // 优化内外多边形 start fill edge
#ifdef FILL_OUT_EDGE
            for (int i = 0; i < PGtmp.size() ; i++)
            {
                PathOptimizer inoutfillOpt(IPStart);
                optimizePolygon(PGtmp[i]);
                optimizePolygon2(PGtmp[i]);
                if (PGtmp[i].size()!=0)
                {
                    m_ListAllPoint.push_back(MARK_M2);
                    inoutfillOpt.addPolygon(PGtmp[i]);
                    inoutfillOpt.optimize();
                    int nCount = inoutfillOpt.polyStart[0];
                    for (int pt =0; pt<PGtmp[i].size()+1;pt++)
                    {
                        BOOL bret = JudgeAngleChange(m_ListAllPoint,PGtmp[i],nCount);
                        m_ListAllPoint.push_back(cv::Point2f(PGtmp[i][nCount].X/100,PGtmp[i][nCount].Y/100));
                        if (bret == TRUE && pt<PGtmp[i].size())
                        {
                            m_ListAllPoint.push_back(MARK_DWE);
                        }
                        if (nCount == PGtmp[i].size()-1)
                        {
                            nCount = 0;
                            continue;
                        }
                        nCount++;
                    }

                    IPStart=ClipperLib::IntPoint(m_ListAllPoint.back().x*100,m_ListAllPoint.back().y*100);
                }
            }
#ifdef SHOW_DEBUG_IMAGE
            for (int pt=0; pt<PGtmp[0].size(); pt++)
            {
                cv::Point d = cv::Point(PGtmp[0][pt].X/100,PGtmp[0][pt].Y/100);
                cv::circle(lines,d,2,cv::Scalar(0,255,0));
            }
#endif
#endif
            //end fill edge

            //m_ListAllPoint.push_back(cv::Point2f(-2,-2));
            // 修正PGtmp数据, 必须为ClipperLib可接受. 另外,完成为-10的OFFSET(100倍)
            ClipperLib::OffsetPolygons(PGtmp,PGtmp,-1000,ClipperLib::JoinType::jtMiter);

            //计算infill  需要测试参数
            generateLineInfill(PGtmp,m_PG,1000,m_ResolutionY*100,m_ResolutionY,45,PGtmp);
            //int offsetlen[] = {1000};
            //generateConcentricInfill(PGtmp,m_PG,offsetlen,1);

#ifdef SHOW_DEBUG_IMAGE
            for (int ptl=0; ptl<m_PG.size();ptl++)
            {
                cv::Point p1,p2;
                p1.x=m_PG[ptl][0].X/100;
                p1.y=m_PG[ptl][0].Y/100;
                p2.x=m_PG[ptl][1].X/100;
                p2.y=m_PG[ptl][1].Y/100;
                cv::line(lines,p1,p2,cv::Scalar(100,100,100),2);
            }
            imshow("test",lines);
            cv::waitKey();

            //for (int ptl=0; ptl<m_PG.size();ptl++)
            //{
            //	for (int ttp=0; ttp< m_PG[ptl].size();ttp++)
            //	{
            //			cv::Point p1,p2;
            //			if (ttp < m_PG[ptl].size()-1)
            //			{
            //				p1.x=m_PG[ptl][ttp].X/100;
            //				p1.y=m_PG[ptl][ttp].Y/100;
            //				p2.x=m_PG[ptl][ttp+1].X/100;
            //				p2.y=m_PG[ptl][ttp+1].Y/100;
            //			}
            //			else
            //			{
            //				p1.x=m_PG[ptl][ttp].X/100;
            //				p1.y=m_PG[ptl][ttp].Y/100;
            //				p2.x=m_PG[ptl][0].X/100;
            //				p2.y=m_PG[ptl][0].Y/100;
            //			}
            //		cv::line(lines,p1,p2,cv::Scalar(100,100,100),2);
            //	}
            //}
            //imshow("test",lines);
            //cv::waitKey();
#endif

            // 排序infill
            PathOptimizer orderOptimize(IPStart);
            orderOptimize.addPolygons(m_PG);
            orderOptimize.optimize();

            // 生成M2文件内存
            vector<cv::Point2f> ret;
            ret.push_back(MARK_M2);
            for (int pt=0; pt<orderOptimize.polygons.size(); pt++)
            {
                BOOL bRote = TRUE;
                if (ret.back().x==-20 && ret.back().y==-20)
                {
                    bRote =FALSE;
                    if (orderOptimize.polyStart[orderOptimize.polyOrder[pt]]==0)
                    {
                        JudgeAngleChange(ret,*orderOptimize.polygons[orderOptimize.polyOrder[pt]],0);// 插入一点
                    }
                    else
                    {
                        ClipperLib::Polygon ppoly = *orderOptimize.polygons[orderOptimize.polyOrder[pt]];
                        reverse(ppoly.begin(), ppoly.end());
                        JudgeAngleChange(ret,ppoly,0);// 插入一点
                    }

                }
                if (orderOptimize.polyStart[orderOptimize.polyOrder[pt]]==0)
                {
                    ClipperLib::Polygon poly = *orderOptimize.polygons[orderOptimize.polyOrder[pt]];
                    ClipperLib::IntPoint p0 = poly[0];
                    ret.push_back(cv::Point(p0.X/100,p0.Y/100));
                    if (pt!=0 && bRote)
                    {
                        ret.push_back(MARK_DWE);
                    }

                    ClipperLib::IntPoint p1 = poly[1];
                    ret.push_back(cv::Point(p1.X/100,p1.Y/100));
                }
                else
                {
                    ClipperLib::Polygon poly = *orderOptimize.polygons[orderOptimize.polyOrder[pt]];
                    ClipperLib::IntPoint p0 = poly[1];
                    ret.push_back(cv::Point(p0.X/100,p0.Y/100));

                    if (pt!=0 && bRote)
                        ret.push_back(MARK_DWE);

                    ClipperLib::IntPoint p1 = poly[0];
                    ret.push_back(cv::Point(p1.X/100,p1.Y/100));
                }
                if (pt<orderOptimize.polygons.size()-1)
                {
                    ClipperLib::Polygon poly1 = *orderOptimize.polygons[orderOptimize.polyOrder[pt]];
                    ClipperLib::IntPoint p1,p2;
                    if (orderOptimize.polyStart[orderOptimize.polyOrder[pt]]==0)
                    {
                        p1 = poly1[1];
                    }
                    else
                    {
                        p1 = poly1[0];
                    }
                    ClipperLib::Polygon poly2 = *orderOptimize.polygons[orderOptimize.polyOrder[pt+1]];
                    if (orderOptimize.polyStart[orderOptimize.polyOrder[pt+1]]==0)
                    {
                        p2 = poly2[0];
                    }
                    else
                    {
                        p2 = poly2[1];
                    }

                    if (!shorterThen(p1-p2,(m_ResolutionX>m_ResolutionY?m_ResolutionX*4:m_ResolutionY*4)*100))
                    {
                        ret.push_back(MARK_M2);
                        continue;
                    }

                }
                if (pt<orderOptimize.polygons.size()-1)
                {
                    ret.push_back(MARK_DWE);
                }
            }
            m_ListAllPoint.insert(m_ListAllPoint.end(),ret.begin(),ret.end());
            IPStart=ClipperLib::IntPoint(m_ListAllPoint.back().x*100,m_ListAllPoint.back().y*100);
            ret.clear();
            m_PG.clear();
        }
        //TestFindContours(hierarchy, dst, contours, nmin, nmax, bkphrone);

        DrawM2Path();
        // 写M2文件
        WriteM2File(strTxtOutPath);

    }
    return 0;
}


BOOL JudgeAngleChange(vector<cv::Point2f> &vctPoint, ClipperLib::Polygon& poly, int count)
{
    // 判断是否为刚进入M2
    cv::Point2f pt = vctPoint.back();
    BOOL bInsertBackPoint = FALSE;
    if (pt.x==-20.0 && pt.y==-20.0)
    {
        bInsertBackPoint = TRUE;
    }

    ClipperLib::IntPoint pM = poly[count];//middle
    ClipperLib::IntPoint pN ,pP;// point Next , point Pervious

    if (count >= poly.size()-1) // 获取下一个点
    {
        pN = poly[0];
    }
    else
    {
        pN = poly[count+1];
    }

    // XY----AB---CD
    if (bInsertBackPoint)// 以NEXT 和 MIDDLE做直线插入一点
    {
        int A,B,C,D;
        int X,Y;
        A=pM.X;		B=pM.Y;		C=pN.X;		D=pN.Y;
        float K;
        BOOL bMaxK=FALSE;
        if (B != D)
        {
            K = (A-C)/(B-D);
        }
        else
            bMaxK=TRUE;
        if (!bMaxK)
        {
            if (D>B)
            {
                Y=B-sqrt(160000/(pow((float)((A-C)/(B-D)),2)+1));
            }
            else
            {
                Y=B+sqrt(160000/(pow((float)((A-C)/(B-D)),2)+1));
            }
            X=A+(A-C)/(B-D)*(Y-B);
        }
        else
        {
            if (A>C)
            {
                X=A+400;
            }
            else
            {
                X=A-400;
            }
            Y=B;
        }

        pt.x = float(X/100.0)*3;//by eros 2016年4月21日 为了加速度考虑, 增加3倍距离
        pt.y = float(Y/100.0)*3;
        vctPoint.push_back(pt);
        vctPoint.push_back(MARK_M1);
        return FALSE;
    }
    else// 判断是否要转针头方向
    {

        if (count == 0)// 获取上一个点
        {
            pP = poly[poly.size()-1];
        }
        else
        {
            pP = poly[count-1];
        }
        Point diff0 = normal(pM - pN, 1000000);
        Point diff2 = normal(pM - pP, 1000000);

        int64_t d = dot(diff0, diff2);
        if (d > -980000000000 && d<980000000000)
        {
            return TRUE;
        }
        else
            return FALSE;
    }
    return TRUE;
}


void TestFindContours( vector< cv::Vec4i > hierarchy, cv::Mat dst, vector< vector< cv::Point > > contours, double nmin, double nmax, cv::Mat bkphrone )
{
    cv::Mat ret;
    cv::cvtColor(dst,ret,CV_BGR2GRAY);
    cv::minMaxLoc(ret,&nmin,&nmax);
    cv::Mat mSub = ret-bkphrone;
    cv::minMaxLoc(mSub,&nmin,&nmax);

}

void TreePlot( vector< cv::Vec4i > &hierarchy, vector< vector< cv::Point > > &contours, cv::Mat &lines )
{
    CBinaryTreeNode<CInt,int>* lnode;
    CBinaryTreeNode<CInt,int>* rnode;
    CBinaryTreeNode<CInt,int>* node;
    m_BST.RemoveAll();
    m_BST.Insert(0)->Data=0;
    lnode=m_BST.Root;
    rnode=m_BST.Root;
    node=m_BST.Root;
    for (int k=0; k<hierarchy.size(); k++)
    {
        if (hierarchy[k][0]!=-1)
        {
            lnode=m_BST.InsertLeft(hierarchy[k][0],node);
            lnode->Data=hierarchy[k][0];
            node=lnode;
        }
        if (hierarchy[k][2]!=-1)
        {
            rnode=m_BST.InsertRight(hierarchy[k][2],rnode);
            rnode->Data=hierarchy[k][2];
        }
        else
            rnode=node;
    }
    node=m_BST.Root;
    node = m_BST.Min(node);
    for (int k=0; k<hierarchy.size()-1; k++)
    {
        int nddata;
        node = m_BST.Successor(node);
        nddata=node->Data;

        CString str;
        str.Format("%d\n",nddata);
        TRACE0(str);

        for (int pt=0; pt<contours[nddata].size(); pt++)
        {
            cv::Point ppos = contours[nddata][pt];
            lines.ptr<char>(ppos.y)[ppos.x] = 255; 
        }

        //imshow("test",lines);
        //cv::waitKey();
    }
}


void ReadM2File( CString strPath ) 
{
    CStdioFile file(strPath ,CFile::modeRead);
    CString strRead, strTail;
    char* bufTail;
    while(1)
    {

        if (!file.ReadString(strRead) )
        {
            break;
        }
        if (strRead == "M1")
        {
            m_ListAllPoint.push_back(MARK_M1);
        }
        else if (strRead == "DWE0")
        {
            m_ListAllPoint.push_back(MARK_DWE);
        }
        else if (strRead == "M2")
        {
            m_ListAllPoint.push_back(MARK_M2);
        }
        else
        {
            cv::Point2f oPt;
            strRead.Remove(' ');
            strRead = strRead.Right(strRead.GetLength()-3);
            oPt.x = strtod(strRead.GetBuffer(strRead.GetLength()),&bufTail);
            strTail.Format("%s", bufTail);
            strTail = strTail.Right(strTail.GetLength()-1);
            oPt.y = strtod(strTail.GetBuffer(strTail.GetLength()),NULL);
            m_ListAllPoint.push_back(oPt);
        }
    }
}
