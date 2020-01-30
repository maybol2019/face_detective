
#include "HaarDetecting.h"
#include "stdafx.h"

#define MAX(a, b) ( (a) > (b) ? (a) : (b) )
#define MIN(a, b) ( (a) < (b) ? (a) : (b) )
///////////////////////////////////////////////////////////////////////////////////////////
BOOL IntegralImage(BYTE* pbyteSrcImg, int SAT[], int RSAT[],double SSAT[],int W,int H)
{
	assert(pbyteSrcImg != NULL);
		unsigned char* pixel=NULL; 
	    unsigned char* p_orignal=NULL; 	 	  
        pixel=pbyteSrcImg;
	   //p_orignal=pixel;	    	
	         for(int j=0;j<W*H;j++)
			 {        	
			 RSAT[j]=*pixel;			  
			 SAT[j]=*(pixel++);
               // RSAT[j]=1;
               //   SAT[j]=1;
              SSAT[j]=(SAT[j]*SAT[j]);//存平方象素值			 
			 }	
		//	 delete []p_orignal;
///////////////////////////////////////////////////////////////////////////									
		int  **const A= new int*[H];
		int temp=0;
		int m,n,m0,n0,i ;
        int a,b,c;   //a is A[m][n-1],b is A[m-1][n],c is A[m-1][n-1] 
        for ( i=0; i<H; i++)
		{
           A[i] = new int[W];
		}
             	 			 
          A[0][0]=SAT[0];         
          for( m=0;m<H;m++)
            for( n=0;n<W;n++)
            {
                if (n==0) a=0;
                else a=A[m][n-1];
                if(m==0) b=0;
                else b=A[m-1][n];
                if((n==0)||(m==0))
                c=0;
                else c=A[m-1][n-1];
                A[m][n]=a+b+SAT[m*W+n]-c ;//A[m][n]=A[m][n-1]+A[m-1][n]+x[m][n]-A[m-1][n-1]
			//	cout<<A[m][n]<<" ";
            }
             temp=0;
            for( m0=0;m0<H;m0++)
              for( n0=0;n0<W;n0++)
			  {
				  SAT[temp]=A[m0][n0];			     
			    temp=temp+1;
			  }
			 		
////////////////////////////以下计算SSAT
		double x1,x2,x3;
		double **const B=new double*[H]; 
		 for ( i=0; i<H; i++)
		 {
             B[i] = new double[W];
		 }
		 
          B[0][0]=SSAT[0];         
          for( m=0;m<H;m++)
            for( n=0;n<W;n++)
            {
                if (n==0) x1=0.0F;
                else x1=B[m][n-1];
                if(m==0) x2=0.0F;
                else x2=B[m-1][n];
                if((n==0)||(m==0))
                 x3=0.0F;
                else x3=B[m-1][n-1];
                B[m][n]=x1+x2+SSAT[m*W+n]-x3 ;//A[m][n]=A[m][n-1]+A[m-1][n]+x[m][n]-A[m-1][n-1]

            }
             temp=0;
            for( m0=0;m0<H;m0++)
              for( n0=0;n0<W;n0++)
			  {
				  SSAT[temp]=B[m0][n0];
				  
			      temp=temp+1;
			  }
		// cout<<"*********** "<<Big_SSAT[i][W*H-1]<<endl;
	
///////////////////////////////////////////
			   ////初始化，给第一行用象素值，第一例用，和最后一例自己先计算好

       int d,e;   //a: RSAT(x-1,y-1) b:RSAT(x+1,y-1) c:RSAT(x,y-2)
                        // d: I(X,Y) e:I(x,y-1)
       for(int k=0;k<W;k++) A[0][k]=RSAT[k];
             
       for( j=0;j<H;j++)
       {
          int temp1=0,temp2=0;
          for(int h=j;h>=0;h--)
            for(int count=0;count<=j-h&&count<W;count++)
            {   temp1+=RSAT[h*W+count];
                temp2+=RSAT[(h+1)*W-count-1];
            }
            
            A[j][0]=temp1;A[j][W-1]=temp2;
       } 
	   
        for(int y=1;y<H;y++)
       for(int x=1;x<W-1;x++)
       {				
		   b=A[y-1][x+1];		
		  a=A[y-1][x-1];
         d=RSAT[y*W+x];
         e=RSAT[(y-1)*W+x];
         if(y==1) c=0;
          else c=A[y-2][x];		
          A[y][x]=a+b-c+d+e;
       }
        temp=0;
            for( m0=0;m0<H;m0++)
			{ for( n0=0;n0<W;n0++)
			  {
				  RSAT[temp]=A[m0][n0];				
			      temp=temp+1;
			  }		
			}

      for (i=0;i<H;i++)
      {
		  delete []B[i];
		  delete []A[i];
      }
	  delete []B;
	  delete []A; 
		return true;

}


BYTE* ReadBmpFile(const char* pszFileName, int& nImgWidth, int& nImgHeight)
{
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	HGLOBAL hImgData;
	BYTE *lpImgData;
	
	int LineBytes =0;
	int ImgSize =0;
	int NumColors = 0;

	FILE* fin = NULL;
	fin = fopen(pszFileName,"rb");
	if(fin == NULL)
	{
		printf("The file is not opened!\n");
		fclose(fin);
		return NULL;
	}

	int nread =0;
	nread = fread(&bf,1,sizeof(BITMAPFILEHEADER),fin);
	if(nread != sizeof(BITMAPFILEHEADER))
	{
		printf("read file header error!\n");
		fclose(fin);
		return NULL;
	}

	nread = fread(&bi,1,sizeof(BITMAPINFOHEADER),fin);
	if(nread != sizeof(BITMAPINFOHEADER))
	{
		printf("read info header error!\n");
		fclose(fin);
		return NULL;
	}
	if(bi.biBitCount != 8)
	{
		printf("the colors of the image is not 256!\n");
		fclose(fin);
		return NULL;
	}

	nImgWidth=bi.biWidth;
	nImgHeight=bi.biHeight;
	LineBytes=(DWORD)WIDTHBYTES(bi.biWidth*bi.biBitCount);
	nImgWidth = LineBytes;
	ImgSize=(DWORD)LineBytes*bi.biHeight;
	BYTE* pbyteSrcImg = NULL;
	pbyteSrcImg = new unsigned char[ImgSize];
	assert(pbyteSrcImg != NULL);
    if(bi.biClrUsed!=0)
	{
		NumColors=(DWORD)bi.biClrUsed;
	}
	else
	{
		if(bi.biBitCount == 8)
			NumColors=256;
		else
		{
			printf("Invalid image format!\n");
			fclose(fin);
			return NULL;
        }
	}

	if(bf.bfOffBits!=(DWORD)(NumColors*sizeof(RGBQUAD)+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)))
	{
		fclose(fin);
		return NULL;
	}

	bf.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+NumColors*sizeof(RGBQUAD)+ImgSize;

	if((hImgData=GlobalAlloc(GHND,(DWORD)ImgSize))==NULL)
	{
	    fclose(fin);
		return NULL;
	}  
	lpImgData = (BYTE*)GlobalLock(hImgData); 
	fseek(fin,sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD),SEEK_SET);
	fread(lpImgData,sizeof(unsigned char),ImgSize,fin);

	int i,j;
	for(i=0;i<nImgHeight;i++)
	{
		for(j=0;j<nImgWidth;j++)
		{
			pbyteSrcImg[i*LineBytes+j] = lpImgData[(nImgHeight - i - 1)*LineBytes + j];
		}
	}

	fclose(fin);
	GlobalUnlock(hImgData);
	GlobalFree(hImgData);

	return pbyteSrcImg; 
}




BOOL Rgb2Gray(BYTE* pbyteSrcImg, BYTE* pbyteDstImg, int nwidth, int nheight)
{
	if( pbyteSrcImg == NULL || pbyteDstImg == NULL )
		return FALSE;

	int nRowBytes = nwidth * 3;
	BYTE blue=0, red=0, green=0;
	BYTE gray;

	int i=0, j=0, pos=0;
	for(j = 0; j < nheight; j++)
	for(i = 0; i < nwidth; i++)
	{
		pos = (nheight-1-j) * nRowBytes + i*3;
		blue = pbyteSrcImg[pos];
		green = pbyteSrcImg[pos+1];
		red = pbyteSrcImg[pos+2];
		gray = (30*red+59*green+11*blue)/100;
		pbyteDstImg[j*nwidth+i] = gray;
	}

	return TRUE;
}


void WriteBmpFile(const char* pszFileName, BYTE* pbyteSrcImg, int nWidth, int nHeight)
{
	BITMAPFILEHEADER bmfHdr;

	int nImgHeight=(nWidth+3)/4*4;
	int imageSize=nImgHeight*nHeight;
	
	BYTE *saveImage=new BYTE[imageSize];
	memset(saveImage,0,imageSize);
	for(int i=0;i<nHeight;i++)
		memcpy(saveImage+i*nImgHeight,pbyteSrcImg+(nHeight-i-1)*nWidth,nWidth);

	bmfHdr.bfType=0x4d42; 
    bmfHdr.bfSize=imageSize+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD); 
    bmfHdr.bfReserved1=0; 
    bmfHdr.bfReserved2=0; 
    bmfHdr.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD); 

	BITMAPINFO *pInfo=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));
	pInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); 
    pInfo->bmiHeader.biWidth=nWidth; 
    pInfo->bmiHeader.biHeight=nHeight; 
    pInfo->bmiHeader.biPlanes=1; 
    pInfo->bmiHeader.biBitCount=8; 
    pInfo->bmiHeader.biCompression=BI_RGB; 
    pInfo->bmiHeader.biSizeImage=imageSize; 
    pInfo->bmiHeader.biXPelsPerMeter=0; 
    pInfo->bmiHeader.biYPelsPerMeter=0; 
    pInfo->bmiHeader.biClrUsed=0; 
    pInfo->bmiHeader.biClrImportant=0; 

  	for(i=0;i<256;i++)
	{
		pInfo->bmiColors[i].rgbBlue=i;
		pInfo->bmiColors[i].rgbGreen=i;
		pInfo->bmiColors[i].rgbRed=i;
		pInfo->bmiColors[i].rgbReserved=0;
	}

	FILE* mFile = NULL;
	//printf("Creat bmpfile : %s\n",pszFileName);
	mFile=fopen(pszFileName,"wb");
	if(mFile == NULL)
	{
		AfxMessageBox(_T("the file is not opened!"));
		return;
	}
	fwrite((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER),1,mFile);
    fwrite(pInfo,sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD),1, mFile);
	fwrite(saveImage,sizeof(BYTE),imageSize,mFile); 
	fclose(mFile);
	
	free(pInfo);
	delete[]saveImage;
}



///////////////////////////////////////////////////////////////////////////////////////////

BOOL LoadStageClassifier(const char* filename, StageClassifier* stageClassifier)
{
	if( NULL == filename || NULL == stageClassifier )
		return FALSE;		
	FILE* file = NULL;
	file = fopen(filename, "r");
	if( !file )
		return FALSE;
	int count = 0;
	float threshold = 0.0F;	
	fscanf( file, "%f", &threshold );
	fscanf( file, "%d", &count );
	if(count > 0)
	{
		stageClassifier->count = count;
		stageClassifier->threshold = threshold;
		stageClassifier->classifier = new HaarClassifier[count];

		for(int i = 0; i < stageClassifier->count; i++)
		{
			HaarClassifier* classifier = stageClassifier->classifier + i;
			//指针偏移
			R_Feature r_feature;
			fscanf( file, "%d %d %d %d %d", &r_feature.i,
				    &r_feature.x,&r_feature.y,
					&r_feature.w,&r_feature.h);
			change_R_Feature2HaarFeature(r_feature,classifier->haarfeature);
			fscanf( file, "%f %d %f", 
				&(classifier->threshold),
				&(classifier->parity),
				&(classifier->alpha));
			
			
		}/* i */
	}

	fclose(file);
	file = NULL;
	return TRUE;
}


ClassifierCascade* LoadClassifierCascade(const char* directory, int maxstep, MySize origWindowSize)
{
	if(maxstep <= 0 || origWindowSize.width <= 0 || origWindowSize.height <= 0 )
	{
		return NULL;
	}

	ClassifierCascade* cascade = NULL;
	if( directory && *directory )
	{
		cascade = new ClassifierCascade;
		if( NULL == cascade )
			return NULL;

		cascade->count = maxstep;        ///层数
		cascade->origWindowSize.width = origWindowSize.width;
		cascade->origWindowSize.height = origWindowSize.height;

		cascade->stageClassifier = NULL;
		cascade->stageClassifier = new StageClassifier[maxstep];//指向层分类器的数组
		if( NULL == cascade->stageClassifier )
		{
			delete cascade;
			cascade = NULL;
			return NULL;
		}
		memset( cascade->stageClassifier, 0, sizeof(StageClassifier)*maxstep);

		CString strFileName = _T("");
		CString suffix = _T("");
		for(int i = 1; i <=maxstep; i++)
		{		
			strFileName = directory;
			strFileName += _T("\\stage");
			suffix.Format("%d.txt", i);
			strFileName += suffix;
			//printf("strFileName is : %s\n",strFileName);
			LoadStageClassifier(strFileName, &(cascade->stageClassifier[i-1]) );
		}
	}//end for if

	return cascade;
}



DetCascadeClassifier*
CreateDetCascadeClassifier( ClassifierCascade* cascade, 
						    int nImgWidth, int nImgHeight,
						    
							double scale )
{
	assert( NULL != cascade );

	DetCascadeClassifier* out = NULL;
	out = new DetCascadeClassifier;
	assert(out != NULL);

	out->count = cascade->count;
	out->origWindowSize.width  = cascade->origWindowSize.width;
	out->origWindowSize.height = cascade->origWindowSize.height;

	out->stageClassifier = NULL;
	out->stageClassifier = new DetStageClassifier[out->count];
	assert(NULL != out->stageClassifier);

	int count = 0;
	int i, j;
	for(i = 0; i < cascade->count; i++)
	{
		count = cascade->stageClassifier[i].count;
		out->stageClassifier[i].count = count;
		out->stageClassifier[i].threshold = cascade->stageClassifier[i].threshold;
		out->stageClassifier[i].classifier = new DetHaarClassifier[count];
		assert(NULL != out->stageClassifier[i].classifier);

		for(j = 0; j < count; j++)
		{
			out->stageClassifier[i].classifier[j].alpha = cascade->stageClassifier[i].classifier[j].alpha;
			out->stageClassifier[i].classifier[j].parity = cascade->stageClassifier[i].classifier[j].parity;
			out->stageClassifier[i].classifier[j].threshold = cascade->stageClassifier[i].classifier[j].threshold;
			CopyHaarFeature(cascade->stageClassifier[i].classifier[j].haarfeature, out->stageClassifier[i].classifier[j].origFeature);
		}
	}	



	return out;

}

void SetImgForDetCascadeClassifier( DetCascadeClassifier* cascade,
								    int* SAT, int* RSAT,double SSAT[],
									int nImgWidth, int nImgHeight,
									double scale )
{
	assert(NULL != cascade);
	assert(NULL != SAT);
	assert(NULL != SSAT);
	assert(NULL!=RSAT);
	assert(nImgWidth > 0 && nImgHeight > 0);
	assert(scale >= 1.0F);

	cascade->scale = scale;
	cascade->realWindowSize.width = Round(cascade->origWindowSize.width * scale);
	cascade->realWindowSize.height = Round(cascade->origWindowSize.height * scale);
	cascade->imgSize.width = nImgWidth;
	cascade->imgSize.height = nImgHeight;
//	cascade->weight_scale=float(scale*scale);
    double weight_scale;
	weight_scale = 1/(scale * scale);
	int step = cascade->imgSize.width;
	MyRect equ_rect;
	equ_rect.x = 0;
	equ_rect.y = 0;
	equ_rect.width = Round(cascade->origWindowSize.width * scale);
	equ_rect.height = Round(cascade->origWindowSize.height * scale);	

	cascade->invWindowArea = 1.0 / (equ_rect.width * equ_rect.height);

	cascade->p0 = SAT+equ_rect.y*step+equ_rect.x;
	cascade->p1 = SAT+equ_rect.y*step+equ_rect.x+equ_rect.width;
	cascade->p2 = SAT+(equ_rect.y+equ_rect.height)*step+equ_rect.x;
	cascade->p3 = SAT+(equ_rect.y+equ_rect.height)*step+equ_rect.x+equ_rect.width;

	cascade->ps0=SSAT+equ_rect.y*step+equ_rect.x;
	cascade->ps1=SSAT+equ_rect.y*step+equ_rect.x+equ_rect.width;
	cascade->ps2= SSAT+(equ_rect.y+equ_rect.height)*step+equ_rect.x;
	cascade->ps3=SSAT+(equ_rect.y+equ_rect.height)*step+equ_rect.x+equ_rect.width;

	int i, j, k;
	for(i = 0; i < cascade->count; i++)
	{
		for(j = 0; j < cascade->stageClassifier[i].count; j++)
		{
			HaarFeature* origFeature = 
				&(cascade->stageClassifier[i].classifier[j].origFeature);
		
			DetHaarFeature* detFeature = 
				&(cascade->stageClassifier[i].classifier[j].feature);
		
			double sum0 = 0.0F, area0 = 0.0F;
			MyRect tr;
		if ((*origFeature).type<8)
		{
		
			
			for(k = 0; k < HAAR_FEATURE_MAX; k++)
			{
				if(origFeature->rect[k].r.width == 0)
				{
					detFeature->rect[k].weight = NULL;
					detFeature->rect[k].p0=NULL;
					detFeature->rect[k].p1 = NULL;
					detFeature->rect[k].p2 = NULL;
					detFeature->rect[k].p3 = NULL;
					break;
				}
				

				tr.x = Round(origFeature->rect[k].r.x * scale);
                tr.y = Round(origFeature->rect[k].r.y * scale);
				tr.width = Round(origFeature->rect[k].r.width * scale);
				
				tr.height = Round(origFeature->rect[k].r.height * scale);

 				detFeature->rect[k].p0 = SAT + tr.y * step + tr.x;
				detFeature->rect[k].p1 = SAT + (tr.y -tr.height)* step + tr.x ;
				detFeature->rect[k].p2 = SAT + tr.y * step + tr.x-tr.width;
				detFeature->rect[k].p3 = SAT + (tr.y -tr.height) * step + tr.x -tr.width;
                detFeature->rect[k].weight = (float)(origFeature->rect[k].weight * weight_scale);
                if(k == 0)
					area0 = tr.width * tr.height;
				else
					sum0 += detFeature->rect[k].weight * tr.width * tr.height;
			}/* k */
        }/*if*/
		else
		{
				assert((*origFeature).type>7);
			for(k = 0; k < HAAR_FEATURE_MAX; k++)
			{
				if(origFeature->rect[k].r.width == 0)
				{
					detFeature->rect[k].weight = NULL;
					detFeature->rect[k].p1 = NULL;
					detFeature->rect[k].p2 = NULL;
					detFeature->rect[k].p3 = NULL;
					break;
				}

				tr.x = Round(origFeature->rect[k].r.x * scale);
				tr.width = Round(origFeature->rect[k].r.width * scale);
				tr.y = Round(origFeature->rect[k].r.y * scale);
				tr.height = Round(origFeature->rect[k].r.height * scale);

				detFeature->rect[k].p0 = RSAT + tr.y * step + tr.x;
				detFeature->rect[k].p1 = RSAT + (tr.y-tr.width) * step + tr.x + tr.width;
				detFeature->rect[k].p2 = RSAT + (tr.y - tr.height) * step + tr.x- tr.height;
				detFeature->rect[k].p3 = RSAT + (tr.y-tr.width-tr.height) * step + tr.x+ tr.width-tr.height;
                detFeature->rect[k].weight = (float)(origFeature->rect[k].weight * weight_scale);
			if(k == 0)
					area0 = tr.width * tr.height;
				else
					sum0 += detFeature->rect[k].weight * tr.width * tr.height;
			
			}
			
		}/* if */
		detFeature->rect[0].weight = (float)(-sum0 / area0);
	}/* j */
}/* i */
}

int RunDetCascadeClassifier( DetCascadeClassifier* cascade,							 
							POS pt )
{
	assert( NULL != cascade );

	int sumImgWidth = cascade->imgSize.width;
	int sumImgHeight = cascade->imgSize.height;
	if( pt.x < 0 || pt.y < 0 ||
		pt.x + cascade->realWindowSize.width >= sumImgWidth - 2 ||
		pt.y + cascade->realWindowSize.height >= sumImgHeight - 2 )
	{
		return 0;
	}

	double mean, norm_factor;
	int p_offset = pt.y * sumImgWidth + pt.x;
	int pq_offset = pt.y * sumImgWidth + pt.x;
	mean = (cascade->p0[p_offset] - cascade->p1[p_offset] -
		    cascade->p2[p_offset] + cascade->p3[p_offset]) * cascade->invWindowArea;
	norm_factor = cascade->ps0[pq_offset] - cascade->ps1[pq_offset] -
		          cascade->ps2[pq_offset] + cascade->ps3[pq_offset];
	norm_factor = 
		sqrt(norm_factor*cascade->invWindowArea - mean*mean);
//	if(norm_factor == 0.0 || mean == 0.0)
	//	return -1;

	float stage_sum = 0.0F;
	int i, j;
	for(i = 0; i < cascade->count; i++)
	{		
		stage_sum = 0.0F;

		for(j = 0; j < cascade->stageClassifier[i].count; j++)
		{
			DetHaarClassifier* classifier = cascade->stageClassifier[i].classifier + j;
			DetHaarFeature* feature = &(classifier->feature);
			double sum = 0.0F;
			float thresh = classifier->threshold * norm_factor;

			sum += (feature->rect[0].p0[p_offset] - 
				    feature->rect[0].p1[p_offset] -
				    feature->rect[0].p2[p_offset] + 
					feature->rect[0].p3[p_offset])* 
   					feature->rect[0].weight;
			    
			sum += (feature->rect[1].p0[p_offset] -
				    feature->rect[1].p1[p_offset] -
				    feature->rect[1].p2[p_offset] + 
					feature->rect[1].p3[p_offset])* 
					feature->rect[1].weight;

			if(feature->rect[2].weight)
				sum += (feature->rect[2].p0[p_offset] - 
				        feature->rect[2].p1[p_offset] - 
						feature->rect[2].p2[p_offset] + 
						feature->rect[2].p3[p_offset])* 
						feature->rect[2].weight;
				         
           //  assert(sum==0);
			if(0.5*sum * classifier->parity < thresh * classifier->parity )
				stage_sum += classifier->alpha;
		
		}

		if(stage_sum < cascade->stageClassifier[i].threshold )
		{
			return -1;
		}
	}/* i */

	return 1;
}

int is_equal( const void* _r1, const void* _r2)
{
	const MyRect* r1 = (const MyRect*)_r1;
	const MyRect* r2 = (const MyRect*)_r2;

    int distance = Round(r1->width*0.1);

    if( r2->x <= r1->x + distance &&
        r2->x >= r1->x - distance &&
        r2->y <= r1->y + distance &&
        r2->y >= r1->y - distance &&
        r2->width <= Round( r1->width * 1.1 ) &&
        Round( r2->width * 1.1 ) >= r1->width )
	{
		return 1;
	}

	return 0;
}

typedef struct CvPTreeNode
{
    struct CvPTreeNode* parent;
    MyRect* element;
    int rank;
}
CvPTreeNode;

int RectsPartition(vector<MyRect>* vecFaces, vector<int>* labels, ComFunc is_equal)
{
	assert(vecFaces && labels);

	int class_idx = 0;
	vector<CvPTreeNode> *nodes = 0; 
	nodes = new vector<CvPTreeNode>;
	nodes->clear();  //nodes 是容器指针，该容器里存放的是CvPTreeNode

	int i, j;

	// Initial O(N) pass. Make a forest of single-vertex trees.
	for(i = 0; i < vecFaces->size(); i++)
	{
		CvPTreeNode node = {0, 0, 0};
		node.element = &(vecFaces->at(i));
		nodes->push_back(node);//nodes 中存放的CvPTreeNode的element指向vecfaces中的RECT
	}

	 // The main O(N^2) pass. Merge connected components.
	for(i = 0; i < nodes->size(); i++)
	{
		CvPTreeNode* node1 = &nodes->at(i);
		CvPTreeNode* root = node1;

		while( root->parent )
            root = root->parent;

		for(j = 0; j < nodes->size(); j++)
		{
			CvPTreeNode* node2 = &nodes->at(j);

			if( node2->element && node2 != node1 &&
                is_equal( node1->element, node2->element))
            {
				CvPTreeNode* root2 = node2;
                
                // unite both trees
                while( root2->parent )
                    root2 = root2->parent;

				if( root2 != root )
                {
                    if( root->rank > root2->rank )
                        root2->parent = root;
                    else
                    {
                        root->parent = root2;
                        root2->rank += root->rank == root2->rank;
                        root = root2;
                    }
                    assert( root->parent == 0 );

                    // compress path from node2 to the root
                    while( node2->parent )
                    {
                        CvPTreeNode* temp = node2;
                        node2 = node2->parent;
                        temp->parent = root;
                    }

                    // compress path from node to the root
                    node2 = node1;
                    while( node2->parent )
                    {
                        CvPTreeNode* temp = node2;
                        node2 = node2->parent;
                        temp->parent = root;
                    }
                }
			}
		}// end for j
	}// end for i

	labels->clear();
	for( i = 0; i < nodes->size(); i++ )
    {
        CvPTreeNode* node = &nodes->at(i);
        int idx = -1;
        
        if( node->element )
        {
            while( node->parent )
                node = node->parent;
            if( node->rank >= 0 )
                node->rank = ~class_idx++;
            idx = ~node->rank;
        }
		labels->push_back(idx);
    }

	nodes->clear();
	delete nodes;

	return class_idx;
}

void HaarDetectFaces( BYTE* pbyteSrcImg, 
					  int iw, int ih,
					  MyRect detRegion,
					  DetCascadeClassifier* cascade,
					  vector<MyRect>* vecFaces,
					  MySize min_size,
					  MySize max_size,
					  int min_neighbors,
					  double scale_factor )		
{
	assert(pbyteSrcImg != NULL);
	assert(NULL != cascade && NULL != vecFaces);
	assert(detRegion.x >= 0 && detRegion.y >=0 && 
		   detRegion.width <= iw && detRegion.height <= ih);		   
    assert(min_size.width > 0 && min_size.height > 0);
    assert(max_size.width <= detRegion.width&& max_size.width <= detRegion.height); 
	assert(max_size.height <= detRegion.width&&max_size.height <= detRegion.height);
		

	cascade->imgSize.width = iw;
	cascade->imgSize.height = ih;
	int size = iw * ih;
	int *SAT = NULL;
	int *RSAT = NULL;
	double *SSAT=NULL;
	SAT = new int[size];
	RSAT = new int[size];
    SSAT=new double[size];
    IntegralImage(pbyteSrcImg, SAT,RSAT,SSAT, iw, ih);//积分图
	
	vector<MyRect> origFaces, tempFaces;
	origFaces.clear();
	tempFaces.clear();
	vecFaces->clear();

	double factor = 1.0;
	int iMaxSize = MIN(max_size.width, max_size.height);
	for( factor = 1; factor*cascade->origWindowSize.width < iMaxSize; factor *= scale_factor )
	{
		const double ystep = MAX( 2, factor );
		MySize win_size = {Round(cascade->origWindowSize.width * factor), 
			              Round(cascade->origWindowSize.height * factor)};
		int stop_height = Round((detRegion.height - win_size.height) / ystep);

		if(win_size.width < min_size.width || win_size.height < min_size.height)
			continue;

		SetImgForDetCascadeClassifier(cascade, SAT,RSAT,SSAT, iw, ih, factor);

		for(int _iy = 1; _iy < stop_height; _iy++)
		{
			int iy = detRegion.y + Round(_iy*ystep);
			int _xstep = 1;
			int stop_width = Round((detRegion.width - win_size.width) / ystep);
			for(int _ix = 0; _ix < stop_width; _ix += _xstep)
			{
				int ix = detRegion.x + Round(_ix*ystep);
				POS pt;
				pt.x = ix;
				pt.y = iy;	           
				int result;
			
				result = RunDetCascadeClassifier(cascade, pt);
				if(result > 0)
				{
					MyRect rect;
					rect.x = pt.x;
					rect.y = pt.y;
					rect.width = win_size.width;
					rect.height = win_size.height;
					origFaces.push_back(rect);
				}
			}/* ix */
		}/* iy */
	}/* factor */
if( min_neighbors != 0 )
	{
		// group retrieved rectangles
		vector<int> labels;
		int nclasses = RectsPartition(&origFaces, &labels, is_equal);
		
		int* neighbors = new int[nclasses];
		MyRect *rects = new MyRect[nclasses];
		memset(neighbors, 0, nclasses*sizeof(neighbors[0]));
		memset(rects, 0, nclasses*sizeof(rects[0]));		

		// count number of neighbors
		for(int i = 0; i < origFaces.size(); i++)
		{
			MyRect r = origFaces.at(i);
			int idx = labels.at(i);
			
			neighbors[idx]++;
			rects[idx].x += r.x;
			rects[idx].y += r.y;
			rects[idx].width += r.width;
			rects[idx].height += r.height;
		}

		// calculate average bounding box
//		vector<int> tempNeighbors;
//		tempNeighbors.clear();
		for(i = 0; i < nclasses; i++)
		{
			int n = neighbors[i];
			if( n >= min_neighbors )
			{
				MyRect rect;
				rect.x = (rects[i].x*2 + n) / (2*n);
				rect.y = (rects[i].y*2 + n) / (2*n);
				rect.width = (rects[i].width*2 + n) / (2*n);
				rect.height = (rects[i].height*2 + n) / (2*n);
				tempFaces.push_back(rect);
//				tempNeighbors.push_back(neighbors[i]);
			}
		}
		delete[] neighbors;
		delete[] rects;

		// filter out small face rectangles inside large face rectangles
		for(i = 0; i < tempFaces.size(); i++)
		{
			MyRect r1 = tempFaces.at(i);
//			int nb1 = tempNeighbors.at(i);
			int flag = 1;

//			printf("%d %d %d %d %d\n", r1.x, r1.y, r1.width, r1.height, nb1);

			for(int j = 0; j < tempFaces.size(); j++)
			{
				MyRect r2 = tempFaces.at(j);
//				int nb2 = tempNeighbors.at(j);
				int distance = Round(r2.width * 0.2);

				if( (i != j) &&
					(r1.x >= r2.x) &&
					(r1.y >= r2.y) && 
					(r1.x + r1.width <= r2.x + r2.width) &&
					(r1.y + r1.height <= r2.y + r2.height) )//&&
//					(nb2 > MAX(3, nb1) || nb1 < 3 ) )
				{
					flag = 0;
					break;
				}
			}

			if( flag )
				vecFaces->push_back(r1);
		}
	}

	//释放内存
	delete[] SAT;
	delete[] RSAT;
	delete []SSAT;
	SAT=NULL;
	RSAT=NULL;
	SSAT=NULL;
}

void ReleaseDetCascadeClassifier(DetCascadeClassifier* cascade)
{
	if( NULL == cascade )
		return;

	for(int i = 0; i < cascade->count; i++)
	{
		for(int j = 0; j < cascade->stageClassifier[i].count; j++)
		{
			DetHaarClassifier* classifier = &(cascade->stageClassifier[i].classifier[j]);
			for(int k = 0; k < HAAR_FEATURE_MAX; k++)
			{
				classifier->feature.rect[k].p0 = NULL;
				classifier->feature.rect[k].p1 = NULL;
				classifier->feature.rect[k].p2 = NULL;
				classifier->feature.rect[k].p3 = NULL;
			}
		}
		delete[] cascade->stageClassifier[i].classifier;
		cascade->stageClassifier[i].classifier = NULL;		
	}

	delete[] cascade->stageClassifier;
	cascade->stageClassifier = NULL;

	delete cascade;
	cascade = NULL;
}

void ReleaseClassifierCascade(ClassifierCascade* cascade)
{
	if( NULL == cascade )
		return;

	for(int i = 0; i < cascade->count; i++)
	{
		delete[] cascade->stageClassifier[i].classifier;
		cascade->stageClassifier[i].classifier = NULL;
	}
	
	delete[] cascade->stageClassifier;
	cascade->stageClassifier = NULL;

	delete cascade;
	cascade = NULL;
}