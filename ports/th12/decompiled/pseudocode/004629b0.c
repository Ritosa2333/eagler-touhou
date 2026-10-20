// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4629b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct JOYCAPSA {
    unsigned short wMid;
    unsigned short wPid;
    CHAR szPname[32];
    unsigned int wXmin;
    unsigned int wXmax;
    unsigned int wYmin;
    unsigned int wYmax;
    unsigned int wZmin;
    unsigned int wZmax;
    unsigned int wNumButtons;
    unsigned int wPeriodMin;
    unsigned int wPeriodMax;
    unsigned int wRmin;
    unsigned int wRmax;
    unsigned int wUmin;
    unsigned int wUmax;
    unsigned int wVmin;
    unsigned int wVmax;
    unsigned int wCaps;
    unsigned int wMaxAxes;
    unsigned int wNumAxes;
    unsigned int wMaxButtons;
    CHAR szRegKey[32];
    CHAR szOEMVxD[260];
} JOYCAPSA;

extern int g_4a3570;
extern JOYCAPSA g_4ce570;

unsigned int sub_4629b0(void)
{
    unsigned int v0[13];  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]

    v0 = (unsigned int (32 bits)[13])52;
    v1 = 0xff;
    if (joyGetPosEx(0, v0) && joyGetPosEx(1, v0))
    {
        sub_464220(&g_4a3570);
        return 1;
    }
    joyGetDevCapsA(NULL, &g_4ce570.wMid, 404);
    return 0;
}
