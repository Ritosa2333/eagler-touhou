// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41c6a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454d10_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454d10_0;

typedef struct st_454d10_2 {
    char padding_0[104];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[4];
    unsigned int field_78;
    char padding_7c[874];
    unsigned short field_3e6;
    char padding_3e8[2];
    unsigned short field_3ea;
    unsigned int field_3ec;
    unsigned int field_3f0;
    char padding_3f4[4];
    struct st_454d10_0 *field_3f8;
    char padding_3fc[128];
    unsigned int field_47c;
} st_454d10_2;

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

int sub_41c6a0(void)
{
    st_454d10_0 *index;  // edi
    st_454d10_2 *idx;  // esi
    int v10;  // eax
    unsigned int *v11;  // eax
    unsigned int v12;  // 4099
    int v0;  // [bp-0x1c]
    int v1;  // [bp-0x18]
    int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]
    unsigned int *v6;  // [bp+0x4]
    unsigned int v7;  // [bp+0x8]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    *((unsigned int *)&index[1].padding_2[6]) = *((int *)&index[1].padding_2[6]) + 1;
    idx = sub_4621c0(v0, v1, v2);
    if (v10 >= 0)
        *((int *)&idx->padding_0[32]) = v10;
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&idx[1].padding_0[0]) = *((int *)&idx[1].padding_0[0]) | 1;
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)&idx->padding_3fc[52]) = v3;
    *((unsigned int *)&idx->padding_3fc[56]) = v4;
    *((unsigned int *)&idx->padding_3fc[60]) = v5;
    sub_454d10(index, v5, idx, v7);
    v11 = sub_4612d0(v0, v1);
    v12 = g_4cee78 & 0x8000;
    *(v6) = *(v11);
    if (v12)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return;
}
