// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46a5f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46a5f0_1 {
    char padding_0[64];
    unsigned int field_40;
    char padding_44[4];
    unsigned int field_48;
    char padding_4c[1208];
    struct st_46a5f0_0 *field_504;
    unsigned int field_508;
    unsigned int field_50c;
    unsigned int field_510;
    unsigned int field_514;
    unsigned int field_518;
} st_46a5f0_1;

typedef struct st_46a5f0_0 {
    char padding_0[16];
    unsigned int field_10;
} st_46a5f0_0;

typedef struct st_46a5f0_7 {
    char padding_0[304];
    unsigned int field_130;
} st_46a5f0_7;

typedef struct st_46a5f0_9 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_46a5f0_9;

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

typedef struct st_46a5f0_2 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
    char padding_988[47744];
    unsigned int field_c408;
    char padding_c40c[4];
    unsigned int field_c410;
} st_46a5f0_2;

typedef struct st_46a5f0_8 {
    char padding_0[27972];
    struct st_46a5f0_7 *field_6d44;
} st_46a5f0_8;

typedef struct st_46a5f0_6 {
    char padding_0[20];
    unsigned int field_14;
} st_46a5f0_6;

typedef struct st_46a5f0_4 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_46a5f0_4;

typedef struct st_46a5f0_5 {
    char padding_0[60];
    unsigned int field_3c;
} st_46a5f0_5;

extern unsigned int g_4ad138;
extern char g_4b2ed0;
extern st_46a5f0_5 *g_4b43c4;
extern st_46a5f0_4 *g_4b43cc;
extern st_46a5f0_6 *g_4b43dc;
extern st_46a5f0_8 *g_4b43e4;
extern st_46a5f0_2 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_46a5f0(st_46a5f0_1 *idx)
{
    unsigned long v19;  // ldt
    unsigned long v20;  // gdt
    st_46a5f0_0 *v29;  // eax
    unsigned int v30;  // esi
    int i;  // eax
    int v32;  // eax
    unsigned int v33;  // ecx
    unsigned int v34;  // ecx
    unsigned int v35;  // ecx
    st_46a5f0_1 *idx1;  // esi
    unsigned int v37;  // ecx
    st_46a5f0_7 *idx2;  // ebx
    unsigned short v21;  // fs
    st_46a5f0_9 *index;  // ebp
    unsigned int v40;  // esi
    unsigned int *v41;  // edi
    unsigned long long v42;  // 4125
    unsigned long long v22;  // 4138
    unsigned int v23;  // ebx
    unsigned int v24;  // esi
    unsigned int v25;  // edi
    unsigned long long v26;  // 4160
    unsigned int *v27;  // eax
    st_46a5f0_0 *v28;  // esi
    unsigned int v0;  // [bp-0x58]
    unsigned int v1;  // [bp-0x54]
    unsigned int v2;  // [bp-0x50]
    unsigned int v3;  // [bp-0x4c]
    unsigned int v4;  // [bp-0x48]
    char *v5;  // [bp-0x40]
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x38]
    unsigned int v8;  // [bp-0x34]
    unsigned int v9;  // [bp-0x30]
    unsigned int v10;  // [bp-0x2c]
    unsigned int v11;  // [bp-0x24]
    char v12;  // [bp-0x20]
    unsigned int v13;  // [bp-0x14]
    unsigned int v14;  // [bp-0x10]
    unsigned int v15;  // [bp-0xc]
    unsigned int v16;  // [bp-0x8]
    unsigned int v17;  // [bp-0x4]

    v17 = 0xffffffff;
    v16 = sub_49757b;
    v22 = _ccall(v19, v20, v21, 0);
    v15 = *((int *)(unsigned int)v22);
    v11 = v23;
    v10 = v24;
    v9 = v25;
    v8 = g_4ad138 ^ &v9;
    v26 = _ccall(v19, v20, v21, 0);
    *((unsigned int **)(unsigned int)v26) = &v15;
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
        idx->field_514 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_514 = nan;
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
    v7 = &g_4b4514->field_97c;
    v6 = 4;
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
    v5 = &v12;
    v27 = sub_40fbe0();
    v28 = idx->field_504;
    idx->field_40 = *(v27);
    if (v28)
    {
        sub_402870();
        sub_46ca4f(v28);
    }
    idx->field_504 = NULL;
    v10 = sub_46c9ea(24);
    v14 = 0;
    v29 = (!v10 ? NULL : sub_40ff10(0x11));
    v13 = 0xffffffff;
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
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
        /* unsupported instruction */
    }
    idx->field_504 = v29;
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
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_410020();
    v30 = idx->field_504->field_10;
    i = 0;
    if (*((int *)&idx->field_504->padding_0[4]) > NULL)
    {
        do
        {
            *((unsigned int *)(v30 + *((int *)&idx->field_504->padding_0[4]) * 28 + 16)) = ((unsigned int)((i >= *((int *)&idx->field_504->padding_0[4]) - 1) + 1) & 0xff000081) - 1;
            v32 = i + 1;
            v30 += 28;
            *((unsigned int *)(v30 + *((int *)&idx->field_504->padding_0[4]) * 56 - 12)) = (unsigned int)((i >= *((int *)&idx->field_504->padding_0[4]) - 1) + 2);
            i = v32;
        } while (i < *((int *)&idx->field_504->padding_0[4]));
    }
    v33 = g_4b43cc->field_7c;
    if ((char)v33 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v34 = v33 & 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_46a782;
            v34 = v33 | 32;
        }
        g_4b43cc->field_7c = v34;
    }
LABEL_46a782:
    v35 = g_4b4514->field_c410;
    if (!((char)g_4b4514->field_c410 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b4514->field_c408 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&g_4b4514->padding_988[47740]) = 0;
        *((unsigned int *)&g_4b4514->padding_988[47736]) = 0xfff0bdc1;
        *((char **)&g_4b4514->padding_c40c[0]) = &g_4b2ed0;
        g_4b4514->field_c410 = v35 | 1;
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
        g_4b4514->field_c408 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b4514->field_c408 = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&g_4b4514->padding_988[47740]) = 200;
    *((unsigned int *)&g_4b4514->padding_988[47736]) = 199;
    g_4b43dc->field_14 = g_4b43dc->field_14 + 1;
    idx1 = &idx->field_508;
    *((unsigned int *)&idx1->padding_0[0]) = g_4b4514->field_97c;
    *((unsigned int *)&idx1->padding_0[4]) = g_4b4514->field_980;
    *((unsigned int *)&idx1->padding_0[8]) = g_4b4514->field_984;
    v37 = g_4b43cc->field_7c;
    if ((char)v37 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v37 &= 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_46a82c;
            v37 |= 32;
        }
        g_4b43cc->field_7c = v37;
    }
LABEL_46a82c:
    g_4b43dc->field_14 = g_4b43dc->field_14 + 1;
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
    v0 = v37;
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
    sub_453e20();
    idx2 = g_4b43e4->field_6d44;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx2->field_130 = idx2->field_130 + 1;
    index = sub_4621c0();
    index->field_480 = index->field_480 | 1;
    index->field_20 = 23;
    if (!idx1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
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
    v40 = *((int *)sub_461250(index, 1));
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    idx->field_48 = v40;
    v41 = sub_46c9ea(0x44);
    if (v41)
    {
        v41[16] = v41[16] & 0xfffffffe;
        sub_477420(v41, 0, 0x44);
        *(v41) = *(v41) | 2;
        sub_452a60(v41, 8, 2, 60, 240, 30, 70);
    }
    v42 = _ccall(v19, v20, v21, 0);
    *((unsigned int *)(unsigned int)v42) = v1;
    return 0;
}
