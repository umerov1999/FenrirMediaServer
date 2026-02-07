// DO NOT MODIFY MANUALLY
// Generated on Mon 03/06/2023 at  3:56:17.95 using cj-update-iids.cmd

// XTPPrintingIIDs.h: GUID definitions for Printing.odl.
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

#ifndef __XTPPrintingIIDs_H__
#define __XTPPrintingIIDs_H__

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

XTP_DEFINE_GUID(DIID_XtremePrinterInfo,0xC0DE2210,0xED8F,0x4ce1,0x9E,0x36,0x40,0x38,0x76,0xBE,0xE5,0x57);
XTP_DEFINE_GUID(DIID_XtremePrintPreviewOptions,0xC0DE2210,0x8BC5,0x4677,0x99,0x99,0x0A,0x29,0xB4,0x4F,0x3B,0x23);
XTP_DEFINE_GUID(DIID_PrintPageHeaderFooter,0xC0DE2210,0xE75C,0x4a50,0xA9,0x08,0xF3,0x6D,0x35,0xF0,0x98,0xD2);
XTP_DEFINE_GUID(DIID_PrintInfo,0xC0DE2210,0x0E2C,0x47FC,0xBF,0xB7,0x46,0xE7,0x0C,0x56,0x1A,0xA8);

#undef XTP_DEFINE_GUID

// clang-format on

#endif // __XTPPrintingIIDs_H__

