//3.3 ���.c�ļ�ʵ��tLib.h�е�λͼ
#include "tLib.h"

//3.3 ��ʼ��λͼ
void tBitmapInit(tBitmap* bitmap) //ע�����ﴫ�����tBitmap��ָ��
{
	bitmap->bitmap = 0;
}
//3.3 ��ĳposλ���ó�1
void tBitmapSet(tBitmap* bitmap, uint32_t pos)
{
	bitmap->bitmap |= (1 << pos); //���ͼ�mac��RTOS�ʼ�
}
//3.3 ��ĳposλ���ó�0
void tBitmapClear(tBitmap* bitmap, uint32_t pos)
{
	bitmap->bitmap &= ~(1 << pos);
}
//3.3 ���Ҵ����λ��ʼ,��һ������Ϊ1��pos��
uint32_t tBitmapGetFirstSet(tBitmap* bitmap)
{
	//�����������ǵĿ��ٲ��ұ�: ע����static, ��const, ��uint8_t, ��Ϊ���ص�ֵֻ��0-7,����0xff (��ʵ���������᷵��0xff, ��Ϊ���ǻ���if�ж�)
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
	}; //�ǵü��Ϸֺ�
	
	/*�Һ�ϲ�����µĴ���:
	1. ���������ж����8λ,���&0xff����0, ˵�����8λ����0,�����ж���, �ж�ǰ���8λ
	2. �ǵ���λ��8,16,24, Ȼ����&0xff, ��ΪquickFindTableֻ��256��Ԫ��
	3. �ǵ�quickFindTableֻ�Ƿ���0-7, ����0xff, ������Ҫ����ƫ����8, 16, 24  (��ʵ���������᷵��0xff, ��Ϊ���ǻ���if�ж�)
	4. ע������: ��������ȫ1, ������ȫ0
			ȫ0��ʱ��,û�д�
			�������Ƿ���32, ��Ϊ�Ϸ���Χ��0-31
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
	else //˵����ȫ0
	{
		return tBitmapPosCount();//����32
	}
	
}
//3.3 bitmap������, ����32,64
uint32_t tBitmapPosCount(void)
{
	return 32;
}
