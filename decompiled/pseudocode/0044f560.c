// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

typedef struct MSG {
    HWND hwnd;
    unsigned int message;
    WPARAM wParam;
    LPARAM lParam;
    unsigned int time;
    POINT pt;
} MSG;

typedef struct CRITICAL_SECTION_DEBUG {
    unsigned short Type;
    unsigned short CreatorBackTraceIndex;
    struct CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    unsigned int EntryCount;
    unsigned int ContentionCount;
    unsigned int Flags;
    unsigned short CreatorBackTraceIndexHigh;
    unsigned short Identifier;
} CRITICAL_SECTION_DEBUG;

typedef struct st_44f560_2 {
    unsigned int field_0;
} st_44f560_2;

typedef struct st_44f560_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_44f560_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[4];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    unsigned int field_34;
    struct st_44f560_0 *field_38;
    unsigned int field_3c;
    unsigned int field_40;
    char padding_44[4];
    unsigned int field_48;
} st_44f560_0;

typedef struct POINT {
    int x;
    int y;
} POINT;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct st_44f560_1 {
    char padding_0[8];
    struct st_44f560_2 *field_8;
} st_44f560_1;

typedef struct st_44f560_3 {
    struct st_44f560_1 *field_0;
} st_44f560_3;

extern int g_4a24cc;
extern int g_4a260c;
extern int g_4a2664;
extern int g_4a2984;
extern unsigned int g_4ad138;
extern char g_4b0ec8;
extern unsigned int g_4b2ec8;
extern unsigned int g_4ce898;
extern void g_4ce89c;
extern int g_4ce8cc;
extern unsigned int g_4ce8e8;
extern st_44f560_3 *g_4ce8ec;
extern st_44f560_3 *g_4ce8f0;
extern int g_4ce8f8[4];
extern unsigned int g_4ce8fc;
extern unsigned int g_4ce9dc;
extern unsigned int g_4ce9e4;
extern unsigned int g_4ce9fc;
extern unsigned int g_4cea0c;
extern unsigned int g_4cea10;
extern unsigned int g_4cea14;
extern unsigned int g_4cea8c;
extern unsigned int g_4cea90;
extern st_44f560_3 *g_4cea94;
extern st_44f560_3 *g_4cea98;
extern st_44f560_3 *g_4cea9c;
extern int g_4ceab0;
extern char g_4ceaca;
extern char g_4ceacd;
extern char g_4ceace;
extern unsigned int g_4cead8;
extern unsigned int g_4ceadc;
extern unsigned int g_4ceae8;
extern unsigned int g_4cee5c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern unsigned int g_4cf3f0;
extern unsigned int g_4cf3f4;
extern unsigned int g_4cf3f8;
extern unsigned int g_4cf400;
extern char g_4cf404;
extern unsigned int g_4cf41c;
extern unsigned int g_4cf420;
extern unsigned int g_4cf424;
extern void g_4cf428;
extern unsigned int g_4cf42c;
extern unsigned long long g_4cf430;
extern unsigned long long g_4cf438;
extern unsigned long long g_4cf440;
extern unsigned long long g_4cf448;
extern unsigned long long g_4cf450;
extern unsigned long long g_4cf458;
extern unsigned int g_4d4770;
extern unsigned int g_4d4ea8;

int sub_44f560(unsigned int a0)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned int v21;  // eax
    char v22;  // cl
    unsigned int v23;  // ecx
    unsigned int *k;  // esi
    unsigned int *iter;  // edi
    unsigned int v26;  // edx
    unsigned int v27;  // 4163
    unsigned int v28;  // eax
    unsigned int v29;  // 4112
    unsigned int v12;  // edi
    unsigned int v30;  // eax
    unsigned int v31;  // eax
    int v32;  // edi
    unsigned int v33;  // ebx
    int v34;  // eax
    char v35;  // cl
    unsigned int v36;  // eax
    unsigned int v37;  // esi
    unsigned int v38;  // esi
    void* v39;  // edi
    unsigned int *v13;  // eax
    CRITICAL_SECTION *v40;  // esi
    void* j;  // edi
    unsigned int v42;  // eax
    CRITICAL_SECTION *v14;  // edi
    unsigned int v15;  // esi
    unsigned int i;  // esi
    st_44f560_0 *idx;  // eax
    st_44f560_0 *index;  // ecx
    int v19;  // eax
    st_44f560_1 **v0;  // [bp-0x14c]
    MSG v1;  // [bp-0x144]
    unsigned int v2;  // [bp-0x13c]
    MSG v3;  // [bp-0x13c]
    unsigned int v4;  // [bp-0x138]
    unsigned int v5;  // [bp-0x134]
    int <0x44f560[is_1]|Stack bp-0x130, 1 B>;  // [bp-0x130]
    unsigned int v6;  // [bp-0x130]
    Byte v7[16];  // [bp-0x110]
    char v8;  // [bp-0x100]
    unsigned int v9;  // [bp-0x8]

    v9 = g_4ad138 ^ &<0x44f560[is_1]|Stack bp-0x130, 1 B>;
    v5 = v10;
    v4 = v11;
    v2 = v12;
    v6 = 0;
    g_4cf3f8 = a0;
    timeBeginPeriod(1);
    v13 = sub_46c9ea(4);
    if (v13)
        *(v13) = 0;
    else
        v13 = NULL;
    g_4cee78 = g_4cee78 | 0x8000;
    g_4ce898 = v13;
    v14 = &g_4cf0f8.DebugInfo;
    v15 = 12;
    do
    {
        i = v15;
        InitializeCriticalSection(v14);
        v14 += 1;
        v15 = i - 1;
    } while (i != 0x1);
    sub_464220(&g_4a24cc);
    g_4d4ea8 = CreateMutexA(NULL, 1, "Touhou 12 App");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        sub_464300(&g_4a2984);
    }
    else if (sub_451530() != 0xffffffff)
    {
        g_4ce8e8 = a0;
        sub_44ffb0();
        if (!sub_42feb0(&g_4ce8e8))
        {
            GetKeyboardState(v7);
            if (g_4ceae8 & 0x100 || v8 & 128)
            {
                v0 = NULL;
                DialogBoxParamA(a0, 0xcb, NULL, sub_4518c0, NULL);
            }
            if (!(g_4cf428 & 96))
            {
                *((unsigned int *)&g_4cf428) = *((int *)&g_4cf428) ^ (g_4ceacd * 4 ^ *((int *)&g_4cf428)) & 12;
                sub_4516a0();
                goto LABEL_44f6aa;
            }
        }
    }
    while (1)
    {
        g_4d4770 = 2;
        sub_453030();
        sub_453500();
        if (g_4ce8cc)
        {
            sub_45ea40(g_4ce8cc);
            sub_46ca4f(g_4ce8cc);
        }
        g_4ce8cc = 0;
        if (g_4cea94)
        {
            g_4cea94->field_0->field_8(g_4cea94);
            g_4cea94 = 0;
        }
        if (g_4cea98)
        {
            v0 = g_4cea98;
            g_4cea98->field_0->field_8(g_4cea98);
            g_4cea98 = 0;
        }
        if (g_4cea9c)
        {
            g_4cea9c->field_0->field_8(g_4cea9c);
            g_4cea9c = 0;
        }
        if (g_4ce8f0)
        {
            g_4ce8f0->field_0->field_8(g_4ce8f0);
            g_4ce8f0 = 0;
        }
        v36 = g_4ce8ec;
        if (v36)
        {
            (*((int *)(*((int *)v36) + 8)))(v36);
            g_4ce8ec = 0;
        }
        if (g_4cf3f0)
        {
            ShowWindow(g_4cf3f0, SW_HIDE);
            MoveWindow(g_4cf3f0, 0, 0, 0, 0, 0);
            DestroyWindow(g_4cf3f0);
            g_4cf3f0 = 0;
        }
        do
        { } while (ShowCursor(1) < 0);
        if (v0 != 0x2)
            break;
        g_4b2ec8 = &g_4b0ec8;
        g_4b0ec8 = 0;
        sub_464220(&g_4a260c);
        if (!g_4ce9fc)
            WINNLSEnableIME(NULL, 1);
        v37 = 60;
        do
        {
            v38 = v37;
            if (PeekMessageA(&v1, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&v1);
                DispatchMessageA(&v1);
            }
        } while ((v37 = v38 - 1, v38 != 1));
        g_4cee78 = g_4cee78 & 0xfffffe7f;
LABEL_44f6aa:
        idx = sub_46c9ea(76);
        if (idx)
        {
            idx->field_8 = 0;
            idx->field_c = 0;
            idx->field_10 = 0;
            idx->field_0 = 0;
            index = &idx->field_24;
            idx->field_4 = idx->field_4 & 0xfffffffe;
            idx->field_14 = idx;
            idx->field_18 = 0;
            idx->field_1c = 0;
            index->field_4 = index->field_4 & 0xfffffffe;
            index->field_8 = 0;
            index->field_c = 0;
            index->field_10 = 0;
            index->field_0 = 0;
            index->field_14 = index;
            index->field_18 = 0;
            index->field_1c = 0;
            idx->field_48 = 0;
        }
        else
        {
            idx = NULL;
        }
        *((st_44f560_0 **)&g_4ce89c) = idx;
        g_4ce8ec = (unsigned int)Direct3DCreate9(32);
        if (!g_4ce8ec)
        {
            sub_464300(&g_4a2664);
        }
        else
        {
            if (!sub_450a70())
            {
                sub_4629b0();
                sub_463980();
                sub_451d20();
                sub_464c40();
                sub_452fd0(g_4cf3f0);
                if (!sub_450cc0(g_4cf3f0))
                {
                    v19 = sub_46c9ea(8973652);
                    g_4ce8cc = (!v19 ? 0 : sub_45dfa0(v19));
                    if (!g_4ce9fc)
                    {
                        WINNLSEnableIME(NULL, 0);
                        do
                        { } while (ShowCursor(0) >= 0);
                        SetCursor(NULL);
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    g_4cf448 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_4508b0();
                    if (/* unsupported instruction */)
                        g_4cf440 = /* unsupported instruction */;
                    else
                        g_4cf440 = nan;
                    if (/* unsupported instruction */)
                        g_4cf430 = /* unsupported instruction */;
                    else
                        g_4cf430 = nan;
                    if (/* unsupported instruction */)
                    {
                        g_4cf438 = /* unsupported instruction */;
                        /* unsupported instruction */
                    }
                    else
                    {
                        g_4cf438 = nan;
                        /* unsupported instruction */
                    }
                    sub_4508b0();
                    if (/* unsupported instruction */)
                        g_4cf458 = /* unsupported instruction */;
                    else
                        g_4cf458 = nan;
                    if (/* unsupported instruction */)
                    {
                        g_4cf450 = /* unsupported instruction */;
                        /* unsupported instruction */
                    }
                    else
                    {
                        g_4cf450 = nan;
                        /* unsupported instruction */
                    }
                    v0 = g_4cf3f0;
                    SetForegroundWindow(g_4cf3f0);
                    v2 = sub_42f8c0();
                    if (!v2)
                    {
                        *((unsigned int *)&g_4cf428) = *((int *)&g_4cf428) | 1;
                        v2 = 0;
                        g_4cf404 = 252;
                        if (!g_4cf3f4)
                        {
                            while (1)
                            {
                                if (PeekMessageA(&v3, NULL, 0, 0, PM_REMOVE))
                                {
                                    TranslateMessage(&v3);
                                    DispatchMessageA(&v3);
                                }
                                else
                                {
                                    v21 = (*((int *)&g_4ce8f0->field_0[1].padding_0[0]))(g_4ce8f0);
                                    v22 = g_4cf428;
                                    if (!v21)
                                    {
                                        if (v22 & 2)
                                            goto LABEL_44f8d2;
                                        if (v22 & 16)
                                        {
                                            v0 = sub_450080(&g_4cf3f0);
                                        }
                                        else
                                        {
                                            if (g_4cea10 == 0x1 && !g_4ceace)
                                                v0 = sub_450600(&g_4cf3f0);
                                            else
                                                v0 = sub_4503f0(&g_4cf3f0);
                                        }
                                        if (v0)
                                            break;
                                        g_4cee78 = g_4cee78 & 0xffffffef;
                                    }
                                    else
                                    {
                                        if (v21 == 2289436777)
                                        {
LABEL_44f8d2:
                                            g_4cf42c = 10;
                                            if (v22 & 2)
                                            {
                                                if (!(v22 & 12))
                                                {
                                                    GetWindowRect(g_4cf3f0, &g_4ce8f8[0]);
                                                    v23 = 14;
                                                    k = &g_4ce9dc;
                                                    for (iter = &g_4cea14; v23; k += 1)
                                                    {
                                                        v23 -= 1;
                                                        *(iter) = *(k);
                                                        iter += 1;
                                                    }
                                                    v26 = -(0 < (g_4cf428 & 16));
                                                    v27 = g_4ceaca;
                                                    g_4cea0c = 60;
                                                    g_4cea10 = (v26 & 0x7fffffff) + 1;
                                                    g_4ce9fc = 0;
                                                    g_4ce9e4 = (unsigned int)(((char)v27) + 22);
                                                }
                                                else
                                                {
                                                    g_4ce9e4 = g_4cea90;
                                                    g_4cea0c = 0;
                                                    if (v22 & 16)
                                                        g_4cea10 = 0x80000000;
                                                    else
                                                        g_4cea10 = (-(0 < g_4cea8c - 60) & 0x7fffffff) + 1;
                                                    g_4ce9fc = 1;
                                                }
                                            }
                                            sub_431700();
                                            sub_44f370();
                                            v28 = (*((int *)&g_4ce8f0->field_0[5].padding_0[4]))(g_4ce8f0, &g_4ce9dc);
                                            if (v28)
                                            {
                                                if (v28 != 2289436776)
                                                    break;
                                            }
                                            else
                                            {
                                                sub_451200();
                                                sub_44f400();
                                                g_4cee78 = g_4cee78 | 16;
                                                v29 = g_4cf428 & 2;
                                                g_4cee5c = 3;
                                                if ((char)v29)
                                                {
                                                    v30 = (unsigned int)(*((int *)&g_4cf428)) >> 2;
                                                    v31 = v30 & 3;
                                                    if (v30 & 3)
                                                    {
                                                        if (v31 == 3)
                                                        {
                                                            v32 = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 0x500;
                                                            v33 = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 960;
                                                        }
                                                        else
                                                        {
                                                            if (v31 == 0x2)
                                                            {
                                                                v32 = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 960;
                                                                v33 = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 720;
                                                            }
                                                            else
                                                            {
                                                                v32 = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 640;
                                                                v33 = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 480;
                                                            }
                                                        }
                                                        v34 = GetSystemMetrics(SM_CYCAPTION);
                                                        SetWindowLongA(g_4cf3f0, 0xfffffff0, 0x10cb0000);
                                                        SetWindowPos(g_4cf3f0, NULL, g_4ce8f8, g_4ce8fc, v32, v34 + v33, 96);
                                                        ShowWindow(g_4cf3f0, SW_NORMAL);
                                                        WINNLSEnableIME(NULL, 1);
                                                        do
                                                        { } while (ShowCursor(1) < 0);
                                                    }
                                                    else
                                                    {
                                                        SetWindowLongA(g_4cf3f0, 0xfffffff0, -0x70000000);
                                                        SetWindowPos(g_4cf3f0, NULL, 0, 0, 640, 480, SWP_FRAMECHANGED);
                                                        WINNLSEnableIME(NULL, 0);
                                                        do
                                                        { } while (ShowCursor(0) >= 0);
                                                        SetCursor(NULL);
                                                        g_4cf400 = 0;
                                                    }
                                                }
                                                sub_431630();
                                                *((unsigned int *)&g_4cf428) = *((int *)&g_4cf428) & 0xfffffffd;
                                            }
                                        }
                                    }
                                }
                                if (g_4cf3f4)
                                    break;
                            }
                        }
                    }
                    else if (v2 != 0xffffffff)
                    {
                        v2 = 2;
                    }
                    v35 = (unsigned int)(*((int *)&g_4cf428)) >> 2;
                    g_4ceacd = v35 & 3;
                    if (v35 & 3)
                    {
                        GetWindowRect(g_4cf3f0, &g_4ce8f8[0]);
                        g_4cead8 = g_4ce8f8;
                        g_4ceadc = g_4ce8fc;
                    }
                    sub_42f650();
                    if (*((int *)&g_4ce89c))
                    {
                        sub_44f200(*((int *)&g_4ce89c));
                        sub_46ca4f(*((int *)&g_4ce89c));
                    }
                    *((unsigned int *)&g_4ce89c) = 0;
                }
            }
        }
    }
    sub_463e80(&g_4ceab0);
    timeEndPeriod(1);
    sub_44f2c0(&g_4ceab0);
    g_4cee78 = g_4cee78 & 0xffff7fff;
    v39 = 0xc;
    v40 = &g_4cf0f8.DebugInfo;
    do
    {
        j = v39;
        DeleteCriticalSection(v40);
        v40 += 1;
        v39 = j - 1;
    } while (j != 0x1);
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, g_4cf41c, v39, SPIF_SENDWININICHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, g_4cf420, v39, SPIF_SENDWININICHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, g_4cf424, v39, SPIF_SENDWININICHANGE);
    WINNLSEnableIME(v39, 1);
    if (g_4ce898)
        sub_46ca4f(g_4ce898);
    _security_check_cookie();
    return v42;
}
