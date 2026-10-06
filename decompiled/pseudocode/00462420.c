// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462420; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462420_1 {
    struct st_462420_0 *field_0;
    char padding_4[4];
    struct st_462420_1 *field_8;
    struct st_462420_2 *field_c;
    char padding_10[8];
    struct st_462420_1 *field_18;
    struct st_462420_1 *field_1c;
} st_462420_1;

typedef struct st_462420_0 {
    struct st_462420_0 *field_0;
} st_462420_0;

typedef struct st_462420_2 {
    unsigned int field_0;
} st_462420_2;

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

typedef struct st_462420_3 {
    char padding_0[60];
    struct st_462420_1 *field_3c;
} st_462420_3;

extern st_462420_3 *g_4ce89c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf0f8;
extern char g_4cf218;

unsigned int sub_462420(void)
{
    unsigned int v3;  // ebp
    st_462420_1 *v4;  // esi
    st_462420_1 *iter;  // eax
    st_462420_0 **v6;  // ebx
    st_462420_1 *index;  // ecx
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v3 = 0;
    if (v4->field_c)
    {
        v3 = v4->field_c(v0, v1);
        v4->field_c = NULL;
    }
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf0f8.DebugInfo);
        g_4cf218 = g_4cf218 + 1;
    }
    iter = &g_4ce89c->padding_0[56];
    v4->field_0 = v6;
    if (iter->padding_4)
    {
        do
        {
        } while (*((int *)*((int *)iter->padding_4)) < v6 && (iter = (st_462420_1 *)iter->padding_4, iter->padding_4));
    }
    index = &v4->padding_10[4];
    if (iter->padding_4)
    {
        index->padding_4 = iter->padding_4;
        *((st_462420_1 **)&iter->padding_4[8]) = index;
    }
    *((st_462420_1 **)&iter->padding_4[0]) = index;
    index->field_8 = iter;
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf0f8.DebugInfo);
        g_4cf218 = g_4cf218 - 1;
    }
    return v3;
}
