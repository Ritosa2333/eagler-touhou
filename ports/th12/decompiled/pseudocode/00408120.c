// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408120; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408120_0 {
    char padding_0[72];
    unsigned int field_48;
    char padding_4c[1204];
    int field_500;
    char padding_504[4];
    unsigned int field_508;
    unsigned int field_50c;
    unsigned int field_510;
    unsigned int field_514;
    unsigned int field_518;
} st_408120_0;

typedef struct st_408120_5 {
    char padding_0[304];
    unsigned int field_130;
} st_408120_5;

typedef struct st_408120_7 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_408120_7;

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

typedef struct st_408120_1 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
} st_408120_1;

typedef struct st_408120_6 {
    char padding_0[27972];
    struct st_408120_5 *field_6d44;
} st_408120_6;

typedef struct st_408120_4 {
    char padding_0[20];
    unsigned int field_14;
} st_408120_4;

typedef struct st_408120_2 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_408120_2;

typedef struct st_408120_3 {
    char padding_0[60];
    unsigned int field_3c;
} st_408120_3;

extern st_408120_3 *g_4b43c4;
extern st_408120_2 *g_4b43cc;
extern st_408120_4 *g_4b43dc;
extern st_408120_6 *g_4b43e4;
extern st_408120_1 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_408120(void)
{
    st_408120_0 *index;  // edi
    st_408120_5 *idx;  // ebx
    st_408120_7 *idx1;  // esi
    unsigned int v17;  // esi
    unsigned int v8;  // ebx
    unsigned int v9;  // ebp
    st_408120_0 *idx2;  // ebp
    unsigned int v11;  // esi
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    int v14;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

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
        index->field_514 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        index->field_514 = nan;
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
    v2 = v8;
    if (/* unsupported instruction */)
    {
        index->field_518 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        index->field_518 = nan;
        /* unsupported instruction */
    }
    v1 = v9;
    idx2 = &index->field_508;
    *((unsigned int *)&idx2->padding_0[0]) = g_4b4514->field_97c;
    *((unsigned int *)&idx2->padding_0[4]) = g_4b4514->field_980;
    v0 = v11;
    *((unsigned int *)&idx2->padding_0[8]) = g_4b4514->field_984;
    sub_453d90();
    v12 = g_4b43cc->field_7c;
    if ((char)v12 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v13 = v12 & 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_4081a2;
            v13 = v12 | 32;
        }
        g_4b43cc->field_7c = v13;
    }
LABEL_4081a2:
    g_4b43dc->field_14 = g_4b43dc->field_14 + 1;
    if (index->field_500)
    {
        sub_46c941(index->field_500);
        index->field_500 = 0;
    }
    v14 = sub_46d04a(1440);
    index->field_500 = v14;
    sub_477420(v14, 0, 1440);
    idx = g_4b43e4->field_6d44;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx->field_130 = idx->field_130 + 1;
    idx1 = sub_4621c0();
    idx1->field_480 = idx1->field_480 | 1;
    idx1->field_20 = 23;
    if (!idx2)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx1->field_430 = v3;
        idx1->field_434 = v4;
        idx1->field_438 = v5;
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
    sub_454d10(idx1, 1);
    v17 = *((int *)sub_461250(idx1, 1));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    index->field_48 = v17;
    return 0;
}
