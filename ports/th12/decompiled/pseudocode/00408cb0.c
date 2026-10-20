// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408cb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408cb0_0 {
    char padding_0[304];
    unsigned int field_130;
} st_408cb0_0;

typedef struct st_408cb0_2 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_408cb0_2;

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

typedef struct st_408cb0_1 {
    char padding_0[16];
    struct st_408cb0_0 *field_10;
    char padding_14[2408];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
} st_408cb0_1;

typedef struct st_408cb0_7 {
    char padding_0[27972];
    struct st_408cb0_0 *field_6d44;
} st_408cb0_7;

typedef struct st_408cb0_5 {
    char padding_0[20];
    unsigned int field_14;
} st_408cb0_5;

typedef struct st_408cb0_3 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_408cb0_3;

typedef struct st_408cb0_4 {
    char padding_0[60];
    unsigned int field_3c;
} st_408cb0_4;

extern st_408cb0_4 *g_4b43c4;
extern st_408cb0_3 *g_4b43cc;
extern st_408cb0_5 *g_4b43dc;
extern st_408cb0_7 *g_4b43e4;
extern st_408cb0_1 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_408cb0(unsigned int *idx)
{
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    unsigned int v17;  // esi
    unsigned int *idx1;  // edi
    st_408cb0_0 *idx2;  // esi
    st_408cb0_2 *v11;  // ebx
    unsigned int v12;  // esi
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    st_408cb0_0 *v15;  // ebx
    st_408cb0_2 *index;  // esi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp-0x4]

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
        idx[325] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx[325] = nan;
        /* unsupported instruction */
    }
    v1 = v7;
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
        idx[326] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx[326] = nan;
        /* unsupported instruction */
    }
    v0 = v8;
    idx1 = idx + 322;
    *(idx1) = g_4b4514->field_97c;
    idx1[1] = g_4b4514->field_980;
    idx1[2] = g_4b4514->field_984;
    sub_453d90();
    idx2 = g_4b4514->field_10;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx2->field_130 = idx2->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = sub_4621c0();
    v11->field_480 = v11->field_480 | 1;
    v11->field_20 = 23;
    if (!idx1)
    {
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : (unsigned int)nan);
        if (/* unsupported instruction */)
        {
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v6 = nan;
            /* unsupported instruction */
        }
        v11->field_430 = v4;
        v11->field_434 = v5;
        v11->field_438 = v6;
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
        v11->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v11->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v11->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    sub_454d10(v11, 21);
    v12 = *((int *)sub_461250(v11, 21));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    idx[16] = v12;
    v13 = g_4b43cc->field_7c;
    if ((char)v13 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v14 = v13 & 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_408e07;
            v14 = v13 | 32;
        }
        g_4b43cc->field_7c = v14;
    }
LABEL_408e07:
    g_4b43dc->field_14 = g_4b43dc->field_14 + 1;
    v15 = g_4b43e4->field_6d44;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    v15->field_130 = v15->field_130 + 1;
    index = sub_4621c0();
    index->field_480 = index->field_480 | 1;
    index->field_20 = 23;
    if (!idx1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        index->field_430 = v2;
        index->field_434 = v3;
        index->field_438 = v4;
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
    sub_454d10(index, 1);
    v17 = *((int *)sub_461250(index, 1));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    idx[18] = v17;
    return 0;
}
