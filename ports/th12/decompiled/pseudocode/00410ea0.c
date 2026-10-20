// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410ea0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410ea0_0 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[4];
    int field_14;
    int field_18;
} st_410ea0_0;

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

extern unsigned int g_4b43d8;
extern char g_4b5130;
extern char g_4b5134;
extern char g_4b5138;
extern char g_4b513c;
extern unsigned int g_4ce55c;
extern unsigned int g_4ce8cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

unsigned int sub_410ea0(void)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    st_410ea0_0 *idx;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // esi
    unsigned int v8;  // esi
    unsigned int v9;  // esi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v3;
    v0 = v4;
    if (idx->field_8)
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
    if (idx->field_c)
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
    if (idx->field_18)
    {
        sub_410a50();
        sub_46ca4f(idx->field_18);
    }
    v6 = &(&g_4b5130)[g_4ce8cc];
    idx->field_18 = 0;
    if (*((int *)v6))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v6));
        *((unsigned int *)v6) = 0;
    }
    v7 = &(&g_4b5134)[g_4ce8cc];
    if (*((int *)&(&g_4b5134)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v7));
        *((unsigned int *)v7) = 0;
    }
    v8 = &(&g_4b5138)[g_4ce8cc];
    if (*((int *)&(&g_4b5138)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v8));
        *((unsigned int *)v8) = 0;
    }
    v9 = &(&g_4b513c)[g_4ce8cc];
    if (*((int *)&(&g_4b513c)[g_4ce8cc]))
    {
        sub_4604e0();
        sub_46ca4f(*((int *)v9));
        *((unsigned int *)v9) = 0;
    }
    if (idx->field_14)
    {
        sub_46c941(idx->field_14);
        idx->field_14 = 0;
    }
    idx->field_14 = 0;
    g_4b43d8 = 0;
    g_4ce55c = 0;
    return 0;
}
