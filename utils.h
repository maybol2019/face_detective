#ifndef __UTILS_H_
#define __UTILS_H_

#define LOGRATIO_THRESHOLD  0.00001F
#define THRESHOLD_EPS  0.00001
#define RATE_EPS       0.00001

#define MAX(a, b) ( (a) > (b) ? (a) : (b) )
#define MIN(a, b) ( (a) < (b) ? (a) : (b) )

typedef unsigned __int64 uint64;

typedef struct MyRect
{
	int x;
	int y;
	int width;
	int height;

}MyRect;

typedef struct MySize
{
	int width;
	int height;
}MySize;

inline float LogRatio(float val);//log( val / (1-val) )
inline float LogRatio(float val)
{
	float tval;

    tval = MAX(LOGRATIO_THRESHOLD, MIN(1.0F - LOGRATIO_THRESHOLD, val ));
    return logf( tval / (1.0F - tval) );
}


inline  int  Round( double val );
inline  int  Round( double val )
{
    double temp = val + 6755399441055744.0;
    return (int)*((uint64*)&temp);
}

#endif