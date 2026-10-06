// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x414af0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_414af0_3 {
    char padding_0[304];
    unsigned int field_130;
} st_414af0_3;

typedef struct st_414af0_1 {
    unsigned int field_0;
} st_414af0_1;

typedef struct st_414af0_0 {
    char padding_0[4156];
    struct st_414af0_1 *field_103c;
    char padding_1040[5664];
    unsigned int field_2660;
    char padding_2664[84];
    int field_26b8;
    int field_26bc;
    unsigned int field_26c0;
} st_414af0_0;

typedef struct st_414af0_2 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_414af0_2;

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

extern unsigned int g_4b43dc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_414af0(void)
{
    unsigned int v11;  // ebx
    unsigned int v12;  // edi
    st_414af0_0 *index;  // esi
    unsigned int v14;  // ecx
    st_414af0_3 *idx;  // edi
    st_414af0_2 *idx1;  // ebx
    st_414af0_0 *v17;  // ebx
    unsigned int v0;  // [bp-0x38]
    st_414af0_0 *v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3;  // [bp-0x2c]
    unsigned int v4;  // [bp-0x20]
    unsigned int v5;  // [bp-0x1c]
    int v6;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0x10]
    unsigned int v8;  // [bp-0xc]
    unsigned int v9;  // [bp-0x8]

    v6 = v11;
    v5 = v12;
    if (index->field_26b8 >= NULL)
    {
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
        v4 = v14;
        if (/* unsupported instruction */)
        {
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = nan;
            /* unsupported instruction */
        }
        sub_453e20();
    }
    v6 = index->field_26bc;
    if (index->field_26bc >= NULL)
    {
        idx = *((int *)(g_4b43dc + index->field_26c0 * 4 + 64));
        if (g_4cee78 & 0x8000)
        {
            EnterCriticalSection(&g_4cf1d0.DebugInfo);
            g_4cf221 = g_4cf221 + 1;
        }
        idx->field_130 = idx->field_130 + 1;
        idx1 = sub_4621c0();
        idx1->field_480 = idx1->field_480 | 1;
        idx1->field_20 = 4;
        if (index == 0xffffef8c)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx1->field_430 = v7;
            idx1->field_434 = v8;
            idx1->field_438 = v9;
        }
        else
        {
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
            {
                idx1->field_430 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx1->field_430 = nan;
                /* unsupported instruction */
            }
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
            {
                idx1->field_434 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx1->field_434 = nan;
                /* unsupported instruction */
            }
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
            {
                idx1->field_438 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx1->field_438 = nan;
                /* unsupported instruction */
            }
        }
        sub_454d10(idx1, v6);
        sub_461250(idx1, v6);
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf1d0.DebugInfo);
            g_4cf221 = g_4cf221 - 1;
        }
    }
    v17 = &index->field_2660;
    if (index->field_2660)
    {
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
        {
            v3 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v3 = nan;
            /* unsupported instruction */
        }
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
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        v1 = &index->padding_1040[52];
        v0 = index->field_2660;
        sub_4273f0();
    }
    sub_412140(v17);
    *((unsigned int *)&v17->padding_0[0]) = 0;
    if (index->field_103c)
        index->field_103c();
    return 1;
}
