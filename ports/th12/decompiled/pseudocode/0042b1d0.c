// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42b1d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42b1d0_3 {
    struct st_42b1d0_1 *field_0;
    struct st_42b1d0_3 *field_4;
    struct st_42b1d0_3 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_42b1d0_3;

typedef struct st_42b1d0_4 {
    char padding_0[304];
    unsigned int field_130;
} st_42b1d0_4;

typedef struct st_42b1d0_2 {
    unsigned int field_0;
} st_42b1d0_2;

typedef struct st_42b1d0_1 {
    char padding_0[4];
    struct st_42b1d0_2 *field_4;
} st_42b1d0_1;

typedef struct st_42b1d0_9 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42b1d0_9;

typedef struct st_42b1d0_5 {
    char padding_0[1124];
    struct st_42b1d0_3 *field_464;
    int field_468;
    int field_46c;
    char padding_470[24];
    struct st_42b1d0_4 *field_488;
} st_42b1d0_5;

typedef struct st_42b1d0_0 {
    char padding_0[80];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[16];
    unsigned int field_6c;
    char padding_70[988];
    unsigned int field_44c;
    char padding_450[84];
    unsigned short field_4a4;
    unsigned short field_4a6;
} st_42b1d0_0;

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

extern st_42b1d0_5 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_42b1d0(st_42b1d0_0 *idx, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    unsigned int v33;  // esi
    int v34;  // ebx
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // 4162
    unsigned int v49;  // ftop
    unsigned int v50;  // 4153
    unsigned int v51;  // ftop
    unsigned int v35;  // edi
    unsigned int *v53;  // ecx
    st_42b1d0_4 *idx1;  // esi
    st_42b1d0_9 *index;  // ebx
    int v56;  // eax
    unsigned int v58;  // ftop
    int iter;  // esi
    unsigned int v60;  // eax
    unsigned int v61;  // eax
    unsigned int v62;  // ecx
    unsigned int v36;  // eax
    unsigned int v63;  // ebx
    st_42b1d0_3 *idx2;  // eax
    st_42b1d0_3 *v66;  // ecx
    int v67;  // ebx
    unsigned int v68;  // eax
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v0;  // [bp-0x374]
    st_42b1d0_5 *v1;  // [bp-0x370], Other Possible Types: int, unsigned int
    unsigned int v2;  // [bp-0x36c]
    unsigned int v3;  // [bp-0x368]
    unsigned int v4;  // [bp-0x364]
    unsigned int v5;  // [bp-0x360]
    unsigned int v6;  // [bp-0x35c]
    unsigned int v7;  // [bp-0x358]
    unsigned int v8;  // [bp-0x350]
    int v9;  // [bp-0x34c], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x348]
    unsigned int v11;  // [bp-0x344]
    int v12;  // [bp-0x340]
    unsigned int v13;  // [bp-0x33c]
    unsigned int v14;  // [bp-0x338]
    unsigned int v15;  // [bp-0x330]
    unsigned int v16;  // [bp-0x32c]
    unsigned int v17;  // [bp-0x328]
    unsigned int v18;  // [bp-0x320]
    unsigned int v19;  // [bp-0x31c]
    unsigned int v20;  // [bp-0x318]
    unsigned int v21;  // [bp-0x314]
    unsigned int v22;  // [bp-0x310]
    unsigned int v23;  // [bp-0x30c]
    unsigned int v24;  // [bp-0x308]
    unsigned int v25;  // [bp-0x304]
    unsigned int v26;  // [bp-0x300]
    unsigned int v27;  // [bp-0x2fc]
    unsigned int v28;  // [bp-0x2f8]
    unsigned short v29;  // [bp-0x2f4]
    unsigned short v30;  // [bp-0x2f2]
    char v31;  // [bp-0x130]
    unsigned short v69;  // [bp-0x12c]
    char v32;  // [bp-0x10c]

    v7 = v33;
    v34 = 0;
    v6 = v35;
    v9 = a3;
    if (a5 && idx->field_44c)
        return v36;
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v19 = idx->field_50;
    v15 = 0;
    v16 = 0;
    v20 = idx->field_54;
    v21 = idx->field_58;
    sub_477420(&v32, 0, 0x100);
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
    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = v11;
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
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = v12;
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
    v10 = v13;
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
    v39 = v38;
    if (!((char)((((unsigned short)v39 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4030000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        while (1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v41 = v39 - 2;
            v42 = v41 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            if ((char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v44 = v42;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v45 = v44 + 1;
                v46 = _ccall(10, 13, (char)((((unsigned short)v44 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v46 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v49 = v45 + 1;
                        v50 = _ccall(10, 13, (char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                        if (v50 & 1)
                        {
                            v8 += 1;
                            *((char *)&v69 + v34) = 1;
                            if (a4)
                            {
                                /* unsupported instruction */
                                v51 = v49 + 1;
                                if (!sub_42e820((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    sub_4273f0(9, v53, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                }
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v51 = v49 + 1;
                            }
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            if (!sub_42e820((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
                            {
                                idx1 = g_4b44f4->field_488;
                                v4 = idx->field_4a6 * 2 + 4;
                                if (g_4cee78 & 0x8000)
                                {
                                    EnterCriticalSection(&g_4cf1d0.DebugInfo);
                                    g_4cf221 = g_4cf221 + 1;
                                }
                                idx1->field_130 = idx1->field_130 + 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                index = sub_4621c0();
                                index->field_480 = index->field_480 | 1;
                                v56 = idx->field_4a6 * 2 + 4;
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
                                sub_454d10(index, v56);
                                sub_461250(index, v56);
                                if (g_4cee78 & 0x8000)
                                {
                                    LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                                    g_4cf221 = g_4cf221 - 1;
                                }
                                v34 = v9;
                                goto LABEL_42b4c9;
                            }
                        }
                    }
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v45 = v42 + 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v49 = v45 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v51 = v49 + 1;
LABEL_42b4c9:
            /* unsupported instruction */
            /* unsupported instruction */
            v34 += 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v9 = v34;
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v58 = v51 - 1;
            if ((char)((((unsigned short)v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                break;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v39 = v58 + 1;
        }
        if (v8)
        {
            iter = 0;
            if (v34 > 0)
            {
                do
                {
                } while (*((char *)&v69 + iter) && (iter = (int)(iter + 1), iter < v34));
                if (!iter)
                    goto LABEL_42b544;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_42b583;
            }
LABEL_42b544:
            v60 = 0;
            if (v34 <= 0)
                goto LABEL_42b55a;
            while (!*((char *)&v69 + iter))
            {
                v60 += 1;
                if (iter + 1 >= v34)
                    goto LABEL_42b55a;
            }
            v1 = v60;
            if (iter >= v34)
                goto LABEL_42b55a;
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_42b583:
            idx->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            if (iter < v34)
            {
                while (1)
                {
                    if ((&v31)[iter])
                    {
                        iter += 1;
                        if (iter >= v34)
                            break;
                    }
                    else
                    {
                        if (iter >= v34)
                            break;
                        v61 = 0;
                        v1 = iter;
                        do
                        {
                        } while (!(&v31)[iter] && (iter = (int)(iter + 1), v61 += 1, iter < v34));
                        v0 = v61;
                        sub_477420(&v20, 0, 488);
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
                        v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
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
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v20 = v2;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v29 = idx->field_4a4;
                        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v30 = idx->field_4a6;
                        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v62 = &g_4b44f4->field_468;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v1 = g_4b44f4;
                        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v0 = &g_4b44f4->field_468;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v27 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        if (*((int *)v62) < 0x100)
                        {
                            g_4b44f4->field_46c = g_4b44f4->field_46c + 1;
                            v63 = &g_4b44f4->field_46c;
                            if (g_4b44f4->field_46c < 0x10000)
                                *((int *)v63) = 0x10000;
                            idx2 = (!sub_46c9ea(4004) ? NULL : sub_428520());
                            idx2->field_80 = *((int *)v63);
                            v66 = g_4b44f4->field_464;
                            idx2->field_4 = v66;
                            v66->field_8 = idx2;
                            *((unsigned int *)v62) = *((int *)v62) + 1;
                            g_4b44f4->field_464 = idx2;
                            idx2->field_0->field_4(&v20);
                            v34 = v67;
                        }
                        if (iter >= v34)
                            break;
                    }
                }
            }
        }
        else
        {
LABEL_42b55a:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    _security_check_cookie();
    return v68;
}
