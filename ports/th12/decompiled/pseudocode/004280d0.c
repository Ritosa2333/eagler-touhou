// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4280d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4280d0_0 {
    char padding_0[16];
    struct st_4280d0_1 *field_10;
} st_4280d0_0;

typedef struct st_4280d0_4 {
    char padding_0[8];
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[8];
    struct st_4280d0_3 *field_18;
} st_4280d0_4;

typedef struct st_4280d0_1 {
    unsigned int field_0;
} st_4280d0_1;

typedef struct st_4280d0_2 {
    char padding_0[8];
    struct st_4280d0_3 *field_8;
} st_4280d0_2;

typedef struct st_4280d0_3 {
    struct st_4280d0_0 *field_0;
    struct st_4280d0_2 *field_4;
    struct st_4280d0_3 *field_8;
} st_4280d0_3;

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

extern unsigned int g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

void sub_4280d0(void)
{
    st_4280d0_4 *v1;  // ebx
    st_4280d0_3 *idx;  // esi
    st_4280d0_3 *v3;  // edi

    if (v1->field_8)
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
    if (v1->field_c)
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
    idx = v1->field_18;
    if (idx)
    {
        do
        {
            v3 = idx->field_8;
            idx->field_0->field_10();
            idx->field_4->field_8 = idx->field_8;
            if (idx->field_8)
                idx->field_8->field_4 = idx->field_4;
        } while ((sub_46ca4f(idx), idx = v3, v3));
    }
    g_4b44f4 = 0;
    return;
}
