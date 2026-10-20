// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4357c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4357c0_0 {
    char padding_0[304];
    unsigned int field_130;
} st_4357c0_0;

typedef struct st_4357c0_1 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4357c0_1;

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

extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int * sub_4357c0(unsigned int *a0, int a1)
{
    st_4357c0_0 *index;  // edi
    int v4;  // esi
    int v5;  // ebp
    int v6;  // ebx
    st_4357c0_1 *idx;  // esi
    unsigned int *v8;  // eax
    unsigned int v9;  // 4099
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    unsigned int v2;  // [bp-0x4]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    index->field_130 = index->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    idx = sub_4621c0(v4, v5, v6);
    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_480 = idx->field_480 | 1;
    idx->field_430 = v0;
    idx->field_434 = v1;
    idx->field_438 = v2;
    sub_454d10(idx, a1);
    v8 = sub_461350(idx, a1);
    v9 = g_4cee78 & 0x8000;
    *(a0) = *(v8);
    if (v9)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return a0;
}
