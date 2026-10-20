// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43ceb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43ceb0_0 {
    unsigned short field_0;
    unsigned short field_2;
    char padding_4[4];
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    char field_14;
    char padding_15[49];
    unsigned short field_46;
} st_43ceb0_0;

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

extern void g_4ce568;
extern unsigned int g_4ce56c;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1e8;
extern char g_4cf222;

int sub_43ceb0(void)
{
    st_43ceb0_0 *idx;  // eax
    st_43ceb0_0 *iter;  // esi
    unsigned int v7;  // edi
    unsigned int i;  // edi
    unsigned short v9;  // cx
    unsigned short v10;  // ax
    unsigned int v11;  // 4125
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    sub_477420(idx, 0, 1096, v0, v1, v2, v3);
    idx->field_0 = 21587;
    idx->field_2 = 2;
    idx->field_8 = 1096;
    idx->field_c = 0x20202020;
    idx->field_10 = 0x20202020;
    idx->field_14 = 0;
    iter = &idx->field_46;
    v7 = 0x200;
    do
    {
        i = v7;
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf1e8.DebugInfo);
            g_4cf222 = g_4cf222 + 1;
        }
        g_4ce56c = g_4ce56c + 1;
        v9 = (*((int *)&g_4ce568) ^ 38448) - 25939;
        v10 = (v9 >> 14) + (unsigned short)(v9 * 4);
        v11 = g_4cee78 & 0x8000;
        *((unsigned short *)&g_4ce568) = v10;
        if (v11)
        {
            LeaveCriticalSection(&g_4cf1e8.DebugInfo);
            g_4cf222 = g_4cf222 - 1;
            v10 = *((short *)&g_4ce568);
        }
        iter->field_0 = v10;
        iter = &iter->field_2;
        v7 = i - 1;
    } while (i != 1);
    return;
}
