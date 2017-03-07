#ifndef USDataDataFileTypesH
#define USDataDataFileTypesH


/// !! wrong place
#define MAX_CHANNEL_COUNT	    32
#define US_CHANNEL_COUNT	    18
#define ECO_US_CHANNEL_COUNT	18
#define MG_CHANNEL_COUNT	    2
#define	US_TICK_COUNT			128
#define US_TICK_TIME        3.33f
#define MG_BUF_SIZE	(4 * 5 * 2)		// 4 Magnetic channels with 5 min & max int each
#define	US_BUF_SIZE	(US_CHANNEL_COUNT * US_TICK_COUNT)// 32 UltraSound channel with 128 bytes each


#define	FRAME_VIDEO_1		0x00010000
#define	FRAME_VIDEO_2		0x00020000
#define	FRAME_VIDEO_3		0x00040000
#define	FRAME_VIDEO_4		0x00080000
#define MASTER_FRAME		0x00000001	// Master Frame
#define US_LEFT				0x00000002	// Synteth Left Side data
#define US_RIGHT			0x00000004	// Synteth Right Side data
#define ECO                 0x00000008	// EchoComplex data as MasterFrame
#define ULTRA_SOUND			(US_LEFT | US_RIGHT)
#define	FRAME_VIDEO         (FRAME_VIDEO_1 | FRAME_VIDEO_2 | FRAME_VIDEO_3 | FRAME_VIDEO_4 )

#define EC_CH_COUNT 16
#define EC_DWORD_COUNT  5
#define EC_BUF_SIZE (1024 * 256)

namespace DataFile
{
enum DiskTuneFlag : unsigned char
{
    tfNone = 0,
    tfScanInterval = 1,
    tfFilter = 2,
    tfRSMode = 4
};

struct DiskTuneParam
{
	unsigned char	FAmpOne;
	unsigned char	FAmpTwo;
	unsigned char	FVRC;
	DiskTuneFlag	FFlags;
};
struct DiskTuneParam2
{
	unsigned short FPorog;
	unsigned char FAmpOne;
	unsigned char FAmpTwo;
	unsigned char FVRC;
	DiskTuneFlag FFlags;
};

struct EcoDiskTuneParam : public DiskTuneParam
{
    unsigned short FPorog;
};

struct EcoFrame
{
    unsigned int FMasterFrame;
    unsigned short FSpeed;
    unsigned short FButton;
    unsigned int FUSData[EC_DWORD_COUNT][EC_CH_COUNT];
    int FMGData[5][2];
    int FNewMG[5][2];
};
}
#endif