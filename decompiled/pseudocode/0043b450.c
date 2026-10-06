// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43b450; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43b450_0 {
    char padding_0[8];
    unsigned int field_8;
    char padding_c[12];
    int field_18;
    int field_1c;
    int field_20;
    char padding_24[432];
    unsigned int field_1d4;
} st_43b450_0;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

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

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

extern unsigned int g_4ad138;
extern st_43b450_0 *g_4b4518;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_43b450(st_43b450_0 *a0)
{
    unsigned long v2;  // ldt
    unsigned long v3;  // gdt
    int i;  // esi
    st_43b450_0 *iter;  // esi
    unsigned int v14;  // edi
    unsigned int j;  // edi
    unsigned long long v16;  // 4122
    unsigned short v4;  // fs
    unsigned long long v5;  // 4142
    unsigned int v6;  // eax
    unsigned long long v7;  // 4164
    int v8;  // edi
    int v9;  // esi
    int v10;  // ebp
    int v11;  // ebx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v1 = 0xffffffff;
    v5 = _ccall(v2, v3, v4, 0);
    v6 = *((int *)(unsigned int)v5);
    v0 = *((int *)(unsigned int)v5);
    v7 = _ccall(v2, v3, v4, 0);
    *((unsigned int **)(unsigned int)v7) = &v0;
    sub_46ca4f(a0->field_18, g_4ad138 ^ &v8, v8, v9, v10, v11, v6, sub_497078, 0);
    i = 0;
    do
    {
        sub_43ccd0();
        i += 1;
    } while (i < 8);
    sub_46ca4f(a0->field_1c);
    a0->field_1c = 0;
    iter = &a0->field_20;
    v14 = 8;
    do
    {
        j = v14;
        sub_46ca4f(*((int *)&iter->padding_0[0]));
        *((unsigned int *)&iter->padding_0[0]) = 0;
        iter = &iter->padding_0[4];
        v14 = j - 1;
    } while (j != 1);
    if (a0->field_8)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (a0->field_1d4)
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (*((int *)&a0->padding_c[0]))
    {
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 + 1;
        }
        sub_462890();
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf0f8.DebugInfo);
            g_4cf218 = g_4cf218 - 1;
        }
    }
    if (a0 == g_4b4518)
        g_4b4518 = 0;
    v1 = 0xffffffff;
    sub_46cf1e(&a0->padding_24[132], 36, 8, sub_43cd80);
    v16 = _ccall(v2, v3, v4, 0);
    *((int *)(unsigned int)v16) = v8;
    return;
}
