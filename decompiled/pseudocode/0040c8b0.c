// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40c8b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40c8b0_0 {
    unsigned int field_0;
    char padding_4[968];
    unsigned short field_3cc;
    char padding_3ce[206];
    struct st_40c8b0_1 *field_49c;
    char padding_4a0[68];
    unsigned int field_4e4;
    unsigned int field_4e8;
    unsigned int field_4ec;
    char padding_4f0[4];
    unsigned int field_4f4;
    char padding_4f8[44];
    int field_524;
    char padding_528[10];
    unsigned short field_532;
} st_40c8b0_0;

typedef struct st_40c8b0_2 {
    char padding_0[304];
    unsigned int field_130;
} st_40c8b0_2;

typedef struct st_40c8b0_1 {
    unsigned int field_0;
} st_40c8b0_1;

typedef struct st_40c8b0_3 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_40c8b0_3;

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

extern char g_4b2ed0;
extern unsigned int g_4b43c8;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_40c8b0(void)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // edi
    st_40c8b0_0 *idx;  // esi
    st_40c8b0_0 *idx1;  // edi
    int v11;  // ebp
    st_40c8b0_2 *idx2;  // edi
    st_40c8b0_3 *index;  // ebx
    unsigned int v14;  // eax
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]

    v3 = v7;
    v2 = v8;
    if (idx->field_532 != 2 && idx->field_532 != 1)
        return 0;
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
    if (/* unsupported instruction */)
        v1 = /* unsupported instruction */;
    else
        v1 = nan;
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    idx1 = &idx->padding_4[4];
    if (!sub_40d560())
    {
        idx->field_532 = 3;
        if (*((int *)&idx1->padding_3ce[198]))
            (*((int *)&idx1->padding_3ce[198]))();
        *((unsigned short *)&idx1->padding_4[960]) = 1;
        v11 = idx->field_524;
        if (v11 >= 0)
        {
            idx2 = *((int *)(g_4b43c8 + 5106652));
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            idx2->field_130 = idx2->field_130 + 1;
            index = sub_4621c0();
            index->field_480 = index->field_480 | 1;
            index->field_20 = 23;
            if (idx == 0xfffffb44)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                index->field_430 = v3;
                index->field_434 = v4;
                index->field_438 = v5;
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
            sub_454d10(index, v11);
            sub_461250(index, v11);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
            }
        }
        v14 = idx->field_4f4;
        if (!((char)idx->field_4f4 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_4ec = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx->field_4e8 = 0;
            idx->field_4e4 = 0xfff0bdc1;
            *((char **)&idx->padding_4f0[0]) = &g_4b2ed0;
            idx->field_4f4 = v14 | 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_4e8 = 0;
        idx->field_4ec = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_4e4 = 0xffffffff;
        return 1;
    }
    else
    {
        if (*((int *)&idx1->padding_3ce[198]))
            (*((int *)&idx1->padding_3ce[198]))();
        *((unsigned short *)&idx1->padding_4[960]) = 1;
        idx->field_0 = idx->field_0 | 8;
        idx->field_532 = 3;
        return 1;
    }
}
