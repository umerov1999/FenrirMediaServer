#if !defined(__XTPCOMBOBOXNOTEDITED__)
#	define __XTPCOMBOBOXNOTEDITED__
//}}AFX_CODEJOCK_PRIVATE

#	if _MSC_VER > 1000
#		pragma once
#	endif // _MSC_VER > 1000

#	include "Common/Base/Diagnostic/XTPDisableNoisyWarnings.h"

class CXTPComboBoxNotEdited : public CComboBox
{
	DECLARE_DYNCREATE(CXTPComboBoxNotEdited)
public:
	CXTPComboBoxNotEdited();
	virtual ~CXTPComboBoxNotEdited();
protected:
	afx_msg void PreSubclassWindow();

	DECLARE_MESSAGE_MAP()
};

#	include "Common/Base/Diagnostic/XTPEnableNoisyWarnings.h"
#endif // !defined(__XTPCOMBOBOXNOTEDITED__)