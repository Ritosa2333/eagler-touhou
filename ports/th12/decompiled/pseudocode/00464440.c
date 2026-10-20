// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464440; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464440_0 {
    unsigned short field_0;
    char padding_2[2];
    unsigned int field_4;
} st_464440_0;

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
extern CRITICAL_SECTION g_4cf1e8;
extern char g_4cf222;

unsigned int sub_464440(void)
{
    st_464440_0 *idx;  // esi
    unsigned short v2;  // ax
    unsigned short v3;  // bx

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1e8.DebugInfo);
        g_4cf222 = g_4cf222 + 1;
    }
    v2 = 38448 ^ idx->field_0;
    idx->field_4 = idx->field_4 + 2;
    v3 = ((v2 - 25939 >> 14) + (unsigned short)((v2 - 25939) * 4) ^ 38448) - 25939;
    idx->field_0 = (v3 >> 14) + (unsigned short)(v3 * 4);
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1e8.DebugInfo);
        g_4cf222 = g_4cf222 - 1;
    }
    return (v2 - 25939) * 0x10000 | v3;
}
