#ifndef LOG_H_
#define LOG_H_

#include "common_def.h"

#ifdef __ANDROID_API__
#include <android/log.h>
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif
#ifdef LINUX_OS
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
static void __linux_log_print(int priority, const char* Tag, const char* Msg, ...)
{
	va_list vl;
	va_start(vl, Msg);
	char message[512];
	sprintf_s(message, sizeof(message) / sizeof(message[0]), Msg, vl);
	va_end(vl);
	std::cout << message << std::endl;
}
#define  LOGI(...)  __linux_log_print(1, LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __linux_log_print(2, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __linux_log_print(3, LOG_TAG, __VA_ARGS__)
#endif

#ifdef WINDOWS_OS
#include <stdarg.h>
#include <stdio.h>

static void __windows_log_print(int priority, const char* Tag, const char* Msg, ...)
{
	va_list vl;
	va_start(vl, Msg);
	char message[512];
	sprintf_s(message, sizeof(message) / sizeof(message[0]), Msg, vl);
	va_end(vl);
}
#define  LOGI(...)  __windows_log_print(1, LOG_TAG, __VA_ARGS__)
#define  LOGW(...)  __windows_log_print(2, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __windows_log_print(3, LOG_TAG, __VA_ARGS__)
#endif

#include <GLES2/gl2.h>

#define  LOG_TAG    "GurUSPad"

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) 
	{
        LOGE("after %s glError (0x%x)\n", op, error);
    }
}

#endif
