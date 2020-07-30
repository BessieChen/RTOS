//这个.h文件会放一些数据结构的定义和声明
//3.3 放关于类图bitmap的类型定义和函数声明
//3.5 放入关于tNode和tList的数据结构定义和函数声明

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

//3.5 通用双节点,双链表
//3.5 节点
typedef struct _tNode
{
	struct _tNode* preNode; //所以这个是一个地址, 具体的详见我的mac的RTOS文档的图片
	struct _tNode* nextNode;
}tNode;
//3.5 节点的初始化
void tNodeInit(tNode* node);

//3.5 链表
typedef struct _tList
{
	struct _tNode headNode; //所以这是一个_tNode, 里面包含两个地址: _tNode* preNode; _tNode* nextNode;
	uint32_t nodeCount;
}tList;

//3.5 链表的操作
void tListInit(tList* list); //初始
uint32_t tListCount(tList* list); //个数
//3.5 增:
void tListAddFirst(tList* list, tNode* node);
void tListAddLast(tList* list, tNode* node);
void tListInsertAfter(tList* list, tNode* nodeBefore, tNode* nodeToInsert);
void tListInsertBefore(tList* list, tNode* nodeAfter, tNode* nodeToInsert);//by bessie
//3.5 删:
void tListRemoveAll(tList* list);
tNode* tListRemoveFirst(tList* list);
tNode* tListRemoveLast(tList* list); //by bessie
void tListRemove(tList* list, tNode* node);
void tListRemovePre(tList* list, tNode* node); //by bessie
void tListRemoveNext(tList* list, tNode* node); //by bessie
//3.5 查:
tNode* tListFirst(tList* list);
tNode* tListLast(tList* list);
tNode* tListPre(tList* list, tNode* node);
tNode* tListNext(tList* list, tNode* node);

//3.5: 已知一个结构parent的一个成员node的实际地址nodeRealAddr, 求parent结构的起始地址. 其中nodeInStruct是node的声明
#define tNodeParentAddr(parent, nodeInStruct, nodeRealAddr) (parent*)((uint32_t)nodeRealAddr - (uint32_t)&((parent*)0->nodeInStruct))

#endif
