//.h

class CTransparentImage : public CStatic
{
public:
   CTransparentImage() : m_bLoaded(false), m_width(0), m_height(0) {};
   virtual ~CTransparentImage() {};
   void loadImage();
   void SetIcon(HICON hIcon, int width, int height);
protected:
	//{{AFX_MSG(CTransparentImage)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
   CImageList m_imgList;
   bool       m_bLoaded;
   int        m_width;
   int        m_height;
};
