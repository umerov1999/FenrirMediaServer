// DO NOT MODIFY MANUALLY
// Generated on Mon 03/06/2023 at  3:56:15.50 using cj-update-iids.cmd

// XTPColorManagerIIDs.h: GUID definitions for ColorManager.odl.
//
// (c)1998-2023 Codejock Software, All Rights Reserved.
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

#ifndef __XTPColorManagerIIDs_H__
#define __XTPColorManagerIIDs_H__

// clang-format off

#define XTP_DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
       EXTERN_C __declspec(selectany) const IID XTP##name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}; \
       const unsigned long  XTP##name##__Data1   = (l);  \
       const unsigned short XTP##name##__Data2   = (w1); \
       const unsigned short XTP##name##__Data3   = (w2); \
       const unsigned char  XTP##name##__Data4_0 = (b1); \
       const unsigned char  XTP##name##__Data4_1 = (b2); \
       const unsigned char  XTP##name##__Data4_2 = (b3); \
       const unsigned char  XTP##name##__Data4_3 = (b4); \
       const unsigned char  XTP##name##__Data4_4 = (b5); \
       const unsigned char  XTP##name##__Data4_5 = (b6); \
       const unsigned char  XTP##name##__Data4_6 = (b7); \
       const unsigned char  XTP##name##__Data4_7 = (b8);

XTP_DEFINE_GUID(DIID_IColorManager,0xC0DE2210,0xDE43,0x4b56,0xBC,0xEB,0xE1,0x4B,0x82,0x5C,0xF2,0x45);
XTP_DEFINE_GUID(CLSID_ColorManager,0xC0DE2210,0xF3FE,0x46E7,0xBB,0x64,0xDB,0x03,0x3F,0x43,0x9F,0x8B);

#undef XTP_DEFINE_GUID

// clang-format on

#endif // __XTPColorManagerIIDs_H__

