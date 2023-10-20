#include "StdAfx.h"

#include "SkinFramework/ApiHook/XTPSkinManagerApiHookConfig.h"

#ifdef _M_IX86

#	if _MSC_VER >= 1931
#		pragma warning(push)
// warning C5251: data_seg changed after including header
#		pragma warning(disable : 5251)
#	endif

#	include "disasm.hpp"

#	if _MSC_VER >= 1931
#		pragma warning(pop)
#	endif

#endif
