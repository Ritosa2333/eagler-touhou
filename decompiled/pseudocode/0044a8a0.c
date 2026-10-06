// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44a8a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44a8a0_6 {
    char padding_0[304];
    unsigned int field_130;
} st_44a8a0_6;

typedef struct st_44a8a0_5 {
    unsigned int field_0;
} st_44a8a0_5;

typedef struct st_44a8a0_1 {
    char padding_0[16];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    char padding_1c[4];
    unsigned int field_20;
    char padding_24[4];
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    struct st_44a8a0_0 *field_34;
    unsigned int field_38;
    unsigned int field_3c;
    unsigned int field_40;
    unsigned int field_44;
    unsigned int field_48;
    unsigned int field_4c;
    unsigned int field_50;
} st_44a8a0_1;

typedef struct st_44a8a0_0 {
    char padding_0[10164];
    unsigned int field_27b4;
} st_44a8a0_0;

typedef struct st_44a8a0_7 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_44a8a0_7;

typedef struct st_44a8a0_4 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_44a8a0_5 *field_494;
} st_44a8a0_4;

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

typedef struct st_44a8a0_3 {
    char padding_0[27972];
    int field_6d44;
} st_44a8a0_3;

extern int g_4b0c48;
extern int g_4b0cd4;
extern char g_4b2ed0;
extern unsigned int g_4b33e0[4];
extern unsigned int g_4b33f0[4];
extern unsigned int g_4b43c8;
extern st_44a8a0_3 *g_4b43e4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_44a8a0(void)
{
    unsigned int v17;  // ebx
    unsigned int v18;  // edi
    unsigned int v27;  // edi
    st_44a8a0_7 *index;  // ebx
    unsigned int v29;  // edi
    st_44a8a0_1 *idx;  // esi
    unsigned int idx1;  // eax
    unsigned int v21;  // eax
    st_44a8a0_0 *idx2;  // eax
    st_44a8a0_4 *v23;  // ebx
    unsigned int v24;  // eax
    unsigned int v25;  // ecx
    st_44a8a0_6 *v26;  // ebx
    unsigned int v0;  // [bp-0xb0]
    unsigned int v1;  // [bp-0xa0]
    unsigned int v2;  // [bp-0x98]
    unsigned int v3;  // [bp-0x94]
    unsigned int v4;  // [bp-0x90]
    unsigned int v5;  // [bp-0x8c]
    char *v6;  // [bp-0x80]
    unsigned int *v7;  // [bp-0x74]
    unsigned int v8;  // [bp-0x6c]
    unsigned int v9;  // [bp-0x68]
    char v10;  // [bp-0x64]
    char v11;  // [bp-0x54], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x50]
    unsigned int v13;  // [bp-0x48]
    unsigned int v14;  // [bp-0x44]
    unsigned int *v15;  // [bp-0x40]

    v9 = v17;
    v8 = v18;
    if (idx->field_2c)
        return;
    idx->field_2c = 1;
    sub_477420(&v11, 0, 80);
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v14 = 0;
    v13 = 0;
    v15 = g_4b33f0[g_4b0c48 / g_4b0cd4];
    idx1 = 3;
    if (v21 != 0xffffffff)
        idx1 = v21 - 1;
    v7 = g_4b33e0[idx1];
    idx2 = sub_412990(g_4b33e0[idx1], &v11);
    idx->field_34 = idx2;
    idx->field_38 = idx2->field_27b4;
    *((void* *)&idx2->padding_0[4156]) = sub_44a890;
    idx->field_30 = v21;
    sub_4214b0(g_4b43e4, -0x1);
    v6 = &v10;
    sub_40fbe0(&v7, 2, &v10);
    sub_4615a0(g_4b43e4->field_6d44, &v6, 82, 0);
    v23 = sub_461920(82);
    if (v23->field_494)
        v23->field_494();
    v23->field_3c4 = (v21 == 0xffffffff ? 3 : (unsigned short)(v21 - 1)) + 7;
    v24 = idx->field_20;
    if (!((char)idx->field_20 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_14 = 0;
        idx->field_10 = 0xfff0bdc1;
        *((char **)&idx->padding_1c[0]) = &g_4b2ed0;
        idx->field_20 = v24 | 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_14 = 0;
    idx->field_10 = 0xffffffff;
    sub_453d90();
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v1 = v25;
    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    sub_453e20();
    v26 = *((int *)(g_4b43c8 + 5106652));
    v27 = &idx->field_34->padding_0[4212];
    v2 = *((int *)(g_4b43c8 + 5106652));
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    v26->field_130 = v26->field_130 + 1;
    index = sub_4621c0();
    index->field_480 = index->field_480 | 1;
    index->field_20 = 23;
    if (!v27)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        index->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        index->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        index->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    sub_454d10(index, 207);
    v29 = *((int *)sub_461250(index, 207));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_3c = v29;
    idx->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v0 = 134;
    idx->field_48 = 0;
    idx->field_4c = 0;
    sub_4615a0(g_4b43e4->field_6d44, &v1, 134, 0);
    idx->field_40 = 134;
    sub_4615a0(g_4b43e4->field_6d44, &v0, 0x88, 0);
    idx->field_44 = 0x88;
    idx->field_28 = 720;
    return;
}
