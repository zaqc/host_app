#pragma once
#ifndef DataTypeH
#define DataTypeH

#include "common_def.h"
//#pragma warning(disable : 4996)
#ifdef WINDOWS_OS
	#include <windows.h>
	//#include "DataTypeResource.h"
	#ifdef __BORLANDC__
	#include <Classes.hpp>
#endif

#endif
#include <cstdio>
#include <string>
#include <set>
#define __STDINT_LIMITS
#include <stdint.h>
#include <cstdint>

typedef uint32_t MASTERFRAME;
#define INVALID_MASTERFRAME 0xFFFFFFFF //UINT32_MAX

enum DataType : uint32_t
{
	dtNONE       		= 0,
	dtUS_LEFT			= 1,
	dtUS_RIGHT			= 2,
	dtUSM				= 3,
	dtGeo		        = 4,
	dtWM				= 5,
	dtEchoComplex       = 6,
	dtGus				= 7,
	dtMagDef			= 8,
	dtLineVideo1		= 101,
	dtLineVideo2		= 102,
	dtLineVideo3		= 103,
	dtLineVideo4		= 104,
	dtLineVideo5		= 105,
	dtLineVideo6		= 106,
	dtLineVideo7		= 107,
	dtLineVideo8		= 108,
	dtLineVideo9		= 109,
	dtLineVideo10		= 110,
	dtLineVideo11		= 111,
	dtLineVideo12		= 112,
	dtLineVideo13		= 113,
	dtLineVideo14		= 114,
	dtLineVideo15		= 115,
	dtLineVideo16		= 116,

	dtFrameVideo        = 1000, //
	dtFrameVideo1     	= 1001, //Р В¤РЎР‚Р ВµР в„–Р С�-Р Р†Р С‘Р Т‘Р ВµР С• Р В¦Р С‘РЎвЂћРЎР‚Р С•Р Р†Р С•Р Вµ Beward
	dtFrameVideo2     	= 1002, //
	dtFrameVideo3     	= 1003, //
	dtFrameVideo4     	= 1004, //
	dtFrameVideo5     	= 1005, //
	dtFrameVideo6     	= 1006, //
	dtFrameVideo7     	= 1007, //
	dtFrameVideo8     	= 1008, //
	dtFrameVideo9      	= 1009, //
	dtFrameVideo10     	= 1010, //

	dtFrameVideo11		= 1011, //
	dtFrameVideo12		= 1012, //Р В¤РЎР‚Р ВµР в„–Р С�-Р Р†Р С‘Р Т‘Р ВµР С• Р В°Р Р…Р В°Р В»Р С•Р С–Р С•Р Р†Р С•Р Вµ РЎРѓ Р Р†Р С‘Р Т‘Р ВµР С•РЎРѓР ВµРЎР‚Р Р†Р ВµРЎР‚Р В°
	dtFrameVideo13		= 1013, //
	dtFrameVideo14		= 1014, //
	dtFrameVideo15		= 1015, //
	dtFrameVideo16		= 1016, //

	dtFrameVideo17  	= 1017, //
	dtFrameVideo18  	= 1018, //
	dtFrameVideo19  	= 1019, //Р В¤РЎР‚Р ВµР в„–Р С�-Р Р†Р С‘Р Т‘Р ВµР С• РЎРѓР С”Р С•РЎР‚Р С•РЎРѓРЎвЂљР Р…Р С•Р Вµ РЎРѓ Р С”Р В°Р С�Р ВµРЎР‚ jAI,Dalsa
	dtFrameVideo20  	= 1020, //

	dtGyroScope 		= 2001, //Р РЋР С‘РЎРѓРЎвЂљР ВµР С�Р В° Р Р…Р В°Р Р†Р ВµР Т‘Р ВµР Р…Р С‘РЎРЏ Р С’Р пїЅР РЋ
	dtGPS               = 2501,

	dtSWD				= 2601,  //[ Р Т‘Р ВµРЎвЂљР ВµР С”РЎвЂљР С•РЎР‚ РЎРѓРЎвЂљРЎР‚Р ВµР В»Р С•Р С” ]//
	dtSWDAntenna1		= 2602,
	dtSWDAntenna2		= 2603, //Р С’Р Р…РЎвЂљР ВµР Р…Р Р…РЎвЂ№ Р Т‘Р ВµРЎвЂљР ВµР С”РЎвЂљР С•РЎР‚РЎвЂ№ РЎРѓРЎвЂљРЎР‚Р ВµР В»Р С•Р С” (Р Т‘Р В»РЎРЏ Р С—РЎР‚Р С‘Р ВµР С�Р В° "РЎРѓРЎвЂ№РЎР‚РЎвЂ№РЎвЂ¦" Р Т‘Р В°Р Р…Р Р…РЎвЂ№РЎвЂ¦ РЎРѓ Р Т‘Р В°РЎвЂљРЎвЂЎР С‘Р С”Р В° РЎРѓРЎвЂљРЎР‚Р ВµР В»Р С•Р С”)
	dtSWDAntenna3		= 2604,

	dtHandCart          = 2801,

	dtRF603_1			= 3000,
	dtRF603_2			= 3001,

	dtLaserProfile1     = 4001,
	dtLaserProfile2     = 4002,
	dtLaserProfile3     = 4003,
	dtLaserProfile4     = 4004,

	dtRouteTrack		= 4101,

	dtBeam				= 5001,
	dtCAN				= 6001,
	dtAstra				= 7000,
	dtWagonConfig		= 10000,

	END_OF_DATATYPE
};


typedef std::set<DataType> SetOfDataType;
namespace DataTypes
{
	enum DataClass
	{
		dcNONE				= 0,
    	dcUSM				= 1,
        dcLineVideo			= 2,
        dcFrameVideo		= 3,
		dcLaserProfile		= 4,
		dcWayMeter			= 5,
		dcFrameAnalogVideo	= 6,
		dcFrameFastVideo	= 7,
        dcBeam				= 8,
		dcGPS				= 9,
		dcSWD				= 10,

		END_OF_DATACLASS
    };
};

namespace Marks
{
//Р С—Р ВµРЎР‚Р ВµРЎвЂЎР С‘РЎРѓР В»Р С‘Р С�РЎвЂ№Р в„– РЎвЂљР С‘Р С— Р Т‘Р В»РЎРЏ РЎвЂљР С‘Р С—Р С•Р Р† Р С�Р В°РЎР‚Р С”Р ВµРЎР‚Р С•Р Р† Р С•Р В±РЎвЂ°Р ВµР С–Р С• Р Р…Р В°Р В·Р Р…Р В°РЎвЂЎР ВµР Р…Р С‘РЎРЏ (РЎС“ Р С”Р С•РЎвЂљР С•РЎР‚РЎвЂ№РЎвЂ¦ DataType Р С‘ DataClass == 0 )
	enum MarkType : unsigned char
	{
		NONE				= 0,
		/////Common section
		Settings			= 1,
		Chat				= 2,
		
		
		/////Trip server section
		tsRedButton			= 10,
		tsGreenButton		= 11,
		tsAudio				= 12,
		tsSwitcher			= 13,
		tsSettings			= 14,
		/////Line video section
        lvJoint				= 40,
        lvMeasure			= 41,
        lvGreenMark			= 42,
        lvYellowMark		= 43,
        lvRedMark			= 44,
		lvSwitch			= 45,
		lvAutoMaybeJunc		= 46,
		lvAutoSpotDef		= 47,
		lvAutoSpotLong		= 48,
		lvAutoSwitchEnd		= 49,
		lvControlMark		= 58,
		lvTemporaryMark		= 59,
		/////Frame video and Lubri section
		fvLubrication		= 60
	};

};
enum DeviceLocation
{
	locationNONE		= 0,
	locationKupe 		= 1,
	locationProxod		= 2,
	locationDiesel  	= 4,/*РЎРѓР С• РЎРѓРЎвЂљР С•РЎР‚Р С•Р Р…РЎвЂ№ РЎР‚Р В°Р В±Р С•РЎвЂЎР ВµР С–Р С• РЎРѓР В°Р В»Р С•Р Р…Р В° (Р Т‘Р С‘Р В·Р ВµР В»РЎРЏ)*/
	locationBoiler		= 8, /*РЎРѓР С• РЎРѓРЎвЂљР С•РЎР‚Р С•Р Р…РЎвЂ№ Р С”Р С•РЎвЂљР В»Р В° */
	locationTelega		= 16, /*Р Р† РЎР‚Р В°Р в„–Р С•Р Р…Р Вµ Р Т‘Р ВµРЎвЂћР ВµР С”РЎвЂљР С•РЎРѓР С”Р С•Р С—Р Р…Р С•Р в„– РЎвЂљР ВµР В»Р ВµР В¶Р С”Р С‘*/
	locationRoof		= 32,
	locationForsunka	= 64,  /*Р Р† РЎР‚Р В°Р в„–Р С•Р Р…Р Вµ РЎвЂћР С•РЎР‚РЎРѓРЎС“Р Р…Р С•Р С” Р Р…Р В° РЎР‚Р ВµР В»РЎРЉРЎРѓР С•РЎРѓР С�Р В°Р В·РЎвЂ№Р Р†Р В°РЎвЂљР ВµР В»Р Вµ*/
	locationCabin1		= 128, /*1-РЎРЏ Р С”Р В°Р В±Р С‘Р Р…Р В° Р С�Р С•РЎвЂљРЎР‚Р С‘РЎРѓРЎвЂ№*/
	locationCabin2		= 256  /*2-РЎРЏ Р С”Р В°Р В±Р С‘Р Р…Р В° Р С�Р С•РЎвЂљРЎР‚Р С‘РЎРѓРЎвЂ№*/
};
DataType& operator++(DataType &aDataType);
DataTypes::DataClass& operator++(DataTypes::DataClass &aDataClass);
class DataTypeConverter
{
private:
#ifdef WINDOWS_OS
	static HINSTANCE m_hResInstance;
#endif
	//static char m_Str[128];
	//static std::string m_Str;
public:
#ifdef WINDOWS_OS
	static void SetResourceHInstance(HINSTANCE hResInstance) {m_hResInstance = hResInstance;}
#endif
	static DataType GetFirstLVType() { return dtLineVideo1; }
	static DataType GetLastLVType() { return dtLineVideo16; }
	//РЎвЂ Р С‘РЎвЂћРЎР‚Р С•Р Р†Р С•Р Вµ Р С�Р В°РЎвЂљРЎР‚Р С‘РЎвЂЎР Р…Р С•Р Вµ Р Р†Р С‘Р Т‘Р ВµР С•
	static DataType GetFirstDFVType() { return dtFrameVideo1; }
	static DataType GetLastDFVType() { return dtFrameVideo10; }
	//Р В°Р Р…Р В°Р В»Р С•Р С–Р С•Р Р†Р С•Р Вµ Р С�Р В°РЎвЂљРЎР‚Р С‘РЎвЂЎР Р…Р С•Р Вµ Р Р†Р С‘Р Т‘Р ВµР С•
	static DataType GetFirstAFVType() { return dtFrameVideo11; }
	static DataType GetLastAFVType() { return dtFrameVideo16; }
	//Р РЋР С”Р С•РЎР‚Р С•РЎРѓРЎвЂљР Р…Р С•Р Вµ РЎвЂ Р С‘РЎвЂћРЎР‚Р С•Р Р†Р С•Р Вµ Р С�Р В°РЎвЂљРЎР‚Р С‘РЎвЂЎР Р…Р С•Р Вµ Р Р†Р С‘Р Т‘Р ВµР С•
	static DataType GetFirstFFVType() { return dtFrameVideo17; }
	static DataType GetLastFFVType() { return dtFrameVideo20; }
	//Р В»Р В°Р В·Р ВµРЎР‚Р Р…РЎвЂ№Р в„– Р С—РЎР‚Р С•РЎвЂћР С‘Р В»РЎРЉ
	static DataType GetFirstProfileType() { return dtLaserProfile1; }
	static DataType GetLastProfileType() { return dtLaserProfile4; }

	static int LaserProfileIdx(DataType aDataType)
	{
		return aDataType - GetFirstProfileType();
	}
	static int FastFrameVideoIdx(DataType aDataType)
	{
		return aDataType - GetFirstFFVType();
	}
	static int FrameVideoIdx(DataType aDataType)
	{
		return aDataType - GetFirstDFVType();
	}
	static int AnalogFrameVideoIdx(DataType aDataType)
	{
		return aDataType - GetFirstAFVType();
	}
	static int LineVideoIdx(DataType aDataType)
	{
		return aDataType - GetFirstLVType();
	}

	static SetOfDataType GetLineVideoTypes()
	{
		SetOfDataType SetOfTypes;
		for(DataType i=GetFirstLVType(); i<=GetLastLVType(); ++i)
			SetOfTypes.insert(i);
		return SetOfTypes;
	}
	static SetOfDataType GetFrameVideoTypes()
	{
		SetOfDataType SetOfTypes;
		for(DataType i=GetFirstDFVType(); i<=GetLastDFVType(); ++i)
			SetOfTypes.insert(i);
		return SetOfTypes;
	}
	static SetOfDataType GetAnalogFrameVideoTypes()
	{
		SetOfDataType SetOfTypes;
		for(DataType i=GetFirstAFVType(); i<=GetLastAFVType(); ++i)
			SetOfTypes.insert(i);
		return SetOfTypes;
	}
	static SetOfDataType GetFastFrameVideoTypes()
	{
		SetOfDataType SetOfTypes;
		for(DataType i=GetFirstFFVType(); i<=GetLastFFVType(); ++i)
			SetOfTypes.insert(i);
		return SetOfTypes;
	}
	static SetOfDataType GetAllFrameVideoTypes()
	{
		SetOfDataType SetOfTypes;
		for(DataType i=GetFirstDFVType(); i<=GetLastFFVType(); ++i)
			SetOfTypes.insert(i);
		return SetOfTypes;
	}
	static SetOfDataType GetProfileTypes()
	{
		SetOfDataType SetOfTypes;
		for(DataType i=GetFirstProfileType(); i<=GetLastProfileType(); ++i)
			SetOfTypes.insert(i);
		return SetOfTypes;
	}
	static SetOfDataType GetTypes(DataTypes::DataClass aClass)
	{
		SetOfDataType SetOfTypes;
		switch( aClass )
		{
			case DataTypes::dcLineVideo: return GetLineVideoTypes();
			case DataTypes::dcFrameVideo: 
			case DataTypes::dcFrameAnalogVideo:
			case DataTypes::dcFrameFastVideo: return GetAllFrameVideoTypes();
			case DataTypes::dcLaserProfile: return GetProfileTypes();
			default: return SetOfTypes;
		}
		return SetOfTypes;
	}

	
	static DataTypes::DataClass ToDataClass( DataType aDataType )
	{
    	if(aDataType == dtBeam)
        	return DataTypes::dcBeam;
		if(aDataType == dtUSM || aDataType == dtUS_LEFT || aDataType == dtUS_RIGHT ||
			aDataType == dtEchoComplex || aDataType == dtGus || aDataType == dtMagDef)
			return DataTypes::dcUSM;
		if( aDataType >= GetFirstLVType() && aDataType <= GetLastLVType() )
			return DataTypes::dcLineVideo;
		if( aDataType >= GetFirstDFVType() && aDataType <= GetLastDFVType() )
			return DataTypes::dcFrameVideo;
		if( aDataType >= GetFirstAFVType() && aDataType <= GetLastAFVType() )
			return DataTypes::dcFrameAnalogVideo;
		if( aDataType >= GetFirstFFVType() && aDataType <= GetLastFFVType() )
			return DataTypes::dcFrameFastVideo;
		if( aDataType >= GetFirstProfileType() && aDataType <= GetLastProfileType() )
			return DataTypes::dcLaserProfile;
		if( aDataType == dtWM )
			return DataTypes::dcWayMeter;
		if( aDataType == dtSWD || aDataType == dtSWDAntenna1 || aDataType == dtSWDAntenna2 || aDataType == dtSWDAntenna3)
			return DataTypes::dcSWD;

		return DataTypes::dcNONE;
	}
	static const wchar_t* DataClassToStringW(DataTypes::DataClass aClass)
	{
#if (defined WINDOWS_OS && defined __BORLANDC__)
		static UnicodeString builderStr;
#endif

#ifdef DataTypeResourceH
		static wchar_t m_Str[128];
		switch( aClass )
		{
#ifdef _MSC_VER
			case DataTypes::dcNONE: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCNONE, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcUSM: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCUSM, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcLineVideo: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCLINEVIDEO, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcFrameVideo: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCFRAMEVIDEO, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcLaserProfile: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCLASERPROFILE, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcWayMeter: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCWAYMETER, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcFrameAnalogVideo: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCFRAMEANALOGVIDEO, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcFrameFastVideo: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCFRAMEFASTVIDEO, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
			case DataTypes::dcSWD: ::LoadString(m_hResInstance, IDS_DATATYPERES_DCSWD, m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ])); return m_Str;
#else
			case DataTypes::dcNONE: builderStr = LoadStr(IDS_DATATYPERES_DCNONE); return builderStr.c_str();
			case DataTypes::dcUSM: builderStr = LoadStr(IDS_DATATYPERES_DCUSM); return builderStr.c_str();
			case DataTypes::dcLineVideo: builderStr = LoadStr(IDS_DATATYPERES_DCLINEVIDEO); return builderStr.c_str();
			case DataTypes::dcFrameVideo: builderStr = LoadStr(IDS_DATATYPERES_DCFRAMEVIDEO); return builderStr.c_str();
			case DataTypes::dcLaserProfile: builderStr = LoadStr(IDS_DATATYPERES_DCLASERPROFILE); return builderStr.c_str();
			case DataTypes::dcWayMeter: builderStr = LoadStr(IDS_DATATYPERES_DCWAYMETER); return builderStr.c_str();
			case DataTypes::dcFrameAnalogVideo: builderStr = LoadStr(IDS_DATATYPERES_DCFRAMEANALOGVIDEO); return builderStr.c_str();
			case DataTypes::dcFrameFastVideo: builderStr = LoadStr(IDS_DATATYPERES_DCFRAMEFASTVIDEO); return builderStr.c_str();
			case DataTypes::dcSWD: builderStr = LoadStr(IDS_DATATYPERES_DCSWD); return builderStr.c_str();
#endif
		}
#endif //WINDOW_OS
		return NULL;
	}
	static const wchar_t* DataClassToShortStringW(DataTypes::DataClass aClass)
	{
		static wchar_t m_Str[128];
		switch( aClass )
		{
			case DataTypes::dcNONE: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"NONE"); return m_Str;
			case DataTypes::dcUSM: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"USM"); return m_Str;
			case DataTypes::dcLineVideo: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"LineVideo"); return m_Str;
			case DataTypes::dcFrameVideo: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"FrameVideo"); return m_Str;
			case DataTypes::dcLaserProfile: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"LaserProfile"); return m_Str;
			case DataTypes::dcWayMeter: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"WayMeter"); return m_Str;
			case DataTypes::dcFrameAnalogVideo: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"FrameAnalogVideo"); return m_Str;
			case DataTypes::dcFrameFastVideo: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"FrameFastVideo"); return m_Str;
			case DataTypes::dcSWD: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"SWD"); return m_Str;
			default: return NULL;
		}
		return NULL;
	}
	static DataTypes::DataClass ToDataClassW(const wchar_t* strClass)
	{
		if( wcscmp( strClass, L"NONE" ) == 0 ) return DataTypes::dcNONE;
		if( wcscmp( strClass, L"USM" ) == 0 ) return DataTypes::dcUSM;
		if( wcscmp( strClass, L"LineVideo" ) == 0 ) return DataTypes::dcLineVideo;
		if( wcscmp( strClass, L"FrameVideo" ) == 0 ) return DataTypes::dcFrameVideo;
		if( wcscmp( strClass, L"LaserProfile" ) == 0 ) return DataTypes::dcLaserProfile;
		if( wcscmp( strClass, L"WayMeter" ) == 0 ) return DataTypes::dcWayMeter;
		if( wcscmp( strClass, L"FrameAnalogVideo" ) == 0 ) return DataTypes::dcFrameAnalogVideo;
		if( wcscmp( strClass, L"FrameFastVideo" ) == 0 ) return DataTypes::dcFrameFastVideo;
		if( wcscmp( strClass, L"SWD" ) == 0 ) return DataTypes::dcSWD;
		return DataTypes::dcNONE;
	}
	static const char* ToString(DataType aType)
	{
		static char m_Str[128];
		switch( aType )
		{
			case dtNONE: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "NONE"); return m_Str;
			case dtUS_LEFT:
			case dtUS_RIGHT:
			case dtUSM: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "USM"); return m_Str;
			case dtGeo: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "Geo"); return m_Str;
			case dtWM: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "WM"); return m_Str;
			case dtEchoComplex: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "EC"); return m_Str;
			case dtGus: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "Gus"); return m_Str;
			case dtMagDef: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "MagDef"); return m_Str;
			case dtCAN:	strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "CAN"); return m_Str;
			case dtGyroScope: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "GyroScope"); return m_Str;
			case dtGPS: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "GPS"); return m_Str;
			case dtSWD:	strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "SWD"); return m_Str;
			case dtSWDAntenna1:
			case dtSWDAntenna2:
			case dtSWDAntenna3:
				sprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "SWDAntenna%d", aType - dtSWDAntenna1 + 1 ); return m_Str;
			case dtHandCart:
				strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "HandCart"); return m_Str;
			case dtRF603_1:
			case dtRF603_2:
				sprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "RF603_%d", aType - dtRF603_1 + 1 ); return m_Str;
			case dtRouteTrack: strcpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "RouteTrack"); return m_Str;
			default: strcpy_s(m_Str, sizeof(m_Str) / sizeof(m_Str[0]), "NONE"); return m_Str;
		}
		if( aType >= GetFirstLVType() && aType <= GetLastLVType() )
		{
			sprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "LineVideo%d", aType - GetFirstLVType() + 1 );
			return m_Str;
		}
		if( aType >= GetFirstDFVType() && aType <= GetLastFFVType() )
		{
			sprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "FrameVideo%d", aType - GetFirstDFVType() + 1 );
			return m_Str;
		}
		if( aType >= GetFirstProfileType() && aType <= GetLastProfileType() )
		{
			sprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), "LaserProfile%d", aType - GetFirstProfileType() + 1 );
			return m_Str;
		}
		return NULL;
	}
	static const wchar_t* ToStringW(DataType aType)
	{
		static wchar_t m_Str[128];
		switch( aType )
		{
			case dtNONE: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"NONE"); return m_Str;
			case dtUS_LEFT:
			case dtUS_RIGHT:
			case dtUSM: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"USM"); return m_Str;
			case dtBeam: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"Beam"); return m_Str;
			case dtGeo: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"Geo"); return m_Str;
			case dtWM: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"WM"); return m_Str;
			case dtEchoComplex: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"EC"); return m_Str;
			case dtGus: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"Gus"); return m_Str;
			case dtMagDef: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"MagDef"); return m_Str;
			case dtCAN: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"CAN"); return m_Str;
			case dtGyroScope:	wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"GyroScope"); return m_Str;
			case dtGPS: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"GPS"); return m_Str;
			case dtSWD: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"SWD"); return m_Str;
			case dtSWDAntenna1:
			case dtSWDAntenna2:
			case dtSWDAntenna3:
				swprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"SWDAntenna%d", aType - dtSWDAntenna1 + 1 ); return m_Str;
			case dtHandCart: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"HandCart"); return m_Str;
			case dtRF603_1:
			case dtRF603_2:
				swprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"RF603_%d", aType - dtRF603_1 + 1 ); return m_Str;
			case dtRouteTrack: wcscpy_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"RouteTrack"); return m_Str;
			default: wcscpy_s(m_Str, sizeof(m_Str) / sizeof(m_Str[0]), L"NONE"); return m_Str;
		}
		if( aType >= GetFirstLVType() && aType <= GetLastLVType() )
		{
			swprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"LineVideo%d", aType - GetFirstLVType() + 1 ); 
			return m_Str;
		}
		if( aType >= GetFirstDFVType() && aType <= GetLastFFVType() )
		{
			swprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"FrameVideo%d", aType - GetFirstDFVType() + 1 ); 
			return m_Str;
		}
		if( aType >= GetFirstProfileType() && aType <= GetLastProfileType() )
		{
			swprintf_s(m_Str, sizeof( m_Str ) / sizeof( m_Str[ 0 ] ), L"LaserProfile%d", aType - GetFirstProfileType() + 1 ); 
			return m_Str;
		}
		return NULL;
	}

	static const wchar_t* ToLongStringW(DataType aType)
	{
		static wchar_t m_Str1[128];
#ifdef __BORLANDC__
		static UnicodeString builderStr1;
#endif
		switch( aType )
		{
			case dtNONE: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"NONE"); return m_Str1;
			case dtUS_LEFT:
			case dtUS_RIGHT:
			case dtUSM: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"USM"); return m_Str1;
			case dtBeam: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"Beam"); return m_Str1;
			case dtGeo: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"Geo"); return m_Str1;
			case dtWM: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"WM"); return m_Str1;
			case dtEchoComplex: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"EC"); return m_Str1;
			case dtGus:
				{
#ifdef DataTypeResourceH
#ifdef _MSC_VER
				::LoadString(m_hResInstance, IDS_DATATYPERES_GUS, m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ])); return m_Str1;
#else
				builderStr1 = LoadStr(IDS_DATATYPERES_GUS); return builderStr1.c_str();
#endif
#else
				wcscpy_s(m_Str1, sizeof(m_Str1) / sizeof(m_Str1[0]), L"GUS"); return m_Str1;
#endif
				}
			case dtCAN: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"CAN"); return m_Str1;
			case dtGyroScope:	wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"GyroScope"); return m_Str1;
			case dtGPS: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"GPS"); return m_Str1;
			case dtSWD: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"SWD"); return m_Str1;
			case dtSWDAntenna1:
			case dtSWDAntenna2:
			case dtSWDAntenna3:
				swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"SWDAntenna%d", aType - dtSWDAntenna1 + 1 ); return m_Str1;
			case dtHandCart: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"HandCart"); return m_Str1;
			case dtRF603_1:
			case dtRF603_2:
				swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"RF603_%d", aType - dtRF603_1 + 1 ); return m_Str1;
			case dtRouteTrack: wcscpy_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), L"RouteTrack"); return m_Str1;
			default: wcscpy_s(m_Str1, sizeof(m_Str1) / sizeof(m_Str1[0]), L"NONE"); return m_Str1;
		}
		if(aType >= GetFirstLVType() && aType <= GetLastLVType())
		{
			const wchar_t* szTemplate = NULL;
#ifdef DataTypeResourceH
#ifdef _MSC_VER
			::LoadString(m_hResInstance, IDS_DATATYPERES_LINEVIDEO, m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ]));
			szTemplate = m_Str1;
#else
			builderStr1 = LoadStr(IDS_DATATYPERES_LINEVIDEO);
			szTemplate = builderStr1.c_str();
#endif
#endif
			swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), szTemplate, aType - GetFirstLVType() + 1 );
			return m_Str1;
		}
		if(aType >= GetFirstDFVType() && aType <= GetLastDFVType())
		{
			const wchar_t* szTemplate = NULL;
#ifdef DataTypeResourceH
#ifdef _MSC_VER
			::LoadString(m_hResInstance, IDS_DATATYPERES_DFRAMEVIDEO, m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ]));
			szTemplate = m_Str1;
#else
			builderStr1 = LoadStr(IDS_DATATYPERES_DFRAMEVIDEO);
			szTemplate = builderStr1.c_str();
#endif
#endif
			swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), szTemplate, aType - GetFirstDFVType() + 1 );
			return m_Str1;
		}
		if(aType >= GetFirstFFVType() && aType <= GetLastFFVType())
		{
			const wchar_t* szTemplate = NULL;
#ifdef WINDODataTypeResourceHWS_OS
#ifdef _MSC_VER
			::LoadString(m_hResInstance, IDS_DATATYPERES_FFRAMEVIDEO, m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ]));
			szTemplate = m_Str1;
#else
			builderStr1 = LoadStr(IDS_DATATYPERES_FFRAMEVIDEO);
			szTemplate = builderStr1.c_str();
#endif
#endif
			swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), szTemplate, aType - GetFirstFFVType() + 1 ); 
			return m_Str1;
		}
		if(aType >= GetFirstAFVType() && aType <= GetLastAFVType())
		{
			const wchar_t* szTemplate = NULL;
#ifdef DataTypeResourceH
#ifdef _MSC_VER
			::LoadString(m_hResInstance, IDS_DATATYPERES_AFRAMEVIDEO , m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ]));
			szTemplate = m_Str1;
#else
			builderStr1 = LoadStr(IDS_DATATYPERES_AFRAMEVIDEO);
			szTemplate = builderStr1.c_str();
#endif
#endif
			swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), szTemplate, aType - GetFirstAFVType() + 1 ); 
			return m_Str1;
		}
		if(aType >= GetFirstProfileType() && aType <= GetLastProfileType())
		{
			const wchar_t* szTemplate = NULL;
#ifdef DataTypeResourceH
#ifdef _MSC_VER
			::LoadString(m_hResInstance, IDS_DATATYPERES_LASERPROFILE, m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ]));
			szTemplate = m_Str1;
#else
			builderStr1 = LoadStr(IDS_DATATYPERES_LASERPROFILE);
			szTemplate = builderStr1.c_str();
#endif
#endif
			swprintf_s(m_Str1, sizeof( m_Str1 ) / sizeof( m_Str1[ 0 ] ), szTemplate, aType - GetFirstProfileType() + 1 ); 
			return m_Str1;
		}
		return NULL;
	}

	static DataType ToDataType(const char* strType)
	{
		if( strcmp( strType, "NONE" ) == 0 ) return dtNONE;
		if( strcmp( strType, "USM" ) == 0 ) return dtUSM;
		if( strcmp( strType, "Geo" ) == 0 ) return dtGeo;
		if( strcmp( strType, "WM" ) == 0 ) return dtWM;
		if( strcmp( strType, "EC" ) == 0 ) return dtEchoComplex;
		if( strcmp( strType, "Gus" ) == 0 ) return dtGus;
		if( strcmp( strType, "MagDef" ) == 0 ) return dtMagDef;
		if( strcmp( strType, "Beam" ) == 0 ) return dtBeam;
		if( strcmp( strType, "LineVideo1" ) == 0 ) return dtLineVideo1;
		if( strcmp( strType, "LineVideo2" ) == 0 ) return dtLineVideo2;
		if( strcmp( strType, "LineVideo3" ) == 0 ) return dtLineVideo3;
		if( strcmp( strType, "LineVideo4" ) == 0 ) return dtLineVideo4;
		if( strcmp( strType, "LineVideo5" ) == 0 ) return dtLineVideo5;
		if( strcmp( strType, "LineVideo6" ) == 0 ) return dtLineVideo6;
		if( strcmp( strType, "LineVideo7" ) == 0 ) return dtLineVideo7;
		if( strcmp( strType, "LineVideo8" ) == 0 ) return dtLineVideo8;
		if( strcmp( strType, "LineVideo9" ) == 0 ) return dtLineVideo9;
		if( strcmp( strType, "LineVideo10" ) == 0 ) return dtLineVideo10;
		if( strcmp( strType, "LineVideo11" ) == 0 ) return dtLineVideo11;
		if( strcmp( strType, "LineVideo12" ) == 0 ) return dtLineVideo12;
		if( strcmp( strType, "LineVideo13" ) == 0 ) return dtLineVideo13;
		if( strcmp( strType, "LineVideo14" ) == 0 ) return dtLineVideo14;
		if( strcmp( strType, "LineVideo15" ) == 0 ) return dtLineVideo15;
		if( strcmp( strType, "LineVideo16" ) == 0 ) return dtLineVideo16;

		if( strcmp( strType, "FrameVideo1" ) == 0 ) return dtFrameVideo1;
		if( strcmp( strType, "FrameVideo2" ) == 0 ) return dtFrameVideo2;
		if( strcmp( strType, "FrameVideo3" ) == 0 ) return dtFrameVideo3;
		if( strcmp( strType, "FrameVideo4" ) == 0 ) return dtFrameVideo4;
		if( strcmp( strType, "FrameVideo5" ) == 0 ) return dtFrameVideo5;
		if( strcmp( strType, "FrameVideo6" ) == 0 ) return dtFrameVideo6;
		if( strcmp( strType, "FrameVideo7" ) == 0 ) return dtFrameVideo7;
		if( strcmp( strType, "FrameVideo8" ) == 0 ) return dtFrameVideo8;
		if( strcmp( strType, "FrameVideo9" ) == 0 ) return dtFrameVideo9;
		if( strcmp( strType, "FrameVideo10" ) == 0 ) return dtFrameVideo10;
		if( strcmp( strType, "FrameVideo11" ) == 0 ) return dtFrameVideo11;
		if( strcmp( strType, "FrameVideo12" ) == 0 ) return dtFrameVideo12;
		if( strcmp( strType, "FrameVideo13" ) == 0 ) return dtFrameVideo13;
		if( strcmp( strType, "FrameVideo14" ) == 0 ) return dtFrameVideo14;
		if( strcmp( strType, "FrameVideo15" ) == 0 ) return dtFrameVideo15;
		if( strcmp( strType, "FrameVideo16" ) == 0 ) return dtFrameVideo16;
		if( strcmp( strType, "FrameVideo17" ) == 0 ) return dtFrameVideo17;
		if( strcmp( strType, "FrameVideo18" ) == 0 ) return dtFrameVideo18;
		if( strcmp( strType, "FrameVideo19" ) == 0 ) return dtFrameVideo19;
		if( strcmp( strType, "FrameVideo20" ) == 0 ) return dtFrameVideo20;

		if( strcmp( strType, "GPS" ) == 0 ) return dtGPS;
		if( strcmp( strType, "GPS1" ) == 0 ) return dtGPS; //remove in future
		if( strcmp( strType, "GyroScope" ) == 0 ) return dtGyroScope;
		if( strcmp( strType, "Switchers" ) == 0 ) return dtSWD; //remove in future
		if( strcmp( strType, "SWD" ) == 0 ) return dtSWD;
		if( strcmp( strType, "SWDAntenna1" ) == 0 ) return dtSWDAntenna1;
		if( strcmp( strType, "SWDAntenna2" ) == 0 ) return dtSWDAntenna2;
		if( strcmp( strType, "SWDAntenna3" ) == 0 ) return dtSWDAntenna3;

		if( strcmp( strType, "HandCart" ) == 0 ) return dtHandCart;
		if( strcmp( strType, "RF603_1" ) == 0 ) return dtRF603_1;
		if( strcmp( strType, "RF603_2" ) == 0 ) return dtRF603_2;

		if( strcmp( strType, "LaserProfile1" ) == 0 ) return dtLaserProfile1;
		if( strcmp( strType, "LaserProfile2" ) == 0 ) return dtLaserProfile2;
		if( strcmp( strType, "LaserProfile3" ) == 0 ) return dtLaserProfile3;
		if( strcmp( strType, "LaserProfile4" ) == 0 ) return dtLaserProfile4;

		if( strcmp( strType, "RouteTrack" ) == 0 ) return dtRouteTrack;

		return dtNONE;
	}
	static DataType ToDataTypeW(const wchar_t* strType)
	{
		if( wcscmp( strType, L"NONE" ) == 0 ) return dtNONE;
		if( wcscmp( strType, L"USM" ) == 0 ) return dtUSM;
		if( wcscmp( strType, L"Geo" ) == 0 ) return dtGeo;
		if( wcscmp( strType, L"WM" ) == 0 ) return dtWM;
		if( wcscmp( strType, L"EC" ) == 0 ) return dtEchoComplex;
		if( wcscmp( strType, L"Gus" ) == 0 ) return dtGus;
		if( wcscmp( strType, L"MagDef" ) == 0 ) return dtMagDef;
		if( wcscmp( strType, L"Beam" ) == 0 ) return dtBeam;
		if( wcscmp( strType, L"LineVideo1" ) == 0 ) return dtLineVideo1;
		if( wcscmp( strType, L"LineVideo2" ) == 0 ) return dtLineVideo2;
		if( wcscmp( strType, L"LineVideo3" ) == 0 ) return dtLineVideo3;
		if( wcscmp( strType, L"LineVideo4" ) == 0 ) return dtLineVideo4;
		if( wcscmp( strType, L"LineVideo5" ) == 0 ) return dtLineVideo5;
		if( wcscmp( strType, L"LineVideo6" ) == 0 ) return dtLineVideo6;
		if( wcscmp( strType, L"LineVideo7" ) == 0 ) return dtLineVideo7;
		if( wcscmp( strType, L"LineVideo8" ) == 0 ) return dtLineVideo8;
		if( wcscmp( strType, L"LineVideo9" ) == 0 ) return dtLineVideo9;
		if( wcscmp( strType, L"LineVideo10" ) == 0 ) return dtLineVideo10;
		if( wcscmp( strType, L"LineVideo11" ) == 0 ) return dtLineVideo11;
		if( wcscmp( strType, L"LineVideo12" ) == 0 ) return dtLineVideo12;
		if( wcscmp( strType, L"LineVideo13" ) == 0 ) return dtLineVideo13;
		if( wcscmp( strType, L"LineVideo14" ) == 0 ) return dtLineVideo14;
		if( wcscmp( strType, L"LineVideo15" ) == 0 ) return dtLineVideo15;
		if( wcscmp( strType, L"LineVideo16" ) == 0 ) return dtLineVideo16;

		if( wcscmp( strType, L"FrameVideo1" ) == 0 ) return dtFrameVideo1;
		if( wcscmp( strType, L"FrameVideo2" ) == 0 ) return dtFrameVideo2;
		if( wcscmp( strType, L"FrameVideo3" ) == 0 ) return dtFrameVideo3;
		if( wcscmp( strType, L"FrameVideo4" ) == 0 ) return dtFrameVideo4;
		if( wcscmp( strType, L"FrameVideo5" ) == 0 ) return dtFrameVideo5;
		if( wcscmp( strType, L"FrameVideo6" ) == 0 ) return dtFrameVideo6;
		if( wcscmp( strType, L"FrameVideo7" ) == 0 ) return dtFrameVideo7;
		if( wcscmp( strType, L"FrameVideo8" ) == 0 ) return dtFrameVideo8;
		if( wcscmp( strType, L"FrameVideo9" ) == 0 ) return dtFrameVideo9;
		if( wcscmp( strType, L"FrameVideo10" ) == 0 ) return dtFrameVideo10;
		if( wcscmp( strType, L"FrameVideo11" ) == 0 ) return dtFrameVideo11;
		if( wcscmp( strType, L"FrameVideo12" ) == 0 ) return dtFrameVideo12;
		if( wcscmp( strType, L"FrameVideo13" ) == 0 ) return dtFrameVideo13;
		if( wcscmp( strType, L"FrameVideo14" ) == 0 ) return dtFrameVideo14;
		if( wcscmp( strType, L"FrameVideo15" ) == 0 ) return dtFrameVideo15;
		if( wcscmp( strType, L"FrameVideo16" ) == 0 ) return dtFrameVideo16;
		if( wcscmp( strType, L"FrameVideo17" ) == 0 ) return dtFrameVideo17;
		if( wcscmp( strType, L"FrameVideo18" ) == 0 ) return dtFrameVideo18;
		if( wcscmp( strType, L"FrameVideo19" ) == 0 ) return dtFrameVideo19;
		if( wcscmp( strType, L"FrameVideo20" ) == 0 ) return dtFrameVideo20;

		if( wcscmp( strType, L"GyroScope" ) == 0 ) return dtGyroScope;
		if( wcscmp( strType, L"GPS1" ) == 0 ) return dtGPS; //remove in future
		if( wcscmp( strType, L"GPS" ) == 0 ) return dtGPS;
		if( wcscmp( strType, L"Switchers" ) == 0 ) return dtSWD;//remove in future
		if( wcscmp( strType, L"SWD" ) == 0 ) return dtSWD;
		if( wcscmp( strType, L"SWDAntenna1" ) == 0 ) return dtSWDAntenna1;
		if( wcscmp( strType, L"SWDAntenna2" ) == 0 ) return dtSWDAntenna2;
		if( wcscmp( strType, L"SWDAntenna3" ) == 0 ) return dtSWDAntenna3;
		if( wcscmp( strType, L"HandCart1" ) == 0 ) return dtHandCart;
		if( wcscmp( strType, L"RF603_1" ) == 0 ) return dtRF603_1;
		if( wcscmp( strType, L"RF603_2" ) == 0 ) return dtRF603_2;

		if( wcscmp( strType, L"LaserProfile1" ) == 0 ) return dtLaserProfile1;
		if( wcscmp( strType, L"LaserProfile2" ) == 0 ) return dtLaserProfile2;
		if( wcscmp( strType, L"LaserProfile3" ) == 0 ) return dtLaserProfile3;
		if( wcscmp( strType, L"LaserProfile4" ) == 0 ) return dtLaserProfile4;

		if( wcscmp( strType, L"RouteTrack" ) == 0 ) return dtRouteTrack;

		return dtNONE;
	}

};
//char DataTypeConverter::m_Str[128];
//#pragma warning(default : 4996)
#endif
