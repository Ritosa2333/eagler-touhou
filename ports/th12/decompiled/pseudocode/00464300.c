// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464300; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464300_0 {
    char padding_0[8192];
    struct st_464300_1 *field_2000;
    char field_2004;
} st_464300_0;

typedef struct st_464300_1 {
    char field_0;
} st_464300_1;

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
extern CRITICAL_SECTION g_4cf140;
extern char g_4cf21b;

void sub_464300(int a0)
{
    char *v2;  // eax
    char *v3;  // eax
    char *v4;  // eax
    unsigned int v5;  // eax
    st_464300_0 *idx;  // edi
    char *iter;  // esi
    char *node;  // edx
    char i;  // 4103
    char v0;  // [bp-0x204]
    char v1;  // [bp+0x8]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf140.DebugInfo);
        g_4cf21b = g_4cf21b + 1;
    }
    sub_46cad8(&v0, a0, &v1);
    v2 = &v0;
    do
    {
        v3 = v2;
        v4 = v3 + 1;
        v2 = v4;
    } while (*(v3));
    v5 = v4 - (&v0 + 1);
    iter = &idx->field_2000->field_0;
    if (&idx->field_2000[v5] < &idx->padding_0[0x1fff])
    {
        node = &v0;
        do
        {
            i = *(node);
            *(iter) = i;
            node += 1;
            iter += 1;
        } while (i);
        idx->field_2000 = &idx->field_2000[v5];
        idx->field_2000->field_0 = i;
    }
    idx->field_2004 = 1;
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf140.DebugInfo);
        g_4cf21b = g_4cf21b - 1;
    }
    return;
}
