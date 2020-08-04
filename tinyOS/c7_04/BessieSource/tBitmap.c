//3.3 添加.c文件实现tLib.h中的位图
#include "tLib.h"

//3.3 初始化位图
void tBitmapInit(tBitmap* bitmap) //注意这里传入的是tBitmap的指针
{
	bitmap->bitmap = 0;
}
//3.3 将某pos位设置成1
void tBitmapSet(tBitmap* bitmap, uint32_t pos)
{
	bitmap->bitmap |= (1 << pos); //解释见mac的RTOS笔记
}
//3.3 将某pos位设置成0
void tBitmapClear(tBitmap* bitmap, uint32_t pos)
{
	bitmap->bitmap &= ~(1 << pos);
}
//3.3 查找从最低位开始,第一个设置为1的pos数
uint32_t tBitmapGetFirstSet(tBitmap* bitmap)
{
	//首先设置我们的快速查找表: 注意是static, 是const, 是uint8_t, 因为返回的值只有0-7,或者0xff (其实从来都不会返回0xff, 因为我们会有if判断)
	static const uint8_t quickFindTable[] = 
	{
		/*00: 0-15*/ 0xff,0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*10: 16-31*/	4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*20*/				5,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*30*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*40*/				6,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*50*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*60*/				5,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*70*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*80*/				7,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*90*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*A0*/				5,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*B0*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*C0*/				6,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*D0*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*E0*/				5,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		/*F0*/				4,  0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	}; //记得加上分号
	
	/*我很喜欢以下的代码:
	1. 首先我们判断最后8位,如果&0xff都是0, 说明最后8位都是0,不用判断了, 判断前面的8位
	2. 记得先位移8,16,24, 然后再&0xff, 因为quickFindTable只有256个元素
	3. 记得quickFindTable只是返回0-7, 或者0xff, 所以你要加上偏移量8, 16, 24  (其实从来都不会返回0xff, 因为我们会有if判断)
	4. 注意特例: 特例不是全1, 特例是全0
			全0的时候,没有答案
			所以我们返回32, 因为合法范围是0-31
	*/
	if(bitmap->bitmap & 0xff)
	{
		return quickFindTable[bitmap->bitmap & 0xff];
	}
	else if((bitmap->bitmap >> 8) & 0xff)
	{
		return quickFindTable[(bitmap->bitmap >> 8) & 0xff] + 8;
	}
	else if((bitmap->bitmap >> 16) & 0xff)
	{
		return quickFindTable[(bitmap->bitmap >> 16) & 0xff] + 16;
	}
	else if((bitmap->bitmap >> 24) & 0xff)
	{
		return quickFindTable[(bitmap->bitmap >> 24) & 0xff] + 24;
	}
	else //说明是全0
	{
		return tBitmapPosCount();//返回32
	}
	
}
//3.3 bitmap的容量, 例如32,64
uint32_t tBitmapPosCount(void)
{
	return 32;
}
