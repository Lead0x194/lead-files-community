//  Project     : general base class
//  Compiler    : Visual C++ 5.0 / 6.0
//  Plattform   : Windows 95/98, Windows NT 3.51/4.0/2000
//  File        : ST_SplitterWnd.cpp 
//  Programmer  : dz, SoftToys
//  Copyright   : 2001 SoftToys
//  Contact     : info@softtoys.com
//  Description : base class for handling splitter windows 
//  History     : 02.Sept. 2001 Version 1.0
//


#include "stdafx.h"
#include "ST_SplitterWnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ST_SplitterWnd::ST_SplitterWnd(int nLevel/* = 0*/) : m_nHiddenCol(-1),m_nHiddenRow(-1),
                                                         m_sRegKey(_T("SplitterWnd")) , m_nLevel(nLevel)
{
	m_nPaneSize[0]    = 100;
	m_nPaneSize[1]    = 100;
	m_nPaneMinSize[0] = 10;
	m_nPaneMinSize[1] = 10;
   m_pSubSplitterWnd[0] = NULL;
   m_pSubSplitterWnd[1] = NULL;
	m_nCurrentView[0] = 0;
	m_nCurrentView[1] = 0;
}

ST_SplitterWnd::~ST_SplitterWnd()
{
   SaveToRegistry();
}

bool ST_SplitterWnd::Create(CWnd* pParentWnd, CRuntimeClass* pView1, CRuntimeClass* pView2,
									  CCreateContext* pContext, bool bVertical,int nID)
{
	int nRow, nCol;
   m_bVertical = bVertical;
	if (bVertical) {
		nRow = 1;
		nCol = 2;
	}
	else {
		nRow = 2;
		nCol = 1;
	}
	VERIFY(CreateStatic(pParentWnd,nRow,nCol,WS_CHILD|WS_VISIBLE|WS_BORDER,nID));
   if (pView1 != NULL) {
      VERIFY(CreateView(0,0,pView1,CSize(100,100),pContext));
   }
   if (pView2 != NULL) {
	   if (bVertical) {
		   VERIFY(CreateView(0,1,pView2,CSize(100,100),pContext));
	   }
	   else {
		   VERIFY(CreateView(1,0,pView2,CSize(100,100),pContext));
	   }
   }
	return(true);
}

void ST_SplitterWnd::SetInitialStatus()
{
	int c,r;
   CString sSubKey;
   sSubKey.Format(_T("Panes_%d"),m_nLevel);
   CString s;
	s = AfxGetApp()->GetProfileString(m_sRegKey,sSubKey,_T("-1 150 -1 150"));
	   sscanf(s,_T("%d %d %d %d"),&c,&m_nPaneSize[0],&r,&m_nPaneSize[1]);

   if (IsSplittverticaly()) {
		SetColumnInfo(0,m_nPaneSize[0],m_nPaneMinSize[0]);
		SetColumnInfo(1,m_nPaneSize[1],m_nPaneMinSize[1]);
		if (c > -1) ToggleSide(c); 
      if (c == -1)  RecalcLayout();
   }
	else {
		SetRowInfo(0,m_nPaneSize[0],m_nPaneMinSize[0]);
		SetRowInfo(1,m_nPaneSize[1],m_nPaneMinSize[1]);
		if (r > -1) ToggleSide(r); 
      if (r == -1) RecalcLayout();
	}
   if (m_pSubSplitterWnd[0] != NULL) m_pSubSplitterWnd[0]->SetInitialStatus();
   if (m_pSubSplitterWnd[1] != NULL) m_pSubSplitterWnd[1]->SetInitialStatus();
}



void ST_SplitterWnd::SaveToRegistry()
{
   CString s;
   CString sSubKey;
   sSubKey.Format(_T("Panes_%d"),m_nLevel);

   RememberSize();
   if (m_nPaneSize[0] == -1) m_nPaneSize[0] = 100;
   if (m_nPaneSize[1] == -1) m_nPaneSize[1] = 100;

   s.Format(_T("%d %d %d %d"),m_nHiddenCol,m_nPaneSize[0],m_nHiddenRow,m_nPaneSize[1]);
   AfxGetApp()->WriteProfileString(m_sRegKey,sSubKey,s);

   if (m_pSubSplitterWnd[0] != NULL) m_pSubSplitterWnd[0]->SaveToRegistry();
   if (m_pSubSplitterWnd[1] != NULL) m_pSubSplitterWnd[1]->SaveToRegistry();
}

bool ST_SplitterWnd::IsSideHidden(int nSide /* = LEFT_SIDE */)
{
   if (IsSplittverticaly()) {
      if (m_nHiddenCol == nSide) {
         return(true);
      }
   }
   else {
      if (m_nHiddenRow == nSide) {
         return(true);
      }
   }
   return(false);
}


void ST_SplitterWnd::ShowColumn()
{
     ASSERT_VALID(this);
     ASSERT(m_nCols < m_nMaxCols);
     ASSERT(m_nHiddenCol != -1);

     int colNew = m_nHiddenCol;
     m_nHiddenCol = -1;
     m_nCols++;  // add a column
     ASSERT(m_nCols == m_nMaxCols);

    // fill the hidden column
     int col;
     for (int row = 0; row < m_nRows; row++)
     {
          CWnd* pPaneShow = GetDlgItem(
               AFX_IDW_PANE_FIRST + row * 16 + m_nCols);
          ASSERT(pPaneShow != NULL);
          pPaneShow->ShowWindow(SW_SHOWNA);

          for (col = m_nCols - 2; col >= colNew; col--)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row, col + 1));
          }

         pPaneShow->SetDlgCtrlID(IdFromRowCol(row, colNew));
     }
     // new panes have been created -- recalculate layout
     RecalcLayout();
}

void ST_SplitterWnd::HideColumn(int colHide)
{
     ASSERT_VALID(this);
     ASSERT(m_nCols > 1);
     ASSERT(colHide < m_nCols);
     ASSERT(m_nHiddenCol == -1);
     if (m_nHiddenCol != -1) return;

     RememberSize();

     m_nHiddenCol = colHide;

    // if the column has an active window -- change it
     int rowActive, colActive;
     if (GetActivePane(&rowActive, &colActive) != NULL &&
         colActive == colHide)
     {
          if (++colActive >= m_nCols)
              colActive = 0;
          SetActivePane(rowActive, colActive);
     }

    // hide all column panes
     for (int row = 0; row < m_nRows; row++)
     {
          CWnd* pPaneHide = GetPane(row, colHide);
          ASSERT(pPaneHide != NULL);
          pPaneHide->ShowWindow(SW_HIDE);
          pPaneHide->SetDlgCtrlID(
               AFX_IDW_PANE_FIRST + row * 16 + m_nCols);

          for (int col = colHide + 1; col < m_nCols; col++)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row, col - 1));
          }
     }
     m_nCols--;
     m_pColInfo[m_nCols].nCurSize = m_pColInfo[colHide].nCurSize;
     RecalcLayout();
}


void ST_SplitterWnd::ShowRow()
{
     ASSERT_VALID(this);
     ASSERT(m_nRows < m_nMaxRows);
     ASSERT(m_nHiddenRow != -1);

     int rowNew = m_nHiddenRow;
     m_nHiddenRow = -1;
     m_nRows++;  // add a row
     ASSERT(m_nRows == m_nMaxRows);

    // fill the hidden row
     int row;
     for (int col = 0; col < m_nCols; col++)
     {
          CWnd* pPaneShow = GetDlgItem(
               AFX_IDW_PANE_FIRST + m_nRows * 16 + col);
          ASSERT(pPaneShow != NULL);
          pPaneShow->ShowWindow(SW_SHOWNA);

          for (row = m_nRows - 2; row >= rowNew; row--)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row + 1, col));
          }

         pPaneShow->SetDlgCtrlID(IdFromRowCol(rowNew, col));
     }

    // new panes have been created -- recalculate layout
     RecalcLayout();
}

void ST_SplitterWnd::HideRow(int rowHide)
{
     ASSERT_VALID(this);
     ASSERT(m_nRows > 1);
     ASSERT(rowHide < m_nRows);
     ASSERT(m_nHiddenRow == -1);
     if (m_nHiddenRow != -1) return;

     RememberSize();

     m_nHiddenRow = rowHide;

    // if the column has an active window -- change it
     int rowActive, colActive;
     if (GetActivePane(&rowActive, &colActive) != NULL &&
         rowActive == rowHide)
     {
          if (++rowActive >= m_nRows)
              rowActive = 0;
          SetActivePane(rowActive, colActive);
     }

    // hide all row panes
     for (int col = 0; col < m_nCols; col++)
     {
          CWnd* pPaneHide = GetPane(rowHide, col);
          ASSERT(pPaneHide != NULL);
          pPaneHide->ShowWindow(SW_HIDE);
          pPaneHide->SetDlgCtrlID(
               AFX_IDW_PANE_FIRST + m_nRows * 16);

          for (int row = rowHide + 1; row < m_nRows; row++)
          {
               CWnd* pPane = GetPane(row, col);
               ASSERT(pPane != NULL);
               pPane->SetDlgCtrlID(IdFromRowCol(row - 1, col));
          }
     }
     m_nRows--;
     m_pRowInfo[m_nRows].nCurSize = m_pRowInfo[rowHide].nCurSize;
     
     RecalcLayout();
}


void ST_SplitterWnd::ToggleSide(int rc)
{
   if (IsSplittverticaly()) {
      if (m_nHiddenCol == -1) {
         // can only hide this row, if the other row in not hidden
         HideColumn(rc);
      }
      else if (m_nHiddenCol == rc) {
         // show this row, only if this row is hidden
   	   ShowColumn();
      }
   }
   else {
      if (m_nHiddenRow == -1) {
         // can only hide this column, if the other colum in not hidden
         HideRow(rc);
      }
      else if (m_nHiddenRow == rc) {
         // show this column, only if this column is hidden
   	   ShowRow();
      }
   }
}

void ST_SplitterWnd::RememberSize()
{
   if (m_pSubSplitterWnd[0] != NULL) m_pSubSplitterWnd[0]->RememberSize();
   if (m_pSubSplitterWnd[1] != NULL) m_pSubSplitterWnd[1]->RememberSize();

   if (IsSplittverticaly()) {
      if (m_nHiddenCol == -1) { // if not hidden
		   GetColumnInfo(0,m_nPaneSize[0],m_nPaneMinSize[0]);
		   GetColumnInfo(1,m_nPaneSize[1],m_nPaneMinSize[1]);
      }
	}
	else {
      if (m_nHiddenRow == -1) { // if not hidden
		   GetRowInfo(0,m_nPaneSize[0],m_nPaneMinSize[0]);
		   GetRowInfo(1,m_nPaneSize[1],m_nPaneMinSize[1]);
      }
	}
}

ST_SplitterWnd* ST_SplitterWnd::AddSubDivision(int nSide,
                                CRuntimeClass* pView1,CRuntimeClass* pView2,CCreateContext* pContext,bool bVertical)
{
   ASSERT((nSide == 0) || (nSide == 1));
   ASSERT(m_pSubSplitterWnd[nSide] == NULL);

	int nRow, nCol;
	SideToRowCol(nSide,&nRow,&nCol);

   int nID = IdFromRowCol(nRow,nCol);
   m_pSubSplitterWnd[nSide] = new  ST_SplitterWnd(m_nLevel+1);
   m_pSubSplitterWnd[nSide]->Create(this,pView1,pView2,pContext,bVertical,nID);
   return(m_pSubSplitterWnd[nSide]);
}

bool ST_SplitterWnd::HideView(int nRow,int nCol)
{
	CWnd* pWnd = GetPane(nRow,nCol);
	if (!pWnd) return(false);
	pWnd->SetDlgCtrlID(0);
	pWnd->ShowWindow(SW_HIDE);
	return(true);
}


bool ST_SplitterWnd::ShowView(int nRow,int nCol,CWnd* pWnd)
{
	pWnd->SetDlgCtrlID(IdFromRowCol(nRow, nCol));
	pWnd->ShowWindow(SW_SHOW);
	return(true);
}


int ST_SplitterWnd::AddView(int nSide, CRuntimeClass * pViewClass, CCreateContext* pContext)
{
	int nRow, nCol;
	SideToRowCol(nSide,&nRow,&nCol);

   // hide the current view of the pane if there is a view attached already
	if (GetDlgItem(IdFromRowCol(nRow, nCol))) {	
		HideView(nRow, nCol);
	}

	// create the new view, if fail, set the previous view current 
	if (CreateView(nRow, nCol, pViewClass, CSize(10,10), pContext) == 0) return -1;

	// get and store the new view
	CWnd* pWnd = GetPane(nRow, nCol);
	m_views[nSide].push_back(pWnd);
	m_nCurrentView[nSide] = m_views[nSide].size() - 1;

	ShowView(nRow, nCol,pWnd);

	RedrawWindow();
	return(m_nCurrentView[nSide]);

}


void ST_SplitterWnd::SwitchToView(int nSide, int nViewIX /* = -1 */)
{
	// if the View is -1 then just use the next view...
	if (nViewIX == -1)
	{
		nViewIX = m_nCurrentView[nSide] + 1;

		if (nViewIX >= (signed) m_views[nSide].size())
			nViewIX = 0; // rollover to first view
	}
	
	CWnd* pWnd = m_views[nSide][nViewIX];

	int nRow, nCol;

	if (IsSideHidden(LEFT_SIDE)) {
		nRow = 0;
		nCol = 0;
	}
	else {
		SideToRowCol(nSide,&nRow,&nCol);
	}
	HideView(nRow, nCol);
	ShowView(nRow, nCol, pWnd);

	m_nCurrentView[nSide] = nViewIX;
	RecalcLayout();
	RedrawWindow();
}

