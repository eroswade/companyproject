// BinaryTree.h:	interface for the CBinaryTree template.
// Inventor Name:	Hatem Mostafa
// Created:			18/1/2003
// Modified:		20/12/2004
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>

#define TREENODE CBinaryTreeNode<KEY, DATA>
#define LEFT	Childs[0]
#define RIGHT	Childs[1]

class CInt
{
public:
	int m_nKey;
	int compare(int nKey) 
	{ return nKey == m_nKey ? 0 : (nKey >= m_nKey ? -1 : 1); }
	void operator=(int nKey) { m_nKey = nKey; }
	bool operator==(int nKey) { return m_nKey == nKey; } 
}; 


template <class KEY, class DATA> class CBinaryTreeNode
{
public:
	CBinaryTreeNode()
	{
		Parent = LEFT = RIGHT = NULL;
		Count = ID = 0;
		ArrayEqualIDs = NULL;
	}
	~CBinaryTreeNode()
	{
		if(ArrayEqualIDs)
			delete ArrayEqualIDs;
	}
public:
	// node parent, left, right respectively
	TREENODE *Parent, *Childs[2];
	// node key
	KEY Key;
	// node data
	DATA Data;
	// node repetition count
	int Count;
	// node ID
	int ID;
	// node repeated keys' IDs
	vector<int> *ArrayEqualIDs;
	const TREENODE& operator=(const TREENODE& node)
	{
		Key = node.Key;
		Data = node.Data;
		Count = node.Count;
		ID = node.ID;
		if(node.ArrayEqualIDs)
		{
			if(ArrayEqualIDs == NULL)
				ArrayEqualIDs = new vector<int>;
			*ArrayEqualIDs = *node.ArrayEqualIDs;
		}
		return *this;
	}
};

template <class KEY, class ARG_KEY, class DATA, class ARG_DATA> class CBinaryTree
{
public:
	CBinaryTree()
	{
		Root = Nil = NULL;
		Count = Serial = 0;
		Modified = NoRepeat = false;
	}
	~CBinaryTree()
	{
		RemoveAll();
	}
public:
	// tree root node
	TREENODE* Root, * Nil;
	// tree nodes count
	int Count, Serial;
	// flag to indicate if the tree is modified or not
	bool Modified;
	// ignore repeated keys in the Add function
	bool NoRepeat;

	// return tree nodes count
	inline int GetCount() const	{	return Count;	}
	// check if the tree is empty or not
	inline bool IsEmpty() const	{	return Count == 0;	}
	// remove all tree nodes
	void RemoveAll()
	{
		TREENODE *node = Root, *pTemp;
		while(node != Nil)
		{
			// check for left child
			if(node->LEFT != Nil)
				node = node->LEFT;
			// check for right child
			else	if(node->RIGHT != Nil)
				node = node->RIGHT;
			else	// node has no childs
			{	// save node pointer
				pTemp = node;
				// set node pointer at its parent to NULL
				if(node->Parent != Nil)
					node->Parent->Childs[node != node->Parent->LEFT] = Nil;
				// update pointer node to its parent
				node = node->Parent;
				// delete the saved node
				delete pTemp;
			}
		}
		Count = Serial = 0;
		Root = Nil;
		Modified = false;
	}
	// insert key in the tree
	inline TREENODE* Insert(ARG_KEY key, int nID = -1, TREENODE* node = NULL)
	{
		if(Root == Nil)
		{
			Root = NewNode();
			node = Root;
		}
		else	
		{
			if(node == NULL)
				node = Root;
			int nResult;
			while(true)
			{
				nResult = node->Key.compare(key);//KEY对比,node的KEY大,返回1,否则返回-1 相等,返回0
				if(nResult == 0)// 两个KEY相等 表示有重复
				{
					node->Count++;
					if(NoRepeat == false)
					{
						if(node->ArrayEqualIDs == NULL)
							node->ArrayEqualIDs = new vector<int>;
						node->ArrayEqualIDs->push_back(nID == -1 ? Serial : nID);
						Serial++;
						Count++;
					}
					return node;
				}
				nResult = nResult > 0 ? 0 : 1;
				if(node->Childs[nResult] == Nil)//无重复 看对比情况来添加左右
				{
					node->Childs[nResult] = NewNode();
					node->Childs[nResult]->Parent = node;
					node = node->Childs[nResult];
					break;
				}
				node = node->Childs[nResult];
			}	
		}
		node->Key = key;
		node->ID = nID == -1 ? Serial : nID;
		Serial++;
		Count++;
		node->Count++;
		Modified = true;
		
		return node;
	}

	inline TREENODE* InsertLeft(ARG_KEY key, TREENODE* node)
	{
		if(Root == Nil)
		{
			Root = NewNode();
			node = Root;
		}
		else	
		{

			{
				if(node->Childs[0] == Nil)//无重复 看对比情况来添加左右
				{
					node->Childs[0] = NewNode();
					node->Childs[0]->Parent = node;
					node = node->Childs[0];
				}
			}	
		}
		node->Key = key;
		node->ID = -1;
		Serial++;
		Count++;
		node->Count++;
		Modified = true;

		return node;
	}

	inline TREENODE* InsertRight(ARG_KEY key, TREENODE* node)
	{
		if(Root == Nil)
		{
			Root = NewNode();
			node = Root;
		}
		else	
		{
			{
				if(node->Childs[1] == Nil)//无重复 看对比情况来添加左右
				{
					node->Childs[1] = NewNode();
					node->Childs[1]->Parent = node;
					node = node->Childs[1];
				}
			}	
		}
		node->Key = key;
		node->ID = -1;
		Serial++;
		Count++;
		node->Count++;
		Modified = true;

		return node;
	}
	// search for a key in the tree
	inline TREENODE* Search(ARG_KEY key, TREENODE* node = NULL) const
	{
		if(node == NULL)
			node = Root;
		int nResult;
		while(node != Nil && (nResult = node->Key.compare(key)) != 0)
			node = node->Childs[nResult < 0];
		return node == Nil ? NULL : node;
	}	
	// return minimum key in the tree
	TREENODE* Min(TREENODE* node) const
	{	
		// iterate in the left branch
		while(node != Nil && node->LEFT != Nil)
			node = node->LEFT;
		return node;
	}
	// return maximum key in the tree
	TREENODE* Max(TREENODE* node) const
	{	
		// iterate in the right branch
		while(node != Nil && node->RIGHT != Nil)
			node = node->RIGHT;
		return node;
	}
	// return node successor
	TREENODE* Successor(TREENODE* node) const
	{
		// return the left most node in the right subtree
		if(node->RIGHT != Nil)
			return Min(node->RIGHT);
		// go up from node until we find a node that is the left of its parent
		TREENODE* Parent = node->Parent;
		while(Parent != Nil && node == Parent->RIGHT)
		{
			node = Parent;
			Parent = node->Parent;
		}
		return Parent;
	}
	// return node predecessor
	TREENODE* Predecessor(TREENODE* node) const
	{	
		// return the right most node in the left subtree
		if(node->LEFT != Nil)
			return Max(node->LEFT);
		// go up from node until we find a node that is the right of its parent
		TREENODE* Parent = node->Parent;
		while(Parent != Nil && node == Parent->LEFT)
		{
			node = Parent;
			Parent = node->Parent;
		}
		return Parent;
	}

	// delete node
	// 1- node has no child, remove it
	// 2- node has one child, splice it (connect its parent and child)
	// 3- node has two childs, splice its successor and put it in its place
	void Delete(TREENODE* node)
	{	
		TREENODE *pSplice = (node->LEFT == Nil || node->RIGHT == Nil)?node:Successor(node);
		TREENODE *pChild = pSplice->Childs[pSplice->LEFT == Nil];
		// connect child to spliced node parent
		if(pChild != Nil)
			pChild->Parent = pSplice->Parent;
		// connect spliced node parent to child
		if(pSplice->Parent == Nil)
			Root = pChild;
		else
			pSplice->Parent->Childs[pSplice != pSplice->Parent->LEFT] = pChild;
		// put spliced node in place of node (if required)
		if(pSplice != node)
		{	// copy spliced node
			*node = *pSplice;
			// delete the spliced node
			delete pSplice;
		}
		else
			// delete the node
			delete node;
		Count--;
	}

	// save all tree nodes in a vector of integers
	void Save(vector<int> &nArraySort, bool bAscending = true, bool (* lpfn)(int, int) = NULL)
	{
		nArraySort.resize(Count);
		int nIndex = 0, *pArray = &*nArraySort.begin();

		TREENODE* node = bAscending ? Min(Root) : Max(Root);
		while(node != Nil)
		{
			if(lpfn)
				(*lpfn)(nIndex++, Count);
			SaveNode(node, pArray);
			node = bAscending ? Successor(node) : Predecessor(node);
		}
		Modified = false;
	}
	// add one node to a vector of integers
	void SaveNode(TREENODE* node, int*& pArray)
	{
		*pArray++ = node->ID;
		if(node->ArrayEqualIDs)
		{
			memcpy(pArray, &*node->ArrayEqualIDs->begin(), node->ArrayEqualIDs->size()*sizeof(int));
			pArray += node->ArrayEqualIDs->size();
		}
	}
protected:
	virtual TREENODE* NewNode()
	{
		return new TREENODE();
	}
};

