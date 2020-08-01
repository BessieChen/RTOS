//3.5 �����ʵ��tNode��tList��ز������ļ�
#include "tLib.h"

//3.5 ͨ��˫�ڵ�,˫����
//3.5 �ڵ�ĳ�ʼ��
void tNodeInit(tNode* node)
{
	node->nextNode = node; //��Ϊnode�ǵ�ַ,����nextNodeҲ�ǵ�ַ
	node->preNode = node;
}

//3.5 ����tList�ĺ�:
#define lastNode headNode.preNode
#define firstNode headNode.nextNode //��Ϊlist�����и���ԱtNode headNode, Ȼ��tNode��һ����Ա��tNode* nextnode
																		//��ΪheadNode��һ���ṹ��tNode,������ָ��,�����õ�.����ȡ��Ա

//3.5 ����: ��ʼ
void tListInit(tList* list)
{
	list->firstNode = &(list->headNode); //�൱��list->headNode.preNode = &(list->headNode); ע��: preNode�ǵ�ַ, &(xx)Ҳ�ǵ�ַ
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

//3.5 ����
uint32_t tListCount(tList* list)
{
	return list->nodeCount;
}

//3.5 ��:
void tListAddFirst(tList* list, tNode* node)
{
	//byb
	/*
	if(list->nodeCount == 0)
	{
		//node��һ����ַ, �������ַ�浽headNode.pre��next��
		list->headNode.preNode = node;
		list->headNode.nextNode = node;
		
		//nodeҲ������һ��tNode�ṹ,ͨ��node->xx�ҵ��Ǹ��ṹ�еĳ�Ա:tNode* pre, tNode* next;
		node->preNode = &(list->headNode); //���� list->headNode.nextNode; //ע��, ��headNode�е��°벿��: tNode* nextNode; ��ʵҲ�൱����: ��list->headNode�ĵ�ַ, ��ΪnodeCount==0��ʱ��,����list->headNode�ĵ�ַ,Ҳ����List->headNode.nextNode
		node->nextNode = &(list->headNode);
	}
	else{
		//������node����ȥ����list��headNode��nextNode, ��list֮ǰ��headNode��nextNode. ע����һ�����ܷ��ں���, ��Ϊ����ȸı���list��headNode.nextNode,�ᵼ��node֮��֪������˭
		node->preNode = &(list->headNode);
		node->nextNode = list->headNode.nextNode;
		
		//�������list����node��ǰ��, Ȼ��֮ǰ�ĵ�һ��node����node����. 
		list->headNode.nextNode->preNode = node; //Ҳ����֮ǰlist�ĵ�һ��node, ���ӵ�node����
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
		node->nextNode = &(list->headNode); //node������ĵ�tNode�е�nextNode�Ǵ��list��headNode�ĵ�ַ
		node->preNode = list->headNode.preNode;
		
		list->headNode.preNode->nextNode = node; //֮ǰlist��ǰһ���ڵ�, �����ڵ���һ���ڵ����node
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
	//����: ��list->headNode�Ҳ����node, Ҳ����nodeBefore��������list->headNode�ĵ�ַ, ����Ҫ���⴦��:
	if(nodeBefore == &(list->headNode))
	{
		tListAddFirst(list, nodeToInsert);
		return;
	}
	
	//��������֮���: Ҳ�����ڳ�����list->headNode�Ҳ����node
	tNode* n;
	for(n = list->headNode.nextNode; n != &(list->headNode); n = n->nextNode)
	{
		if(n == nodeBefore)
		{
			//��nodeToInsert���ӵ�n��n����һ��:
			nodeToInsert->preNode = n;
			nodeToInsert->nextNode = n->nextNode;
			
			//��n����һ������nodeToInsert, ��n����һ����ǰһ������nodeToInsert
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
	//����: ��list->headNode������node, Ҳ����nodeAfter��������list->headNode�ĵ�ַ, ����Ҫ���⴦��:
	if(nodeAfter == &(list->headNode))
	{
		tListAddLast(list, nodeToInsert);
		return;
	}
	
	//��������֮���: Ҳ�����ڳ�����list->headNode������node
	//ע��, ��Ϊ����ǰ�����,��������ֻ��Ҫ�ҵ�nodeAfterǰ����Ǹ�node, �����ж���if(n->nextNode == nodeAfter)
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
//3.5 ɾ:
void tListRemoveAll(tList* list)
{
	//by bessie
	/*
	tNode* n;
	for(n = list->headNode.nextNode; n != &(list->headNode); n = n->nextNode)
	{
		//���ȴ������list->headNode.nextNode, ��node���Ľڵ����һ��ָ�� node�Ҳ�Ľڵ�(ע��,����ǵ�һ��,��Ϊnode���Ҳ�Ľڵ���Ϣ�����ܱ�ɾ)
		n->preNode->nextNode = n->nextNode;
		n->nextNode->preNode = n->preNode;
		
		//�Ȱ����node��pre��nextָ���Լ�(����node���Ҳ�Ľڵ���Ϣ���Ա�ɾ)
		n->preNode = n;
		n->nextNode = n;
	}
	//����, ����list->headNode֮��,���еĽڵ㶼��ɾ��, ͬʱlist->headNode��pre��next�����ʱ��Ҳ�Ѿ�ָ�����Լ�, ����forѭ�����Ѿ�ʵ����
	
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
	nodeToDelete->nextNode->preNode = nodeToDelete->preNode; //�൱�� &(list->headNode);
	list->headNode.nextNode = nodeToDelete->nextNode;
	
	//��nodeToDeleteָ���Լ�
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
		list->headNode.nextNode = node->nextNode; //���������ɾ�������е�һ��node,Ҳ�Ƿ����߼���,��Ϊɾ��֮��list->headNode��pre��next����ָ��&(list->headNode)
		
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
	//����Ϊ,���Ҫɾ����node,������&(list->headNode)
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
void tListRemovePre(tList* list, tNode* nodeAfter) //ɾ��nodeAfter��ǰһ���ڵ�, by bessie
{
	//byb
	/*
	//����Ϊ,���Ҫɾ����node,������&(list->headNode)
	if(nodeAfter == (tNode*)&(list->headNode.nextNode)) return;
	
	for()
	*/
	
}
void tListRemoveNext(tList* list, tNode* node) //by bessie
{
	//todo
}
//3.5 ��:
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
tNode* tListPre(tList* list, tNode* node) //��Ȼ����listû���õ�
{
	//by 01
	//�ж�node�ĺϷ���
	if(node->preNode == node)
	{
		return (tNode*) 0;
	}
	
	return node->preNode;
	
}
tNode* tListNext(tList* list, tNode* node)
{
	//by 01
	//�ж�node�ĺϷ���
	if(node->nextNode == node)
	{
		return (tNode*) 0;
	}
	
	return node->nextNode;
}
