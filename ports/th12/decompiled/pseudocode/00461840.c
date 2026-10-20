// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454d10_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454d10_0;

typedef struct st_461840_0 {
    char padding_0[32];
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[1012];
    unsigned int field_424;
    unsigned int field_428;
    unsigned int field_42c;
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
} st_461840_0;

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

int sub_461840(void)
{
    st_454d10_0 *idx;  // eax
    st_454d10_2 *idx1;  // esi
    st_461840_0 *index;  // edi
    unsigned int v8;  // eax
    unsigned int v9;  // edx
    st_461840_0 *idx2;  // eax
    unsigned int *v11;  // eax
    unsigned int v12;  // 4099
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]
    unsigned int *v3;  // [bp+0x4]
    unsigned int v4;  // [bp+0x8]

    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    *((unsigned int *)&idx[1].padding_2[6]) = *((int *)&idx[1].padding_2[6]) + 1;
    idx1 = sub_4621c0(v0, v1, v2);
    v8 = index->field_20;
    *((unsigned int *)&idx1[1].padding_0[0]) = *((int *)&idx1[1].padding_0[0]) | 1;
    *((unsigned int *)&idx1->padding_0[32]) = v8;
    *((unsigned int *)&idx1->padding_3fc[52]) = index->field_430;
    v9 = index->field_434;
    *((unsigned int *)&idx1->padding_3fc[56]) = v9;
    *((unsigned int *)&idx1->padding_3fc[60]) = index->field_438;
    sub_454d10(idx, v9, idx1, v4);
    *((unsigned int *)&idx1->padding_0[36]) = index->field_24;
    *((unsigned int *)&idx1->padding_0[40]) = index->field_28;
    idx2 = &index->field_424;
    *((unsigned int *)&idx1->padding_0[44]) = index->field_2c;
    *((int *)&idx1->padding_3fc[64]) = *((int *)&idx2->padding_0[0]);
    *((int *)&idx1->padding_3fc[0x44]) = *((int *)&idx2->padding_0[4]);
    *((int *)&idx1->padding_3fc[72]) = *((int *)&idx2->padding_0[8]);
    v11 = sub_461250(v0, v1);
    v12 = g_4cee78 & 0x8000;
    *(v3) = *(v11);
    if (v12)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    return;
}
