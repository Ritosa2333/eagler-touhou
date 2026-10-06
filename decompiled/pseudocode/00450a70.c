// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450a70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct WNDCLASSA {
    enum WNDCLASS_STYLES style;
    LRESULT (*lpfnWndProc)(int *, unsigned int, unsigned int *, int *);
    int cbClsExtra;
    int cbWndExtra;
    HINSTANCE hInstance;
    HICON hIcon;
    HCURSOR hCursor;
    HBRUSH hbrBackground;
    PSTR lpszMenuName;
    PSTR lpszClassName;
} WNDCLASSA;

extern Byte g_4a269c;
extern int g_4ce8f8[4];
extern unsigned int g_4ce940;
extern char g_4ce9fc;
extern char g_4ceacd;
extern char g_4ceace;
extern char g_4cead3;
extern int g_4cead8;
extern int g_4ceadc;
extern unsigned int g_4cf3f0;
extern unsigned int g_4cf3fc;
extern unsigned int g_4cf400;
extern unsigned int g_4cf428;
extern unsigned int g_4cf468;
extern unsigned int g_4cf46c;
extern unsigned int g_4cf470;
extern unsigned int g_4cf474;
extern unsigned int g_4cf478;
extern unsigned int g_4cf47c;
extern unsigned int g_4cf480;
extern unsigned int g_4cf484;
extern unsigned int g_4cf488;
extern unsigned int g_4cf48c;
extern unsigned int g_4cf490;
extern unsigned int g_4cf494;
extern unsigned int g_4cf498;

unsigned int sub_450a70(void)
{
    HINSTANCE v7;  // ebx
    unsigned int v8;  // eax
    unsigned int v9;  // 4149
    unsigned int v10;  // eax
    HWND v11;  // eax
    unsigned int v12;  // eax
    int v13;  // esi
    unsigned int v14;  // ebp
    int v15;  // eax
    WNDCLASSA v0;  // [bp-0x2c], Other Possible Types: int
    unsigned int v1;  // [bp-0x28]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0x8]

    *((unsigned int *)&v0) = 0;
    v1 = 0;
    *((unsigned int *)&(&v0)[8]) = 0;
    *((unsigned int *)&(&v0)[12]) = 0;
    v2 = 0;
    *((unsigned int *)&(&v0)[20]) = 0;
    v3 = 0;
    v4 = 0;
    *((unsigned int *)&(&v0)[32]) = 0;
    v5 = 0;
    *((HGDIOBJ *)&(&v0)[28]) = GetStockObject(BLACK_BRUSH);
    v0.hCursor = LoadCursorA(NULL, 0x7f00);
    *((HINSTANCE *)&(&v0)[16]) = v7;
    *((void* *)&(&v0)[4]) = sub_44fe00;
    g_4cf3fc = 1;
    g_4cf400 = 0;
    v0.lpszClassName = "BASE";
    RegisterClassA(&v0);
    v8 = g_4cf428 ^ (g_4ceacd * 4 ^ g_4cf428) & 12;
    v9 = g_4ceace;
    *((unsigned int *)&g_4ce9fc) = (char)v8 & 12;
    if (!(char)v9 && g_4cead3 == 2)
        v10 = v8 | 16;
    else
        v10 = v8 & 0xffffffef;
    g_4cf428 = v10;
    g_4cf46c = 15;
    g_4cf470 = 15;
    g_4cf474 = 0;
    g_4cf478 = 12;
    g_4cf47c = 12;
    g_4cf480 = 0;
    g_4cf484 = 12;
    g_4cf488 = 12;
    g_4cf48c = 0;
    g_4cf490 = 8;
    g_4cf494 = 8;
    g_4cf498 = 0;
    g_4cf468 = 0;
    if (!*((int *)&g_4ce9fc))
    {
        v11 = CreateWindowExA(WS_EX_RIGHTSCROLLBAR, "BASE", &g_4a269c, 0x90000000, 0, 0, 640, 480, NULL, NULL, v7, NULL);
    }
    else
    {
        v12 = v10 >> 2 & 3;
        if (v12 == 3)
        {
            v13 = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 0x500;
            v14 = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 960;
        }
        else if (v12 == 2)
        {
            v13 = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 960;
            v14 = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 720;
        }
        else
        {
            v13 = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 640;
            v14 = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 480;
        }
        v15 = GetSystemMetrics(SM_CYCAPTION);
        v11 = CreateWindowExA(WS_EX_RIGHTSCROLLBAR, "BASE", &g_4a269c, 0x100b0000, g_4cead8, g_4ceadc, v13, v15 + v14, NULL, NULL, v7, NULL);
    }
    g_4cf3f0 = v11;
    GetWindowRect(v11, &g_4ce8f8[0]);
    g_4ce940 = g_4cf3f0;
    if (g_4cf3f0)
    {
        SendMessageA(g_4cf3f0, 274, 0xf020, NULL);
        Sleep(16);
        SendMessageA(g_4cf3f0, 274, 0xf120, NULL);
        return 0;
    }
    return 1;
}
