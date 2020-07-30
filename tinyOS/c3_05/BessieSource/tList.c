//3.5 这个是实现tNode和tList相关操作的文件
#include "tLib.h"

//3.5 通用双节点,双链表
//3.5 节点的初始化
void tNodeInit(tNode* node)
{
	node->nextNode = node; //因为node是地址,并且nextNode也是地址
	node->preNode = node;
}

//3.5 关于tList的宏:
#define lastNode headNode.preNode
#define firstNode headNode.nextNode //因为list里面有个成员tNode headNode, 然而tNode有一个成员是tNode* nextnode
																		//因为headNode是一个结构体tNode,而不是指针,所以用点.来调取成员

//3.5 操作: 初始
void tListInit(tList* list)
{
	list->firstNode = &(list->headNode); //相当于list->headNode.preNode = &(list->headNode); 注意: preNode是地址, &(xx)也是地址
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

//3.5 个数
uint32_t tListCount(tList* list)
{
	return list->nodeCount;
}

//3.5 增:
void tListAddFirst(tList* list, tNode* node)
{
	//byb
	/*
	if(list->nodeCount == 0)
	{
		//node是一个地址, 将这个地址存到headNode.pre和next中
		list->headNode.preNode = node;
		list->headNode.nextNode = node;
		
		//node也代表了一个tNode结构,通过node->xx找到那个结构中的成员:tNode* pre, tNode* next;
		node->preNode = &(list->headNode); //或者 list->headNode.nextNode; //注意, 是headNode中的下半部分: tNode* nextNode; 其实也相当于是: 是list->headNode的地址, 因为nodeCount==0的时候,就是list->headNode的地址,也就是List->headNode.nextNode
		node->nextNode = &(list->headNode);
	}
	else{
		//首先让node主动去连接list的headNode的nextNode, 和list之前的headNode的nextNode. 注意这一步不能放在后面, 因为如果先改变了list的headNode.nextNode,会导致node之后不知道连接谁
		node->preNode = &(list->headNode);
		node->nextNode = list->headNode.nextNode;
		
		//其次再是list连到node的前面, 然后之前的第一个node连在node后面. 
		list->headNode.nextNode->preNode = node; //也就是之前list的第一个node, 连接到node后面
		list->headNode.nextNode = node;
	}
	list->nodeCount++;
	*/
	
	//by 01
	node->preNode = list->headNode.nextNode->preNode;
	node->nextNode = list->headNode.nextNode;
	
	list->headNode.nextNode->preNode = node;
	list->headNode.nextNode = node;
	
	list->nodeCount++;
}
void tListAddLast(tList* list, tNode* node)
{
	//byb
	/*
	if(list->nodeCount == 0)
	{
		list->headNode.preNode = node;
		list->headNode.nextNode = node;
		
		node->preNode = &(list->headNode);
		node->nextNode = &(list->headNode);
	}
	else
	{
		node->nextNode = &(list->headNode); //node所代表的的tNode中的nextNode是存的list的headNode的地址
		node->preNode = list->headNode.preNode;
		
		list->headNode.preNode->nextNode = node; //之前list的前一个节点, 他现在的下一个节点就是node
		list->headNode.preNode = node;
	}
	list->nodeCount++;
	*/
	
	//by 01
	node->nextNode = &(list->headNode);
	node->preNode = list->headNode.preNode;
	
	list->headNode.preNode->nextNode = node;
	list->headNode.preNode = node;
	
	list->nodeCount++;
}
void tListInsertAfter(tList* list, tNode* nodeBefore, tNode* nodeToInsert)
{
	//byb
	/*
	//特例: 在list->headNode右侧加入node, 也就是nodeBefore就是我们list->headNode的地址, 就需要特殊处理:
	if(nodeBefore == &(list->headNode))
	{
		tListAddFirst(list, nodeToInsert);
		return;
	}
	
	//除了特例之外的: 也就是在除了在list->headNode右侧加入node
	tNode* n;
	for(n = list->headNode.nextNode; n != &(list->headNode); n = n->nextNode)
	{
		if(n == nodeBefore)
		{
			//将nodeToInsert连接到n和n的下一个:
			nodeToInsert->preNode = n;
			nodeToInsert->nextNode = n->nextNode;
			
			//将n的下一个连到nodeToInsert, 将n的下一个的前一个连到nodeToInsert
			n->nextNode->preNode = nodeToInsert;
			n->nextNode = nodeToInsert;
			list->nodeCount++;
		}
	}
	*/
	
	//by 01
	nodeToInsert->preNode = nodeBefore;
	nodeToInsert->nextNode = nodeBefore->nextNode;
	
	nodeBefore->nextNode->preNode = nodeToInsert;
	nodeBefore->nextNode = nodeToInsert;
	
	list->nodeCount++;
}
void tListInsertBefore(tList* list, tNode* nodeAfter, tNode* nodeToInsert)//by bessie
{
	tNode* n;
	//特例: 在list->headNode左侧加入node, 也就是nodeAfter就是我们list->headNode的地址, 就需要特殊处理:
	if(nodeAfter == &(list->headNode))
	{
		tListAddLast(list, nodeToInsert);
		return;
	}
	
	//除了特例之外的: 也就是在除了在list->headNode左侧加入node
	//注意, 因为是在前面插入,所以我们只需要找到nodeAfter前面的那个node, 所以判断是if(n->nextNode == nodeAfter)
	for(n = &(list->headNode); n != list->headNode.preNode; n = n->nextNode)
	{
		if(n->nextNode == nodeAfter)
		{
			nodeToInsert->preNode = n;
			nodeToInsert->preNode = n->nextNode;
			
			n->nextNode = nodeToInsert;
			n->nextNode->preNode = nodeToInsert;
			
			list->nodeCount++;
		}
	}
}
//3.5 删:
void tListRemoveAll(tList* list)
{
	//by bessie
	/*
	tNode* n;
	for(n = list->headNode.nextNode; n != &(list->headNode); n = n->nextNode)
	{
		//首先处理的是list->headNode.nextNode, 将node左侧的节点的下一个指向 node右侧的节点(注意,这个是第一步,因为node左右侧的节点信息还不能被删)
		n->preNode->nextNode = n->nextNode;
		n->nextNode->preNode = n->preNode;
		
		//先把这个node的pre和next指向自己(现在node左右侧的节点信息可以被删)
		n->preNode = n;
		n->nextNode = n;
	}
	//到此, 除了list->headNode之外,所有的节点都被删了, 同时list->headNode的pre和next在这个时候也已经指向了自己, 即在for循环中已经实现了
	
	list->nodeCount = 0;
	*/
	
	//by 01
	uint32_t count;
	tNode* nextNode;
	
	nextNode = list->headNode.nextNode;
	for(count = list->nodeCount; count != 0; count--)
	{
		tNode* currentNode = nextNode;
		nextNode = nextNode->nextNode;
		
		currentNode->nextNode = currentNode;
		currentNode->preNode = currentNode;
	}
	
	list->headNode.nextNode = &(list->headNode);
	list->headNode.preNode = &(list->headNode);
	
	list->nodeCount = 0;
}
tNode* tListRemoveFirst(tList* list)
{
	//byb
	/*
	tNode* nodeToDelete = list->headNode.nextNode;
	nodeToDelete->nextNode->preNode = nodeToDelete->preNode; //相当于 &(list->headNode);
	list->headNode.nextNode = nodeToDelete->nextNode;
	
	//将nodeToDelete指向自己
	nodeToDelete->preNode = nodeToDelete;
	nodeToDelete->nextNode = nodeToDelete;
	
	list->nodeCount--;
	
	return nodeToDelete;
	*/
	
	//by 01
	tNode* node = (tNode*) 0;
	if(list->nodeCount != 0)
	{
		node = list->headNode.nextNode;
		
		node->nextNode->preNode = &(list->headNode);
		list->headNode.nextNode = node->nextNode; //这里如果是删除掉仅有的一个node,也是符合逻辑的,因为删除之后list->headNode的pre和next都是指向&(list->headNode)
		
		node->preNode = node;
		node->nextNode = node;
		
		list->nodeCount--;
	}
	
	return node;
}
tNode* tListRemoveLast(tList* list) //by bessie
{
	tNode* nodeToDelete = list->headNode.preNode;
	nodeToDelete->preNode->nextNode = &(list->headNode);
	list->headNode.preNode = nodeToDelete->preNode;
	
	nodeToDelete->preNode = nodeToDelete;
	nodeToDelete->nextNode = nodeToDelete;
	
	list->nodeCount--;
	return nodeToDelete;
}
void tListRemove(tList* list, tNode* nodeToDelete)
{
	//byb
	/*
	//我认为,这个要删除的node,不能是&(list->headNode)
	if(nodeToDelete == &(list->headNode)) return;
	
	tNode* n;
	for(n = list->headNode.nextNode; n != &(list->headNode); n = n->nextNode)
	{
		if(n == nodeToDelete)
		{
			nodeToDelete->nextNode->preNode = nodeToDelete->preNode;
			nodeToDelete->preNode->nextNode = nodeToDelete->nextNode;
			
			nodeToDelete->preNode = nodeToDelete;
			nodeToDelete->nextNode = nodeToDelete;
			
			list->nodeCount--;
		}
	}
	*/
	
	//by 01
	nodeToDelete->preNode->nextNode = nodeToDelete->nextNode;
	nodeToDelete->nextNode->preNode = nodeToDelete->preNode;
	
	list->nodeCount--;
}
void tListRemovePre(tList* list, tNode* nodeAfter) //删除nodeAfter的前一个节点, by bessie
{
	//byb
	/*
	//我认为,这个要删除的node,不能是&(list->headNode)
	if(nodeAfter == (tNode*)&(list->headNode.nextNode)) return;
	
	for()
	*/
	
}
void tListRemoveNext(tList* list, tNode* node) //by bessie
{
	//todo
}
//3.5 查:
tNode* tListFirst(tList* list)
{
	//by 01
	tNode* node = (tNode*) 0;
	if(list->nodeCount != 0)
	{
		node = list->headNode.nextNode;
	}
	return node;
}
tNode* tListLast(tList* list)
{
	//by 01
	tNode* node = (tNode*) 0;
	if(list->nodeCount != 0)
	{
		node = list->headNode.preNode;
	}
	
	return node;
}
tNode* tListPre(tList* list, tNode* node) //虽然这里list没有用到
{
	//by 01
	//判断node的合法性
	if(node->preNode == node)
	{
		return (tNode*) 0;
	}
	
	return node->preNode;
	
}
tNode* tListNext(tList* list, tNode* node)
{
	//by 01
	//判断node的合法性
	if(node->nextNode == node)
	{
		return (tNode*) 0;
	}
	
	return node->nextNode;
}
