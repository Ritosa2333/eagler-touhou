// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c73a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
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

unsigned int sub_46c73a(JOYCAPSA *a0, unsigned int a1, unsigned int a2)
{
    UIntPtr v0;  // [bp+0x0]

    return joyGetDevCapsA(v0, a0, a1);
}
