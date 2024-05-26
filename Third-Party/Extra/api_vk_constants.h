#pragma once

#define VKAPI_VERSION "5.186"
#define VKAPI_VERSION_AUTH VKAPI_VERSION
#define VKAPI_VERSION_FOR_AUDIO "5.90"

#define KATE_VERSION_STRING "114"
#define KATE_VERSION_CODE "560"

#define ANDROID_VERSION_STRING "8.15"
#define ANDROID_VERSION_CODE "15271"

//////////////////////////////////////////////////////////////////////////////////////////

#define KATE_USERAGENT "KateMobileAndroid/" KATE_VERSION_STRING " lite-" KATE_VERSION_CODE " (Android 14; SDK 34; arm64-v8a; SAMSUNG SM-G980F; ru; 3200x1440)"
#define ANDROID_USERAGENT "VKAndroidApp/" ANDROID_VERSION_STRING "-" ANDROID_VERSION_CODE " (Android 14; SDK 34; arm64-v8a; SAMSUNG SM-G980F; ru; 3200x1440)"

#define KATE_USERAGENT_DEVICED(device) (std::string("KateMobileAndroid/" KATE_VERSION_STRING " lite-" KATE_VERSION_CODE " (Android 14; SDK 34; arm64-v8a; ") + device + std::string("; ru; 3200x1440)"))
#define ANDROID_USERAGENT_DEVICED(device) (std::string("VKAndroidApp/" ANDROID_VERSION_STRING "-" ANDROID_VERSION_CODE " (Android 14; SDK 34; arm64-v8a; ") + device + std::string("; ru; 3200x1440)"))

#define IPHONE_USERAGENT "com.vk.vkclient/3893 (iPhone, iOS 16.1, iPhone11,2, Scale/3.0)"
#define IPAD_USERAGENT "com.vk.vkclient/3893 (iPhone, iOS 16.1, iPhone11,2, Scale/3.0)"