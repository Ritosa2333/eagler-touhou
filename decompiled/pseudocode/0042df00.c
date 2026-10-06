// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42df00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42df00_0 {
    int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[8];
    char field_14;
} st_42df00_0;

typedef struct st_42df00_3 {
    char padding_0[304];
    unsigned int field_130;
} st_42df00_3;

typedef struct st_42df00_1 {
    char padding_0[124];
    char field_7c;
    char padding_7d[975];
    unsigned int field_44c;
    char padding_450[30];
    short field_46e;
    int field_470;
    char padding_474[2856];
    struct st_42df00_0 *field_f9c;
} st_42df00_1;

typedef struct st_42df00_6 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42df00_6;

typedef struct st_42df00_5 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[1060];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42df00_5;

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

typedef struct st_42df00_4 {
    char padding_0[1160];
    struct st_42df00_3 *field_488;
} st_42df00_4;

extern unsigned int g_4ad138;
extern st_42df00_4 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_42df00(st_42df00_1 *a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    int i;  // ebp
    unsigned int v28;  // edi
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    int v39;  // edx
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // 4292
    unsigned int v45;  // eax
    unsigned int v46;  // edx
    st_42df00_1 *idx;  // edi
    unsigned int v47;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // ecx
    st_42df00_3 *idx1;  // esi
    st_42df00_5 *v56;  // eax
    int v30;  // esi
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int v62;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // ftop
    int iter;  // ebx
    int v31;  // ebx
    int node;  // esi
    unsigned int v67;  // edx
    st_42df00_0 *v68;  // ecx
    unsigned int *idx2;  // eax
    unsigned int *v70;  // ecx
    int k;  // ecx
    st_42df00_5 *v72;  // eax
    unsigned int v73;  // edx
    unsigned int v74;  // eax
    unsigned int v75;  // ftop
    unsigned int v32;  // ftop
    unsigned int v76;  // ftop
    unsigned int v78;  // ftop
    unsigned int v79;  // ftop
    unsigned int v80;  // ftop
    unsigned int v81;  // ftop
    unsigned int v82;  // ftop
    st_42df00_3 *v83;  // ebp
    st_42df00_6 *v84;  // eax
    unsigned int v85;  // ftop
    unsigned int v33;  // ftop
    unsigned int v86;  // ftop
    int v87;  // edx
    st_42df00_6 *v88;  // ebx
    unsigned int v89;  // ftop
    unsigned int v90;  // ftop
    unsigned int v91;  // ftop
    unsigned int v92;  // ftop
    unsigned int v93;  // ftop
    unsigned int v94;  // ftop
    unsigned int v34;  // ftop
    unsigned int v95;  // ebx
    unsigned int v96;  // edx
    st_42df00_0 *v97;  // ecx
    unsigned int *v98;  // eax
    unsigned int *v99;  // ecx
    unsigned int v100;  // eax
    st_42df00_0 *v35;  // ecx
    unsigned int v36;  // ftop
    unsigned int v0;  // [bp-0x184]
    unsigned int v1;  // [bp-0x180]
    unsigned int v2;  // [bp-0x17c]
    unsigned int v3;  // [bp-0x178]
    unsigned int v4;  // [bp-0x174]
    unsigned int v5;  // [bp-0x170]
    unsigned int v6;  // [bp-0x16c]
    unsigned int v7;  // [bp-0x168]
    unsigned int v8;  // [bp-0x164]
    unsigned int v9;  // [bp-0x160]
    st_42df00_5 *iter1;  // [bp-0x15c], Other Possible Types: int
    int v11;  // [bp-0x158]
    unsigned int v12;  // [bp-0x154]
    unsigned int v13;  // [bp-0x150]
    int j;  // [bp-0x14c], Other Possible Types: unsigned int
    st_42df00_0 *v15;  // [bp-0x148], Other Possible Types: int
    unsigned int v16;  // [bp-0x140]
    unsigned int v17;  // [bp-0x13c]
    int v18;  // [bp-0x138], Other Possible Types: unsigned int
    unsigned int v19;  // [bp-0x134]
    char v20;  // [bp-0x12c]
    st_42df00_0 *v21;  // [bp-0x128]
    unsigned short v101;  // [bp-0x124]
    unsigned int v22;  // [bp-0x118]
    char v23;  // [bp-0x104]
    unsigned int v24;  // [bp-0x24]
    unsigned int v25;  // [bp-0x14]
    unsigned int v26;  // [bp-0x4]

    v26 = g_4ad138 ^ &v20;
    i = 0;
    v19 = v28;
    idx = a0;
    if (!a5 || !idx->field_44c)
    {
        v15 = &v23;
        v22 = 0;
        sub_477420(&v23, 0, 0x100, v30, v31);
        v33 = v32 - 1;
        if (/* unsupported instruction */)
        {
            v34 = v33 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v34 = v33 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v35 = idx->field_f9c;
        /* unsupported instruction */
        /* unsupported instruction */
        a3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v36 = v34 + 1;
        v21 = v35;
        if (idx->field_470 > 0)
        {
            do
            {
                v37 = v36 - 1;
                if (/* unsupported instruction */)
                {
                    v38 = v37 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v38 = v37 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v39 = v35->field_0;
                v16 = v35->field_4;
                v40 = v38 - 1;
                if (/* unsupported instruction */)
                {
                    v41 = v40 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v41 = v40 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v39 = v35->field_0;
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
                /* unsupported instruction */
                /* unsupported instruction */
                v17 = v35->field_8;
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
                j = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v43 = v41 + 1;
                v44 = _ccall(10, 13, (char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v44 & 1 && !(v18 = (int)(v18 + 1), v45 = ((int)((unsigned int)(1431655766 * i) >> 63) + (unsigned int)(1431655766 * i >> 32)) * 3, v46 = (unsigned int)i, *((char *)((char *)&v101 + i)) = 1, v46 != v45))
                {
                    if (v25 != v46 - v45)
                    {
                        if (/* unsupported instruction */)
                            v9 = /* unsupported instruction */;
                        else
                            v9 = nan;
                        if (/* unsupported instruction */)
                        {
                            v8 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v47 = v43 + 1;
                        }
                        else
                        {
                            v8 = nan;
                            /* unsupported instruction */
                            v47 = v43 + 1;
                        }
                        if (!sub_42e820())
                        {
                            v49 = v47 - 1;
                            if (/* unsupported instruction */)
                            {
                                v50 = v49 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v50 = v49 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v7 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v51 = v50 + 1;
                            }
                            else
                            {
                                v7 = nan;
                                /* unsupported instruction */
                                v51 = v50 + 1;
                            }
                            v52 = v51 - 1;
                            if (/* unsupported instruction */)
                            {
                                v53 = v52 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v53 = v52 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v6 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v47 = v53 + 1;
                            }
                            else
                            {
                                v6 = nan;
                                /* unsupported instruction */
                                v47 = v53 + 1;
                            }
                            v5 = v54;
                            v4 = 9;
                            sub_4273f0();
                        }
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v47 = v43 + 1;
                    }
                    idx1 = g_4b44f4->field_488;
                    v39 = idx->field_46e * 2 + 4;
                    if (g_4cee78 & 0x8000)
                    {
                        EnterCriticalSection(&g_4cf1d0.DebugInfo);
                        g_4cf221 = g_4cf221 + 1;
                    }
                    idx1->field_130 = idx1->field_130 + 1;
                    v56 = sub_4621c0();
                    v57 = v47 - 1;
                    if (/* unsupported instruction */)
                    {
                        v58 = v57 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v58 = v57 - 1;
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
                    v11 = idx->field_46e * 2 + 4;
                    iter1 = v56;
                    iter1->field_480 = iter1->field_480 | 1;
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
                    *((unsigned int *)&iter1->padding_c[20]) = 23;
                    if (/* unsupported instruction */)
                    {
                        iter1->field_430 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v59 = v58 + 1;
                    }
                    else
                    {
                        iter1->field_430 = nan;
                        /* unsupported instruction */
                        v59 = v58 + 1;
                    }
                    v60 = v59 - 1;
                    if (/* unsupported instruction */)
                    {
                        v61 = v60 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v61 = v60 - 1;
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
                        iter1->field_434 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v62 = v61 + 1;
                    }
                    else
                    {
                        iter1->field_434 = nan;
                        /* unsupported instruction */
                        v62 = v61 + 1;
                    }
                    v63 = v62 - 1;
                    if (/* unsupported instruction */)
                    {
                        v64 = v63 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v64 = v63 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    if (/* unsupported instruction */)
                    {
                        iter1->field_438 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v36 = v64 + 1;
                    }
                    else
                    {
                        iter1->field_438 = nan;
                        /* unsupported instruction */
                        v36 = v64 + 1;
                    }
                    sub_454d10();
                    sub_461250(iter1, v11);
                    if (g_4cee78 & 0x8000)
                    {
                        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                        g_4cf221 = g_4cf221 - 1;
                    }
                    v35 = v15;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v36 = v43 + 1;
                }
                v15 = &v35->field_14;
                i += 1;
                v35 = v15;
            } while (i < idx->field_470);
            iter = 0;
            if (v18)
            {
                if (v18 >= i)
                {
                    idx->field_7c = 1;
                }
                else
                {
                    node = 0;
                    if (i > 0)
                    {
                        do
                        {
                        } while (*((char *)&v101 + node) && (node = (int)(node + 1), node < i));
                        if (!node)
                            goto LABEL_42e198;
                        j = 0;
                        if (idx->field_470 - node > 0)
                        {
                            v67 = node * 20;
                            do
                            {
                                v68 = idx->field_f9c;
                                idx2 = v67 + (char *)v68;
                                v70 = (char *)v68 + iter;
                                *(v70) = *((int *)(v67 + (char *)v68));
                                v70[1] = idx2[1];
                                v70[2] = idx2[2];
                                v70[3] = idx2[3];
                                v70[4] = idx2[4];
                                iter += 20;
                                v67 += 20;
                                j += 1;
                            } while (j < idx->field_470 - node);
                        }
                        idx->field_470 = idx->field_470 - node;
                    }
                    else
                    {
LABEL_42e198:
                        k = 0;
                        if (i > 0)
                        {
                            do
                            {
                                if (*((char *)&v101 + node))
                                {
                                    v15 = iter;
                                    if (node < i)
                                    {
                                        if (iter)
                                        {
                                            do
                                            {
                                                if (!*((char *)&v101 + node))
                                                {
                                                    if (node >= i)
                                                    {
                                                        idx->field_470 = iter;
                                                        goto LABEL_42e3b9;
                                                    }
                                                    else
                                                    {
                                                        v19 = 0;
                                                        if (iter > 0)
                                                        {
                                                            j = 0;
                                                            do
                                                            {
                                                                v72 = idx->field_f9c + iter1;
                                                                v12 = v72->field_0;
                                                                j = v72->field_8;
                                                                v13 = v72->field_4;
                                                                v73 = (((unsigned int)(1431655766 * k) >> 63) + (unsigned int)(1431655766 * k >> 32)) * 3;
                                                                v74 = k;
                                                                if (v74 == v73)
                                                                {
                                                                    if (v24 != v74 - v73)
                                                                    {
                                                                        v75 = v36 - 1;
                                                                        if (/* unsupported instruction */)
                                                                        {
                                                                            v76 = v75 - 1;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                        }
                                                                        else
                                                                        {
                                                                            v76 = v75 - 1;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                        }
                                                                        if (/* unsupported instruction */)
                                                                            v5 = /* unsupported instruction */;
                                                                        else
                                                                            v5 = nan;
                                                                        if (/* unsupported instruction */)
                                                                        {
                                                                            v4 = /* unsupported instruction */;
                                                                            /* unsupported instruction */
                                                                            v36 = v76 + 1;
                                                                        }
                                                                        else
                                                                        {
                                                                            v4 = nan;
                                                                            /* unsupported instruction */
                                                                            v36 = v76 + 1;
                                                                        }
                                                                        if (!sub_42e820())
                                                                        {
                                                                            v78 = v36 - 1;
                                                                            if (/* unsupported instruction */)
                                                                            {
                                                                                v79 = v78 - 1;
                                                                                /* unsupported instruction */
                                                                                /* unsupported instruction */
                                                                            }
                                                                            else
                                                                            {
                                                                                v79 = v78 - 1;
                                                                                /* unsupported instruction */
                                                                                /* unsupported instruction */
                                                                            }
                                                                            if (/* unsupported instruction */)
                                                                            {
                                                                                v3 = /* unsupported instruction */;
                                                                                /* unsupported instruction */
                                                                                v80 = v79 + 1;
                                                                            }
                                                                            else
                                                                            {
                                                                                v3 = nan;
                                                                                /* unsupported instruction */
                                                                                v80 = v79 + 1;
                                                                            }
                                                                            v81 = v80 - 1;
                                                                            if (/* unsupported instruction */)
                                                                            {
                                                                                v82 = v81 - 1;
                                                                                /* unsupported instruction */
                                                                                /* unsupported instruction */
                                                                            }
                                                                            else
                                                                            {
                                                                                v82 = v81 - 1;
                                                                                /* unsupported instruction */
                                                                                /* unsupported instruction */
                                                                            }
                                                                            if (/* unsupported instruction */)
                                                                            {
                                                                                v2 = /* unsupported instruction */;
                                                                                /* unsupported instruction */
                                                                                v36 = v82 + 1;
                                                                            }
                                                                            else
                                                                            {
                                                                                v2 = nan;
                                                                                /* unsupported instruction */
                                                                                v36 = v82 + 1;
                                                                            }
                                                                            v1 = v54;
                                                                            v0 = 9;
                                                                            sub_4273f0();
                                                                        }
                                                                    }
                                                                    v83 = g_4b44f4->field_488;
                                                                    v18 = idx->field_46e * 2 + 4;
                                                                    if (g_4cee78 & 0x8000)
                                                                    {
                                                                        EnterCriticalSection(&g_4cf1d0.DebugInfo);
                                                                        g_4cf221 = g_4cf221 + 1;
                                                                    }
                                                                    v83->field_130 = v83->field_130 + 1;
                                                                    v84 = sub_4621c0();
                                                                    v85 = v36 - 1;
                                                                    if (/* unsupported instruction */)
                                                                    {
                                                                        v86 = v85 - 1;
                                                                        /* unsupported instruction */
                                                                        /* unsupported instruction */
                                                                    }
                                                                    else
                                                                    {
                                                                        v86 = v85 - 1;
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
                                                                    v87 = idx->field_46e * 2 + 4;
                                                                    v88 = v84;
                                                                    v88->field_480 = v88->field_480 | 1;
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
                                                                    v88->field_20 = 23;
                                                                    if (/* unsupported instruction */)
                                                                    {
                                                                        v88->field_430 = /* unsupported instruction */;
                                                                        /* unsupported instruction */
                                                                        v89 = v86 + 1;
                                                                    }
                                                                    else
                                                                    {
                                                                        v88->field_430 = nan;
                                                                        /* unsupported instruction */
                                                                        v89 = v86 + 1;
                                                                    }
                                                                    v90 = v89 - 1;
                                                                    if (/* unsupported instruction */)
                                                                    {
                                                                        v91 = v90 - 1;
                                                                        /* unsupported instruction */
                                                                        /* unsupported instruction */
                                                                    }
                                                                    else
                                                                    {
                                                                        v91 = v90 - 1;
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
                                                                        v88->field_434 = /* unsupported instruction */;
                                                                        /* unsupported instruction */
                                                                        v92 = v91 + 1;
                                                                    }
                                                                    else
                                                                    {
                                                                        v88->field_434 = nan;
                                                                        /* unsupported instruction */
                                                                        v92 = v91 + 1;
                                                                    }
                                                                    v93 = v92 - 1;
                                                                    if (/* unsupported instruction */)
                                                                    {
                                                                        v94 = v93 - 1;
                                                                        /* unsupported instruction */
                                                                        /* unsupported instruction */
                                                                    }
                                                                    else
                                                                    {
                                                                        v94 = v93 - 1;
                                                                        /* unsupported instruction */
                                                                        /* unsupported instruction */
                                                                    }
                                                                    if (/* unsupported instruction */)
                                                                    {
                                                                        v88->field_438 = /* unsupported instruction */;
                                                                        /* unsupported instruction */
                                                                        v36 = v94 + 1;
                                                                    }
                                                                    else
                                                                    {
                                                                        v88->field_438 = nan;
                                                                        /* unsupported instruction */
                                                                        v36 = v94 + 1;
                                                                    }
                                                                    sub_454d10();
                                                                    sub_461250(v88, v87);
                                                                    if (g_4cee78 & 0x8000)
                                                                    {
                                                                        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                                                                        g_4cf221 = g_4cf221 - 1;
                                                                    }
                                                                    k = v39;
                                                                    iter = v11;
                                                                }
                                                                iter1 = &iter1->padding_c[8];
                                                                v39 = k + 1;
                                                                k = v39;
                                                            } while (k < iter);
                                                        }
                                                        v95 = 0;
                                                        iter1 = 0;
                                                        if (idx->field_470 - node > 0)
                                                        {
                                                            v96 = node * 20;
                                                            do
                                                            {
                                                                v97 = idx->field_f9c;
                                                                v98 = v96 + (char *)v97;
                                                                v99 = (char *)v97 + v95;
                                                                *(v99) = *((int *)(v96 + (char *)v97));
                                                                v99[1] = v98[1];
                                                                v99[2] = v98[2];
                                                                v99[3] = v98[3];
                                                                v99[4] = v98[4];
                                                                iter1 += 1;
                                                                v95 += 20;
                                                                v96 += 20;
                                                            } while (iter1 < idx->field_470 - node);
                                                        }
                                                    }
                                                }
                                            } while ((node = (int)(node + 1), node < i));
                                            idx->field_470 = iter;
                                            break;
                                        }
                                        break;
                                    }
                                }
                            } while ((node = (int)(node + 1), iter = (int)(iter + 1), node < i));
                        }
                    }
                }
            }
        }
LABEL_42e3b9:
    }
    _security_check_cookie();
    return v100;
}
