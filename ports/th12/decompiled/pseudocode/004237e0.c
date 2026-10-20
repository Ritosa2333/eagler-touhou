// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4237e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4237e0_4 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[8];
    struct st_4237e0_1 *field_10;
    struct st_4237e0_0 *field_14;
    char padding_18[72];
    int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    char padding_70[12];
    unsigned int field_7c;
    char padding_80[4];
    char field_84[4];
    char field_87;
} st_4237e0_4;

typedef struct st_4237e0_13 {
    struct st_4237e0_11 *field_0;
} st_4237e0_13;

typedef struct st_4237e0_10 {
    char padding_0[420];
    unsigned int field_1a4;
} st_4237e0_10;

typedef struct st_4237e0_0 {
    char padding_0[4];
    struct st_4237e0_1 *field_4;
} st_4237e0_0;

typedef struct st_4237e0_5 {
    struct st_4237e0_4 *field_0;
    struct st_4237e0_5 *field_4;
} st_4237e0_5;

typedef struct st_4237e0_11 {
    char padding_0[1148];
    unsigned int field_47c;
} st_4237e0_11;

typedef struct st_4237e0_12 {
    char padding_0[20];
    struct st_4237e0_13 *field_14;
    char padding_18[1124];
    unsigned int field_47c;
} st_4237e0_12;

typedef struct st_4237e0_21 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4237e0_21;

typedef struct st_4237e0_20 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4237e0_20;

typedef struct st_4237e0_6 {
    char padding_0[956];
    unsigned int field_3bc;
    char padding_3c0[60];
    unsigned int field_3fc;
    unsigned int field_400;
    char padding_404[44];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[64];
    unsigned int field_47c;
    char padding_480[28];
    char field_49c;
    char field_49d;
} st_4237e0_6;

typedef struct st_4237e0_1 {
    char padding_0[8];
    struct st_4237e0_0 *field_8;
} st_4237e0_1;

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

typedef struct st_4237e0_19 {
    char padding_0[304];
    unsigned int field_130;
} st_4237e0_19;

extern char g_4b0cb8;
extern unsigned int g_4ce8cc;
extern st_4237e0_19 *g_4cee70;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_4237e0(void)
{
    unsigned int v25;  // ebx
    unsigned int v26;  // edi
    st_4237e0_5 *node;  // eax
    st_4237e0_12 *idx;  // eax
    st_4237e0_5 *iter;  // eax
    unsigned int v37;  // eax
    unsigned int v38;  // eax
    unsigned short v40;  // ftop
    unsigned short v41;  // ftop
    unsigned short v42;  // ftop
    unsigned short v43;  // ftop
    st_4237e0_5 *v27;  // eax
    st_4237e0_20 *idx1;  // ebx
    unsigned int v46;  // esi
    st_4237e0_6 *k;  // ecx
    st_4237e0_5 *iter1;  // eax
    st_4237e0_0 *iter2;  // eax
    unsigned short v50;  // ftop
    unsigned short v51;  // ftop
    unsigned int v52;  // fc3210
    unsigned short v53;  // ftop
    st_4237e0_21 *idx2;  // ebx
    unsigned int v55;  // esi
    st_4237e0_6 *index;  // esi
    unsigned int v57;  // fpround
    unsigned int v58;  // 4113
    st_4237e0_21 *v59;  // ebx
    unsigned int v60;  // esi
    unsigned int v61;  // eax
    unsigned int v62;  // edx
    unsigned int *v63;  // eax
    st_4237e0_5 *i;  // eax
    unsigned int v64;  // eax
    unsigned int *v65;  // eax
    unsigned int v66;  // eax
    unsigned int *v67;  // eax
    st_4237e0_5 *v29;  // ecx
    st_4237e0_4 *v30;  // ebp
    st_4237e0_10 *v31;  // edi
    unsigned int v32;  // eax
    unsigned int j;  // ecx
    unsigned int v0;  // [bp-0x94]
    unsigned int v1;  // [bp-0x90]
    unsigned int v2;  // [bp-0x8c]
    unsigned int v3;  // [bp-0x88]
    unsigned int v4;  // [bp-0x84]
    st_4237e0_5 *v5;  // [bp-0x68]
    st_4237e0_5 *v6;  // [bp-0x68], Other Possible Types: unsigned short
    unsigned int v7;  // [bp-0x64]
    unsigned int v8;  // [bp-0x5c]
    unsigned int v9;  // [bp-0x58]
    st_4237e0_5 *v10;  // [bp-0x54]
    unsigned int v11;  // [bp-0x4c]
    unsigned int v12;  // [bp-0x48]
    unsigned int v13;  // [bp-0x44]
    unsigned int v14;  // [bp-0x40]
    unsigned int v15;  // [bp-0x3c]
    unsigned int v16;  // [bp-0x38]
    unsigned int v17;  // [bp-0x34]
    unsigned int v18;  // [bp-0x30]
    unsigned int v19;  // [bp-0x2c]
    unsigned int v20;  // [bp-0x28]
    st_4237e0_10 *v21;  // [bp-0x24]
    st_4237e0_10 *v22;  // [bp-0x24]
    st_4237e0_10 *v23;  // [bp-0x10]

    v11 = v25;
    v9 = v26;
    if (!v27)
        return;
    do
    {
        v29 = i->field_4;
        v30 = i->field_0;
        v10 = v29;
        if (!v30->field_68)
        {
            v30->field_60 = v30->field_60 - 1;
        }
        else if (v30->field_68 == *((int *)&g_4b0cb8))
        {
            v30->field_60 = v30->field_60 - 1;
        }
        i = v29;
        if (v30->field_60 > 0)
            continue;
        v31 = v23;
        v32 = v31->field_1a4;
        j = *((int *)&v31->padding_0[220 + 4 * v32]);
        if (j)
        {
            node = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (node->field_0->padding_0 == j)
                    {
                        idx = node->field_0;
                        goto LABEL_42386d;
                    }
                } while ((node = (st_4237e0_5 *)node->field_4, node));
            }
            iter = *((int *)(g_4ce8cc + 8935104));
            if (*((int *)(g_4ce8cc + 8935104)))
            {
                while (iter->field_0->padding_0 != j)
                {
                    if (!iter->field_4)
                        goto LABEL_42389f;
                }
                idx = iter->field_0;
LABEL_42386d:
                if (idx)
                {
                    idx->field_47c = idx->field_47c | 0x10000000;
                    if (!*((int *)&idx->padding_18[0]))
                    {
                        v37 = idx->field_14;
                        if (idx->field_14)
                        {
                            do
                            {
                                v38 = v37;
                                *((unsigned int *)(*((int *)v38) + 1148)) = *((int *)(*((int *)v38) + 1148)) | 0x10000000;
                                v37 = *((int *)(v38 + 4));
                            } while (*((int *)(v38 + 4)));
                        }
                    }
                }
            }
        }
LABEL_42389f:
        *((unsigned int *)&v31->padding_0[220 + 4 * v32]) = 0;
        v41 = v40 - 1;
        if (/* unsupported instruction */)
        {
            v42 = v41 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v42 = v41 - 1;
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
            v30->field_4 = /* unsupported instruction */;
            /* unsupported instruction */
            v43 = v42 + 1;
        }
        else
        {
            v30->field_4 = nan;
            /* unsupported instruction */
            v43 = v42 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!(((v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v30->field_7c) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            v8 = v31->field_1a4 + 4;
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            g_4cee70->field_130 = g_4cee70->field_130 + 1;
            idx1 = sub_4621c0();
            idx1->field_480 = idx1->field_480 | 1;
            idx1->field_20 = 23;
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
            sub_454d10();
            v46 = *((int *)sub_461250(idx1, v8));
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
            }
            *((unsigned int *)&v31->padding_0[220 + 4 * v31->field_1a4]) = v46;
            k = *((int *)&v31->padding_0[220 + 4 * v31->field_1a4]);
            if (k)
            {
                iter1 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        if (iter1->field_0->padding_0 == k)
                        {
                            iter2 = iter1->field_0;
                            goto LABEL_4239dc;
                        }
                    } while ((iter1 = (st_4237e0_5 *)iter1->field_4, iter1));
                }
                iter2 = *((int *)(g_4ce8cc + 8935104));
                if (*((int *)(g_4ce8cc + 8935104)))
                {
                    while (*((int *)iter2->padding_0) != k)
                    {
                        iter2 = iter2->field_4;
                        if (!iter2)
                            goto LABEL_4239e0;
                    }
                    iter2 = (st_4237e0_0 *)iter2->padding_0;
LABEL_4239dc:
                    if (iter2)
                        goto LABEL_4239e6;
LABEL_4239e0:
                    *((unsigned int *)&v31->padding_0[220 + 4 * v31->field_1a4]) = 0;
LABEL_4239e6:
                    *((char *)&iter2[147].field_4 + 1) = 15;
                    *((char *)&iter2[147].field_4) = 15;
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
                        v13 = /* unsupported instruction */;
                        /* unsupported instruction */
                    }
                    else
                    {
                        v13 = nan;
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v6 = v5;
                    v22 = v21;
                    goto LABEL_423c98;
                }
            }
            iter2 = NULL;
            goto LABEL_4239e0;
        }
        else
        {
            v50 = v43 - 1;
            if (/* unsupported instruction */)
            {
                v51 = v50 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v51 = v50 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v52 = CmpF(/* unsupported instruction */, v30->field_7c) * 0x100 & 0x4500;
                /* unsupported instruction */
                v53 = v51 + 1;
            }
            else
            {
                v52 = CmpF(nan, v30->field_7c) * 0x100 & 0x4500;
                /* unsupported instruction */
                v53 = v51 + 1;
            }
            if (!(((v53 & 7) * 0x800 | (unsigned short)v52 & 0x4700) >> 8 & 1))
            {
                v8 = v31->field_1a4 + 14;
                if (g_4cee78 & 0x8000)
                {
                    EnterCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 + 1;
                }
                g_4cee70->field_130 = g_4cee70->field_130 + 1;
                idx2 = sub_4621c0();
                idx2->field_480 = idx2->field_480 | 1;
                *((unsigned int *)&idx2->padding_0[32]) = 23;
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
                    idx2->field_430 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    idx2->field_430 = nan;
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
                    idx2->field_434 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    idx2->field_434 = nan;
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
                    idx2->field_438 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    idx2->field_438 = nan;
                    /* unsupported instruction */
                }
                sub_454d10();
                v55 = *((int *)sub_461250(idx2, v8));
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 - 1;
                }
                *((unsigned int *)&v31->padding_0[220 + 4 * v31->field_1a4]) = v55;
                index = sub_461920(*((int *)&v31->padding_0[220 + 4 * v31->field_1a4]));
                if (!index)
                    *((st_4237e0_6 **)&v31->padding_0[220 + 4 * v31->field_1a4]) = index;
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
                v58 = _ccall(v57);
                v6 = v58;
                v7 = v6 | 0xc00;
                if (/* unsupported instruction */)
                {
                    v7 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v7 = nan;
                    /* unsupported instruction */
                }
                index->field_49d = v7;
                index->field_49c = v7;
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
                /* unsupported instruction */
                /* unsupported instruction */
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v22 = v21;
                goto LABEL_423c9a;
            }
            else
            {
                v9 = v31->field_1a4 + 14;
                if (g_4cee78 & 0x8000)
                {
                    EnterCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 + 1;
                }
                g_4cee70->field_130 = g_4cee70->field_130 + 1;
                v59 = sub_4621c0();
                v59->field_480 = v59->field_480 | 1;
                *((unsigned int *)&v59->padding_0[32]) = 23;
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
                    v59->field_430 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v59->field_430 = nan;
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
                    v59->field_434 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v59->field_434 = nan;
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
                    v59->field_438 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v59->field_438 = nan;
                    /* unsupported instruction */
                }
                sub_454d10();
                v60 = *((int *)sub_461250(v59, v9));
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 - 1;
                }
                *((unsigned int *)&v31->padding_0[220 + 4 * v31->field_1a4]) = v60;
                iter2 = sub_461920(*((int *)&v31->padding_0[220 + 4 * v31->field_1a4]));
                if (!iter2)
                    *((unsigned int *)&v31->padding_0[220 + 4 * v31->field_1a4]) = 0;
                *((char *)&iter2[147].field_4 + 1) = 30;
                *((char *)&iter2[147].field_4) = 30;
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
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
LABEL_423c98:
            index = iter2;
LABEL_423c9a:
            sub_425790(8, 4);
            v61 = v30->field_64 - 0;
            if (!v30->field_64)
            {
                v66 = v31->field_1a4 * 3;
                *((unsigned int *)&v31->padding_0[260 + 4 * v66]) = index->field_430;
                v67 = &v31->padding_0[4 * v66 + 260];
                v67[1] = index->field_434;
                v67[2] = index->field_438;
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
                    *((int *)&v31->padding_0[380 + 4 * v31->field_1a4]) = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    *((unsigned int *)&v31->padding_0[380 + 4 * v31->field_1a4]) = nan;
                    /* unsupported instruction */
                }
                v4 = &v30->padding_18[4];
                v3 = 0;
                v2 = 0;
                v1 = 0;
                v0 = 0xffffff;
                sub_4608f0();
            }
            else if (v61 == 1)
            {
                v64 = v31->field_1a4 * 3;
                *((unsigned int *)&v31->padding_0[260 + 4 * v64]) = index->field_430;
                v65 = &v31->padding_0[4 * v64 + 260];
                v65[1] = index->field_434;
                v65[2] = index->field_438;
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
                /* unsupported instruction */
                /* unsupported instruction */
                v4 = &v30->padding_18[4];
                v3 = 0;
                *((int *)&v31->padding_0[260 + 12 * v31->field_1a4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = 0;
                *((int *)&v31->padding_0[380 + 4 * v31->field_1a4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v1 = 0;
                v0 = 0xffffff;
                sub_460760();
                v31 = v22;
                index->field_47c = index->field_47c & 0xffefffff | 0x80000;
            }
            else if (v61 == 2)
            {
                v62 = v31->field_1a4 * 3;
                *((unsigned int *)&v31->padding_0[260 + 4 * v62]) = index->field_430;
                v63 = &v31->padding_0[4 * v62 + 260];
                v63[1] = index->field_434;
                v63[2] = index->field_438;
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
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v31->padding_0[260 + 12 * v31->field_1a4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v31->padding_0[380 + 4 * v31->field_1a4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_460800(0xffffff, 0, 0, 0, &v30->padding_18[4]);
                index->field_47c = index->field_47c & 0xfff7ffff | 0x100000;
            }
            *((char [4])&index->field_3bc) = v30->field_84;
            *((char *)&index->field_3bc + 3) = 0;
            index->field_3fc = v30->field_6c;
            index->field_400 = v30->field_84[3];
            v31->field_1a4 = (v31->field_1a4 + 1) % 10;
            if (v30->field_10)
                v30->field_10->field_8 = v30->field_14;
            if (v30->field_14)
                v30->field_14->field_4 = v30->field_10;
            v30->field_10 = NULL;
            v30->field_14 = NULL;
            sub_46ca4f(v30);
            i = v6;
        }
    } while (i);
    return;
}
