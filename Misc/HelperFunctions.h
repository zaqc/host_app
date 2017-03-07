
#pragma once
#ifndef HelperFunctionsH
#define HelperFunctionsH

#include "common_def.h"

#ifdef WINDOWS_OS
	#ifndef _MSC_VER
	#include <windows.h>
	#include <WinSock2.h>
	#endif

	#include <ShlObj.h>
	#include <Dwmapi.h>
	#include <WinIoCtl.h>
	//#include "Display.h"

	#ifdef NOMINMAX
	namespace Gdiplus
	{
		using std::min;
		using std::max;
	}
	#endif
	#include <tchar.h>
	#include <shellapi.h>
	#ifndef _MSC_VER
	#include <dinkumware/functional>
	#include <Classes.hpp>
	#else
	#include <functional>
	#endif
	#include <intsafe.h>
	#include <process.h>

	#pragma warning(disable : 4351) //warning C4351: new behavior: elements of array suppressing
	//bracodelgo's warning 8022 suppressing
	#ifndef _MSC_VER
	#pragma warn -hid
	#endif
	#include <gdiplus.h>
	#ifndef _MSC_VER
	#pragma warn .hid
	#endif

	#pragma comment (lib, "shell32.lib")
	#pragma comment (lib, "advapi32.lib")
	#pragma comment (lib, "Dwmapi.lib")
#else
	#include <libgen.h>
	#include <functional>
	#include <thread>
	#include <mutex>
	#include <pthread.h>
	#include <unistd.h>
	#include "Events.h"
#endif

#include <climits>
#include <locale>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <errno.h>
#include <vector>
#include <utility>
#include <cctype>
#include <cstdio>
#include <cstdarg>
#include <cstdint>

#ifndef UINT64_MAX
#define UINT64_MAX 0xFFFFFFFFFFFFFFFF
#endif
#define MS_VC_EXCEPTION 0x406D1388

#define DIGITS TEXT("0123456789-")

#ifdef __BORLANDC__
#define nullptr NULL
#endif

#ifdef LINUX_OS
#define TCHAR wchar_t
#define TEXT(quote) L##quote
#endif
extern const std::basic_string<TCHAR> kSpaces;

template< typename A >
void inline SAFE_DELETE(A &a)
{
	if ( (a) != NULL ) {
		delete (a);
		a = NULL;
	}
}

template< typename A >
void inline SAFE_RELEASE(A& a)
{
	if ( (a) != NULL )
	{
		a->Release();
		a = NULL;
	}
}

template< typename A >
void inline SAFE_ARRAY_DELETE(A& a)
{
	if ((a) != NULL)
	{
		delete[] (a);
		(a) = NULL;
	}
}
#ifdef WINDOWS_OS
BOOL inline SAFE_CLOSE_HANDLE(HANDLE& a)
{
	BOOL res = FALSE;
	if (a != INVALID_HANDLE_VALUE)
	{
		res = CloseHandle(a);
		a = INVALID_HANDLE_VALUE;
	}

	return res;
}
#endif
#define LogThrow(s) {Log(s); throw s;}

//#define StlString std::basic_string<TCHAR>
typedef std::basic_string<TCHAR> StlString;
namespace HelperFunctions
{
#ifdef WINDOWS_OS
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#endif

class String
{
public:
	static StlString Format(const TCHAR* fmt, ...)
	{
		int size = 512 * 2;
		TCHAR* buffer = 0;
		buffer = new TCHAR[size];
		va_list vl;
		va_start(vl,fmt);
#ifdef WINDOWS_OS
		#ifdef _MSC_VER
			int nsize = _vsntprintf_s(buffer, size, size, fmt, vl);
		#else
			#ifdef _UNICODE
				int nsize = _vsnwprintf(buffer, size, fmt, vl);
			#else
				int nsize = _vsnprintf(buffer, size, fmt, vl);
			#endif
		#endif
#else
		int nsize = vswprintf(buffer, size, fmt, vl);
#endif
		if(size<=nsize)
		{//fail delete buffer and try again
			delete [] buffer;
			buffer = 0;
			buffer = new TCHAR[nsize+1];//+1 for /0
#ifdef WINDOWS_OS
			#ifdef _MSC_VER
				nsize = _vsntprintf_s(buffer, size, size, fmt, vl);
			#else
				#ifdef _UNICODE
					nsize = _vsnwprintf(buffer, size, fmt, vl);
				#else
					nsize = _vsnprintf(buffer, size, fmt, vl);
				#endif
			#endif
#else
			nsize = vswprintf(buffer, size, fmt, vl);
#endif
		}
		StlString ret(buffer);
		va_end(vl);
		delete [] buffer;
		return ret;
	}
	static std::string FormatA(const char* fmt, ...)
	{
		int size = 512 * 2;
		char* buffer = 0;
		buffer = new char[size];
		va_list vl;
		va_start(vl,fmt);
#ifdef WINDOWS_OS
		#ifdef _MSC_VER
			int nsize = _vsnprintf_s(buffer, size, size, fmt, vl);
		#else
			int nsize = _vsnprintf(buffer, size, fmt, vl);
		#endif
#else
		int nsize = vsprintf(buffer, fmt, vl);
#endif
		if(size<=nsize)
		{//fail delete buffer and try again
			delete [] buffer;
			buffer = 0;
			buffer = new char[nsize+1];//+1 for /0
#ifdef WINDOWS_OS
			#ifdef _MSC_VER
				nsize = _vsnprintf_s(buffer, size, size, fmt, vl);
			#else
				nsize = _vsnprintf(buffer, size, fmt, vl);
			#endif
#else
			nsize = vsprintf(buffer, fmt, vl);
#endif
		}
		std::string ret(buffer);
		va_end(vl);
		delete [] buffer;
		return ret;
	}

	static std::string WcToMb(const std::wstring &aSourceString )
	{
		std::string strRet;
		strRet.resize(aSourceString.size() + 1);
#ifdef WINDOWS_OS
		WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, aSourceString.c_str(),
				aSourceString.size() + 1, &strRet[0], aSourceString.size(),
				NULL, NULL);
#else
		wcstombs(&strRet[0], aSourceString.c_str(), strRet.size());
#endif
		while( !strRet.empty() && *(strRet.end() - 1) == TEXT('\0') )
			strRet.erase( strRet.end() - 1 );
		return strRet;
	}

	static std::wstring MbToWc(const std::string &aSourceString )
	{
		std::wstring strRet;
		strRet.resize(aSourceString.size() + 1);
#ifdef WINDOWS_OS
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, aSourceString.c_str(),
				aSourceString.size() + 1, &strRet[0], aSourceString.size());
#else
		mbstowcs(&strRet[0], aSourceString.c_str(), strRet.size());
#endif
		while( !strRet.empty() && *(strRet.end() - 1) == TEXT('\0') )
			strRet.erase( strRet.end() - 1 );
		return strRet;
	}
#ifdef WINDOWS_OS
	static StlString LoadFromResource(HINSTANCE hInstance, UINT uID) 
	{
		TCHAR p[512]{0};
		int len = ::LoadString( hInstance, uID, p, sizeof(p) / sizeof(p[0]));
		if ( len > 0 ) {
			return StlString( p, static_cast<size_t>( len ) );
		} else {
			return StlString();
		}
	}
#endif
	static StlString trim_right(const StlString &s,
			const StlString & t = kSpaces)
	{
		StlString d (s);
		StlString::size_type i (d.find_last_not_of (t));
		if (i == std::string::npos)
			return TEXT("");
		else
			return d.erase (d.find_last_not_of (t) + 1) ;
	}
	static StlString trim_left(const StlString &s,
			const StlString & t = kSpaces)
	{
		StlString d (s);
		return d.erase (0, s.find_first_not_of (t)) ;
	}

	static StlString trim (const StlString & s,
			const StlString & t = kSpaces)
	{
		StlString d( s );
		return trim_left (trim_right (d, t), t) ;
	}

	static void trim_final_zeros( StlString& s )
	{
		while( !s.empty() && *(s.end() - 1) == TEXT('\0') )
			s.erase( s.end() - 1  );
	}
	template<class T>
	static T fromStringTo(const StlString& aString)
	{
		std::basic_istringstream<TCHAR> stream (aString);
		T temp;
		stream >> temp;
		return temp;
	}

	/////
	// Удаляет из строки все символы за исключением символов,
	// содержащихся в строке aSymbolsToLeave
	////
	static void LeaveSymbols(StlString& aSourceString,
			const StlString& aSymbolsToLeave )
	{
		size_t npos = 0;
		do
		{
			npos = aSourceString.find_first_not_of( aSymbolsToLeave, npos );
			if( npos != aSourceString.npos )
				aSourceString.erase( npos, 1 );
		} while ( npos != aSourceString.npos );
	}
	/////
	// Удаляет из строки все символы, содержащиеся в строке aSymbolsToRemove
	////
	static void RemoveSymbols(StlString& aSourceString,
			const StlString& aSymbolsToRemove )
	{
		size_t npos = 0;
		do
		{
			npos = aSourceString.find_first_of( aSymbolsToRemove, npos );
			if( npos != aSourceString.npos )
				aSourceString.erase( npos, 1 );
		} while ( npos != aSourceString.npos );
	}
	/////
	// Удаляет из строки все символы, содержащиеся в строке aSymbolsToRemove
	////
	static void RemoveSymbolsA(std::string& aSourceString,
			const std::string& aSymbolsToRemove )
	{
		size_t npos = 0;
		do
		{
			npos = aSourceString.find_first_of( aSymbolsToRemove, npos );
			if( npos != aSourceString.npos )
				aSourceString.erase( npos, 1 );
		} while ( npos != aSourceString.npos );
	}

	/////
	// Удаляет из строки все символы за исключением цифр и знака "-"
	////
	static void LeaveOnlyDigits( StlString& aSourceString )
	{
		LeaveSymbols( aSourceString, DIGITS );
	}
	typedef std::istream_iterator<StlString, TCHAR, std::char_traits<TCHAR> > WStream_Iterator;

	//разбивает строку на массив слов, разделенных пробелом
	static std::vector<StlString> SplitToWords(const StlString& aSourceString)
	{
		std::basic_istringstream<TCHAR> ss(aSourceString);
		WStream_Iterator begin(ss);
		WStream_Iterator end;
		std::vector<StlString> words(begin, end);
		return words;
	}
#ifndef LINUX_OS
	static int ToInt( const StlString& aString )
	{
		int Result = INT_MIN;
		Result = _ttoi( aString.c_str() );
		if (errno == EINVAL)
		{
			return INT_MIN;
		}
		return Result;
	}
	static int64_t ToInt64( const StlString& aString )
	{
		int64_t Result = INT64_MIN;
		Result =  _ttoi64( aString.c_str() );
		if (errno == EINVAL)
		{
			return INT_MIN;
		}
		return Result;
	}

	static GUID ToGuid( const TCHAR* aString )
	{
		GUID Result = GUID_NULL;
#ifdef UNICODE
		if( UuidFromString( (RPC_WSTR)aString, &Result ) == RPC_S_OK )
			return Result;
#else
		if( UuidFromString( (RPC_CSTR)aString, &Result ) == RPC_S_OK )
			return Result;
#endif
		return GUID_NULL;
	}
#endif

	static StlString ToString( int aValue )
	{
		std::basic_ostringstream<TCHAR> stream;
		stream << aValue;
		return stream.str();

		//StlString Result;
		//Result.resize( 256, 0 );

		//#ifdef _MSC_VER
		//_itot_s( aValue, &Result[0], 256, 10 );
		//Result.shrink_to_fit();
		//#else
		//_itot( aValue, &Result[0], 10 );
		//#endif
		//StlString tmpStr = Result;
		//trim_final_zeros( tmpStr );
		//return tmpStr;
	}

	static StlString ToString( int64_t aValue )
	{
		std::basic_ostringstream<TCHAR> stream;
		stream << aValue;
		return stream.str();
	}

	static bool CheckLastSymbol(const StlString &aString, TCHAR aSymbol)
	{
		bool res = false;

		if (!aString.empty())
		{
			TCHAR lastSymbol = aString[ aString.size() - 1 ];
			res = (aSymbol == lastSymbol) ? true : false;
		}

		return res;
	}
#ifndef LINUX_OS
	static StlString ToString( const GUID* aGuid )
	{
#ifdef  UNICODE
		RPC_WSTR guid_str;
#else
		RPC_CSTR guid_str;
#endif
		StlString res;
		if( UuidToString(aGuid, &guid_str) == RPC_S_OK )
		{
			res = (TCHAR*)guid_str;
			RpcStringFree(&guid_str);
			ToUpperCase( res );
		}
		else
			res = TEXT( "" );
		return res;
	}
#endif
	static void ToUpperCase( StlString& aString )
	{
		std::locale locDef;
		std::locale loc("Russian_Russia.1251");
		std::locale locComb(locDef, loc, std::locale::ctype);

		std::transform( aString.begin(), aString.end(), aString.begin()
			, std::bind2nd(std::ptr_fun(&std::toupper<wchar_t>), locComb) );
	}

	static void ToLowerCase( StlString& aString )
	{
		std::locale locDef;
		std::locale loc("Russian_Russia.1251");
		std::locale locComb(locDef, loc, std::locale::ctype);

		std::transform( aString.begin(), aString.end(), aString.begin()
			, std::bind2nd(std::ptr_fun(&std::tolower<wchar_t>), locComb) );
	}
#ifdef WINDOWS_OS
	typedef HRESULT (*StringComGetter)(BSTR* aString);
#endif
//	static std::wstring StrFromComInterface(StringComGetter aCallback)
//	{
//		std::wstring str;
//		BSTR bstr;
//
//		if(S_OK == aCallback(&bstr))
//		{
//			_bstr_t _bstr(bstr, true);
//			str = static_cast<LPCWSTR>(_bstr);
//
//			::SysFreeString(bstr);
//		}
//
//		return str;
//	}
};


class File
{
public:
#ifdef WINDOWS_OS
	static StlString	GetFilePath( const StlString& aFullFilename )
	{
		TCHAR sDrive[10]{0};
		TCHAR sPath[ MAX_PATH ]{0};
		TCHAR sName[MAX_PATH]{0};
		TCHAR sExt[MAX_PATH]{0};
		#ifdef _MSC_VER
		_tsplitpath_s(aFullFilename.c_str(), sDrive, _countof(sDrive), sPath,
				_countof(sPath), sName, _countof( sName), sExt, _countof(sExt));
		#else
		_wsplitpath( aFullFilename.c_str(), sDrive, sPath, sName, sExt );
		#endif

		StlString strRet = sDrive;
		strRet += sPath;
		return strRet;
	}

	static StlString	GetApplicationExeName( HINSTANCE aInstance = NULL )
	{
		TCHAR szPath[MAX_PATH]{0};
		::GetModuleFileName( aInstance, szPath, MAX_PATH-1 );
		StlString strRet = szPath;
		return strRet;
	}

	static StlString	GetApplicationExePath(HINSTANCE aInstance = NULL)
	{
		return GetFilePath( GetApplicationExeName( aInstance ) );
	}

	static std::wstring GetFileName( const std::wstring& aFullFilename )
	{
		wchar_t sDrive[10]{0};
		wchar_t sPath[MAX_PATH]{0};
		wchar_t sName[MAX_PATH]{0};
		wchar_t sExt[MAX_PATH]{0};
		#ifdef _MSC_VER
		_wsplitpath_s(aFullFilename.c_str(), sDrive, _countof(sDrive), sPath,
				_countof(sPath), sName, _countof(sName), sExt, _countof(sExt));
		#else
		_wsplitpath( aFullFilename.c_str(), sDrive, sPath, sName, sExt );
		#endif
		std::wstring strRet = sName;
		strRet += sExt;
		return strRet;
	}

	static StlString GetFileNameWithoutExtension(const StlString& aFullFilename)
	{
		TCHAR sDrive[10]{0};
		TCHAR sPath[ MAX_PATH ]{0};
		TCHAR sName[MAX_PATH]{0};
		TCHAR sExt[MAX_PATH]{0};
		#ifdef _MSC_VER
		_tsplitpath_s(aFullFilename.c_str(), sDrive, _countof(sDrive), sPath,
				_countof(sPath), sName, _countof( sName), sExt, _countof(sExt));
		#else
		_wsplitpath( aFullFilename.c_str(), sDrive, sPath, sName, sExt );
		#endif

		StlString strRet = sName;
		return strRet;
	}

	static StlString GetFileExtension( const StlString& aFullFilename )
	{
		TCHAR sDrive[10]{0};
		TCHAR sPath[ MAX_PATH ]{0};
		TCHAR sName[MAX_PATH]{0};
		TCHAR sExt[MAX_PATH]{0};
		#ifdef _MSC_VER
		_tsplitpath_s( aFullFilename.c_str(), sDrive, _countof(sDrive), sPath, _countof(sPath), sName, _countof( sName), sExt, _countof(sExt) );
		#else
		_wsplitpath( aFullFilename.c_str(), sDrive, sPath, sName, sExt );
		#endif

		StlString strRet = sExt;
		return strRet;
	}

	static StlString	GetDriveLetter( const StlString& aPath )
	{
		TCHAR sDrive[10]{0};
		TCHAR sPath[ MAX_PATH ]{0};
		TCHAR sName[MAX_PATH]{0};
		TCHAR sExt[MAX_PATH]{0};
		#ifdef _MSC_VER
		_tsplitpath_s( aPath.c_str(), sDrive, _countof(sDrive), sPath, _countof(sPath), sName, _countof( sName), sExt, _countof(sExt) );
		#else
		_wsplitpath( aPath.c_str(), sDrive, sPath, sName, sExt );
		#endif

		StlString strRet = sDrive;
		return strRet;
	}

	static DWORD DetectSectorSize(WCHAR * devName, PSTORAGE_ACCESS_ALIGNMENT_DESCRIPTOR pAlignmentDescriptor)
	{
		DWORD                  Bytes = 0;
		BOOL                   bReturn = FALSE;
		DWORD                  Error = NO_ERROR;
		STORAGE_PROPERTY_QUERY Query;

		ZeroMemory(&Query, sizeof(Query));

		HANDLE  hFile = CreateFileW(devName,
			STANDARD_RIGHTS_READ,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile == INVALID_HANDLE_VALUE) 
		{
			Error = GetLastError();
			return Error;
		}

		Query.QueryType = PropertyStandardQuery;
		Query.PropertyId = StorageAccessAlignmentProperty;

		bReturn = DeviceIoControl(hFile,
			IOCTL_STORAGE_QUERY_PROPERTY,
			&Query,
			sizeof(STORAGE_PROPERTY_QUERY),
			pAlignmentDescriptor,
			sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR),
			&Bytes,
			NULL);

		if (bReturn == FALSE)
			Error = GetLastError();

		CloseHandle(hFile);
		return Error;
	}

	static bool EnsurePathEndingBackSlash( StlString& aPath )
	{
		if( aPath.size() < 1 )
			return false;

		if( aPath[aPath.size() - 1] != TEXT('\\') )
			aPath += TEXT("\\");

		return true;
	}

	static bool CheckPathIsValid( const TCHAR* aFilepath )
	{
		return false;
	}

	static bool	TestFileExist( const StlString& aFilename )
	{
		HANDLE hFile = CreateFile( aFilename.c_str(), 0, 0, 0, OPEN_EXISTING, 0, 0);
		bool bExist = INVALID_HANDLE_VALUE != hFile;
		if( bExist )
			CloseHandle( hFile );
		return bExist;
	}

	static StlString AddUniqueIndexToFilename( const StlString& aFilename, int aStartIdx = 1 )
	{
		StlString strResult = aFilename;
		if( File::TestFileExist( aFilename ) )
		{
			StlString strExt = GetFileExtension( aFilename );
			StlString strName = GetFilePath( aFilename ) + GetFileNameWithoutExtension( aFilename );

			do
			{
				TCHAR sPrefix[16]{0};
				wsprintf( sPrefix, TEXT("_%d"), aStartIdx++ );
				strResult = strName + sPrefix + strExt;
			} while( File::TestFileExist( strResult ) );
		}
		return strResult;
	}

	static bool TestDirectoryExist( const StlString& aDirname )
	{
		HANDLE hDir =  CreateFile( aDirname.c_str(), 0, 0, 0, OPEN_EXISTING,  FILE_FLAG_BACKUP_SEMANTICS , 0);
		bool bExist = INVALID_HANDLE_VALUE != hDir;
		if( bExist )
			CloseHandle( hDir );
		return  bExist;
	}

	static bool ForceDirectories( const StlString& aPath )
	{
		BOOL res = ( SHCreateDirectoryEx( NULL, aPath.c_str(), NULL ) == ERROR_SUCCESS );
		if( !res )
		{
			DWORD err = GetLastError();
			if( err == ERROR_ALREADY_EXISTS || err == ERROR_FILE_EXISTS )
				return true;
			return false;
		}
		return true;
	}

	static __int64 GetFileSize( const StlString& aPath)
	{
		LARGE_INTEGER li;
		li.QuadPart = 0;
		WIN32_FIND_DATA findData;
		HANDLE hFile = FindFirstFile( aPath.c_str(), &findData );
		if( hFile != INVALID_HANDLE_VALUE )
		{
			li.LowPart = findData.nFileSizeLow;
			li.HighPart = findData.nFileSizeHigh;
			FindClose( hFile );
		}

		return li.QuadPart;
	}

	static unsigned __int64 GetDiskFreeSpace( const StlString& aDirectoryName )
	{
		ULARGE_INTEGER FreeSpace, TotalSize, AvailableSpace;
		FreeSpace.QuadPart = 0;
		if( TRUE == ::GetDiskFreeSpaceEx( aDirectoryName.c_str(), &AvailableSpace, &TotalSize, &FreeSpace ) )
			return FreeSpace.QuadPart;
		else
			return 0;
	}

	static unsigned __int64 CalculateDirectorySize( const StlString& aPath )
	{
		std::vector< StlString > vecFiles;
		File::GetDirectoryFilesList( aPath, vecFiles );
		unsigned __int64 Size = 0;
		for( std::vector< StlString >::iterator iFile = vecFiles.begin(); iFile != vecFiles.end(); ++iFile )
			Size += File::GetFileSize( *iFile );
		return Size;
	}

	static inline void GetDirectoryFilesList(
			const StlString& aStartDirectory,
			std::vector< StlString >& aFileList)
	{
		GetDirectoryFilesList(aStartDirectory, aFileList, TEXT("*.*"));
	}

	static void GetDirectoryFilesList(
		const StlString& aStartDirectory,
		std::vector< StlString >& aFileList,
		const StlString& aFileTemplate,
        bool bSearchInSubfolders = true)
	{
		WIN32_FIND_DATA findFile;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		StlString StartDir = aStartDirectory;

		if ( !String::CheckLastSymbol(StartDir, '\\') )
			StartDir += TEXT("\\");

		if( bSearchInSubfolders )
        {
            hFind = FindFirstFile((StartDir + TEXT("*.*") ).c_str(), &findFile);

            if( INVALID_HANDLE_VALUE != hFind)
            {
                do
                {
                    StlString fname = findFile.cFileName;
                    if( findFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && fname != TEXT(".") && fname != TEXT("..") )
                    {
                        GetDirectoryFilesList( StartDir + fname, aFileList, aFileTemplate);
                    }
                }
                while( FindNextFile( hFind, &findFile ) != 0 );
                FindClose( hFind );
            }

            hFind = INVALID_HANDLE_VALUE;
        }

		hFind = FindFirstFile((StartDir + aFileTemplate).c_str(), &findFile);
		if( INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				StlString fname = findFile.cFileName;

				if ( (0 == fname.compare(TEXT("."))) || (0 == fname.compare(TEXT(".."))) )
					continue;

				StlString fullname = StartDir + fname;

				aFileList.push_back( fullname );
			}
			while( FindNextFile( hFind, &findFile ) != 0 );
			FindClose( hFind );
		}
	}

	static void LoadIconFromRes( const StlString& aName, Gdiplus::Bitmap **aBmp )
	{
		const HRSRC res = FindResource(NULL, aName.c_str(), RT_RCDATA);
		if(res != NULL)
		{
			const DWORD res_size = SizeofResource(NULL, res);
			const HGLOBAL res_gmem = LoadResource(NULL, res);
			const LPVOID fixed_res_gmem = LockResource(res_gmem);
			const HGLOBAL g_mem = GlobalAlloc(GMEM_FIXED, res_size);
			CopyMemory(g_mem, fixed_res_gmem, res_size);
			GlobalUnlock(g_mem);
			UnlockResource(res_gmem);
			FreeResource(res_gmem);

			IStream *stream = NULL;
			if(CreateStreamOnHGlobal(g_mem, true, &stream) == S_OK)
			{
				Gdiplus::Image *img = Gdiplus::Image::FromStream(stream);
				*aBmp = new Gdiplus::Bitmap(img->GetWidth(), img->GetHeight());
				Gdiplus::Graphics(*aBmp).DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight());
				stream->Release();
			}

			GlobalFree(g_mem);
		}
		else
		{
			//ShowError( "FindResource" );
			*aBmp = NULL;
		}
	};

	static BOOL GetFileTime( const StlString& filename, SYSTEMTIME& st_create, SYSTEMTIME& st_access, SYSTEMTIME& st_write )
	{
		HANDLE handle = CreateFile( filename.c_str(), //file to open
			GENERIC_READ, //open for reading
			FILE_SHARE_READ, //share for reading
			NULL, //default security
			OPEN_EXISTING, //existing file only
			FILE_ATTRIBUTE_NORMAL, //normal file
			NULL );

		if( handle == INVALID_HANDLE_VALUE )
			return FALSE;

		FILETIME ftCreate, ftAccess, ftWrite;
		if( !::GetFileTime( handle, &ftCreate, &ftAccess, &ftWrite ) ) {
			CloseHandle( handle );
			return FALSE;
		}

		SYSTEMTIME stUTC;

		FileTimeToSystemTime( &ftCreate, &stUTC );
		SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &st_create );

		FileTimeToSystemTime( &ftAccess, &stUTC );
		SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &st_access );

		FileTimeToSystemTime( &ftWrite, &stUTC );
		SystemTimeToTzSpecificLocalTime( NULL, &stUTC, &st_write );

		CloseHandle( handle );
		return TRUE;
	};

    static bool ExecBcp( const StlString& params )
    {
        const StlString bcpPath = TEXT("bcp.exe");

        /*HWND                hWnd;*/
        STARTUPINFO         sInfo;
        PROCESS_INFORMATION pInfo;

        ZeroMemory(&sInfo, sizeof(sInfo));
        sInfo.cb = sizeof(sInfo);
        ZeroMemory(&pInfo, sizeof(pInfo));

        TCHAR *buff = new TCHAR[ params.size() + 1 ];
		_tcsncpy_s(buff, params.size() + 1, params.c_str(), params.size());
		buff[params.size()] = '\0';

        SHELLEXECUTEINFO shExInfo = {0};
        shExInfo.cbSize = sizeof(shExInfo);
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExInfo.hwnd = 0;
        shExInfo.lpVerb = _T("runas");                // Operation to perform
        shExInfo.lpFile = bcpPath.c_str();       // Application to start
        shExInfo.lpParameters = buff;                  // Additional parameters
        shExInfo.lpDirectory = 0;
        shExInfo.nShow = SW_HIDE;//SW_SHOW;
        shExInfo.hInstApp = 0;

        if (ShellExecuteEx(&shExInfo)) 
		{
            WaitForSingleObject(shExInfo.hProcess, INFINITE);
            CloseHandle(shExInfo.hProcess);
        }
        else 
		{
            delete [] buff;
            return false;
        }

        delete [] buff;
        return true;
    }

    static void ExecShellCmd( const StlString& cmd )
    {
        SHELLEXECUTEINFO shExInfo = {0};
        shExInfo.cbSize = sizeof(shExInfo);
        shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        shExInfo.hwnd = 0;
        shExInfo.lpVerb = _T("open");                // Operation to perform
        shExInfo.lpFile = _T("cmd.exe");

        StlString params = String::Format( TEXT("/C %s"), cmd.c_str() );
        shExInfo.lpParameters = params.c_str();                  // Additional parameters
        shExInfo.lpDirectory = 0;
        shExInfo.nShow = SW_SHOW;
        shExInfo.hInstApp = 0;

        STARTUPINFO         sInfo;
        PROCESS_INFORMATION pInfo;

        ZeroMemory(&sInfo, sizeof(sInfo));
        sInfo.cb = sizeof(sInfo);
        ZeroMemory(&pInfo, sizeof(pInfo));

        const StlString rs = HelperFunctions::String::Format( TEXT("\"%s\" %s"), shExInfo.lpFile, params.c_str() );
        TCHAR* data = new TCHAR[ rs.size() + 1 ];
		errno_t error = _tcsncpy_s(data, rs.size()+1, rs.c_str(), rs.size());
		data[rs.size()] = '\0';
		const BOOL res = CreateProcess( NULL, data,
            NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &sInfo, &pInfo );

        WaitForSingleObject( pInfo.hProcess, 3000 );

        delete [] data;
        if( res ) {
            CloseHandle( pInfo.hProcess );
            CloseHandle( pInfo.hThread );
        }
    }
	static bool BrowseForFolders(HWND aOwnerWnd, const StlString& aTitle, bool aAllowCreateDir, StlString& aOutputPath)
	{
		LPITEMIDLIST pMyCompFolder = NULL;
		SHGetSpecialFolderLocation(aOwnerWnd, CSIDL_DRIVES, &pMyCompFolder);
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(bi));
		TCHAR szDisplayName [MAX_PATH] = {0};
		bi.lpszTitle		=  aTitle.c_str();
		bi.pszDisplayName	= szDisplayName;
		bi.hwndOwner		= aOwnerWnd;
		bi.lpfn				= NULL;
		bi.lParam			= (LPARAM) nullptr;
		bi.pidlRoot			= pMyCompFolder;
		bi.ulFlags			= BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | (!aAllowCreateDir ? BIF_NONEWFOLDERBUTTON : 0);
		bi.iImage			= -1;

		LPITEMIDLIST pidlRes = NULL;
		pidlRes = SHBrowseForFolder(&bi);
		bool bResult = false;
		if (pidlRes != NULL)
		{
			TCHAR szPath [MAX_PATH] = {0};
			if (SHGetPathFromIDList(pidlRes, szPath))
			{
				aOutputPath = StlString(szPath);
				bResult = true;
			}
			CoTaskMemFree(pidlRes);
		}
		return bResult;
	}
#else //LINUX_OS
	static bool	TestFileExist(const std::string& aFilename)
	{
		if (access(aFilename.c_str(), F_OK) != -1)
			return true;
		return false;
	}

	static std::string AddUniqueIndexToFilename(const std::string& aFilename, int aStartIdx = 1)
	{
		std::string strResult = aFilename;
		if (File::TestFileExist(aFilename))
		{
			std::string strExt = GetFileExtension(aFilename);
			std::string strName = GetFilePath(aFilename) + "/" + GetFileNameWithoutExtension(aFilename);

			do
			{
				char sPrefix[16]{ 0 };
				sprintf(sPrefix, "_%d", aStartIdx++);
				strResult = strName + sPrefix + strExt;
			} while (File::TestFileExist(strResult));
		}
		return strResult;
	}
	static std::string GetFileExtension(const std::string& path)
	{
		std::string fileName = basename(const_cast<char*>(path.c_str()));
		const size_t pointPosition = fileName.rfind(".");
		const std::string fileExt = (pointPosition == 0 || pointPosition == fileName.npos) ? std::string() : fileName.substr(pointPosition); //return extension with dot symbol
		return fileExt;
	}
	static std::string GetFilePath(const std::string& path)
	{
		std::string dirName = dirname(const_cast<char*>(path.c_str()));
		return dirName;
	}
	static std::string GetFileNameWithoutExtension(const std::string& path)
	{
		std::string fileName = basename(const_cast<char*>(path.c_str()));
		const size_t pointPosition = fileName.rfind(".");
		const std::string fileWoExt = (pointPosition == 0 || pointPosition == fileName.npos) ? fileName : fileName.substr(0, pointPosition);
		return fileWoExt;
	}
#endif
};


#ifdef WINDOWS_OS
class Thread
{
protected:
	HANDLE			m__hThread;
	UINT			m__ThreadID;
	bool			m__bThreadAlive;
	StlString		m_strName;

public:
	Thread(bool bSuspended, const StlString& szThreadName = _T(""), unsigned int aDefStackSize = 256 * 1024)
		: m__hThread(INVALID_HANDLE_VALUE)
		, m__ThreadID(0)
		, m__bThreadAlive(true)
		, m_strName(szThreadName)
	{
		//m__hThread = CreateThread( NULL, 0, __ThreadProc, this, bSuspended ? CREATE_SUSPENDED : 0, &m__ThreadID );
		m__hThread = (HANDLE)_beginthreadex( NULL
											, aDefStackSize
											, __ThreadProc
											, this
											, STACK_SIZE_PARAM_IS_A_RESERVATION | (bSuspended ? CREATE_SUSPENDED : 0)
											, &m__ThreadID);

		if( INVALID_HANDLE_VALUE == m__hThread || 0 == m__hThread || 0 == m__ThreadID )
		{
			m__bThreadAlive = false;
			throw _T("Недостаточно системных ресурсов для выполнения операции (создание потока)");
		}

		if( !szThreadName.empty())
		{
			SetName(szThreadName.c_str(), m__ThreadID);
			m_strName = szThreadName;
		}
	}

	virtual ~Thread()
	{
		BeginTerminate();
		WaitFor( 2000 );
	}

	void SetName( const StlString& szThreadName )
	{
#ifdef _UNICODE
		Thread::SetName(String::WcToMb(szThreadName).c_str(), m__ThreadID);
#else
		Thread::SetName(szThreadName.c_str(), m__ThreadID);
#endif
	}

	unsigned int GetThreadId() const
	{
		return m__ThreadID;
	}

	static void SetName( const char* szThreadName, DWORD aThreadID )
	{
		// нельзя использовать переменные с деструктором где есть __try
		// error code : C2712
		if( aThreadID == 0 /*m__hThread == INVALID_HANDLE_VALUE || m__ThreadID == 0*/ )
			throw L"Невозможно присвоить имя потоку до его создания";

		THREADNAME_INFO info;
		info.dwType		= 0x1000;
		info.szName		= szThreadName;
		info.dwThreadID = aThreadID;
		info.dwFlags	= 0;

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	void __Suspend()
	{
		if( m__hThread != INVALID_HANDLE_VALUE && m__ThreadID != 0 )
			SuspendThread( m__hThread );
	}

	void __Resume()
	{
		if( m__hThread != INVALID_HANDLE_VALUE && m__ThreadID != 0 )
			ResumeThread( m__hThread );
	}

	void __SetPriority( int aPriority )
	{
		if( m__hThread != INVALID_HANDLE_VALUE && m__ThreadID != 0 )
			SetThreadPriority( m__hThread, aPriority );
	}

	virtual void BeginTerminate()
	{
		m__bThreadAlive = false;
	}

	bool WaitFor( DWORD aTimeout = INFINITE )
	{
		if( m__hThread == INVALID_HANDLE_VALUE || m__ThreadID == 0 )
			return true;
		if( aTimeout == INFINITE )
		{
			__Resume(); //на всякий случай, чтобы не зависнуть если поток не был запущен
			WaitForSingleObject( m__hThread, aTimeout );
			CloseHandle( m__hThread );
			m__hThread	= INVALID_HANDLE_VALUE;
			m__ThreadID = 0;
			return true;
		}
		else
		{
			__Resume(); //на всякий случай, чтобы не зависнуть если поток не был запущен
			if( WaitForSingleObject( m__hThread, aTimeout ) == WAIT_TIMEOUT )
			{
				#ifdef _MSC_VER
					TCHAR strLog[256]{0};
					_stprintf_s( strLog, sizeof( strLog ) / sizeof( strLog[0] ), _T("The thread (Name=%s ID=%u, Handle=0x%x) was interrupted with TerminateThread\n"), m_strName.c_str(), (UINT)m__ThreadID, (UINT)m__hThread );
					OutputDebugString( strLog );
				#endif
				TerminateThread( m__hThread, -123 );
				CloseHandle( m__hThread );
				m__hThread = INVALID_HANDLE_VALUE;
				m__ThreadID = 0;
				return false;
			}
			return true;
		}
	}

protected:
	//при переопределении данной функции можно вернуть false чтобы уничтожить поток. true - запустит функцию еще раз
	//больше функция не чисто виртуальная, т.к. выявились вызовы чисто виртуальной функции при запуске потока из конструктора
	virtual bool ThreadExecute()
	{
		Sleep(1);
		return true;
	}

	virtual DWORD ThreadLoop()
	{
		bool bWork = true;
		while( m__bThreadAlive && bWork )
		{
			bWork = ThreadExecute();
		}
		m__bThreadAlive = false;
		_endthreadex(0);
		return 0;
	}

	static UINT WINAPI __ThreadProc( LPVOID pParam )
	{
		if( pParam )
			return ((Thread*)pParam)->ThreadLoop();
		return 0;
	}
};
#endif

#ifdef WINDOWS_OS
class XThreadBase
{
public:
	virtual ~XThreadBase(){};
	virtual void Execute(void) = 0;
};

template <class ThreadClass>
class XThread : public XThreadBase
{
protected:
	ThreadClass*		m_pThreadClass;
	void (ThreadClass::*m_ThreadMethod)(void* aParam);
	HANDLE				m_hThread;
	HANDLE				m_hTerminatingEvent;
    unsigned int		m_ThreadID;
	volatile LONG		m_bRunning;
	StlString			m_strName;
	CRITICAL_SECTION	m_Lock;
	unsigned int		m_DefStackSize;
	LPVOID m_threadParam;

public:
	//hTerminatingEvent - событие, устанавливаемое перед завершение потока. Как правило, это событие, 
	//						которое пробуждает поток на выполнение (и следовательно на проверку состояния IsRunning)
	XThread(ThreadClass *aThreadClass, void (ThreadClass::*aThreadMethod)(void* aParam), LPVOID aParam = NULL, const StlString& aThreadName = _T(""), HANDLE hTerminatingEvent = INVALID_HANDLE_VALUE, unsigned int aDefStackSize = 256 * 1024)
		: XThreadBase()
		, m_strName(aThreadName)
		, m_pThreadClass(aThreadClass)
		, m_ThreadMethod(aThreadMethod)
		, m_hThread(INVALID_HANDLE_VALUE)
		, m_hTerminatingEvent(hTerminatingEvent)
		, m_ThreadID(0)
		, m_bRunning(0)
		, m_DefStackSize(aDefStackSize)
		, m_threadParam(aParam)
	{
		InitializeCriticalSection(&m_Lock);
	}

	virtual ~XThread(void)
	{
		bool wait_for_terminate = false;
		if( IsRunning() )
		{
			InterlockedExchange( &m_bRunning, (LONG)0 );
			if(INVALID_HANDLE_VALUE != m_hTerminatingEvent)
				SetEvent(m_hTerminatingEvent);
			wait_for_terminate = true;
		}
		if(wait_for_terminate)
		{
			if(WaitForSingleObject(m_hThread, 60000) == WAIT_TIMEOUT)
			{
				TerminateThread(m_hThread, -43);
				StlString str = String::Format(_T("WARNING:Thread %s terminated with timeout"), m_strName.c_str());
				OutputDebugString(str.c_str());
			}
			CloseHandle(m_hThread);
		}
		DeleteCriticalSection(&m_Lock);
	}

	void Resume(void)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, m_DefStackSize, &XThreadProc, this, CREATE_SUSPENDED | STACK_SIZE_PARAM_IS_A_RESERVATION, &m_ThreadID);
		if( INVALID_HANDLE_VALUE == m_hThread || 0 == m_hThread || 0 == m_ThreadID )
		{
			throw _T("Недостаточно системных ресурсов для выполнения операции (создание потока %s)");
		}
		if (!m_strName.empty())
		{
#ifdef UNICODE
			Thread::SetName(String::WcToMb(m_strName).c_str(), m_ThreadID);
#else
			Thread::SetName(m_strName.c_str(), m_ThreadID);
#endif
		}
		InterlockedExchange( &m_bRunning, (LONG)1 );
		ResumeThread(m_hThread);
	}

	inline void Lock(void)
	{
		EnterCriticalSection(&m_Lock);
	}

	inline void Unlock(void)
	{
		LeaveCriticalSection(&m_Lock);
	}

	inline bool IsRunning(void)
	{
		if( INVALID_HANDLE_VALUE != m_hThread && TRUE == (BOOL)InterlockedExchangeAdd( &m_bRunning, (LONG)0 ) )
			return true;
		return false;
	}

	unsigned int GetThreadId() const
	{
		return m_ThreadID;
	}

	void SetPriority( int aPriority )
	{
		if( m_hThread != INVALID_HANDLE_VALUE && m_ThreadID != 0 )
			SetThreadPriority( m_hThread, aPriority );
	}

	HANDLE GetHandle() const
	{
		return m_hThread;
	}

protected:
	void Execute(void)
	{
		(m_pThreadClass->*m_ThreadMethod)(m_threadParam);
	}

	static UINT WINAPI XThreadProc(__in LPVOID aParam)
	{
		((XThreadBase*)aParam)->Execute();
		_endthreadex(0);
		return 0;
	}
};
#else  //LINUX_OS
class XThreadBase
{
public:
	virtual ~XThreadBase() {};
	virtual void Execute(void) = 0;
};

template <class ThreadClass>
class XThread : public XThreadBase
{
protected:
	ThreadClass*		m_pThreadClass;
	void (ThreadClass::*m_ThreadMethod)(void* aParam);
	pthread_t			m_hThread;
	bool 				m_bGoingToTerminate;
	pthread_mutex_t 	m_ControlMutex;
	Events::HANDLE		m_hTerminationEvent;

public:
	XThread(ThreadClass *aThreadClass, void (ThreadClass::*aThreadMethod)(void* aParam), void* aParam = nullptr, const std::string& aThreadName = "", Events::HANDLE hTerminationEvent = nullptr)
		: XThreadBase()
		, m_pThreadClass(aThreadClass)
		, m_ThreadMethod(aThreadMethod)
		, m_hThread(0)
		, m_bGoingToTerminate(false)
		, m_hTerminationEvent(hTerminationEvent)
	{
		pthread_mutex_init(&m_ControlMutex, NULL);
	}

	virtual ~XThread(void)
	{
		if (m_hThread != 0 && IsRunning())
		{
			pthread_mutex_lock(&m_ControlMutex);
			m_bGoingToTerminate = true;
			if (m_hTerminationEvent)
				Events::SetEvent(m_hTerminationEvent);
			pthread_mutex_unlock(&m_ControlMutex);

			pthread_join(m_hThread, NULL);
			m_hThread = 0;
		}
		pthread_mutex_destroy(&m_ControlMutex);
	}

	void Resume(void)
	{
		pthread_create(&m_hThread, NULL, &(XThreadProc), reinterpret_cast<void*>(this));
	}
	inline bool IsRunning(void)
	{
		bool isRunning = false;
		pthread_mutex_lock(&m_ControlMutex);
		isRunning = !m_bGoingToTerminate;
		pthread_mutex_unlock(&m_ControlMutex);
		return isRunning;
	}
protected:
	void Execute(void)
	{
		(m_pThreadClass->*m_ThreadMethod)(NULL);
	}

	static void* XThreadProc(void* aParam)
	{
		((XThreadBase*)aParam)->Execute();
		return NULL;
	}
};

#endif

#ifdef WINDOWS_OS
class Graph
{
public:
	static Gdiplus::Bitmap* LoadPNGFromResource( HINSTANCE aModuleInstance, const StlString& aResourceName )
	{
		HRSRC hrsrc = FindResource( aModuleInstance, aResourceName.c_str(), _T("PNG") );
		IStream * pStream = NULL;
		Gdiplus::Bitmap* pBmp = NULL;
		if (hrsrc)
		{
			DWORD dwResourceSize = SizeofResource(aModuleInstance, hrsrc);
			HGLOBAL hglbImage = LoadResource(aModuleInstance, hrsrc);

			if (hglbImage && dwResourceSize )
			{
				LPVOID pvSourceResourceData = LockResource(hglbImage);
				if (pvSourceResourceData == NULL)
					return NULL;
				HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
				if (hgblResourceData == NULL)
					return NULL;
				LPVOID pvResourceData = GlobalLock(hgblResourceData);
				if (pvResourceData )
				{
					CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
					GlobalUnlock(hgblResourceData);
					if( FAILED( CreateStreamOnHGlobal(hgblResourceData, TRUE, &pStream) ) )
					{
						pStream = NULL;
						GlobalFree(hgblResourceData);
					}
				}
				else
					GlobalFree(hgblResourceData);
			}
		}
		if( pStream )
		{
			pBmp = new Gdiplus::Bitmap(pStream);
			if( pBmp && pBmp->GetLastStatus() != Gdiplus::Ok )
			{
				delete pBmp;
				pBmp = NULL;
			}
			pStream->Release();
		}
		return pBmp;
	}

	static Gdiplus::Bitmap* LoadPNGFromResource( HINSTANCE aModuleInstance, UINT aResourceID )
	{
		return LoadPNGFromResource( aModuleInstance, MAKEINTRESOURCE( aResourceID ) );
	}

	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
	   UINT  num = 0;          // number of image encoders
	   UINT  size = 0;         // size of the image encoder array in bytes

	   Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	   Gdiplus::GetImageEncodersSize(&num, &size);
	   if(size == 0)
		  return -1;  // Failure

	   pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	   if(pImageCodecInfo == NULL)
		  return -1;  // Failure

	   Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	   for(UINT j = 0; j < num; ++j)
	   {
		  if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		  {
			 *pClsid = pImageCodecInfo[j].Clsid;
			 free(pImageCodecInfo);
			 return j;  // Success
		  }
	   }

	   free(pImageCodecInfo);
	   return -1;  // lFailure
	}
	//чтобы увидеть функцию захвата экрана, необходимо подключить Display.h
#ifdef DISPLAY_H
	static Gdiplus::Bitmap* CaptureWindow(HWND ahWnd)
	{
		DisplayManager dm;
		Displays displays = dm.GetAllDisplays();
		RECT rcDesktop;
		SetRect(&rcDesktop, 0, 0, 0, 0);
		RECT rcWnd, rcDum;
		HDC hScreenDC = NULL;
		if(DwmGetWindowAttribute(ahWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcWnd, sizeof(rcWnd)) != S_OK)
			GetWindowRect(ahWnd, &rcWnd);

		for(Displays::iterator iDisplay = displays.begin(); iDisplay != displays.end(); ++iDisplay)
		{
			if(IntersectRect(&rcDum, &(iDisplay->rcWorkspace), &rcWnd) && hScreenDC == NULL)
				hScreenDC = dm.CreateDC(*iDisplay);
			UnionRect(&rcDesktop, &rcDesktop, &(iDisplay->rcWorkspace));
		}
		if(hScreenDC == NULL)
			return NULL;
		HDC hBmpDC = CreateCompatibleDC(hScreenDC);
		if(hBmpDC == NULL)
		{
			DeleteDC(hScreenDC);
			return NULL;
		}

		if (rcWnd.left < rcDesktop.left)
            rcWnd.left = rcDesktop.left;
        if (rcWnd.right > rcDesktop.right)
            rcWnd.right	= rcDesktop.right;
        if (rcWnd.top < rcDesktop.top)
            rcWnd.top = rcDesktop.top;
        if (rcWnd.bottom > rcDesktop.bottom)
            rcWnd.bottom = rcDesktop.bottom;

		HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, rcWnd.right -  rcWnd.left, rcWnd.bottom - rcWnd.top);
		if(hBitmap == NULL)
		{
			DeleteDC(hScreenDC);
			DeleteDC( hBmpDC );
			DeleteObject(hBitmap);
			return NULL;
		}
		HGDIOBJ hOldBmp = SelectObject( hBmpDC, hBitmap );
		BitBlt( hBmpDC, 0, 0, rcWnd.right -  rcWnd.left, rcWnd.bottom - rcWnd.top, hScreenDC, rcWnd.left, rcWnd.top, SRCCOPY | CAPTUREBLT );
		SelectObject(hBmpDC, hOldBmp);
		Gdiplus::Bitmap* pSrcBmp = Gdiplus::Bitmap::FromHBITMAP(hBitmap, 0);
		if(!pSrcBmp || pSrcBmp->GetLastStatus() != Gdiplus::Ok)
		{
			DeleteDC( hBmpDC );
			DeleteDC(hScreenDC);
			DeleteObject(hBitmap);
			if(pSrcBmp)
				delete pSrcBmp;
			return NULL;
		}
		Gdiplus::Bitmap* pDstBmp = pSrcBmp->Clone(0, 0, pSrcBmp->GetWidth(), pSrcBmp->GetHeight(), PixelFormat32bppARGB);
		DeleteDC(hBmpDC);
		DeleteDC(hScreenDC);
		DeleteObject(hBitmap);
		delete pSrcBmp;
		return pDstBmp;
	}
#endif
	static int ConvertBimtapToPNGData(Gdiplus::Bitmap* aBmp, char** aOutBuffer)
	{
		CLSID pngClsid;
		HelperFunctions::Graph::GetEncoderClsid(L"image/png", &pngClsid);
		IStream* pStream;
		ULONG bytesSaved = 0;
		if(CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK)
		{
			Gdiplus::Status st = aBmp->Save(pStream, &pngClsid, NULL);
			STATSTG Stats;
			memset(&Stats, 0, sizeof(Stats));
			pStream->Stat(&Stats, 0);
			LARGE_INTEGER li;
			li.QuadPart = 0;
			pStream->Seek(li, STREAM_SEEK_SET, NULL);
			*aOutBuffer = new char[static_cast<int>(Stats.cbSize.QuadPart)];
			pStream->Read(*aOutBuffer, static_cast<int>(Stats.cbSize.QuadPart), &bytesSaved);
			if(0 == bytesSaved)
				delete [] *aOutBuffer;
			pStream->Release();
		}
		return bytesSaved;
	}
	static int ConvertBimtapToJPGData(Gdiplus::Bitmap* aBmp, char** aOutBuffer, ULONG quality = 60)
	{
		CLSID pngClsid;
		HelperFunctions::Graph::GetEncoderClsid(L"image/jpeg", &pngClsid);
		Gdiplus::EncoderParameters encoderParameters;
		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
		encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
		encoderParameters.Parameter[0].Value = &quality;
		IStream* pStream;
		ULONG bytesSaved = 0;
		if(CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK)
		{
			Gdiplus::Status st = aBmp->Save(pStream, &pngClsid, &encoderParameters);
			STATSTG Stats;
			memset(&Stats, 0, sizeof(Stats));
			pStream->Stat(&Stats, 0);
			LARGE_INTEGER li;
			li.QuadPart = 0;
			pStream->Seek(li, STREAM_SEEK_SET, NULL);
			*aOutBuffer = new char[static_cast<int>(Stats.cbSize.QuadPart)];
			pStream->Read(*aOutBuffer, static_cast<int>(Stats.cbSize.QuadPart), &bytesSaved);
			if(0 == bytesSaved)
				delete [] *aOutBuffer;
			pStream->Release();
		}
		return bytesSaved;
	}

	//Загружает картинку из буфера в формате JPEG или PNG
	static Gdiplus::Bitmap* ConvertCompressedDataToBitmap(const char* aPNGData, int aSize)
	{
		IStream* pStream = nullptr;
		if(CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK)
		{
			DWORD written;
			if(S_OK == pStream->Write(aPNGData, aSize, &written))
			{
				LARGE_INTEGER li;
				li.QuadPart = 0;
				pStream->Seek(li, STREAM_SEEK_SET, NULL);
				Gdiplus::Bitmap* pBmp = new Gdiplus::Bitmap(pStream);
				if(pBmp->GetLastStatus() != Gdiplus::Ok)
				{
					delete pBmp;
					pBmp = nullptr;
				}
				pStream->Release();
				return pBmp;
			}
		}
		return nullptr;
	}
};
#endif

#ifdef WINDOWS_OS

//#include "MessageBoxResource.h"
//чтобы увидеть класс CustomMessageBox, необходимо подключить файл ресурсов "MessageBoxResource.h"
#ifdef MessageBoxResourceH
class CustomMessageBox
{
public:
	static INT Show(HWND hwnd, LPCTSTR lpText, LPCTSTR lpCaption, INT uType, HINSTANCE aResInstance = NULL /*only for VC++ projets*/)
	{
		m_Hook = SetWindowsHookEx(WH_CBT, &CustomMessageBox::CBTProc, 0,
								GetCurrentThreadId());
		m_HInstance = aResInstance;
		return MessageBox(hwnd, lpText, lpCaption, uType);

	}
private:
	static HHOOK m_Hook;
	static HINSTANCE m_HInstance;
	static LRESULT CALLBACK CBTProc(INT nCode, WPARAM wParam, LPARAM lParam)
	{
		HWND hChildWnd;    // msgbox is "child"
		if (nCode == HCBT_ACTIVATE)
		{
			// set window handles
			hChildWnd = (HWND)wParam;
			//to get the text of the Yes button
			UINT result;
			if (GetDlgItem(hChildWnd, IDYES)!=NULL)
			{
#ifdef _MSC_VER
				StlString s = HelperFunctions::String::LoadFromResource(m_HInstance, IDS_MSGBOXBUTTON_YES);
#else
				UnicodeString s = LoadStr(IDS_MSGBOXBUTTON_YES);
#endif
				result= SetDlgItemText(hChildWnd,IDYES, s.c_str());
			}
			if (GetDlgItem(hChildWnd, IDOK)!=NULL)
			{
#ifdef _MSC_VER
				StlString s = HelperFunctions::String::LoadFromResource(m_HInstance, IDS_MSGBOXBUTTON_OK);
#else
				UnicodeString s = LoadStr(IDS_MSGBOXBUTTON_OK);
#endif
				result= SetDlgItemText(hChildWnd,IDOK, s.c_str());
			}
			if (GetDlgItem(hChildWnd, IDNO)!=NULL)
			{
#ifdef _MSC_VER
				StlString s = HelperFunctions::String::LoadFromResource(m_HInstance, IDS_MSGBOXBUTTON_NO);
#else
				UnicodeString s = LoadStr(IDS_MSGBOXBUTTON_NO);
#endif
				result= SetDlgItemText(hChildWnd,IDNO, s.c_str());
			}
			if (GetDlgItem(hChildWnd, IDCANCEL)!=NULL)
			{
#ifdef _MSC_VER
				StlString s = HelperFunctions::String::LoadFromResource(m_HInstance, IDS_MSGBOXBUTTON_CANCEL);
#else
				UnicodeString s = LoadStr(IDS_MSGBOXBUTTON_CANCEL);
#endif
				result= SetDlgItemText(hChildWnd,IDCANCEL, s.c_str());
			}
 
			// exit CBT hook
			UnhookWindowsHookEx(m_Hook);
		}
	   // otherwise, continue with any possible chained hooks
	   else CallNextHookEx(m_Hook, nCode, wParam, lParam);
	   return 0;
	}
};
#endif

#endif

class Math
{
public:
	template <typename T> static  int Sign(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	static int Round( double aVal )
	{
		int a1 = (int)std::ceil( aVal ); // 11
		int a2 = (int)std::floor( aVal ); // 10
		if( (double)a1 - aVal < aVal - (double)a2 )
    		return a1;
		else
    		return a2;
	}
	static unsigned long long Distance( unsigned long long aVal1, unsigned long long aVal2 )
	{
		return aVal1 > aVal2 ? (aVal1 - aVal2) : (aVal2 - aVal1 );
	}

	static unsigned long long SafeAddMASTERFRAME(unsigned long long aMF, int aAddend)
	{
		if(aAddend < 0)
			return std::abs(aAddend) > aMF ? 0 : aMF + aAddend;
		return ULLONG_MAX - aMF - 1 < aAddend ? ULLONG_MAX - 1 : aMF + aAddend;
	}

	static uint64_t BCDtoMASTERFRAME( const uint8_t* const data )
	{
		errno = 0;
		uint64_t result{0};
		const uint8_t *p2{data};
		while( *p2 != 0 )
		{
			if( (*p2 & 0xF) >= 10 ) // это был не BCD
			{
				errno = EINVAL;
				return UINT64_MAX;
			}

			const uint8_t b1 = ( *p2 & 0xF );
			const uint8_t b2 = ( (*p2 >> 4) & 0xF );

			result *= 10; result += b2;
			result *= 10; result += b1;

			++p2;
		}

		return result;
	}

	static size_t MASTERFRAMEtoBCD(uint64_t masterframe, uint8_t* const bcd_data )
	{
		uint8_t *p2{bcd_data};
		size_t c = 0;
		while( masterframe > 0 )
		{
			const uint8_t b = static_cast<uint8_t>(masterframe % 10);
			masterframe /= 10;

			const uint8_t b1 = static_cast<uint8_t>(masterframe % 10 << 4);
			masterframe /= 10;

			*p2 = b | b1;
			++p2;
			++c;
		}

		for( size_t i = 0; i < c / 2 - 1; i++ )
			std::swap( bcd_data[ i ], bcd_data[ c - i - 1 ] );

		return c;
	}
};

#ifdef WINDOWS_OS
class SystemShutdown
{
public:
	static BOOL ShutdownWithDialog( LPTSTR lpMsg )
	{
		HANDLE hToken;              // handle to process token
		TOKEN_PRIVILEGES tkp;       // pointer to token structure

		BOOL fResult;               // system shutdown flag

		// Get the current process token handle so we can get shutdown
		// privilege.

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return FALSE;

		// Get the LUID for shutdown privilege.

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
			&tkp.Privileges[0].Luid);

		tkp.PrivilegeCount = 1;  // one privilege to set
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Get shutdown privilege for this process.

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES) NULL, 0);

		// Cannot test the return value of AdjustTokenPrivileges.

		if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

		// Display the shutdown dialog box and start the countdown.

		fResult = InitiateSystemShutdown(
		NULL,    // shut down local computer
		lpMsg,   // message for user
		30,      // time-out period, in seconds
		FALSE,   // ask user to close apps
		TRUE);   // reboot after shutdown

		if (!fResult)
		return FALSE;

		// Disable shutdown privilege.

		tkp.Privileges[0].Attributes = 0;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
			(PTOKEN_PRIVILEGES) NULL, 0);

		return TRUE;
	}
	static BOOL SilentShutdown()
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;

		// Get a token for this process.

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return( FALSE );

		// Get the LUID for the shutdown privilege.

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
			&tkp.Privileges[0].Luid);

		tkp.PrivilegeCount = 1;  // one privilege to set
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Get the shutdown privilege for this process.

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
			(PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

		// Shut down the system and force all applications to close.

		if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,
					SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
					SHTDN_REASON_MINOR_UPGRADE |
					SHTDN_REASON_FLAG_PLANNED))
		return FALSE;

		//shutdown was successful
		return TRUE;
	}
	static BOOL SilentReboot()
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES tkp;

		// Get a token for this process.

		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			return(FALSE);

		// Get the LUID for the shutdown privilege.

		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
			&tkp.Privileges[0].Luid);

		tkp.PrivilegeCount = 1;  // one privilege to set
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		// Get the shutdown privilege for this process.

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
			(PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() != ERROR_SUCCESS)
			return FALSE;

		// Reboot the system and force all applications to close.

		if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE,
			SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
			SHTDN_REASON_MINOR_UPGRADE |
			SHTDN_REASON_FLAG_PLANNED))
			return FALSE;

		//shutdown was successful
		return TRUE;
	}

};
#endif

class CriticalSection
{
public:
	CriticalSection()
	{
#ifdef WINDOWS_OS
		InitializeCriticalSectionAndSpinCount(&m_LockObject, 0);
#endif
	}

public:

#ifdef WINDOWS_OS
	CRITICAL_SECTION m_LockObject;
#else
	mutable std::recursive_mutex m_LockObject;
#endif
#ifdef WINDOWS_OS
public:
	operator CRITICAL_SECTION*()
	{
		return (CRITICAL_SECTION*) &m_LockObject;
	}
#endif
public:
	void Unlock()
	{
#ifdef WINDOWS_OS
		::LeaveCriticalSection( &m_LockObject );
#else
		m_LockObject.unlock();
#endif
	}
	void Lock()
	{
#ifdef WINDOWS_OS
		::EnterCriticalSection( &m_LockObject );
#else
		m_LockObject.lock();
#endif
	}

public:
	~CriticalSection()
	{
#ifdef WINDOWS_OS
		::DeleteCriticalSection( &m_LockObject );
#endif
	}
};

inline void EnterCriticalSection(CriticalSection* aSection)
{
	if(nullptr != aSection)
		aSection->Lock();
}

inline void LeaveCriticalSection(CriticalSection* aSection)
{
	if(nullptr != aSection)
		aSection->Unlock();
}

class LockableResource
{
private:
	mutable CriticalSection Sync;
	friend class LockObject;
	bool Valid;

protected:
	inline void _Lock() const
	{
		Sync.Lock();
	}

	inline void _Unlock() const
	{
		Sync.Unlock();
	}
};

class LockObject
{
private:
	const LockableResource* pLocker;

public:
	LockObject(const LockableResource *_locker)
		: pLocker(NULL)
	{
		if(_locker)
		{
			pLocker = _locker;
			pLocker->_Lock();
		}
	}

	~LockObject()
	{
		if(pLocker)
			pLocker->_Unlock();
	}
};
#ifdef WINDOWS_OS
class AtomicFlag
{
public:
	static inline void Set( volatile LONG* aFlag ) { InterlockedExchange( aFlag, static_cast<LONG>(TRUE) ); }
	static inline void Reset( volatile LONG* aFlag ) { InterlockedExchange( aFlag, static_cast<LONG>(FALSE) ); }
	static inline void SetValue( volatile LONG* aFlag, bool aValue ) { InterlockedExchange( aFlag, aValue == true ? static_cast<LONG>(TRUE) : static_cast<LONG>(FALSE) ); }
	static inline void SetValue( volatile LONG* aFlag, BOOL aValue ) { InterlockedExchange( aFlag, static_cast<LONG>(aValue) ); }
	static inline BOOL Check( volatile LONG* aFlag ) { return (static_cast<LONG>(TRUE) == InterlockedExchangeAdd( aFlag, 0 )) ? TRUE : FALSE; }
};
class AtomicLong
{
public:
	static inline void SetValue( volatile LONG* aVar, LONG aValue ) { InterlockedExchange( aVar, aValue ); }
	static inline LONG GetValue( volatile LONG* aVar ) { return InterlockedExchangeAdd( aVar, 0 ); }
	static inline LONG Inc( volatile LONG* aVar ) { return InterlockedIncrement( aVar ); }
	static inline LONG Dec( volatile LONG* aVar ) { return InterlockedDecrement( aVar ); }
};
#endif

#ifdef WINDOWS_OS
class BCBLocale
{
public:
	static void SetLocalOverrides( const TCHAR* filename, const TCHAR* locale )
	{
		if( filename == NULL || locale == NULL )
			return;

		HKEY Key;
		const TCHAR* LocaleOverrideKey = _T("Software\\Embarcadero\\Locales");
		if( RegOpenKeyEx( HKEY_CURRENT_USER, LocaleOverrideKey, 0, KEY_ALL_ACCESS, &Key ) == ERROR_SUCCESS ) {
			const size_t sz = lstrlen( locale );
			if( sz <= 5 )
				RegSetValueEx( Key, filename, 0, REG_SZ, (const BYTE*)locale, sz * sizeof( locale[ 0 ] ) + 1 );

			RegCloseKey(Key);
		}
	}

	static const TCHAR* GetLocale( const TCHAR* filename )
	{
		if( filename == NULL )
			return NULL;

		HKEY Key;
		const TCHAR* LocaleOverrideKey = _T("Software\\Embarcadero\\Locales");
		if( RegOpenKeyEx( HKEY_CURRENT_USER, LocaleOverrideKey, 0, KEY_ALL_ACCESS, &Key ) == ERROR_SUCCESS ) {
			ULONG sz = 100;
			LPBYTE value = new BYTE[ sz ];
			DWORD res = RegQueryValueEx( Key, filename, NULL, NULL, value, &sz );
			RegCloseKey( Key );
			if( res == ERROR_SUCCESS && sz > 0 ) 
			{
				StlString v;
				TCHAR* str_ptr = reinterpret_cast< TCHAR* >( value );
				return str_ptr;
			}
			else 
			{
				SAFE_ARRAY_DELETE(value);
				return NULL;
			}
		}
		return NULL;
	}
};
#endif

} // namespace HelperFunctions
#define LOCK(obj) HelperFunctions::LockObject lock__(obj);

#endif
