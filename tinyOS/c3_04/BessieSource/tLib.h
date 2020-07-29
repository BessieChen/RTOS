//这个.h文件会放一些数据结构的定义和声明
//3.3 放关于类图bitmap的类型定义和函数声明

#ifndef TLIB_H
#define TLIB_H

#include <stdint.h>

//3.3 定义一个结构,叫做tBitmap位图
typedef struct
{
	uint32_t bitmap; //这个设置成32位,也就是说我们将会处理32个优先级
	
}tBitmap;

//3.3 位图的操作:
void tBitmapInit(tBitmap* bitmap); 
void tBitmapSet(tBitmap* bitmap, uint32_t pos);
void tBitmapClear(tBitmap* bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap* bitmap);
uint32_t tBitmapPosCount(void);



#endif
