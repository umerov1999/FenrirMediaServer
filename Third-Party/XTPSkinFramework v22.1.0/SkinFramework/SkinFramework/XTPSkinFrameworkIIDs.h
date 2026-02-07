// DO NOT MODIFY MANUALLY
// Generated on Mon 03/06/2023 at  3:56:26.91 using cj-update-iids.cmd

// XTPSkinFrameworkIIDs.h: GUID definitions for SkinFramework.odl.
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

#ifndef __XTPSkinFrameworkIIDs_H__
#define __XTPSkinFrameworkIIDs_H__

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

XTP_DEFINE_GUID(LIBID_XtremeSkinFramework,0xBD0C1912,0x66C3,0x49CC,0x8B,0x12,0x7B,0x34,0x7B,0xF6,0xC8,0x46);
XTP_DEFINE_GUID(DIID_SkinIniFile,0xC0DE2210,0x1038,0x498E,0xA9,0x36,0x36,0x1F,0x08,0xB4,0xC4,0xAA);
XTP_DEFINE_GUID(DIID_SkinDescription,0xC0DE2210,0x4834,0x499E,0x95,0xB4,0x30,0xE0,0xC3,0xF4,0x3A,0x10);
XTP_DEFINE_GUID(DIID_SkinDescriptions,0xC0DE2210,0xFB37,0x4B08,0xA2,0xB9,0xF6,0xF8,0xB2,0x53,0xD6,0x97);
XTP_DEFINE_GUID(DIID_SkinSchema,0xC0DE2210,0xC56F,0x43C0,0xBC,0xF1,0x81,0x93,0xB3,0x5F,0xE4,0xC4);
XTP_DEFINE_GUID(DIID__DSkinFramework,0xC0DE2210,0x28D7,0x4F2C,0x87,0xA7,0x72,0x66,0x36,0x7B,0x46,0x55);
XTP_DEFINE_GUID(DIID__DSkinFrameworkEvents,0xC0DE2210,0x4463,0x4030,0xB3,0x24,0xAC,0x6A,0x80,0x75,0xFE,0xC8);
XTP_DEFINE_GUID(CLSID_SkinFramework,0xC0DE2210,0x2217,0x42EE,0xB1,0xB0,0x82,0xC8,0x90,0x43,0x1F,0x17);
XTP_DEFINE_GUID(DIID_ISkinFrameworkGlobalSettings,0xC0DE2210,0xCA9A,0x4AA9,0x86,0x01,0x0A,0xB7,0xF5,0x51,0xAA,0x3D);
XTP_DEFINE_GUID(CLSID_SkinFrameworkGlobalSettings,0xC0DE2210,0xF744,0x4373,0xB3,0x8D,0x29,0xCE,0x83,0xEF,0x0E,0xE5);

#undef XTP_DEFINE_GUID

// clang-format on

#endif // __XTPSkinFrameworkIIDs_H__

