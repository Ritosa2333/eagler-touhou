// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42d9c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42d9c0_0 {
    struct st_42d9c0_1 *field_0;
    char field_1;
    char padding_2[2];
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[8];
    char field_14;
} st_42d9c0_0;

typedef struct st_42d9c0_3 {
    char padding_0[304];
    unsigned int field_130;
} st_42d9c0_3;

typedef struct st_42d9c0_2 {
    char padding_0[124];
    char field_7c;
    char padding_7d[975];
    unsigned int field_44c;
    char padding_450[30];
    short field_46e;
    int field_470;
    char padding_474[2856];
    struct st_42d9c0_0 *field_f9c;
} st_42d9c0_2;

typedef struct st_42d9c0_1 {
    char field_0;
} st_42d9c0_1;

typedef struct st_42d9c0_6 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42d9c0_6;

typedef struct st_42d9c0_5 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[1060];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42d9c0_5;

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

typedef struct st_42d9c0_4 {
    char padding_0[1160];
    struct st_42d9c0_3 *field_488;
} st_42d9c0_4;

extern unsigned int g_4ad138;
extern st_42d9c0_4 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_42d9c0(st_42d9c0_2 *idx, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    unsigned int v28;  // ebx
    int v29;  // esi
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // fc3210
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // 4139
    int i;  // ebp
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v55;  // 4139
    unsigned int v56;  // eax
    unsigned int v31;  // edi
    unsigned int v57;  // edx
    unsigned int v58;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int v62;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // ftop
    unsigned int *v65;  // ecx
    st_42d9c0_3 *idx1;  // esi
    st_42d9c0_0 *node;  // ecx
    st_42d9c0_5 *v67;  // eax
    unsigned int v68;  // ftop
    unsigned int v69;  // ftop
    unsigned int v70;  // ftop
    unsigned int v71;  // ftop
    unsigned int v72;  // ftop
    unsigned int v73;  // ftop
    unsigned int v74;  // ftop
    unsigned int v75;  // ftop
    char *iter;  // ebx
    int iter1;  // esi
    unsigned int v78;  // edx
    st_42d9c0_0 *v79;  // ecx
    unsigned int *idx2;  // eax
    unsigned int *v81;  // ecx
    char *iter2;  // ecx
    st_42d9c0_5 *v83;  // eax
    unsigned int v84;  // edx
    unsigned int v85;  // eax
    unsigned int v86;  // ftop
    unsigned int v33;  // ftop
    unsigned int v87;  // ftop
    unsigned int v89;  // ftop
    unsigned int v90;  // ftop
    unsigned int v91;  // ftop
    unsigned int v92;  // ftop
    unsigned int v93;  // ftop
    st_42d9c0_3 *v94;  // ebp
    st_42d9c0_6 *v95;  // eax
    unsigned int v96;  // ftop
    unsigned int v34;  // ftop
    unsigned int v97;  // ftop
    st_42d9c0_6 *index;  // ebx
    unsigned int v99;  // ftop
    unsigned int v100;  // ftop
    unsigned int v101;  // ftop
    unsigned int v102;  // ftop
    unsigned int v103;  // ftop
    unsigned int v104;  // ftop
    unsigned int v105;  // ebx
    unsigned int v106;  // edx
    unsigned int v35;  // ftop
    st_42d9c0_0 *v107;  // ecx
    unsigned int *v108;  // eax
    unsigned int *v109;  // ecx
    unsigned int v110;  // eax
    unsigned int v36;  // ftop
    unsigned int v0;  // [bp-0x184]
    unsigned int v1;  // [bp-0x180]
    unsigned int v2;  // [bp-0x17c]
    unsigned int v3;  // [bp-0x178]
    unsigned int v4;  // [bp-0x174]
    unsigned int v5;  // [bp-0x170]
    unsigned int v6;  // [bp-0x16c]
    unsigned int v7;  // [bp-0x168]
    st_42d9c0_5 *l;  // [bp-0x164], Other Possible Types: int
    char *v9;  // [bp-0x160]
    unsigned int v10;  // [bp-0x15c]
    unsigned int v11;  // [bp-0x158]
    int j;  // [bp-0x154], Other Possible Types: unsigned int
    char *v13;  // [bp-0x150], Other Possible Types: int
    char *v14;  // [bp-0x14c]
    unsigned int v15;  // [bp-0x148]
    int <0x42d9c0[is_1]|Stack bp-0x144, 1 B>;  // [bp-0x144]
    unsigned int v16;  // [bp-0x144]
    st_42d9c0_0 *v17;  // [bp-0x140], Other Possible Types: int
    unsigned int v18;  // [bp-0x13c]
    unsigned int v19;  // [bp-0x138]
    unsigned int v20;  // [bp-0x130]
    unsigned int v21;  // [bp-0x124]
    unsigned int v22;  // [bp-0x120]
    unsigned int v23;  // [bp-0x118]
    unsigned int v24;  // [bp-0x114]
    char v25;  // [bp-0x104]
    unsigned int v26;  // [bp-0x14]
    unsigned int v27;  // [bp-0x4]

    v27 = g_4ad138 ^ &<0x42d9c0[is_1]|Stack bp-0x144, 1 B>;
    v15 = v28;
    v13 = v29;
    i = 0;
    j = v31;
    if (a5 && idx->field_44c)
    {
        _security_check_cookie();
        return v110;
    }
    v9 = &v25;
    v20 = 0;
    sub_477420(&v25, 0, 0x100);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    node = idx->field_f9c;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v17 = node;
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (idx->field_470 > 0)
    {
        do
        {
            v34 = v33 - 1;
            if (/* unsupported instruction */)
            {
                v35 = v34 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v35 = v34 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v14 = &node->field_0->field_0;
            v36 = v35 - 1;
            if (/* unsupported instruction */)
            {
                v37 = v36 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v37 = v36 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v38 = v37 - 1;
            if (/* unsupported instruction */)
            {
                v39 = v38 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v39 = v38 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v40 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
            /* unsupported instruction */
            v41 = v39 + 1;
            v15 = *((int *)&node->field_1);
            v16 = node->field_4;
            if ((char)((((unsigned short)v41 & 7) * 0x800 | (unsigned short)v40 & 0x4700) >> 8) & 65)
            {
                v42 = v41 - 1;
                if (/* unsupported instruction */)
                {
                    v43 = v42 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v43 = v42 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v45 = v43 + 2;
                v46 = _ccall(10, 13, (char)((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v46 & 1))
                    goto LABEL_42dc10;
                v47 = v45 - 1;
                if (/* unsupported instruction */)
                {
                    v48 = v47 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v48 = v47 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v49 = v48 - 1;
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
                /* unsupported instruction */
                v41 = v50 + 1;
                if ((char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                {
                    v52 = v41 - 1;
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
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v45 = v53 + 2;
                    v55 = _ccall(10, 13, (char)((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (v55 & 1 && !(v17 += 1, v56 = ((int)((unsigned int)(1431655766 * i) >> 63) + (unsigned int)(1431655766 * i >> 32)) * 3, v57 = (unsigned int)i, *((char *)((char *)&v24 + i)) = 1, v57 != v56))
                    {
                        if (v27 != v57 - v56)
                        {
                            if (/* unsupported instruction */)
                                v7 = /* unsupported instruction */;
                            else
                                v7 = nan;
                            if (/* unsupported instruction */)
                            {
                                v6 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v58 = v45 + 1;
                            }
                            else
                            {
                                v6 = nan;
                                /* unsupported instruction */
                                v58 = v45 + 1;
                            }
                            if (!sub_42e820())
                            {
                                v60 = v58 - 1;
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
                                    v5 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v62 = v61 + 1;
                                }
                                else
                                {
                                    v5 = nan;
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
                                    v4 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v58 = v64 + 1;
                                }
                                else
                                {
                                    v4 = nan;
                                    /* unsupported instruction */
                                    v58 = v64 + 1;
                                }
                                sub_4273f0(9, v65, v4, v5);
                            }
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v58 = v45 + 1;
                        }
                        idx1 = g_4b44f4->field_488;
                        v14 = idx->field_46e * 2 + 4;
                        if (g_4cee78 & 0x8000)
                        {
                            EnterCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 + 1;
                        }
                        idx1->field_130 = idx1->field_130 + 1;
                        v67 = sub_4621c0();
                        v68 = v58 - 1;
                        if (/* unsupported instruction */)
                        {
                            v69 = v68 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v69 = v68 - 1;
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
                        v9 = v14;
                        l = v67;
                        l->field_480 = l->field_480 | 1;
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
                        *((unsigned int *)&l->padding_c[20]) = 23;
                        if (/* unsupported instruction */)
                        {
                            l->field_430 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v70 = v69 + 1;
                        }
                        else
                        {
                            l->field_430 = nan;
                            /* unsupported instruction */
                            v70 = v69 + 1;
                        }
                        v71 = v70 - 1;
                        if (/* unsupported instruction */)
                        {
                            v72 = v71 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v72 = v71 - 1;
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
                            l->field_434 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v73 = v72 + 1;
                        }
                        else
                        {
                            l->field_434 = nan;
                            /* unsupported instruction */
                            v73 = v72 + 1;
                        }
                        v74 = v73 - 1;
                        if (/* unsupported instruction */)
                        {
                            v75 = v74 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v75 = v74 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                        {
                            l->field_438 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v33 = v75 + 1;
                        }
                        else
                        {
                            l->field_438 = nan;
                            /* unsupported instruction */
                            v33 = v75 + 1;
                        }
                        sub_454d10();
                        sub_461250(l, v9);
                        if (g_4cee78 & 0x8000)
                        {
                            LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 - 1;
                        }
                        node = v13;
                        continue;
                    }
                    goto LABEL_42dc10;
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = v41 + 1;
LABEL_42dc10:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v33 = v45 + 1;
            node = &node->padding_c[4];
            i += 1;
            v13 = node;
        } while (i < idx->field_470);
        iter = NULL;
        if (v17)
        {
            if (v17 >= i)
            {
                idx->field_7c = 1;
                _security_check_cookie();
                return v110;
            }
            iter1 = 0;
            if (i > 0)
            {
                do
                {
                } while (*((char *)&v24 + iter1) && (iter1 = (int)(iter1 + 1), iter1 < i));
                if (!iter1)
                    goto LABEL_42dcba;
                j = 0;
                if (idx->field_470 - iter1 > 0)
                {
                    v78 = iter1 * 20;
                    do
                    {
                        v79 = idx->field_f9c;
                        idx2 = (char *)v79 + v78;
                        v81 = v79 + iter;
                        *(v81) = *((int *)((char *)v79 + v78));
                        v81[1] = idx2[1];
                        v81[2] = idx2[2];
                        v81[3] = idx2[3];
                        v81[4] = idx2[4];
                        iter += 20;
                        v78 += 20;
                        j += 1;
                    } while (j < idx->field_470 - iter1);
                }
                idx->field_470 = idx->field_470 - iter1;
            }
            else
            {
LABEL_42dcba:
                if (i > 0)
                {
                    do
                    {
                        if (*((char *)&v24 + iter1))
                        {
                            v13 = iter;
                            if (iter1 < i && iter)
                            {
                                do
                                {
                                    if (!*((char *)&v24 + iter1))
                                    {
                                        if (iter1 >= i)
                                        {
                                            idx->field_470 = iter;
                                            _security_check_cookie();
                                            return v110;
                                        }
                                        iter2 = NULL;
                                        v18 = 0;
                                        if (iter > 0)
                                        {
                                            j = 0;
                                            do
                                            {
                                                v83 = idx->field_f9c + l;
                                                v10 = v83->field_0;
                                                j = v83->field_8;
                                                v11 = v83->field_4;
                                                v84 = ((1431655766 * iter2 >> 63) + (1431655766 * iter2 >> 32)) * 3;
                                                v85 = iter2;
                                                if (v85 == v84)
                                                {
                                                    if (v26 != v85 - v84)
                                                    {
                                                        v86 = v33 - 1;
                                                        if (/* unsupported instruction */)
                                                        {
                                                            v87 = v86 - 1;
                                                            /* unsupported instruction */
                                                            /* unsupported instruction */
                                                        }
                                                        else
                                                        {
                                                            v87 = v86 - 1;
                                                            /* unsupported instruction */
                                                            /* unsupported instruction */
                                                        }
                                                        if (/* unsupported instruction */)
                                                            v3 = /* unsupported instruction */;
                                                        else
                                                            v3 = nan;
                                                        if (/* unsupported instruction */)
                                                        {
                                                            v2 = /* unsupported instruction */;
                                                            /* unsupported instruction */
                                                            v33 = v87 + 1;
                                                        }
                                                        else
                                                        {
                                                            v2 = nan;
                                                            /* unsupported instruction */
                                                            v33 = v87 + 1;
                                                        }
                                                        if (!sub_42e820())
                                                        {
                                                            v89 = v33 - 1;
                                                            if (/* unsupported instruction */)
                                                            {
                                                                v90 = v89 - 1;
                                                                /* unsupported instruction */
                                                                /* unsupported instruction */
                                                            }
                                                            else
                                                            {
                                                                v90 = v89 - 1;
                                                                /* unsupported instruction */
                                                                /* unsupported instruction */
                                                            }
                                                            if (/* unsupported instruction */)
                                                            {
                                                                v1 = /* unsupported instruction */;
                                                                /* unsupported instruction */
                                                                v91 = v90 + 1;
                                                            }
                                                            else
                                                            {
                                                                v1 = nan;
                                                                /* unsupported instruction */
                                                                v91 = v90 + 1;
                                                            }
                                                            v92 = v91 - 1;
                                                            if (/* unsupported instruction */)
                                                            {
                                                                v93 = v92 - 1;
                                                                /* unsupported instruction */
                                                                /* unsupported instruction */
                                                            }
                                                            else
                                                            {
                                                                v93 = v92 - 1;
                                                                /* unsupported instruction */
                                                                /* unsupported instruction */
                                                            }
                                                            if (/* unsupported instruction */)
                                                            {
                                                                v0 = /* unsupported instruction */;
                                                                /* unsupported instruction */
                                                                v33 = v93 + 1;
                                                            }
                                                            else
                                                            {
                                                                v0 = nan;
                                                                /* unsupported instruction */
                                                                v33 = v93 + 1;
                                                            }
                                                            sub_4273f0(9, v65, v0, v1);
                                                        }
                                                    }
                                                    v94 = g_4b44f4->field_488;
                                                    v17 = idx->field_46e * 2 + 4;
                                                    if (g_4cee78 & 0x8000)
                                                    {
                                                        EnterCriticalSection(&g_4cf1d0.DebugInfo);
                                                        g_4cf221 = g_4cf221 + 1;
                                                    }
                                                    v94->field_130 = v94->field_130 + 1;
                                                    v95 = sub_4621c0();
                                                    v96 = v33 - 1;
                                                    if (/* unsupported instruction */)
                                                    {
                                                        v97 = v96 - 1;
                                                        /* unsupported instruction */
                                                        /* unsupported instruction */
                                                    }
                                                    else
                                                    {
                                                        v97 = v96 - 1;
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
                                                    index = v95;
                                                    index->field_480 = index->field_480 | 1;
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
                                                    index->field_20 = 23;
                                                    if (/* unsupported instruction */)
                                                    {
                                                        index->field_430 = /* unsupported instruction */;
                                                        /* unsupported instruction */
                                                        v99 = v97 + 1;
                                                    }
                                                    else
                                                    {
                                                        index->field_430 = nan;
                                                        /* unsupported instruction */
                                                        v99 = v97 + 1;
                                                    }
                                                    v100 = v99 - 1;
                                                    if (/* unsupported instruction */)
                                                    {
                                                        v101 = v100 - 1;
                                                        /* unsupported instruction */
                                                        /* unsupported instruction */
                                                    }
                                                    else
                                                    {
                                                        v101 = v100 - 1;
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
                                                        index->field_434 = /* unsupported instruction */;
                                                        /* unsupported instruction */
                                                        v102 = v101 + 1;
                                                    }
                                                    else
                                                    {
                                                        index->field_434 = nan;
                                                        /* unsupported instruction */
                                                        v102 = v101 + 1;
                                                    }
                                                    v103 = v102 - 1;
                                                    if (/* unsupported instruction */)
                                                    {
                                                        v104 = v103 - 1;
                                                        /* unsupported instruction */
                                                        /* unsupported instruction */
                                                    }
                                                    else
                                                    {
                                                        v104 = v103 - 1;
                                                        /* unsupported instruction */
                                                        /* unsupported instruction */
                                                    }
                                                    if (/* unsupported instruction */)
                                                    {
                                                        index->field_438 = /* unsupported instruction */;
                                                        /* unsupported instruction */
                                                        v33 = v104 + 1;
                                                    }
                                                    else
                                                    {
                                                        index->field_438 = nan;
                                                        /* unsupported instruction */
                                                        v33 = v104 + 1;
                                                    }
                                                    sub_454d10();
                                                    sub_461250(index, v17);
                                                    if (g_4cee78 & 0x8000)
                                                    {
                                                        LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                                                        g_4cf221 = g_4cf221 - 1;
                                                    }
                                                    iter2 = v14;
                                                    iter = v9;
                                                }
                                                l = &l->padding_c[8];
                                                iter2 += 1;
                                                v14 = iter2;
                                            } while (iter2 < iter);
                                        }
                                        v105 = 0;
                                        l = 0;
                                        if (idx->field_470 - iter1 > 0)
                                        {
                                            v106 = iter1 * 20;
                                            do
                                            {
                                                v107 = idx->field_f9c;
                                                v108 = (char *)v107 + v106;
                                                v109 = (char *)v107 + v105;
                                                *(v109) = *((int *)((char *)v107 + v106));
                                                v109[1] = v108[1];
                                                v109[2] = v108[2];
                                                v109[3] = v108[3];
                                                v109[4] = v108[4];
                                                l += 1;
                                                v105 += 20;
                                                v106 += 20;
                                            } while (l < idx->field_470 - iter1);
                                        }
                                    }
                                } while ((iter1 = (int)(iter1 + 1), iter1 < i));
                                idx->field_470 = iter;
                                break;
                            }
                        }
                    } while ((iter1 = (int)(iter1 + 1), iter += 1, iter1 < i));
                }
            }
        }
    }
    _security_check_cookie();
    return v110;
}
