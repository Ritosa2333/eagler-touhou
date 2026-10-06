// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46eb53; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

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

typedef struct st_46eb53_0 {
    struct CRITICAL_SECTION *field_0;
    unsigned int field_4;
} st_46eb53_0;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

extern st_46eb53_0 g_4ad2b8;
extern st_46eb53_0 g_4ad3d8;

void sub_46eb53(void)
{
    st_46eb53_0 *iter;  // esi
    unsigned int v3;  // edi
    unsigned int v4;  // edi
    st_46eb53_0 *v5;  // esi
    unsigned int v0;  // [bp-0xc]

    iter = &g_4ad2b8.field_0;
    v0 = v3;
    do
    {
        v4 = iter->field_0;
        if (v4 && iter->field_4 != 1)
        {
            DeleteCriticalSection(v4);
            sub_46c941(v4);
            iter->field_0 = 0;
        }
    } while ((iter += 8, iter < &g_4ad3d8.field_0));
    v5 = &g_4ad2b8.field_0;
    do
    {
        if (v5->field_0 && v5->field_4 == 1)
            DeleteCriticalSection(v5->field_0);
    } while ((v5 += 8, v5 < &g_4ad3d8.field_0));
    return;
}
