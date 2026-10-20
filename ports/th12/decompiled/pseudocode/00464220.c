// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464220; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464220_0 {
    char padding_0[8192];
    struct st_464220_1 *field_2000;
} st_464220_0;

typedef struct st_464220_1 {
    char field_0;
} st_464220_1;

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
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf140;
extern char g_4cf21b;

void sub_464220(unsigned int a0)
{
    unsigned int v8;  // esi
    unsigned int v9;  // edi
    char i;  // 4103
    st_464220_0 *v10;  // ecx
    st_464220_0 *idx;  // edi
    char *v12;  // eax
    char *v13;  // eax
    char *v14;  // eax
    char *iter;  // esi
    unsigned int v16;  // eax
    char *node;  // edx
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]
    char v2;  // [bp+0x0]
    char v3;  // [bp+0x1]
    unsigned int v5;  // [bp+0x2000]
    int v6;  // [bp+0x2008]
    char v7;  // [bp+0x200c]

    sub_48d060();
    v5 = g_4ad138 ^ &v2;
    v1 = v8;
    v0 = v9;
    idx = v10;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf140.DebugInfo);
        g_4cf21b = g_4cf21b + 1;
    }
    sub_46cad8(&v2, v6, &v7);
    v12 = &v2;
    do
    {
        v13 = v12;
        v14 = v13 + 1;
        v12 = v14;
    } while (*(v13));
    iter = &idx->field_2000->field_0;
    v16 = v14 - &v3;
    if (&idx->field_2000[v16] < &idx->padding_0[0x1fff])
    {
        node = &v2;
        do
        {
            i = *(node);
            *(iter) = i;
            node += 1;
            iter += 1;
        } while (i);
        idx->field_2000 = &idx->field_2000[v16];
        idx->field_2000->field_0 = i;
    }
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf140.DebugInfo);
        g_4cf21b = g_4cf21b - 1;
    }
    _security_check_cookie();
    return;
}
