// DO NOT MODIFY MANUALLY
// Generated on Tue 07/27/2021 at 22:36:12.97 using cj-update-iids.cmd

// XTPImageManagerIIDs.h: GUID definitions for ImageManager.odl.
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

#ifndef __XTPImageManagerIIDs_H__
#define __XTPImageManagerIIDs_H__

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

XTP_DEFINE_GUID(DIID_ImageManagerIcon,0xC0DE2010,0xAE6D,0x46a8,0xBC,0x77,0xCE,0xE7,0xD5,0xCE,0x9E,0xD7);
XTP_DEFINE_GUID(DIID_ImageManagerIconSet,0xC0DE2010,0x374B,0x4AED,0x93,0x06,0x0B,0x7A,0xC4,0x34,0x3B,0xCC);
XTP_DEFINE_GUID(DIID_IImageManagerIcons,0xC0DE2010,0xCD87,0x48F0,0xAD,0x57,0x63,0x68,0x2A,0x2F,0x3F,0xCC);
XTP_DEFINE_GUID(CLSID_ImageManagerIcons,0xC0DE2010,0x4290,0x47A7,0x8B,0x05,0x90,0xAC,0x97,0x9F,0x12,0xD5);
XTP_DEFINE_GUID(DIID_IResourceImageList,0xC0DE2010,0xF9CE,0x44F1,0x89,0x91,0x0F,0x6B,0x7C,0x19,0x33,0x6B);

#undef XTP_DEFINE_GUID

// clang-format on

#endif // __XTPImageManagerIIDs_H__
