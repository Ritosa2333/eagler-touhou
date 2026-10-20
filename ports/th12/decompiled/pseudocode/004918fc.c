// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4918fc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME;

typedef struct TIME_ZONE_INFORMATION {
    int Bias;
    Char StandardName[32];
    SYSTEMTIME StandardDate;
    int StandardBias;
    Char DaylightName[32];
    SYSTEMTIME DaylightDate;
    int DaylightBias;
} TIME_ZONE_INFORMATION;

unsigned int sub_4918fc(unsigned int a0)
{
    TIME_ZONE_INFORMATION *v0;  // [bp+0x0]

    return GetTimeZoneInformation(v0);
}
