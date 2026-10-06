// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x407010; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_407010_0 {
    char padding_0[64];
    unsigned int field_40;
    char padding_44[4];
    int field_48;
    char padding_4c[1212];
    unsigned int field_508;
    unsigned int field_50c;
    unsigned int field_510;
    unsigned int field_514;
    unsigned int field_518;
} st_407010_0;

typedef struct st_407010_1 {
    char padding_0[304];
    unsigned int field_130;
} st_407010_1;

typedef struct st_407010_11 {
    char padding_0[636];
    unsigned int field_27c;
} st_407010_11;

typedef struct st_407010_10 {
    char padding_0[959];
    char field_3bf;
} st_407010_10;

typedef struct st_407010_3 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_407010_3;

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

typedef struct st_407010_2 {
    char padding_0[16];
    struct st_407010_1 *field_10;
    char padding_14[2408];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
} st_407010_2;

typedef struct st_407010_8 {
    char padding_0[27972];
    struct st_407010_1 *field_6d44;
} st_407010_8;

typedef struct st_407010_6 {
    char padding_0[20];
    unsigned int field_14;
} st_407010_6;

typedef struct st_407010_4 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_407010_4;

typedef struct st_407010_5 {
    char padding_0[60];
    unsigned int field_3c;
} st_407010_5;

extern st_407010_5 *g_4b43c4;
extern st_407010_4 *g_4b43cc;
extern st_407010_6 *g_4b43dc;
extern st_407010_8 *g_4b43e4;
extern st_407010_2 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_407010(st_407010_0 *idx)
{
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    st_407010_1 *idx1;  // ebx
    st_407010_3 *index;  // esi
    unsigned int v21;  // esi
    st_407010_10 *v22;  // eax
    st_407010_11 *v23;  // eax
    unsigned int v11;  // edi
    st_407010_0 *idx2;  // edi
    st_407010_1 *v13;  // esi
    st_407010_3 *v14;  // ebx
    unsigned int v15;  // esi
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int *v18;  // esi
    unsigned int v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x2c]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]

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
    v5 = v9;
    if (/* unsupported instruction */)
    {
        idx->field_514 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_514 = nan;
        /* unsupported instruction */
    }
    v4 = v10;
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
        idx->field_518 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_518 = nan;
        /* unsupported instruction */
    }
    v3 = v11;
    idx2 = &idx->field_508;
    *((unsigned int *)&idx2->padding_0[0]) = g_4b4514->field_97c;
    *((unsigned int *)&idx2->padding_0[4]) = g_4b4514->field_980;
    *((unsigned int *)&idx2->padding_0[8]) = g_4b4514->field_984;
    sub_453d90();
    v13 = g_4b4514->field_10;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    v13->field_130 = v13->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = sub_4621c0();
    v14->field_480 = v14->field_480 | 1;
    v14->field_20 = 23;
    if (!idx2)
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
        v14->field_430 = v6;
        v14->field_434 = v7;
        v14->field_438 = v8;
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
        v14->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    sub_454d10(v14, 16);
    v15 = *((int *)sub_461250(v14, 16));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    idx->field_40 = v15;
    v16 = g_4b43cc->field_7c;
    if ((char)v16 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v17 = v16 & 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_407167;
            v17 = v16 | 32;
        }
        g_4b43cc->field_7c = v17;
    }
LABEL_407167:
    sub_406f60(120);
    g_4b43dc->field_14 = g_4b43dc->field_14 + 1;
    v18 = sub_46c9ea(0x44);
    if (v18)
    {
        v18[16] = v18[16] & 0xfffffffe;
        sub_477420(v18, 0, 0x44);
        *(v18) = *(v18) | 2;
        sub_452a60(v18, 8, 3, 60, 240, 30, 70);
    }
    idx1 = g_4b43e4->field_6d44;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx1->field_130 = idx1->field_130 + 1;
    index = sub_4621c0();
    index->field_480 = index->field_480 | 1;
    index->field_20 = 23;
    if (!idx2)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    sub_454d10(index, 1);
    v21 = *((int *)sub_461250(index, 1));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    idx->field_48 = v21;
    v22 = sub_461920(v21);
    if (!v22)
        idx->field_48 = 0;
    v22->field_3bf = 224;
    v23 = sub_461920(idx->field_48);
    if (!v23)
        idx->field_48 = 0;
    v23->field_27c = 224;
    return 0;
}
