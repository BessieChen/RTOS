//���.h�ļ����һЩ���ݽṹ�Ķ��������
//3.3 �Ź�����ͼbitmap�����Ͷ���ͺ�������
//3.5 �������tNode��tList�����ݽṹ����ͺ�������

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

//3.5 ͨ��˫�ڵ�,˫����
//3.5 �ڵ�
typedef struct _tNode
{
	struct _tNode* preNode; //���������һ����ַ, ���������ҵ�mac��RTOS�ĵ���ͼƬ
	struct _tNode* nextNode;
}tNode;
//3.5 �ڵ�ĳ�ʼ��
void tNodeInit(tNode* node);

//3.5 ����
typedef struct _tList
{
	struct _tNode headNode; //��������һ��_tNode, �������������ַ: _tNode* preNode; _tNode* nextNode;
	uint32_t nodeCount;
}tList;

//3.5 ����Ĳ���
void tListInit(tList* list); //��ʼ
uint32_t tListCount(tList* list); //����
//3.5 ��:
void tListAddFirst(tList* list, tNode* node);
void tListAddLast(tList* list, tNode* node);
void tListInsertAfter(tList* list, tNode* nodeBefore, tNode* nodeToInsert);
void tListInsertBefore(tList* list, tNode* nodeAfter, tNode* nodeToInsert);//by bessie
//3.5 ɾ:
void tListRemoveAll(tList* list);
tNode* tListRemoveFirst(tList* list);
tNode* tListRemoveLast(tList* list); //by bessie
void tListRemove(tList* list, tNode* node);
void tListRemovePre(tList* list, tNode* node); //by bessie
void tListRemoveNext(tList* list, tNode* node); //by bessie
//3.5 ��:
tNode* tListFirst(tList* list);
tNode* tListLast(tList* list);
tNode* tListPre(tList* list, tNode* node);
tNode* tListNext(tList* list, tNode* node);

//3.5: ��֪һ���ṹparent��һ����Աnode��ʵ�ʵ�ַnodeRealAddr, ��parent�ṹ����ʼ��ַ. ����nodeInStruct��node������
#define tNodeParentAddr(parent, nodeInStruct, nodeRealAddr) (parent*)((uint32_t)nodeRealAddr - (uint32_t)&((parent*)0->nodeInStruct))

#endif
