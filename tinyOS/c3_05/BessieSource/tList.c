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
}
void tListAddLast(tList* list, tNode* node)
{
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
}
void tListInsertAfter(tList* list, tNode* nodeBefore, tNode* nodeToInsert)
{
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
}
void tListInsertBefore(tList* list, tNode* nodeAfter, tNode* nodeToInsert)//by bessie
{
	//����: ��list->headNode������node, Ҳ����nodeAfter��������list->headNode�ĵ�ַ, ����Ҫ���⴦��:
	if(nodeAfter == &(list->headNode))
	{
		tListAddLast(list, nodeToInsert);
		return;
	}
	
	//��������֮���: Ҳ�����ڳ�����list->headNode������node
	tNode* n;
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
}
tNode* tListRemoveFirst(tList* list)
{
	tNode* nodeToDelete = list->headNode.nextNode;
	nodeToDelete->nextNode->preNode = nodeToDelete->preNode; //�൱�� &(list->headNode);
	list->headNode.nextNode = nodeToDelete->nextNode;
	
	//��nodeToDeleteָ���Լ�
	nodeToDelete->preNode = nodeToDelete;
	nodeToDelete->nextNode = nodeToDelete;
	
	list->nodeCount--;
	
	return nodeToDelete;
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
void tListRemove(tList* list, tNode* node);
void tListRemovePre(tList* list, tNode* node); //by bessie
void tListRemoveNext(tList* list, tNode* node); //by bessie
//3.5 ��:
tNode* tListFirst(tList* list);
tNode* tListLast(tList* list);
tNode* tListPre(tList* list, tNode* node);
tNode* tListNext(tList* list, tNode* node);