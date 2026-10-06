// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4089c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4089c0_4 {
    char padding_0[304];
    unsigned int field_130;
} st_4089c0_4;

typedef struct st_4089c0_6 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4089c0_6;

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

typedef struct st_4089c0_0 {
    char padding_0[16];
    int field_10;
    char padding_14[2408];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
} st_4089c0_0;

typedef struct st_4089c0_5 {
    char padding_0[27972];
    struct st_4089c0_4 *field_6d44;
} st_4089c0_5;

typedef struct st_4089c0_3 {
    char padding_0[20];
    unsigned int field_14;
} st_4089c0_3;

typedef struct st_4089c0_1 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_4089c0_1;

typedef struct st_4089c0_2 {
    char padding_0[60];
    unsigned int field_3c;
} st_4089c0_2;

extern st_4089c0_2 *g_4b43c4;
extern st_4089c0_1 *g_4b43cc;
extern st_4089c0_3 *g_4b43dc;
extern st_4089c0_5 *g_4b43e4;
extern st_4089c0_0 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_4089c0(unsigned int *index)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int v9;  // edi
    unsigned int *idx;  // edi
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    unsigned int *idx1;  // esi
    st_4089c0_4 *idx2;  // ebx
    st_4089c0_6 *v15;  // esi
    unsigned int v16;  // esi
    unsigned int v0;  // [bp-0x38]
    unsigned int v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0xc]

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
    v5 = v7;
    if (/* unsupported instruction */)
    {
        index[325] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        index[325] = nan;
        /* unsupported instruction */
    }
    v4 = v8;
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
        index[326] = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        index[326] = nan;
        /* unsupported instruction */
    }
    v3 = v9;
    idx = index + 322;
    *(idx) = g_4b4514->field_97c;
    idx[1] = g_4b4514->field_980;
    idx[2] = g_4b4514->field_984;
    sub_453d90();
    sub_4615a0(g_4b4514->field_10, &index, 19, 0);
    index[16] = v6;
    v11 = g_4b43cc->field_7c;
    if ((char)v11 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v12 = v11 & 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_408a67;
            v12 = v11 | 32;
        }
        g_4b43cc->field_7c = v12;
    }
LABEL_408a67:
    g_4b43dc->field_14 = g_4b43dc->field_14 + 1;
    idx1 = sub_46c9ea(0x44);
    if (idx1)
    {
        idx1[16] = idx1[16] & 0xfffffffe;
        sub_477420(idx1, 0, 0x44);
        *(idx1) = *(idx1) | 2;
        sub_452a60(idx1, 8, 2, 180, 60, 1, 70);
    }
    idx2 = g_4b43e4->field_6d44;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx2->field_130 = idx2->field_130 + 1;
    v15 = sub_4621c0();
    v15->field_480 = v15->field_480 | 1;
    v15->field_20 = 23;
    if (!idx)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v15->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v15->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v15->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v15->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v15->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v15->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    sub_454d10(v15, 1);
    v16 = *((int *)sub_461250(v15, 1));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    index[18] = v16;
    return 0;
}
