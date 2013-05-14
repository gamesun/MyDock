
#include "stdafx.h"
#include "TransparentImage.h"

// Transparent image static control
BEGIN_MESSAGE_MAP(CTransparentImage, CStatic)
	//{{AFX_MSG_MAP(CTransparentImage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CTransparentImage::SetIcon(HICON hIcon, int width, int height)
{
   m_width = width;
   m_height = height;
   CStatic::SetIcon(hIcon);
}

void CTransparentImage::loadImage()
{
   m_imgList.DeleteImageList();

   HBITMAP hbmp = GetBitmap();
   if (hbmp)
   {
       CBitmap *pBmp = CBitmap::FromHandle(hbmp);
       BITMAP bmpInfo;
       pBmp->GetBitmap(&bmpInfo);

       m_imgList.Create(bmpInfo.bmWidth, bmpInfo.bmHeight, ILC_COLOR32|ILC_MASK, 4, 1);
       m_imgList.Add(pBmp, RGB(0,128,128)); 
   }
   else
   {
      HICON hIcon = GetIcon();
      ASSERT(hIcon && m_width>0 && m_height>0);

      m_imgList.Create(m_width, m_height, ILC_COLOR32|ILC_MASK, 4, 1);
      m_imgList.Add(hIcon);
   }

   m_bLoaded = true;
}
      
void CTransparentImage::OnPaint() 
{
   if (!m_bLoaded)
      loadImage();

	CPaintDC paintDC(this);

   RECT rect;
   GetClientRect(&rect);

   if (GetBitmap())
   {
      paintDC.FillSolidRect(&rect, GetSysColor(COLOR_BTNFACE));
   }

   CPoint pt(rect.left, rect.top);
   m_imgList.Draw((CDC*)&paintDC, 0, pt, ILD_TRANSPARENT);
}
