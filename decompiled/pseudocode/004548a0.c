// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4548a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct MSG {
    HWND hwnd;
    unsigned int message;
    WPARAM wParam;
    LPARAM lParam;
    unsigned int time;
    POINT pt;
} MSG;

typedef struct POINT {
    int x;
    int y;
} POINT;

extern char g_4d4754;
extern HANDLE g_4d4758;

unsigned int sub_4548a0(void)
{
    unsigned int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v0;  // [bp-0x2c]
    unsigned int i;  // [bp-0x24]
    MSG v2;  // [bp-0x20], Other Possible Types: int, unsigned int

    i = v4;
    v0 = v5;
    v2 = 0;
    do
    {
        v6 = (unsigned int)MsgWaitForMultipleObjects(1, &g_4d4758, 0, 0xffffffff, QS_ALLEVENTS);
        if (!*((int *)&g_4d4754))
            i = 1;
        v7 = v6;
        if (v6)
        {
            if (v7 == 1)
            {
                v2 = v2;
                if (PeekMessageA(&v2, v7 - 1, v7 - 1, v7 - 1, PM_REMOVE))
                {
                    do
                    {
                        if (*((unsigned int *)(&v2 + 4)) == 18)
                            i = 1;
                    } while ((v2 = v2, PeekMessageA(&v2, NULL, 0, 0, PM_REMOVE)));
                }
            }
        }
        else
        {
            if (*((int *)&g_4d4754) && *((int *)(*((int *)&g_4d4754) + 48)))
            {
                *((unsigned int *)(*((int *)&g_4d4754) + 120)) = 1;
                sub_4667d0(*((int *)&g_4d4754));
                *((unsigned int *)(*((int *)&g_4d4754) + 120)) = 0;
            }
        }
    } while (!i);
    return 0;
}
