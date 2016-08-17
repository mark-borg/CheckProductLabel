// CVWindow.cpp : implementation file
//

#include "stdafx.h"
#include "checklabel.h"
#include "CVWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCVWindow

CCVWindow::CCVWindow()
{
}

CCVWindow::~CCVWindow()
{
}


BEGIN_MESSAGE_MAP(CCVWindow, CStatic)
	//{{AFX_MSG_MAP(CCVWindow)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCVWindow message handlers

void CCVWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
    CV_FUNCNAME( "cvShowImage" );

    __BEGIN__;

    CvWindow* window;
    SIZE size = { 0, 0 };
    int channels = 0;
    void* dst_ptr = 0;
    const int channels0 = 3;
    int origin = 0;
    CvMat stub, dst, *image;

    if( !name )
        CV_ERROR( CV_StsNullPtr, "NULL name" );

    window = icvFindWindowByName(name);
    if( !window || !arr )
        EXIT; // keep silence here.

    if( CV_IS_IMAGE_HDR( arr ))
        origin = ((IplImage*)arr)->origin;

    CV_CALL( image = cvGetMat( arr, &stub ));

    if( window->image )
        icvGetBitmapData( window, &size, &channels, &dst_ptr );

    if( size.cx != image->width || size.cy != image->height || channels != channels0 )
    {
        uchar buffer[sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD)];
        BITMAPINFO* binfo = (BITMAPINFO*)buffer;

        DeleteObject( SelectObject( window->dc, window->image ));
        window->image = 0;

        size.cx = image->width;
        size.cy = image->height;
        channels = channels0;

        FillBitmapInfo( binfo, size.cx, size.cy, channels*8, 1 );

        window->image = SelectObject( window->dc, CreateDIBSection(window->dc, binfo,
                                      DIB_RGB_COLORS, &dst_ptr, 0, 0));
    }

    cvInitMatHeader( &dst, size.cy, size.cx, CV_8UC3,
                     dst_ptr, (size.cx * channels + 3) & -4 );
    cvConvertImage( image, &dst, origin == 0 ? CV_CVTIMG_FLIP : 0 );

    icvUpdateWindowPos(window);
    InvalidateRect(window->hwnd, 0, 0);
    UpdateWindow(window->hwnd);
}
