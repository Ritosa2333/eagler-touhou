// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x407ea0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_407ea0_0 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    char padding_1c[104];
    unsigned int field_84;
    unsigned int field_88;
    unsigned int field_8c;
    unsigned int field_90;
    char padding_94[4];
    unsigned int field_98;
    char padding_9c[16];
    unsigned int field_ac;
    unsigned int field_b0;
} st_407ea0_0;

typedef struct st_407ea0_2 {
    char padding_0[304];
    unsigned int field_130;
} st_407ea0_2;

typedef struct st_407ea0_1 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_407ea0_1;

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

typedef struct st_407ea0_3 {
    char padding_0[16];
    struct st_407ea0_2 *field_10;
} st_407ea0_3;

extern char g_4b2ed0;
extern st_407ea0_3 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_407ea0(void)
{
    st_407ea0_0 *idx;  // esi
    unsigned int *idx1;  // eax
    unsigned int v12;  // edi
    st_407ea0_2 *idx2;  // edi
    st_407ea0_0 *v14;  // ebp
    int v15;  // ebx
    st_407ea0_1 *index;  // ebx
    unsigned int v17;  // edi
    unsigned int v18;  // eax
    st_407ea0_0 *v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x2c]
    unsigned int v3;  // [bp-0x28]
    unsigned int v4;  // [bp-0x24]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]

    idx->field_10 = *(idx1);
    idx->field_14 = idx1[1];
    idx->field_18 = idx1[2];
    v5 = v12;
    idx2 = g_4b4514->field_10;
    v14 = idx->padding_4;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx2->field_130 = idx2->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    index = sub_4621c0(v15);
    index->field_480 = index->field_480 | 1;
    index->field_20 = 23;
    if (!v14)
    {
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
        if (/* unsupported instruction */)
        {
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
            /* unsupported instruction */
        }
        index->field_430 = v6;
        index->field_434 = v7;
        index->field_438 = v8;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
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
    sub_454d10(index, 24);
    v17 = *((int *)sub_461250(index, 24));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_0 = v17;
    idx->field_84 = 1;
    v18 = idx->field_98;
    if (!((char)idx->field_98 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_90 = /* unsupported instruction */;
        else
            idx->field_90 = nan;
        idx->field_8c = 0;
        idx->field_88 = 0xfff0bdc1;
        *((char **)&idx->padding_94[0]) = &g_4b2ed0;
        idx->field_98 = v18 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_90 = /* unsupported instruction */;
    else
        idx->field_90 = nan;
    v4 = 10;
    v3 = 9999;
    if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
        /* unsupported instruction */
    }
    idx->field_8c = 0;
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
    idx->field_88 = 0xffffffff;
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
    v0 = v14;
    idx->field_ac = v8;
    idx->field_b0 = sub_4390f0();
    return;
}
