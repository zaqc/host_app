#include <cstdio>
#include "HelperFunctions.h"
#ifdef WINDOWS_OS
#pragma comment (lib, "Rpcrt4.lib")
#endif
using namespace HelperFunctions;

const std::basic_string<TCHAR> kSpaces = TEXT(" \t\r\n");

#ifdef WINDOWS_OS

#ifdef MessageBoxResourceH
HHOOK CustomMessageBox::m_Hook;
HINSTANCE CustomMessageBox::m_HInstance;
#endif

#endif
