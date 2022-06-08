// XTPMarkupTypedSimpleStack.h: interface for the CXTPMarkupTypedSimpleStack class.
//
// (c)1998-2021 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPMARKUPTYPEDSIMPLESTACK_H__)
#	define __XTPMARKUPTYPEDSIMPLESTACK_H__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

template<class TYPE>
class CXTPMarkupTypedSimpleStack
{
public:
	CXTPMarkupTypedSimpleStack()
	{
		m_pHead = NULL;
		m_pTail = NULL;
	}

	BOOL IsEmpty() const
	{
		return m_pHead == NULL;
	}

	void AddTail(TYPE* pPart)
	{
		if (m_pTail != NULL)
		{
			m_pTail->m_pNextChain = pPart;
		}
		else
		{
			m_pTail = m_pHead = pPart;
		}
		m_pTail = pPart;
	}

	void AddHead(TYPE* pPart)
	{
		if (m_pHead != NULL)
		{
			pPart->m_pNextChain = m_pHead;
			m_pHead				= pPart;
		}
		else
		{
			m_pTail = m_pHead = pPart;
		}
	}

	TYPE* RemoveHead()
	{
		ASSERT(m_pHead);
		if (!m_pHead)
			return NULL;

		TYPE* pHead = m_pHead;

		if (m_pHead == m_pTail)
		{
			m_pHead = m_pTail = NULL;
		}
		else
		{
			m_pHead = pHead->m_pNextChain;
			ASSERT(m_pHead);
		}

		pHead->m_pNextChain = NULL;

		return pHead;
	}

	void RemoveAll()
	{
		m_pHead = m_pTail = NULL;
	}

	TYPE* GetHead() const
	{
		return m_pHead;
	}

	TYPE* GetTail() const
	{
		return m_pTail;
	}

	BOOL Remove(TYPE* p)
	{
		ASSERT(p != NULL);
		if (!p)
			return FALSE;

		if (m_pHead == m_pTail)
		{
			if (p != m_pHead)
				return FALSE;

			p->m_pNextChain = NULL;
			m_pHead = m_pTail = NULL;
			return TRUE;
		}

		if (m_pHead == p)
		{
			m_pHead			= p->m_pNextChain;
			p->m_pNextChain = NULL;
			return TRUE;
		}

		TYPE* pTest = m_pHead;
		while (pTest != NULL && pTest->m_pNextChain != p)
			pTest = pTest->m_pNextChain;

		if (pTest != NULL)
		{
			if (m_pTail == p)
				m_pTail = pTest;
			pTest->m_pNextChain = p->m_pNextChain;
			p->m_pNextChain		= NULL;
			return TRUE;
		}
		return FALSE;
	}

protected:
	TYPE* m_pHead;
	TYPE* m_pTail;
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPMARKUPTYPEDSIMPLESTACK_H__)
