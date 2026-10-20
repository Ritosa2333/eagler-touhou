// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42a1f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42a1f0_3 {
    struct st_42a1f0_1 *field_0;
    struct st_42a1f0_3 *field_4;
    struct st_42a1f0_3 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_42a1f0_3;

typedef struct st_42a1f0_4 {
    char padding_0[304];
    unsigned int field_130;
} st_42a1f0_4;

typedef struct st_42a1f0_2 {
    unsigned int field_0;
} st_42a1f0_2;

typedef struct st_42a1f0_1 {
    char padding_0[4];
    struct st_42a1f0_2 *field_4;
} st_42a1f0_1;

typedef struct st_42a1f0_0 {
    char padding_0[80];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[16];
    unsigned int field_6c;
    char padding_70[8];
    unsigned int field_78;
    char field_7c;
    char padding_7d[975];
    unsigned int field_44c;
    char padding_450[4];
    unsigned int field_454;
    char padding_458[12];
    unsigned int field_464;
    char padding_468[18];
    short field_47a;
} st_42a1f0_0;

typedef struct st_42a1f0_9 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42a1f0_9;

typedef struct st_42a1f0_5 {
    char padding_0[1124];
    struct st_42a1f0_3 *field_464;
    int field_468;
    int field_46c;
    char padding_470[24];
    struct st_42a1f0_4 *field_488;
} st_42a1f0_5;

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

extern st_42a1f0_5 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_42a1f0(st_42a1f0_0 *a0, unsigned int a1, unsigned int a2, unsigned int a3, char a4, unsigned int a5)
{
    int v26;  // ebx
    unsigned int v27;  // esi
    unsigned int v36;  // ftop
    unsigned int v37;  // 4449
    unsigned int v39;  // 4165
    unsigned int v41;  // ftop
    unsigned int v43;  // 4165
    unsigned int v45;  // ftop
    unsigned int v28;  // edi
    st_42a1f0_4 *idx;  // ebx
    st_42a1f0_9 *index;  // esi
    unsigned int v49;  // ftop
    unsigned int v50;  // eax
    unsigned int v51;  // ftop
    int iter;  // ebx
    unsigned int v53;  // ftop
    unsigned int v55;  // ftop
    int v29;  // edi
    unsigned int v56;  // eax
    unsigned int v58;  // ftop
    unsigned int v59;  // 4291
    st_42a1f0_5 *idx1;  // edx
    unsigned int v61;  // eax
    st_42a1f0_0 *j;  // esi
    unsigned int v63;  // ecx
    unsigned int *node;  // edi
    st_42a1f0_0 *idx2;  // esi
    unsigned int v66;  // ftop
    unsigned int v67;  // edi
    unsigned int v68;  // ftop
    unsigned int v69;  // esi
    st_42a1f0_3 *v71;  // eax
    st_42a1f0_3 *v72;  // ecx
    unsigned int v73;  // eax
    unsigned int v31;  // eax
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v0;  // [bp-0x360]
    int v1;  // [bp-0x35c], Other Possible Types: unsigned int
    char *v2;  // [bp-0x358], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x354]
    unsigned int v4;  // [bp-0x350]
    st_42a1f0_5 *v5;  // [bp-0x34c], Other Possible Types: int, unsigned int
    unsigned int v6;  // [bp-0x348]
    unsigned int v7;  // [bp-0x344], Other Possible Types: int
    unsigned int v8;  // [bp-0x340]
    unsigned int v9;  // [bp-0x33c]
    unsigned int v10;  // [bp-0x338]
    st_42a1f0_0 *v11;  // [bp-0x32c]
    unsigned int v12;  // [bp-0x328]
    unsigned int v13;  // [bp-0x324]
    unsigned int v14;  // [bp-0x320]
    unsigned int v15;  // [bp-0x31c]
    unsigned int v16;  // [bp-0x318]
    unsigned int v17;  // [bp-0x314]
    unsigned int v18;  // [bp-0x310]
    unsigned int v19;  // [bp-0x30c]
    st_42a1f0_0 *v20;  // [bp-0x308], Other Possible Types: unsigned int
    unsigned int v21;  // [bp-0x304]
    unsigned int v22;  // [bp-0x300]
    unsigned int v23;  // [bp-0x2fc]
    char v24;  // [bp-0x130]
    int v74;  // [bp-0x12c]
    char v25;  // [bp-0x10c]

    v7 = v26;
    v6 = v27;
    v5 = v28;
    v29 = 0;
    idx2 = a0;
    v20 = idx2;
    if (a5 && idx2->field_44c)
        return v31;
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v21 = idx2->field_50;
    v2 = &v25;
    v13 = 0;
    v22 = idx2->field_54;
    v23 = idx2->field_58;
    sub_477420(&v25, 0, 0x100);
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_42e880((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = v12;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = v13;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = v14;
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    a3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if ((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4030000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
    {
        _security_check_cookie();
        return v73;
    }
    v34 = v33 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    while (1)
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v36 = v34 - 2;
        v37 = _ccall(10, 13, (char)((((unsigned short)v36 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v37 & 1)
            break;
        /* unsupported instruction */
        /* unsupported instruction */
        v29 += 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v49 = v36 - 2;
        if ((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
            goto LABEL_42a509;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v34 = v49 + 4;
    }
    v7 += 1;
    *((char *)&v74 + v29) = 1;
    if (a4 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = _ccall(10, 13, (char)((((unsigned short)v36 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
        if (!(v39 & 1))
            goto LABEL_42a3de;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v41 = v36 + 1;
        if ((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v43 = _ccall(10, 13, (char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v43 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v45 = v41 + 2;
                if ((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    sub_4273f0(9, &v2, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    goto LABEL_42a3e4;
                }
            }
        }
    }
    else
    {
LABEL_42a3de:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v41 = v36 + 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v45 = v41 + 2;
LABEL_42a3e4:
    idx = g_4b44f4->field_488;
    v7 = idx2->field_47a * 2 + 4;
    if (g_4cee78 & 0x8000)
    {
        EnterCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 + 1;
    }
    idx->field_130 = idx->field_130 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index = sub_4621c0();
    index->field_480 = index->field_480 | 1;
    /* unsupported instruction */
    /* unsupported instruction */
    index->field_20 = 23;
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
    sub_454d10(index, v7);
    sub_461250(index, v7);
    if (g_4cee78 & 0x8000)
    {
        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
        g_4cf221 = g_4cf221 - 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v36 = v45 - 3;
    /* unsupported instruction */
    /* unsupported instruction */
    idx2 = v12;
LABEL_42a509:
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = v29;
    if (v7)
    {
        if (v7 >= v29)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx2->field_7c = 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            _security_check_cookie();
            return v50;
        }
        v51 = v49 - 0;
        /* unsupported instruction */
        /* unsupported instruction */
        iter = 0;
        if (v29 > 0)
        {
            do
            {
            } while (*((char *)&v74 + iter) && (iter = (int)(iter + 1), iter < v29));
            v1 = iter;
            if (!iter)
                goto LABEL_42a60e;
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx2->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx2->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx2->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v53 = v51 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            idx2->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            if (!((char)((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                idx2->field_464 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                idx2->field_78 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v55 = v53 + 2;
LABEL_42a610:
                v56 = 0;
                if (iter < v29)
                {
                    do
                    {
                        if (*((char *)&v74 + iter))
                        {
                            v1 = v56;
                            if (iter < v29)
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
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                idx2->field_464 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                idx2->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v59 = _ccall(10, 13, (char)((((unsigned short)v55 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                if (!(v59 & 1))
                                    idx2->field_7c = 1;
                                idx1 = g_4b44f4;
                                while (iter < v29)
                                {
                                    if ((&v24)[iter])
                                    {
                                        iter += 1;
                                    }
                                    else
                                    {
                                        if (iter >= v29)
                                            break;
                                        v61 = 0;
                                        v5 = iter;
                                        do
                                        {
                                        } while (!(&v24)[iter] && (iter = (int)(iter + 1), v61 += 1, iter < v29));
                                        v0 = v61;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        j = &idx2->field_454;
                                        v63 = 122;
                                        node = &v16;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        for (; v63; j = &j->padding_0[4])
                                        {
                                            v63 -= 1;
                                            *(node) = *((int *)&j->padding_0[0]);
                                            node += 1;
                                        }
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                        /* unsupported instruction */
                                        v66 = v58 + 1;
                                        if (!((char)((((unsigned short)v66 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v67 = &idx1->field_468;
                                            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v5 = idx1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v16 = v1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            v68 = v66 + 2;
                                            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            if (*((int *)v67) < 0x100)
                                            {
                                                idx1->field_46c = idx1->field_46c + 1;
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                v69 = &idx1->field_46c;
                                                if (idx1->field_46c < 0x10000)
                                                    *((int *)v69) = 0x10000;
                                                v71 = (!sub_46c9ea(4004) ? NULL : sub_428520());
                                                v71->field_80 = *((int *)v69);
                                                v72 = v5->field_464;
                                                v71->field_4 = v72;
                                                v72->field_8 = v71;
                                                *((unsigned int *)v67) = *((int *)v67) + 1;
                                                v5->field_464 = v71;
                                                v71->field_0->field_4(&v16);
                                                v68 -= 0;
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                idx1 = g_4b44f4;
                                            }
                                        }
                                        else
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v68 = v66 + 2;
                                        }
                                        if (iter >= (/* unsupported instruction */ ? /* unsupported instruction */ : nan))
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            _security_check_cookie();
                                            return v73;
                                        }
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v29 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        idx2 = v11;
                                        v58 = v68 - 3;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                    }
                                }
                            }
                        }
                    } while ((iter = (int)(iter + 1), v56 += 1, iter < v29));
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx2->field_7c = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        else
        {
LABEL_42a60e:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v55 = v51 + 1;
            goto LABEL_42a610;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    _security_check_cookie();
    return v73;
}
