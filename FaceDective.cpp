// CamTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

// int APIENTRY WinMain(HINSTANCE hInstance,
//                      HINSTANCE hPrevInstance,
//                      LPSTR     lpCmdLine,
//                      int       nCmdShow)
// {
//  	// TODO: Place code here.
// 
// 	return 0;
// }

#include <AFX.h>
#include "StdAfx.h"
#include <VFW.H>
#include <stdio.h>
#include "HaarDetecting.h"
#pragma comment(lib,"VFW32.LIB")
 
HWND AVIhWnd = 0;
HWND hWnd = 0;
HDC hDC = 0;
HDC ScenehDC = 0;
DWORD AVIWidth = 640;
DWORD AVIHeight = 480;
//DWORD DCColorBits = 32;
BYTE BmpByte[1920*480];
BYTE GYBmyByte[640*480];
 
BITMAPINFO DCitmap;
HBITMAP SceneBitmap;
HGDIOBJ AboriginaalObject;
LPCSTR szClassName;
LPCSTR szWindowTitle;
 
long X;
long Y;
long OutWidth;
long InputWidth;
long cR;
long cG;
long cB;
 
static LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK FrameCallbackProc(HWND, LPVIDEOHDR);
 
 
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    szClassName = "myClass";
    szWindowTitle = "MyWindow";
 
    WNDCLASS  wc;
    wc.style = CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DSHADOW);
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = szClassName;
    RegisterClass(&wc);
 
    hWnd = CreateWindow(szClassName,
                        szWindowTitle,
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        AVIWidth,
                        AVIHeight,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);
 
    if (!hWnd){
        MessageBox(0,TEXT("创建窗口失败！\r\n"),"", MB_OK); 
        return (FALSE);
    }
    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
 
    ScenehDC = CreateCompatibleDC(hDC);
    ZeroMemory(&DCitmap.bmiHeader, sizeof(BITMAPINFOHEADER));
    DCitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    //DCitmap.bmiHeader.biBitCount = 24;
	DCitmap.bmiHeader.biBitCount = 8;
    DCitmap.bmiHeader.biWidth = AVIWidth;
    DCitmap.bmiHeader.biHeight = AVIHeight;
    DCitmap.bmiHeader.biPlanes = 1;
    DCitmap.bmiHeader.biClrUsed = 0;
    DCitmap.bmiHeader.biClrImportant = 0;
    DCitmap.bmiHeader.biSizeImage = AVIWidth * AVIHeight * (DCitmap.bmiHeader.biBitCount / 8);
	////
	RGBQUAD *ipRGB2 = (RGBQUAD *)malloc(256*sizeof(RGBQUAD));
	for ( int i = 0; i < 256; i++ )
	ipRGB2[i].rgbRed = ipRGB2[i].rgbGreen = ipRGB2[i].rgbBlue = i;
    memcpy(DCitmap.bmiColors,ipRGB2, 256*sizeof(RGBQUAD));
	free(ipRGB2);
	///
    SceneBitmap = CreateDIBSection(ScenehDC, &DCitmap, DIB_RGB_COLORS, NULL ,0 , 0);
 
    AboriginaalObject = SelectObject(ScenehDC, (HGDIOBJ)SceneBitmap);
 
    OutWidth = AVIWidth * 3;
    InputWidth = AVIWidth * 3;
 
 
    int rd;
//    BITMAPINFOHEADER bmpFormat;
    AVIhWnd = capCreateCaptureWindow((LPSTR)"视频追踪", 
                                    WS_CHILD  | WS_VISIBLE, 
                                    0, 0, AVIWidth, AVIHeight, 
                                    hWnd, 
                                    1);
    if (AVIhWnd==0)
    {
        MessageBox(hWnd, "创建AVI窗口失败","提示",64);
    }
	else
	{
        hDC = GetWindowDC(hWnd);
 
        capSetCallbackOnFrame(AVIhWnd,(FARPROC)FrameCallbackProc);
        rd = capDriverConnect(AVIhWnd,0);
 
        if(rd != 0)
		{
            CAPDRIVERCAPS *Caps;
            capDriverGetCaps(AVIhWnd,sizeof(CAPDRIVERCAPS),&Caps);
            capPreviewRate(AVIhWnd, 20);
        }
		else
		{
            MessageBox(hWnd, "不能打开设备驱动。","提示",64);
        }
    }
 
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
 
    SelectObject(ScenehDC, AboriginaalObject);
    DeleteObject((HGDIOBJ)SceneBitmap);
    DeleteDC(ScenehDC);
    return 0;
}
 
LRESULT CALLBACK MainWndProc(HWND hWnd,UINT Message,WPARAM wParam,LPARAM lParam)
{
    switch (Message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return (DefWindowProc(hWnd, Message, wParam, lParam));
    }
    return 0;
}
 
LRESULT CALLBACK FrameCallbackProc(HWND ghWnd,LPVIDEOHDR lpVData)
{
    for(Y=1; Y<=AVIHeight; Y++)
    {
        for(X=1; X<=AVIWidth; X++)
        {
            cB = lpVData->lpData[((AVIHeight-Y)*InputWidth)+(X*3)];
            cG = lpVData->lpData[((AVIHeight-Y)*InputWidth)+(X*3)+1];
            cR = lpVData->lpData[((AVIHeight-Y)*InputWidth)+(X*3)+2];
            BmpByte[((Y-1)*OutWidth)+(X*3)]=cB;
            BmpByte[((Y-1)*OutWidth)+(X*3)+1]=cG;
            BmpByte[((Y-1)*OutWidth)+(X*3)+2]=cR;
			////////////
			GYBmyByte[(Y-1)*AVIWidth+X]=(30*cG+59*cR+11*cB)/100;
        }
    }

	MySize origWindowSize;
	origWindowSize.height=20;
	origWindowSize.width=20;
	const int maxstep=14;
	const char* directory="F:\\hx\\VC60_WorkPlace\\VCProject\\CamDataProcess\\CamTest\\DATA_1\\";
	ClassifierCascade* cascade=NULL;
	DetCascadeClassifier* detcascade=NULL;
    int nImgWidth=640,nImgHeight=480;

	cascade=LoadClassifierCascade(directory, maxstep, origWindowSize);
	if (cascade==NULL)
	{
		cout<<"can not load the data for ClassifierCascade\n";
	}
	
    MyRect detRegion;	
    vector<MyRect>* vecFaces=new vector<MyRect>;
	MySize min_size,max_size;	
	int min_neighbors=2;
	double scale_factor=1.1F;	
//    int liite_face_count=0;
    //string ww=".bmp",bb;
    int c=0;
    int x,y,w,h;
/*	int khj=0;*/
//	BYTE* pb=NULL;
    int temp_nImgWidth=0,temp_nImgHeight=0;
	if ((temp_nImgHeight!=nImgHeight)||(temp_nImgWidth!=nImgWidth))
    {

		detcascade=CreateDetCascadeClassifier(cascade,nImgWidth,nImgHeight);//创建Adaboost级联分类器
		detRegion.x=0;detRegion.y=0;detRegion.height=nImgHeight-1; detRegion.width=nImgWidth-1;
		min_size.width=20; min_size.height=20; max_size.width=(nImgWidth*4)/5; max_size.height=(nImgHeight*4)/5;
		if (max_size.width>max_size.height)
		{
			max_size.width=max_size.height;
		}
		else
			max_size.height=max_size.width;
		temp_nImgHeight=nImgHeight;
		temp_nImgWidth=nImgWidth;
    }

	HaarDetectFaces( GYBmyByte, 
		nImgWidth,nImgHeight,
		detRegion,
		detcascade,
		vecFaces,
		min_size,
		max_size,
		min_neighbors,
		scale_factor ); //应用Adaboost级联强分类器进行人脸检测
	
	vector<MyRect>::iterator iter_begin = vecFaces->begin();
	vector<MyRect>::iterator iter_end = vecFaces->end();
	vector<MyRect>::iterator iter_p;
	
	for (iter_p=iter_begin;iter_p<iter_end;iter_p++)
	{
		x=iter_p->x;y=iter_p->y;w=iter_p->width;h=iter_p->height;
		for (int i=0;i<w;i++)
		{
			GYBmyByte[y*nImgWidth+i+x]=255;
			GYBmyByte[(y+h)*nImgWidth+i+x]=255;
		}
		for (int j=0;j<h;j++)
		{
			GYBmyByte[(y+j)*nImgWidth+x]=255;
			GYBmyByte[(y+j)*nImgWidth+x+w-1]=255;
		}
	}

    //SetBitmapBits(SceneBitmap,sizeof(BmpByte),BmpByte);
	SetBitmapBits(SceneBitmap,sizeof(GYBmyByte),GYBmyByte);
    BitBlt(hDC, 0, 0, AVIWidth, AVIHeight, ScenehDC, 0, 0, SRCCOPY);
	ReleaseDetCascadeClassifier(detcascade);
	ReleaseClassifierCascade( cascade);
	free(vecFaces);
//	delete[] vecFaces;
    return 0;
}

