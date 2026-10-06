// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462740; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462740_1 {
    char padding_0[24];
    struct st_462740_0 *field_18;
} st_462740_1;

typedef struct st_462740_0 {
    unsigned int field_0;
    struct st_462740_0 *field_4;
} st_462740_0;

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
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

int sub_462740(void)
{
    st_462740_1 *v4;  // eax
    st_462740_0 *v5;  // eax
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    st_462740_0 *v9;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v5 = v4->field_18;
    if (!v5)
        return;
    v2 = v6;
    v1 = v7;
    v0 = v8;
    do
    {
        v9 = v5->field_4;
        if (v5->field_0)
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
    } while ((v5 = v9, v5));
    return;
}
