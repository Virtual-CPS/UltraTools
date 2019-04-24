#ifndef __LIST_H__
#define __LIST_H__

#include <assert.h>
#include <new.h>

#if 0
//MFC AFX사용하면 죽임다. 
#ifndef POSITION
struct __POSITION { };
typedef __POSITION* POSITION;
#endif
#endif

#ifndef CPlex
struct CPlex     // warning variable length structure
{
	CPlex* pNext;
	void* data() { return this+1; }

	static CPlex* Create(CPlex*& pHead, UINT nMax, UINT cbElement)
	{
		assert(nMax > 0 && cbElement > 0);
        	CPlex* p = (CPlex*) new BYTE[sizeof(CPlex) + nMax * cbElement];
        	p->pNext = pHead;
        	pHead = p;  // change head (adds in reverse order for simplicity)
        	return p;
	}

	void FreeDataChain()       // free this one and links
	{
		CPlex* p = this;
        	while (p != 0)
        	{
                	BYTE* bytes = (BYTE*) p;
                	CPlex* pNext = p->pNext;
                	delete[] bytes;
                	p = pNext;
        	}
	}
};
#endif

template<class TYPE>
inline void ConstructElements(TYPE* pElements, int nCount)
{
	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements) TYPE;
}

template<class TYPE>
inline void DestructElements(TYPE* pElements, int nCount)
{
	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE, class ARG_TYPE>
BOOL CompareElements(const TYPE* pElement1, const ARG_TYPE* pElement2)
{
	return *pElement1 == *pElement2;
}

/////////////////////////////////////////////////////////////////////////////
// List<TYPE, ARG_TYPE>

template<class TYPE, class ARG_TYPE>
class List
{
protected:
	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE data;
	};
public:
// Construction
	List(int nBlockSize = 10);

// Attributes (head and tail)
	// count of elements
	int GetCount() const			{ return m_nCount; }
	BOOL IsEmpty() const			{ return m_nCount == 0; }

	// peek at head or tail
	TYPE& GetHead()					{ assert(m_pNodeHead != NULL); return m_pNodeHead->data; }
	TYPE GetHead() const			{ assert(m_pNodeHead != NULL); return m_pNodeHead->data; }
	TYPE& GetTail()					{ assert(m_pNodeTail != NULL); return m_pNodeTail->data; }
	TYPE GetTail() const			{ assert(m_pNodeTail != NULL); return m_pNodeTail->data; }

// Operations
	// get head or tail (and remove it) - don't call on empty list !
	TYPE RemoveHead();
	TYPE RemoveTail();

	// add before head or after tail
	POSITION AddHead(ARG_TYPE newElement);
	POSITION AddTail(ARG_TYPE newElement);

	// add another list of elements before head or after tail
	void AddHead(List* pNewList);
	void AddTail(List* pNewList);

	// remove all elements
	void RemoveAll();

	// iteration
	POSITION GetHeadPosition() const	{ return (POSITION) m_pNodeHead; }
	POSITION GetTailPosition() const	{ return (POSITION) m_pNodeTail; }
	TYPE& GetNext(POSITION& rPosition); // return *Position++
	TYPE GetNext(POSITION& rPosition) const; // return *Position++
	TYPE& GetPrev(POSITION& rPosition); // return *Position--
	TYPE GetPrev(POSITION& rPosition) const; // return *Position--

	// getting/modifying an element at a given position
	TYPE& GetAt(POSITION position);
	TYPE GetAt(POSITION position) const;
	void SetAt(POSITION pos, ARG_TYPE newElement);
	void RemoveAt(POSITION position);

	// inserting before or after a given position
	POSITION InsertBefore(POSITION position, ARG_TYPE newElement);
	POSITION InsertAfter(POSITION position, ARG_TYPE newElement);

	// helper functions (note: O(n) speed)
	POSITION Find(ARG_TYPE searchValue, POSITION startAfter = NULL) const;
		// defaults to starting at the HEAD, return NULL if not found
	POSITION FindIndex(int nIndex) const;
		// get the 'nIndex'th element (may return NULL)

// Implementation
protected:
	CNode* m_pNodeHead;
	CNode* m_pNodeTail;
	int m_nCount;
	CNode* m_pNodeFree;
	struct CPlex* m_pBlocks;
	int m_nBlockSize;

	CNode* NewNode(CNode*, CNode*);
	void FreeNode(CNode*);

public:
	~List();
};

/////////////////////////////////////////////////////////////////////////////
// List<TYPE, ARG_TYPE> inline functions

template<class TYPE, class ARG_TYPE>
inline TYPE& List<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) // return *Position++
{ 
	CNode* pNode = (CNode*) rPosition;
	rPosition = (POSITION) pNode->pNext;
	return pNode->data; 
}

template<class TYPE, class ARG_TYPE>
inline TYPE List<TYPE, ARG_TYPE>::GetNext(POSITION& rPosition) const // return *Position++
{
	CNode* pNode = (CNode*) rPosition;
	rPosition = (POSITION) pNode->pNext;
	return pNode->data; 
}

template<class TYPE, class ARG_TYPE>
inline TYPE& List<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) // return *Position--
{
	CNode* pNode = (CNode*) rPosition;
	rPosition = (POSITION) pNode->pPrev;
	return pNode->data; 
}

template<class TYPE, class ARG_TYPE>
inline TYPE List<TYPE, ARG_TYPE>::GetPrev(POSITION& rPosition) const // return *Position--
{ 
	CNode* pNode = (CNode*) rPosition;
	rPosition = (POSITION) pNode->pPrev;
	return pNode->data; 
}

template<class TYPE, class ARG_TYPE>
inline TYPE& List<TYPE, ARG_TYPE>::GetAt(POSITION position)
{ 
	CNode* pNode = (CNode*) position;
	return pNode->data; 
}

template<class TYPE, class ARG_TYPE>
inline TYPE List<TYPE, ARG_TYPE>::GetAt(POSITION position) const
{ 
	CNode* pNode = (CNode*) position;
	return pNode->data; 
}

template<class TYPE, class ARG_TYPE>
inline void List<TYPE, ARG_TYPE>::SetAt(POSITION pos, ARG_TYPE newElement)
{
	CNode* pNode = (CNode*) pos;
	pNode->data = newElement; 
}

template<class TYPE, class ARG_TYPE>
List<TYPE, ARG_TYPE>::List(int nBlockSize)
{
	assert(nBlockSize > 0);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class TYPE, class ARG_TYPE>
void List<TYPE, ARG_TYPE>::RemoveAll()
{
	// destroy elements
	CNode* pNode;
	for (pNode = m_pNodeHead; pNode != NULL; pNode = pNode->pNext)
		DestructElements<TYPE>(&pNode->data, 1);

	m_nCount = 0;
	m_pNodeHead = m_pNodeTail = m_pNodeFree = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class TYPE, class ARG_TYPE>
List<TYPE, ARG_TYPE>::~List()
{
	RemoveAll();
	assert(m_nCount == 0);
}

/////////////////////////////////////////////////////////////////////////////
// Node helpers
//
// Implementation note: CNode's are stored in CPlex blocks and
//  chained together. Free blocks are maintained in a singly linked list
//  using the 'pNext' member of CNode with 'm_pNodeFree' as the head.
//  Used blocks are maintained in a doubly linked list using both 'pNext'
//  and 'pPrev' as links and 'm_pNodeHead' and 'm_pNodeTail'
//   as the head/tail.
//
// We never free a CPlex block unless the List is destroyed or RemoveAll()
//  is used - so the total number of CPlex blocks may grow large depending
//  on the maximum past size of the list.
//

template<class TYPE, class ARG_TYPE>
List<TYPE, ARG_TYPE>::CNode*
List<TYPE, ARG_TYPE>::NewNode(List::CNode* pPrev, List::CNode* pNext)
{
	if (m_pNodeFree == NULL)
	{
		// add another block
		CPlex* pNewBlock = CPlex::Create(m_pBlocks, m_nBlockSize,
				 sizeof(CNode));

		// chain them into free list
		CNode* pNode = (CNode*) pNewBlock->data();
		// free in reverse order to make it easier to debug
		pNode += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pNode--)
		{
			pNode->pNext = m_pNodeFree;
			m_pNodeFree = pNode;
		}
	}
	assert(m_pNodeFree != NULL);  // we must have something

	List::CNode* pNode = m_pNodeFree;
	m_pNodeFree = m_pNodeFree->pNext;
	pNode->pPrev = pPrev;
	pNode->pNext = pNext;
	m_nCount++;
	assert(m_nCount > 0);  // make sure we don't overflow

	ConstructElements<TYPE>(&pNode->data, 1);
	return pNode;
}

template<class TYPE, class ARG_TYPE>
void List<TYPE, ARG_TYPE>::FreeNode(List::CNode* pNode)
{
	DestructElements<TYPE>(&pNode->data, 1);
	pNode->pNext = m_pNodeFree;
	m_pNodeFree = pNode;
	m_nCount--;
	assert(m_nCount >= 0);  // make sure we don't underflow

	// if no more elements, cleanup completely
	if (m_nCount == 0)
		RemoveAll();
}

template<class TYPE, class ARG_TYPE>
POSITION List<TYPE, ARG_TYPE>::AddHead(ARG_TYPE newElement)
{
	CNode* pNewNode = NewNode(NULL, m_pNodeHead);
	pNewNode->data = newElement;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = pNewNode;
	else
		m_pNodeTail = pNewNode;
	m_pNodeHead = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION List<TYPE, ARG_TYPE>::AddTail(ARG_TYPE newElement)
{
	CNode* pNewNode = NewNode(m_pNodeTail, NULL);
	pNewNode->data = newElement;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = pNewNode;
	else
		m_pNodeHead = pNewNode;
	m_pNodeTail = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void List<TYPE, ARG_TYPE>::AddHead(List* pNewList)
{
	assert(pNewList != NULL);

	// add a list of same elements to head (maintain order)
	POSITION pos = pNewList->GetTailPosition();
	while (pos != NULL)
		AddHead(pNewList->GetPrev(pos));
}

template<class TYPE, class ARG_TYPE>
void List<TYPE, ARG_TYPE>::AddTail(List* pNewList)
{
	assert(pNewList != NULL);

	// add a list of same elements
	POSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

template<class TYPE, class ARG_TYPE>
TYPE List<TYPE, ARG_TYPE>::RemoveHead()
{
	assert(m_pNodeHead != NULL);  // don't call on empty list !!!

	CNode* pOldNode = m_pNodeHead;
	TYPE returnValue = pOldNode->data;

	m_pNodeHead = pOldNode->pNext;
	if (m_pNodeHead != NULL)
		m_pNodeHead->pPrev = NULL;
	else
		m_pNodeTail = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
TYPE List<TYPE, ARG_TYPE>::RemoveTail()
{
	assert(m_pNodeTail != NULL);  // don't call on empty list !!!

	CNode* pOldNode = m_pNodeTail;
	TYPE returnValue = pOldNode->data;

	m_pNodeTail = pOldNode->pPrev;
	if (m_pNodeTail != NULL)
		m_pNodeTail->pNext = NULL;
	else
		m_pNodeHead = NULL;
	FreeNode(pOldNode);
	return returnValue;
}

template<class TYPE, class ARG_TYPE>
POSITION List<TYPE, ARG_TYPE>::InsertBefore(POSITION position, ARG_TYPE newElement)
{
	if (position == NULL)
		return AddHead(newElement); // insert before nothing -> head of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode->pPrev, pOldNode);
	pNewNode->data = newElement;

	if (pOldNode->pPrev != NULL)
		pOldNode->pPrev->pNext = pNewNode;
	else
	{
		assert(pOldNode == m_pNodeHead);
		m_pNodeHead = pNewNode;
	}
	pOldNode->pPrev = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
POSITION List<TYPE, ARG_TYPE>::InsertAfter(POSITION position, ARG_TYPE newElement)
{
	if (position == NULL)
		return AddTail(newElement); // insert after nothing -> tail of the list

	// Insert it before position
	CNode* pOldNode = (CNode*) position;
	CNode* pNewNode = NewNode(pOldNode, pOldNode->pNext);
	pNewNode->data = newElement;

	if (pOldNode->pNext != NULL)
		pOldNode->pNext->pPrev = pNewNode;
	else
	{
		assert(pOldNode == m_pNodeTail);
		m_pNodeTail = pNewNode;
	}
	pOldNode->pNext = pNewNode;
	return (POSITION) pNewNode;
}

template<class TYPE, class ARG_TYPE>
void List<TYPE, ARG_TYPE>::RemoveAt(POSITION position)
{
	CNode* pOldNode = (CNode*) position;

	// remove pOldNode from list
	if (pOldNode == m_pNodeHead)
		m_pNodeHead = pOldNode->pNext;
	else
		pOldNode->pPrev->pNext = pOldNode->pNext;
	if (pOldNode == m_pNodeTail)
		m_pNodeTail = pOldNode->pPrev;
	else
		pOldNode->pNext->pPrev = pOldNode->pPrev;
	FreeNode(pOldNode);
}

template<class TYPE, class ARG_TYPE>
POSITION List<TYPE, ARG_TYPE>::FindIndex(int nIndex) const
{
	if (nIndex >= m_nCount || nIndex < 0)
		return NULL;  // went too far

	CNode* pNode = m_pNodeHead;
	while (nIndex--)
		pNode = pNode->pNext;
	return (POSITION) pNode;
}

template<class TYPE, class ARG_TYPE>
POSITION List<TYPE, ARG_TYPE>::Find(ARG_TYPE searchValue, POSITION startAfter) const
{
	CNode* pNode = (CNode*) startAfter;
	if (pNode == NULL)
		pNode = m_pNodeHead;  // start at head
	else
		pNode = pNode->pNext;  // start after the one specified

	for (; pNode != NULL; pNode = pNode->pNext)
		if (CompareElements<TYPE>(&pNode->data, &searchValue))
			return (POSITION)pNode;
	return NULL;
}

#endif
