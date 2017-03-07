#pragma once
#ifndef UExtendedTypeH
#define UExtendedTypeH
//---------------------------------------------------------------------------

#include <string>
#include <vector>

#include "USDataFileTypes.h"
#include "HelperFunctions.h"

// Constants & Macros

//максимальное количество дорожек на экране
#define MAX_TRACK_COUNT			12
//максимальное количество каналов на одной дорожке
#define MAX_CHANNELS_PER_TRACK	4

#define DEFAULT_TRACK_HEIGHT	128;

#define TColor2RGB(c)	(((c & 0xFF0000) >> 16) | (c & 0xFF00) | ((c & 0xFF) << 16))
#define RGB(r,g,b)      ((uint32_t)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint32_t)(uint8_t)(b))<<16)))
#define RGBtoBGR(c)		(uint32_t)(((c >> 16) & 0xFF) | (c & 0xFF00) | ((c << 16) & 0xFF0000));
//---------------------------------------------------------------------------

//	WINDOWS MESSAGES
#define WM_SET_POSITION					(WM_USER + 1001)
#define WM_SHOW_TEST_ITEM				(WM_USER + 1002)
#define WM_CHANGE_CHANNEL				(WM_USER + 1002)
#define WM_UPDATE_CORRECTION_LIST		(WM_USER + 2001)
#define	WM_UPDATE_DEFECT_LIST			(WM_USER + 2002)
#define WM_UPDATE_SECOND_FORM       	(WM_USER + 3001)
#define WM_CLOSE_TRIP_MESSAGE			(WM_USER + 4001)
#define	WM_START_REC_FILE				(WM_USER + 4002)
#define WM_SET_SECOND_FORM_MASTER_FRAME	(WM_USER + 5001)
#define	WM_RECREATE_DX_NEEDED           (WM_USER + 6001)
#define WM_BWE_FIND_FINALIZE            (WM_USER + 0x10)
#define WM_BWE_MARK                     (WM_USER + 0x11)
//---------------------------------------------------------------------------

#define MAX_QUEUE_SLOT_COUNT	32


//---------------------------------------------------------------------------

// режим отображения данных на дорожке
enum TapeMode
{
	tmAllTrackEqual,	// Все дорожки
	tmStretchOne,		// Одна растянута
	tmDefaultHeight,	// Все дорожки предопределённой высоты
	tmOneSide,			// Купейная сторона
	tmTwoSide,			// Корридорная сторона
	tmMainOnly			// Показывать только основные дорожки
};
//---------------------------------------------------------------------------

// режим Zoom панели
enum TZoomType
{
	ztAScan, ztBScan, zt3DScan, ztVScan
};
//---------------------------------------------------------------------------

// режим выделения области на ленте и прочие функции правой кнопки мыши
namespace US
{
	enum TSelectMode
	{
		smNone,
		smSelecting,
		smSelected,
		smChannelAlign,
		smShowDefect,
		smRectSelecting,
		smRectSelected,
		smScroll,
		smRightMousePause,
		smSleepersCount
	};
};
//---------------------------------------------------------------------------

enum TrackType
{
	ttNone,		// Не определено
	ttGrid,		// Координатная линейка
	ttScope,	// Осциограмма
	ttChart		// Точечный рисунок
};
//---------------------------------------------------------------------------

struct TRect
{
	int Left;
	int Top;
	int Right;
	int Bottom;
	int Height() { return abs(Bottom - Top); }
	int Width() { return abs(Right - Left); }
	TRect()
	{}
	TRect(int aLeft, int aTop, int aRight, int aBottom)
		: Left(aLeft)
		, Top(aTop)
		, Right(aRight)
		, Bottom(aBottom)
	{}
};

// Режим отображения (прокрутки) ленты
enum TPresentState
{
	psLatest, psPaused, psFwStep, psBkStep, psFwPlay, psBkPlay, psFwRewind, psBkRewind
};
//---------------------------------------------------------------------------

// Зона контроля для УЗ канала
enum TSonicZone
{
	szNone = 0,		// не указано
	szHead = 1,		// Головка
    szNek = 2,		// Шейка
    szFoot = 4,		// Подошва
    szFull = szHead | szNek | szFoot	// Всё сечение
};

inline TSonicZone SonicZoneFromInt(int aSZ)
{
    switch(aSZ)
    {
    	case 0: return szNone;
        case 1: return szHead;
        case 2: return szNek;
        case 3: return szFoot;
        case 4: return szFull;
        default: return szNone;
    }
}

inline int SonicZoneToInt(TSonicZone aSZ)
{
    switch(aSZ)
    {
        case szNone: return 0;
        case szHead: return 1;
        case szNek: return 2;
        case szFoot: return 3;
        case szFull: return 4;
        default: return 0;
    }
}

inline std::wstring SonicZoneToString(TSonicZone aSZ)
{
    switch(aSZ)
    {
        case szNone: return L"Не указана";
        case szHead: return L"Головка";
        case szNek: return L"Шейка";
        case szFoot: return L"Подошва";
        case szFull: return L"Всё сечение";
        default: return L"Не указана";
    }
}
//---------------------------------------------------------------------------

// Тип УЗ-канала (приёмник, передатчик или совмещённый)
enum TUSChannelType
{
	ctNone = 0,			// Нет
	ctSender = 1,		// Излучатель
    ctReceiver = 2,		// Примник
    ctBi = ctSender | ctReceiver,	// Приёмник+Передатчик
    ctPC = 8	// Раздельно совмещённый
};

inline TUSChannelType USChannelTypeFromInt(int aCT)
{
    switch(aCT)
    {
        case 0: return ::ctNone;
        case 1: return ctSender;
        case 2: return ctReceiver;
        case 3: return ctBi;
        case 4: return ctPC;
		default: return ::ctNone;
    }
}

inline int USChannelTypeToInt(TUSChannelType aCT)
{
    switch(aCT)
    {
        case ::ctNone: return 0;
        case ctSender: return 1;
        case ctReceiver: return 2;
        case ctBi: return 3;
        case ctPC: return 8;
        default: return 0;
    }
}

inline std::wstring USChannelTypeToString(TUSChannelType aCT)
{
    switch(aCT)
    {
        case ::ctNone: return L"Нет";
        case ctSender: return L"Транс.";
        case ctReceiver: return L"Рес.";
        case ctBi: return L"Совм.";
        case ctPC: return L"Раздел.";
        default: return L"Нет";
    }
}
//---------------------------------------------------------------------------

// Направление излучения ПЭП подключенного к каналу
enum TSonicDirection
{
	sdNone,		// Направление неизвестно
    sdForward, 	// Направление вперёд (Наезжающий)
    sdBackward,	// Направление назад (Отезжающий)
    sdZeroDown 	// Направление вниз
};

inline TSonicDirection SonicDirectionFromInt(int aSD)
{
    switch(aSD)
    {
        case 0: return sdNone;
        case 1: return sdForward;
        case 2: return sdBackward;
        case 3: return sdZeroDown;
        default: return sdNone;
    }
}

inline int SonicDirectionToInt(TSonicDirection aSD)
{
    switch(aSD)
    {
        case sdNone: return 0;
        case sdForward: return 1;
        case sdBackward: return 2;
        case sdZeroDown: return 3;
        default: return 0;
    }
}

inline std::wstring SonicDirectionToString(TSonicDirection aSD)
{
	switch(aSD)
    {
        case sdNone: return L"Нет";
        case sdForward: return L"Наезжающий";
        case sdBackward: return L"Отъезжающий";
        case sdZeroDown: return L"Прямой-вниз";
        default: return L"Нет";
    }
}
//---------------------------------------------------------------------------

// Сторона вагона на которой расположен ПЭП, который подключен к каналу
enum TCarSide
{
	csNone,		// сторона не известна
	csCorridor,	// Коридорная сторона вагона
    csRoomette	// Купейная сторона вагона
};

inline TCarSide CarSideFromInt(int aCS)
{
    switch(aCS)
    {
        case 0: return csNone;
        case 1: return csCorridor;
        case 2: return csRoomette;
        default: return csNone;
    }
}

inline int CarSideToInt(TCarSide aCS)
{
    switch(aCS)
	{
        case csNone: return 0;
        case csCorridor: return 1;
        case csRoomette: return 2;
        default: return 0;
    }
}

inline std::wstring CarSideToString(TCarSide aCS)
{
	std::wstring str = L"";
	switch(aCS)
	{
		case csNone:
			str = L"Нет";//LoadStr(STR_NO);
			break;
		case csCorridor:
			str = L"Коридор";//LoadStr(STR_PASSAGE);
			break;
		case csRoomette:
			str = L"Купе";//LoadStr(STR_COMPARTMENT);
			break;
		default:
			str = L"Нет";//LoadStr(STR_NO);
	}

	return str;
}
//---------------------------------------------------------------------------


enum TChannelType
{
	ctUnknown,		// Unknown type
	ctUltraSound,	// Ультразвуковой каал
    ctMagnetic		// Магнитный канал
};
//---------------------------------------------------------------------------

// Информация о канале
struct ChannelInfo
{
    // Common
    bool FUsing;		// Используется или нет
	TChannelType FChannelType;		//  Тип канала (УЗ или Магнитный)
    TCarSide FCarSide;	// Сторона вагона с которой расположен искатель, подключенный к этому каналу
    int FChannel;		// Номер канала	(счёт для одной стороны вагона УЗ=0..11, МГ=0..1)
    int FShift;			// Смещение относительно центра лыжи mm

    // UltraSound
	TSonicZone FSonicZone;			// Зона контроля
    TSonicDirection FSonicDirection;// Направление ввода
    TUSChannelType FUSChannelType;	// Тип канала
    int FAlpha;			// Угол ввода
    int FBeta;			// Угол разворота по оси (по часовой стрелке, вид сверху на лыжу)
    int FDelay;			// Установленная задержка в канале (нс)
    int FPhyDelay;		// Физическая задержка в призме (нс)
    int FTickTime;		// Время одного преобразования АЦП в (нс)
    int FTickCount;		// Количество отсчётов в канале (для Эхо-Комплекса==60, для Синтеза==128)
    int FUSSpeed;		// Скорость распространения УЗ (мкм/сек)
    //unsigned char FTuneParam[50];	// Параметры усиления, ВРЧ и порога
    int FThresholdSens;	// Опорная чуствительность (определение 6мм отверстия на 50% пороге VRC=0 Amp1=Amp2)

    int FSalShift;		// Смещение при движении салоном вперёд
    int FCotShift;		// Смещение при движении котлом вперёд

	int FFromTick;		// Показывать данные от канала от этого тика
    int FToTick;		// Показывать данные от канала до этого тика

    // Magnetic
    float FScale;		// Умножение для сигнала
    float FMaxValue;	// Максимальное значение отображаемое на дорожке
    float FMinValue;	// Минималное значение отображаемое на дорожке
	int FBufSize;

	float FValue[4096];		// Значения которые в данный момент отображаются на экране (min & max)
	float FCache[4096];		// Значения подготовленные при последней отрисовке ленты (min & max)

	float m_MeanValue; //среднее или медианное значение (в зависимости от текущей настройки динамического порога)

	ChannelInfo()
	{
		memset(this, 0, sizeof(ChannelInfo));
		FUsing = false;
	}

	ChannelInfo(ChannelInfo *aCI);

	void Assign(ChannelInfo *aCI);

	ChannelInfo(TChannelType aChannelType, int aChannel, TSonicZone aSonicZone, TSonicDirection aSonicDirection, TUSChannelType aUSChannelType,
		int aAlpha, int aBeta, int aDelay, int aPhyDelay, int aTickTime, int aUSSpeed, int aShift, TCarSide aCarSide)
	{
		Set(aChannelType, aChannel, aSonicZone, aSonicDirection, aUSChannelType, aAlpha, aBeta, aDelay, aPhyDelay, aTickTime, aUSSpeed, aShift, aCarSide);
	}

	ChannelInfo(TChannelType aChannelType, int aChannel, float aMinValue, float aMaxValue, float aScale, int aBufSize, int aShift, TCarSide aCarSide)
	{
		Set(aChannelType, aChannel, aMinValue, aMaxValue, aScale, aBufSize, aShift, aCarSide);
	}

	~ChannelInfo(void);

	void Set(TChannelType aChannelType, int aChannel, TSonicZone aSonicZone, TSonicDirection aSonicDirection, TUSChannelType aUSChannelType,
		int aAlpha, int aBeta, int aDelay, int aPhyDelay, int aTickTime, int aUSSpeed, int aShift, TCarSide aCarSide)
	{
		FUsing = true;
		FChannelType = aChannelType;
		FSonicZone = aSonicZone;
		FSonicDirection = aSonicDirection;
		FUSChannelType = aUSChannelType;
		FAlpha = aAlpha;
		FBeta = aBeta;
		FDelay = aDelay;
		FPhyDelay = aPhyDelay;
		FTickTime = aTickTime;

		FTickCount = US_TICK_COUNT; // Синтез
		FFromTick = 0;
		FToTick = FTickCount;

		FUSSpeed = aUSSpeed;
		FShift = aShift;
		FCarSide = aCarSide;
		FChannel = aChannel;

		FCotShift = 0;
		FSalShift = 0;
	}
	void Set(TChannelType aChannelType, int aChannel, float aMinValue, float aMaxValue, float aScale, int aBufSize, int aShift, TCarSide aCarSide)
	{
		FUsing = true;
		FChannelType = aChannelType;
		FBufSize = aBufSize;
		for(int i = 0; i < aBufSize; i++)
		{
			FValue[i] = 0.0F;
			FCache[i] = 0.0F;
		}
		FShift = aShift;
		FMinValue = aMinValue;
		FMaxValue = aMaxValue;
		FScale = aScale;
		FCarSide = aCarSide;
		FChannel = aChannel;
	}

	std::string GetText(void)
    {
		std::string str;
		std::string format;

		switch(FChannelType)
		{
			case ctUltraSound:
				switch(FCarSide)
				{
					case csRoomette:
						if(FUsing)
						{
							format = "№%d, %d°, %d° Купе";//LoadStr(STR_US_CHANNEL_COMPARTMENT_USING);
							str = HelperFunctions::String::FormatA(format.c_str(), FChannel + 1, FAlpha, FBeta);
						}
						else
						{
							format = "№%d ? Купе";// LoadStr(STR_US_CHANNEL_COMPARTMENT_NOT_USING);
							str = HelperFunctions::String::FormatA(format.c_str(), FChannel + 1);
						}
						break;
					case csCorridor:
						if(FUsing)
						{
							format = "№%d, %d°, %d° Кор.";// LoadStr(STR_US_CHANNEL_PASSAGE_USING);
							str = HelperFunctions::String::FormatA(format.c_str(), FChannel + 1, FAlpha, FBeta);
						}
						else
						{
							format = "№%d, ? Кор.";// LoadStr(STR_US_CHANNEL_PASSAGE_NOT_USING);
							str = HelperFunctions::String::FormatA(format.c_str(), FChannel + 1, FAlpha, FBeta);
						}
						break;
					default:
						throw L"Invalid car side";
				}
				break;
			case ctMagnetic:
				switch(FCarSide)
				{
					case csRoomette:
						format = "Магнитный канал №%d Купе";// LoadStr(STR_MAGNETIC_CHANNEL_COMPARTMENT);
						str = HelperFunctions::String::FormatA(format.c_str(), FChannel + 1);
						break;
					case csCorridor:
						format = "Магнитный канал №%d Кор.";// LoadStr(STR_MAGNETIC_CHANNEL_PASSAGE);
						str = HelperFunctions::String::FormatA(format.c_str(), FChannel + 1);
						break;
					default:
						throw L"Invalid car side";
				}
				break;
			default:
				throw L"Invalid channel type";
		}

        return str;
    }
};
//---------------------------------------------------------------------------
// Дорожка на ленте
struct TrackItem
{
	TrackType FTrackType;	// тип дорожки

							// Common
	bool FX15;
	bool FVisible;			// видимая дорожка
	bool FShowTrack;		// дорожка видна на экране
	bool FMainTrack;
	int FDefaultHeight;
	TRect FSrcRect;
	TRect FDstRect;

	// UltraSound
	int FFromTick;
	int FToTick;
	std::vector<ChannelInfo*> m_vecChannelInfo;
	ChannelInfo *FCh1;
	ChannelInfo *FCh2;
	ChannelInfo *FCh3;
	ChannelInfo *FCh4;

	// Magnetic
	int FMinValue;	// Минимальное значение на дорожке
	int FMaxValue;	// Максимальное значение на дорожке

	void UpdateTick(void)
	{
		int n = 0;
		ChannelInfo *ci[4];
		if (FCh1 != NULL) ci[n++] = FCh1;
		if (FCh2 != NULL) ci[n++] = FCh2;
		if (FCh3 != NULL) ci[n++] = FCh3;
		if (FCh4 != NULL) ci[n++] = FCh4;
		FFromTick = US_TICK_COUNT;
		FToTick = 0;
		for (int i = 0; i < n; i++)
		{
			if (FFromTick > ci[i]->FFromTick) FFromTick = ci[i]->FFromTick;
			if (FToTick < ci[i]->FToTick) FToTick = ci[i]->FToTick;
		}
	}

	TrackItem(void)
	{
		memset(this, 0, sizeof(TrackItem));
		FVisible = false;
		FMainTrack = false;
		FX15 = false;
		FCh1 = FCh2 = FCh3 = FCh4 = NULL;
	}

	TrackItem(TrackItem *aTrackItem)
	{
		memcpy(this, aTrackItem, sizeof(TrackItem));
		FCh1 = FCh2 = FCh3 = FCh4 = NULL;
	}

	TrackItem(TrackItem *aTrackItem, ChannelInfo *aOldCI[MAX_CHANNEL_COUNT], ChannelInfo* aNewCI[MAX_CHANNEL_COUNT])
	{
		memcpy(this, aTrackItem, sizeof(TrackItem));
		FCh1 = FCh2 = FCh3 = FCh4 = NULL;
		for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
		{
			if ((aTrackItem->FCh1 != NULL) && (aTrackItem->FCh1 == aOldCI[i])) FCh1 = aNewCI[i];
			if ((aTrackItem->FCh2 != NULL) && (aTrackItem->FCh2 == aOldCI[i])) FCh2 = aNewCI[i];
			if ((aTrackItem->FCh3 != NULL) && (aTrackItem->FCh3 == aOldCI[i])) FCh3 = aNewCI[i];
			if ((aTrackItem->FCh4 != NULL) && (aTrackItem->FCh4 == aOldCI[i])) FCh4 = aNewCI[i];
		}

		UpdateTick();
	}

	TrackItem(TrackType aTrackType, bool aVisible, bool aMainTrack, ChannelInfo *aCh1, ChannelInfo *aCh2 = NULL,
		ChannelInfo *aCh3 = NULL, ChannelInfo *aCh4 = NULL)
	{
		if (aTrackType != ttChart) throw;
		FTrackType = aTrackType;
		FVisible = aVisible;
		FMainTrack = aMainTrack;
		FCh1 = aCh1;
		FCh2 = aCh2;
		FCh3 = aCh3;
		FCh4 = aCh4;

		FDefaultHeight = DEFAULT_TRACK_HEIGHT;

		UpdateTick();
	}

	TrackItem(TrackType aTrackType, bool aVisible, bool aMainTrack, float aMinValue, float aMaxValue,
		ChannelInfo *aCh1, ChannelInfo *aCh2 = NULL)
	{
		if (aTrackType != ttScope) throw;
		FTrackType = aTrackType;
		FVisible = aVisible;
		FMainTrack = aMainTrack;
		FMinValue = (int)aMinValue;
		FMaxValue = (int)aMaxValue;
		FCh1 = aCh1;
		FCh2 = aCh2;
		FCh3 = NULL;
		FCh4 = NULL;

		FDefaultHeight = DEFAULT_TRACK_HEIGHT;
	}
};


//extern TUSColor class_uscolor;
typedef uint32_t USCOLOR;
#define MAKE_USCOLOR(USColorClass, c1, c2, mask)	0xFF000000 | USColorClass->FC[(c1 << 12) + (c2 << 4) + mask]
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#define COLOR_ITEM(c1, c2, mask)	FC[(c1 << 12) + (c2 << 4) + mask]

//---------------------------------------------------------------------------

#define clrYellow	65535
#define clrAqua		16776960
#define clrLime		65280
#define clrFuchsia	16711935
#define clrRed		255
#define clrNavy		8388608
#define clrBlack	0
#define clrWhite	16777215
#define clrGreen	32768
#define clrBlue		16711680
// Структура для хранения таблицы цветов, которыми отображаются УЗ-данные, при  наложении на одной дорожке
class USColor
{
private:
	unsigned int GetMdlColor(unsigned int aC1, unsigned int aC2, int aIntens, int aMax)
	{
		unsigned char r1 = aC1 & 0xFF;
		unsigned char g1 = (aC1 >> 8) & 0xFF;
		unsigned char b1 = (aC1 >> 16) & 0xFF;
		unsigned char r2 = aC2 & 0xFF;
		unsigned char g2 = (aC2 >> 8) & 0xFF;
		unsigned char b2 = (aC2 >> 16) & 0xFF;

		return (unsigned int)(RGB(	r1 + (r2 - r1) * aIntens / aMax,
									g1 + (g2 - g1) * aIntens / aMax,
									b1 + (b2 - b1) * aIntens / aMax ));
	}

public:
	unsigned int FBC;
	unsigned int FCC[4];
	unsigned int FCX;

	unsigned int *FC;

	USColor(void)
	{
		FC = new unsigned int[16 * 256 * 256];
		memset(FC, 0, sizeof(FC));
		SetColor(RGB(0, 0, 32), clrYellow, clrAqua, clrLime, clrFuchsia, clrRed); //TODO:
		//SetColor(RGB(0, 0, 0), clrRed, clrGreen, clrBlue, clrRed, clrWhite); //TODO:
	}
	virtual ~USColor(void)
	{
		delete [] FC;
	}

	void SetColor(unsigned int aBC, unsigned int aC1, unsigned int aC2, unsigned int aC3, unsigned int aC4, unsigned int aCX)
	{
		FCC[0] = RGBtoBGR(aC1);
		FCC[1] = RGBtoBGR(aC2);
		FCC[2] = RGBtoBGR(aC3);
		FCC[3] = RGBtoBGR(aC4);
		FBC = RGBtoBGR(aBC);
		FCX = RGBtoBGR(aCX);
		for (int cn = 0; cn < 16; cn++)
		{
			unsigned int c1 = FCC[cn & 0x03];
			unsigned int c2 = FCC[(cn >> 2) & 0x03];

			for (int i = 0; i < 256; i++)
			{
				COLOR_ITEM(i, 0, cn) = GetMdlColor(FBC, c1, i, 255);
				COLOR_ITEM(0, i, cn) = GetMdlColor(FBC, c2, i, 255);
				COLOR_ITEM(i, i, cn) = GetMdlColor(FBC, FCX, i, 255);
			}

			for (int i1 = 1; i1 < 256; i1++)
			{
				for (int i2 = 1; i2 < i1; i2++)
				{
					COLOR_ITEM(i1, i2, cn) = GetMdlColor(COLOR_ITEM(i1, 0, cn), COLOR_ITEM(i2, i2, cn), i2, i1);
					COLOR_ITEM(i2, i1, cn) = GetMdlColor(COLOR_ITEM(0, i1, cn), COLOR_ITEM(i2, i2, cn), i2, i1);
				}
			}
		}

		 //RGB -> BGR
		for(int i = 0; i < 256 * 256 * 16; i++)
		{
			unsigned int c = FC[i];
			FC[i] = ((c >> 16) & 0xFF) | (c & 0xFF00) | ((c << 16) & 0xFF0000);
		}
	}

};
//---------------------------------------------------------------------------

enum SecondMonitor { smLeft, smTop, smRight, smBottom };
//---------------------------------------------------------------------------

struct TUSTuneParam
{
	int FPorog;
	int FAmpOne;
	int FAmpTwo;
	int FVRCLen;
	bool FFilter;
	bool FRSMode;
	bool FInterval;
	int FAmpDelta;
};
//---------------------------------------------------------------------------

struct TMGTuneParam
{
	int FOffset;
	int FScale;
	int FFilter;
	bool FInverse;
};
//---------------------------------------------------------------------------

namespace GurUS
{
	//Режим цветов при рисовании УЗ ленты
	enum ColorMode
	{
		cmGradient = 0,
		cmPalette  = 1,
		cmContrast = 2
	};
	//Спобос расчета диманического порога
	enum DynamicThresholdType
	{
		dttMean 	= 0, //среднее арифметическое
		dttMedian  	= 1, //медиана
	};

	enum LanguageUI
	{
		langRussian = 0,
		langEnglish = 1,
		langChinese = 2
	};
	inline std::wstring DynamicThresholdTypeToCfgString(DynamicThresholdType aType)
	{
		std::wstring str = L"";
		switch(aType)
		{
			case dttMean:
				str = L"mean";
				break;
			case dttMedian:
				str = L"median";
				break;
		}
		return str;
	}
	inline DynamicThresholdType CfgStringToDynamicThresholdType(const std::wstring& aType)
	{
		if(aType.compare(L"mean") == 0)
			return dttMean;
		if(aType.compare(L"median") == 0)
			return dttMedian;
		return dttMean;
	}
	inline std::wstring LanguageUIToCfgString(LanguageUI aLang)
	{
		std::wstring str = L"";
		switch(aLang)
		{
			case langRussian:
				str = L"russian";
				break;
			case langEnglish:
				str = L"english";
				break;
			case langChinese:
				str = L"chinese";
				break;
			default:
				str = L"empty";
		}
		return str;
	}
	inline LanguageUI CfgStringToLanguageUI(const std::wstring& aLang)
	{
		if(aLang.compare(L"russian") == 0)
			return langRussian;
		if(aLang.compare(L"english") == 0)
			return langEnglish;
		if(aLang.compare(L"chinese") == 0)
			return langChinese;
		return langRussian;
	}

	enum ControllerType
	{
		ctrlEC2Standart = 0,
		ctrlEC2Vga		= 1
	};
	inline std::wstring ControllerTypeToString(ControllerType aType)
	{
		std::wstring str = L"";
		//switch(aType)
		//{
		//	case ctrlEC2Standart:
		//		str = LoadStr(STR_CTRLTYPE_STANDART);
		//		break;
		//	case ctrlEC2Vga:
		//		str = LoadStr(STR_CTRLTYPE_VGA);
		//		break;
		//	default:
		//		str = LoadStr(STR_CTRLTYPE_STANDART);
		//}
		return str;
	}
	inline ControllerType StringToControllerType(const std::wstring& aType)
	{
		//if(aType == LoadStr(STR_CTRLTYPE_STANDART))
		//	return ctrlEC2Standart;
		//if(aType == LoadStr(STR_CTRLTYPE_VGA))
		//	return ctrlEC2Vga;
		return ctrlEC2Standart;
	}

	inline std::wstring ControllerTypeToCfgString(ControllerType aType)
	{
		std::wstring str = L"";
		switch(aType)
		{
			case ctrlEC2Standart:
				str = L"EC2Standart";
				break;
			case ctrlEC2Vga:
				str = L"EC2Vga";
				break;
			default:
				str = L"EC2Standart";
		}
		return str;
	}
	inline ControllerType CfgStringToControllerType(const std::wstring& aType)
	{
		if(aType.compare(L"EC2Standart") == 0)
			return ctrlEC2Standart;
		if(aType.compare(L"EC2Vga") == 0)
			return ctrlEC2Vga;
		return ctrlEC2Standart;
	}

	enum AmplifierType
	{
		ampLinear		= 1,
		ampLogarithmic = 2
	};
	inline std::wstring AmplifierTypeToString(AmplifierType aType)
	{
		std::wstring str = L"";
		//switch(aType)
		//{
		//	case ampLinear:
		//		str = LoadStr(STR_AMPTYPE_LINEAR);
		//		break;
		//	case ampLogarithmic:
		//		str = LoadStr(STR_AMPTYPE_LOG);
		//		break;
		//	default:
		//		str = LoadStr(STR_AMPTYPE_LINEAR);
		//}
		return str;
	}
	inline AmplifierType StringToAmplifierType(std::wstring aType)
	{
		//if (aType == LoadStr(STR_AMPTYPE_LINEAR))
		//	return ampLinear;
		//if (aType == LoadStr(STR_AMPTYPE_LOG))
		//	return ampLogarithmic;
		return ampLinear;
	}

	inline std::wstring AmplifierTypeToCfgString(AmplifierType aType)
	{
		std::wstring str = L"";
		switch(aType)
		{
			case ampLinear:
				str = L"Linear";
				break;
			case ampLogarithmic:
				str = L"Logarithmic";
				break;
			default:
				str = L"Linear";
		}
		return str;
	}
	inline AmplifierType CfgStringToAmplifierType(const std::wstring& aType)
	{
		if(aType.compare(L"Linear") == 0)
			return ampLinear;
		if(aType.compare(L"Logarithmic") == 0)
			return ampLogarithmic;
		return ampLinear;
	}
	inline std::wstring ChannelTypeToStr(TChannelType aChannelType)
	{
		switch (aChannelType)
		{
		case ctUltraSound: return L"UltraSound";
		case ctMagnetic: return L"Magnetic";
		default: return L"None";
		}
	}
	inline TChannelType StrToChannelType(const std::wstring &aStr)
	{
		if (aStr == L"UltraSound") return ctUltraSound;
		if (aStr == L"Magnetic") return ctMagnetic;
		return TChannelType::ctUnknown;
	}
	inline std::wstring TrackTypeToStr(TrackType aTrackType)
	{
		switch (aTrackType)
		{
		case ttChart: return L"Chart";
		case ttScope: return L"Scope";
		default: return L"None";
		}
	}
	inline TrackType StrToTrackType(const std::wstring &aStr)
	{
		if (aStr == L"Chart") return ttChart;
		if (aStr == L"Scope") return ttScope;
		return ttNone;
	}
	inline std::wstring CarSideToStr(TCarSide aCarSide)
	{
		switch (aCarSide)
		{
		case csCorridor: return L"Corridor";
		case csRoomette: return L"Roomette";
		default: return L"None";
		}
	}
	inline TCarSide StrToCarSide(const std::wstring &aStr)
	{
		if (aStr == L"Corridor") return csCorridor;
		if (aStr == L"Roomette") return csRoomette;
		return csNone;
	}
	inline int GetChannelOrder(ChannelInfo *aCI)
	{
		switch (aCI->FCarSide)
		{
		case csRoomette:
			switch (aCI->FChannelType)
			{
			case ctUltraSound:
				return aCI->FChannel;
			case ctMagnetic:
				return aCI->FChannel + US_CHANNEL_COUNT;
			default: return -1;
			}
			break;
		case csCorridor:
			switch (aCI->FChannelType)
			{
			case ctUltraSound:
				return aCI->FChannel;
			case ctMagnetic:
				return aCI->FChannel + US_CHANNEL_COUNT + MG_CHANNEL_COUNT / 2;
			default: return -1;
			}
			break;
		default: return -1;
		}
	}
}
#endif
