#ifndef GLOBALS_H
#define GLOBALS_H

#define THROWABLE(X) if(!(X)) return FALSE;

typedef unsigned char Byte;
typedef char Int8;

#define BOOL Byte
#define TRUE 1
#define FALSE 0

typedef int Point2[2];
typedef unsigned int Point2U32[2];
typedef unsigned short Point2U16[2];
typedef Byte Point2U8[2];
typedef Int8 Point2S8[2];
typedef float Point2f[2];

typedef int Rect2[4];
typedef unsigned int Rect2U32[4];
typedef unsigned short Rect2U16[4];
typedef Byte Rect2U8[4];
typedef float Rect2f[4]; 

//#define EPSILON 0.000001
#define EPSILON 0.001
#define XF_GREATER_THAN_YF(x, y) ((x) > ((y)+EPSILON))
#define XF_LESS_THAN_YF(x, y) ((x) < ((y)-EPSILON))
#define EQUAL_TO_F(x, y) ((((x)-EPSILON) < (y)) && (((x)+EPSILON) > (y)))
#define XF_GREATER_THAN_OR_EQUAL_YF(x, y) ((x) > ((y)-EPSILON))
#define XF_LESS_THAN_OR_EQUAL_YF(x, y) ((x) < ((y)+EPSILON))

#define WAIT(x, y) if(XF_LESS_THAN_YF(x, y)) return; x -= y

#define ROUND_UP(x) (((int)x) + (XF_GREATER_THAN_YF( ((x)-((int)(x))), 0.4 )? 1 : 0))
#define ROUND_DOWN(x) (((int)x) + (XF_LESS_THAN_YF( 0.6, ((x)-((int)(x))) )? 0 : 1))

#define IF_TRUE_SET_FALSE(condition, extension) if(condition) { condition = FALSE; extension }
#define X2_INBETWEEN_X1_X3(x1, x2, x3) ((x1) < (x2) && (x2) < (x3))

#endif