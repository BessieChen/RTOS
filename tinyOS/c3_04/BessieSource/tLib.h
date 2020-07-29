//���.h�ļ����һЩ���ݽṹ�Ķ��������
//3.3 �Ź�����ͼbitmap�����Ͷ���ͺ�������

#ifndef TLIB_H
#define TLIB_H

#include <stdint.h>

//3.3 ����һ���ṹ,����tBitmapλͼ
typedef struct
{
	uint32_t bitmap; //������ó�32λ,Ҳ����˵���ǽ��ᴦ��32�����ȼ�
	
}tBitmap;

//3.3 λͼ�Ĳ���:
void tBitmapInit(tBitmap* bitmap); 
void tBitmapSet(tBitmap* bitmap, uint32_t pos);
void tBitmapClear(tBitmap* bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet(tBitmap* bitmap);
uint32_t tBitmapPosCount(void);



#endif
