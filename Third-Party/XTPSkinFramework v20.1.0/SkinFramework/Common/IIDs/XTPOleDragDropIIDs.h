// DO NOT MODIFY MANUALLY
// Generated on Tue 07/27/2021 at 22:36:13.79 using cj-update-iids.cmd

// XTPOleDragDropIIDs.h: GUID definitions for OleDragDrop.odl.
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

#ifndef __XTPOleDragDropIIDs_H__
#define __XTPOleDragDropIIDs_H__

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

XTP_DEFINE_GUID(DIID_DataObjectFiles,0xC0DE2010,0x604D,0x4242,0x95,0x4B,0xDF,0x38,0xBB,0xFA,0x8B,0xE3);
XTP_DEFINE_GUID(DIID_DataObject,0xC0DE2010,0x300E,0x4EB2,0xAE,0x9B,0x6F,0x1B,0xA0,0x25,0xDF,0x31);

#undef XTP_DEFINE_GUID

// clang-format on

#endif // __XTPOleDragDropIIDs_H__
