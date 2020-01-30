#ifndef __HAARDETECTING_H_
#define __HAARDETECTING_H_

#include "StdAfx.h"
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cassert>

using namespace std;
#include "utils.h"

#define HAAR_FEATURE_MAX  3
////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////      原始特征和分类器      //////////////////////////
////////////////////////////////////////////////////////////////////////////////////

typedef struct R_Feature
{
	int i;
	int x;
	int y;
	int w;
	int h;
}R_Feature;

typedef struct HaarFeature
{
	int count;
	int type;

	struct
	{
		MyRect r;
		int weight;
	}rect[HAAR_FEATURE_MAX];  //haar feature parameters

}HaarFeature;

typedef struct HaarClassifier
{
	HaarFeature haarfeature; //the haar feature 
	
	float   threshold;             //the threshold of the weak classifier
	int   parity;                //the parity of the weak classifier
	float alpha;                 //the coefficient of the weak classifier

}HaarClassifier;

typedef struct StageClassifier
{
	HaarClassifier* classifier; //the weak classifiers
	int count;       //the number of weakclassifiers included in the stage
	float threshold; //the threshold of the stage classifier

}StageClassifier;

typedef struct ClassifierCascade
{
	int count; //number of stages
	MySize origWindowSize;
	StageClassifier* stageClassifier; //the stage classifiers

}ClassifierCascade;
//拷贝特征
inline void CopyHaarFeature(HaarFeature& src_hf, HaarFeature& dst_hf);
inline void CopyHaarFeature(HaarFeature& src_hf, HaarFeature& dst_hf)
{
	dst_hf.type=src_hf.type;
	for(int i = 0; i < HAAR_FEATURE_MAX; i++)
	{
		dst_hf.rect[i].weight = src_hf.rect[i].weight;
		dst_hf.rect[i].r.x = src_hf.rect[i].r.x;
		dst_hf.rect[i].r.y = src_hf.rect[i].r.y;
		dst_hf.rect[i].r.width = src_hf.rect[i].r.width;
		dst_hf.rect[i].r.height = src_hf.rect[i].r.height;
	}

}


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////      检测用的特征和分类器      //////////////////////////
////////////////////////////////////////////////////////////////////////////////////

typedef struct POS
{
	int x;
	int y;

}POS;

typedef struct DetHaarFeature
{
	struct
	{
		int *p0, *p1, *p2, *p3;
		float weight;
	}
	rect[HAAR_FEATURE_MAX];
}
DetHaarFeature;

typedef struct DetHaarClassifier
{
	HaarFeature origFeature;
	DetHaarFeature  feature;
	float threshold;
	int parity;
	float alpha;
}
DetHaarClassifier;

typedef struct DetStageClassifier
{
	int count;
	float threshold;
	DetHaarClassifier* classifier;
	float alpha0;
	float threshold0;
	int parity0;
}
DetStageClassifier;

typedef struct DetCascadeClassifier
{
	int count;
	MySize origWindowSize;
	MySize realWindowSize;
	double scale, invWindowArea;
	MySize imgSize;
	DetStageClassifier* stageClassifier;
//	float weight_scale;  //为检测扩大的窗口时将计算出的特征值与阀值乘以weight_scale 作比较
	
	int *p0, *p1, *p2, *p3;
	double *ps0,*ps1,*ps2,*ps3;
}
DetCascadeClassifier;

typedef int (* ComFunc)(const void* a, const void* b);

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////  图像处理函数  ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
BYTE* ReadBmpFile(const char* pszFileName, int& nImgWidth, int& nImgHeight);
BOOL IntegralImage(BYTE* pbyteSrcImg, int* sum, double* sqsum, int nwidth, int nheight);
BOOL Rgb2Gray(BYTE* pbyteSrcImg, BYTE* pbyteDstImg, int nwidth, int nheight);
void WriteBmpFile(const char* pszFileName, BYTE* pbyteSrcImg, int nWidth, int nHeight);
BYTE* CropImage(BYTE* pbyteSrcImg, MyRect rect);

////////////////////////////////////////////////////////////////////////////////////////

DetCascadeClassifier*
CreateDetCascadeClassifier( ClassifierCascade* cascade, 
						    int nImgWidth, int nImgHeight,
						  
							double scale=1.0F );

void SetImgForDetCascadeClassifier( DetCascadeClassifier* cascade,
								    int* SAT, int* RSAT,double SSAT[],
									int nImgWidth, int nImgHeight,
									double scale );

int RunDetCascadeClassifier( DetCascadeClassifier* cascade,int SAT[],int RSAT[],double SSAT[]
							 ,POS pt );
							 

void HaarDetectFaces( BYTE* pbyteSrcImg, 
					  int iw, int ih,
					  MyRect detRegion,
					  DetCascadeClassifier* cascade,
					  vector<MyRect>* vecFaces,
					  MySize min_size,
					  MySize max_size,
					  int min_neighbors,
					  double scale_factor = 1.2F );

int RectsPartition(vector<MyRect>* vecFaces, vector<int>* labels, ComFunc is_equal);

ClassifierCascade* LoadClassifierCascade(const char* directory, int count, MySize origWindowSize);
BOOL LoadStageClassifier(const char* filename, StageClassifier* stageClassifier);
void ReleaseClassifierCascade(ClassifierCascade* cascade);
void ReleaseDetCascadeClassifier(DetCascadeClassifier* cascade);




//////////////////////////////////////////////////////////////////////////

inline void CopyRectFeature(int x,int y,int w,int h,MyRect& rec)
{
	rec.x=x;
	rec.y=y;
	rec.height=h;
	rec.width=w;
	
}

inline void Change_f_0(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=0;
    haarfeature.count=2;
    CopyRectFeature(x,y,2*w,h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x,y,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}

inline void Change_f_1(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=1;
    haarfeature.count=2;
    CopyRectFeature(x,y,w,2*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x,y,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
inline void Change_f_2(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=2;
    haarfeature.count=2;
    CopyRectFeature(x,y,3*w,h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x-w,y,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=3;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}

inline void Change_f_3(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=3;
    haarfeature.count=2;
    CopyRectFeature(x,y,4*w,h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x-w,y,2*w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;
}
inline void Change_f_4(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=4;
    haarfeature.count=2;
    CopyRectFeature(x,y,w,3*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x,y-h,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=3;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
inline void Change_f_5(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=5;
    haarfeature.count=2;
    CopyRectFeature(x,y,w,4*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x,y-h,w,2*h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
/////////////////////////////
inline void Change_f_6(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=6;
    haarfeature.count=2;
    CopyRectFeature(x,y,3*w,3*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x-w,y-h,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=9;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
inline void Change_f_7(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=7;
    haarfeature.count=3;
    CopyRectFeature(x,y,2*w,2*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x-w,y,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(x,y-h,w,h,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=2;

}
inline void Change_f_8(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=8;
    haarfeature.count=2;
    CopyRectFeature(x,y,w,2*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x,y,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}

inline void Change_f_9(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=9;
    haarfeature.count=2;
    CopyRectFeature(x,y,w,3*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x-h,y-h,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=3;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}

inline void Change_f_10(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=10;
    haarfeature.count=2;
    CopyRectFeature(x,y,w,4*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x-h,y-h,w,2*h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
inline void Change_f_11(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=11;
    haarfeature.count=2;
    CopyRectFeature(x,y,2*w,h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x+w,y-w,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}

inline void Change_f_12(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=12;
    haarfeature.count=2;
    CopyRectFeature(x,y,3*w,h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x+w,y-w,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=3;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
inline void Change_f_13(int x,int y,int w,int h,HaarFeature& haarfeature)
{
   haarfeature.type=13;
    haarfeature.count=2;
    CopyRectFeature(x,y,4*w,h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x+w,y-w,2*w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=2;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}

inline void Change_f_14(int x,int y,int w,int h,HaarFeature& haarfeature)
{
	haarfeature.type=14;
    haarfeature.count=2;
    CopyRectFeature(x,y,3*w,3*h,haarfeature.rect[0].r);
	haarfeature.rect[0].weight=-1;
	CopyRectFeature(x+w-h,y-w-h,w,h,haarfeature.rect[1].r);
	haarfeature.rect[1].weight=9;
	CopyRectFeature(0,0,0,0,haarfeature.rect[2].r);
	haarfeature.rect[2].weight=0;

}
inline void change_R_Feature2HaarFeature(R_Feature r,HaarFeature& haarfeature)
{
	switch(r.i)
	  {
	  case 0: Change_f_0(r.x,r.y,r.w,r.h, haarfeature);break;
      case 1: Change_f_1(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 2:  Change_f_2(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 3:  Change_f_3(r.x,r.y,r.w,r.h, haarfeature);break;
      case 4:  Change_f_4(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 5:  Change_f_5(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 6:  Change_f_6(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 7:  Change_f_7(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 8:  Change_f_8(r.x,r.y,r.w,r.h, haarfeature);break;
      case 9:  Change_f_9(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 10:  Change_f_10(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 11:  Change_f_11(r.x,r.y,r.w,r.h, haarfeature);break;
      case 12:  Change_f_12(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 13:  Change_f_13(r.x,r.y,r.w,r.h, haarfeature);break;
	  case 14:  Change_f_14(r.x,r.y,r.w,r.h, haarfeature);break;	  
	  }
	


}


#endif