// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4611d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4611d0_1 {
    unsigned int field_0;
} st_4611d0_1;

typedef struct st_4611d0_0 {
    char padding_0[28];
    struct st_4611d0_0 *field_1c;
    char padding_20[1116];
    unsigned int field_47c;
    char padding_480[12];
    struct st_4611d0_1 *field_48c;
} st_4611d0_0;

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

int sub_4611d0(void)
{
    unsigned int v2;  // esi
    unsigned int v3;  // eax
    int v4;  // edi
    st_4611d0_0 *iter;  // esi
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    iter = *((int *)(v3 * 1204 + v4 + 8935140));
    if (*((int *)(v3 * 1204 + v4 + 8935140)))
    {
        do
        {
            if (!(iter->field_47c & 0x10000000))
            {
                if (iter->field_48c)
                    iter->field_48c();
                sub_45c900(v4);
            }
        } while ((iter = (st_4611d0_0 *)iter->field_1c, iter));
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return;
}
