// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40cf60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40cf60_8 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_40cf60_8;

typedef struct st_40cf60_1 {
    char padding_0[100];
    unsigned int field_64;
    char padding_68[1016];
    struct st_40cf60_0 *field_460;
    char padding_464[188];
    char field_520;
    char padding_521[117];
    unsigned short field_596;
    char padding_598[8];
    unsigned int field_5a0;
    char padding_5a4[1206];
    short field_a5a;
    char field_a5c;
} st_40cf60_1;

typedef struct st_40cf60_5 {
    char padding_0[304];
    unsigned int field_130;
} st_40cf60_5;

typedef struct st_40cf60_0 {
    char padding_0[56];
    unsigned int field_38;
} st_40cf60_0;

typedef struct st_40cf60_7 {
    char padding_0[956];
    struct st_40cf60_8 *field_3bc;
} st_40cf60_7;

typedef struct st_40cf60_6 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_40cf60_6;

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

typedef struct st_40cf60_4 {
    char padding_0[120];
    int field_78;
    char field_7c;
} st_40cf60_4;

extern unsigned int g_4b0bd0[4];
extern unsigned int g_4b0c10[4];
extern unsigned int g_4b0c30[4];
extern st_40cf60_1 *g_4b43c8;
extern st_40cf60_4 *g_4b43cc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_40cf60(unsigned int a0)
{
    unsigned int v16;  // ebx
    unsigned int v17;  // esi
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ebp
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    st_40cf60_1 *v18;  // esi
    unsigned int v36;  // 4139
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v19;  // edi
    unsigned int v47;  // 4139
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    st_40cf60_5 *idx;  // ebx
    st_40cf60_6 *idx1;  // esi
    unsigned int iter;  // edi
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int v62;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // esi
    st_40cf60_7 *v65;  // eax
    unsigned int v21;  // ftop
    st_40cf60_0 *v66;  // ecx
    unsigned int v67;  // ftop
    unsigned int v68;  // ftop
    unsigned int v69;  // fc3210
    unsigned int *v70;  // ecx
    unsigned int v71;  // ftop
    unsigned int v72;  // ftop
    unsigned int v73;  // fc3210
    unsigned int v74;  // eax
    unsigned int v75;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v0;  // [bp-0x6c]
    unsigned int v1;  // [bp-0x68]
    st_40cf60_1 *v2;  // [bp-0x5c]
    unsigned int v3;  // [bp-0x58]
    unsigned int v4;  // [bp-0x54]
    unsigned int v5;  // [bp-0x54]
    unsigned int v6;  // [bp-0x4c]
    unsigned int v7;  // [bp-0x48]
    st_40cf60_1 *v8;  // [bp-0x44], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x40]
    unsigned int v10;  // [bp-0x3c]
    unsigned int v11;  // [bp-0x38]
    unsigned int v12;  // [bp-0x24]
    unsigned int v13;  // [bp-0x20]
    unsigned int v14;  // [bp-0xc]
    unsigned int v15;  // [bp-0x8]

    v6 = v16;
    v5 = v17;
    v18 = g_4b43c8;
    v3 = v19;
    v8 = g_4b43c8;
    iter = &g_4b43c8->field_64;
    if (g_4b43cc->field_7c & 1 && g_4b43cc->field_78 >= 96 && g_4b43cc->field_78 <= 99)
        return 0;
    v22 = v21 - 1;
    if (/* unsupported instruction */)
    {
        v23 = v22 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v23 = v22 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v7 = 2000;
    v24 = v23 - 1;
    if (/* unsupported instruction */)
    {
        v25 = v24 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v25 = v24 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v26 = v25 + 1;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    do
    {
        v4 = v5;
        if (*((short *)(iter + 1330)) && *((short *)(iter + 1330)) != 3)
        {
            v27 = v26 - 1;
            if (/* unsupported instruction */)
            {
                v28 = v27 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v28 = v27 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v29 = iter + 1212;
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
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v26 = v28 + 1;
            if ((char)((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            {
                v31 = v26 - 1;
                if (/* unsupported instruction */)
                {
                    v32 = v31 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v32 = v31 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
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
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = v34 + 2;
                v36 = _ccall(10, 13, (char)((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v36 & 1)
                {
                    v37 = v26 - 1;
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
                    v39 = v38 - 1;
                    if (/* unsupported instruction */)
                    {
                        v40 = v39 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v40 = v39 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v40 + 2;
                    if ((char)((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                    {
                        v42 = v26 - 1;
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
                        v44 = v43 - 1;
                        if (/* unsupported instruction */)
                        {
                            v45 = v44 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v45 = v44 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v26 = v45 + 2;
                        v47 = _ccall(10, 13, (char)((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                        if (v47 & 1)
                        {
                            *((unsigned int *)iter) = *((int *)iter) | 8;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v48 = v26 + 1;
                            if (v15)
                            {
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
                                if (/* unsupported instruction */)
                                {
                                    v1 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v51 = v50 + 1;
                                }
                                else
                                {
                                    v1 = nan;
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
                                    v0 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v48 = v53 + 1;
                                }
                                else
                                {
                                    v0 = nan;
                                    /* unsupported instruction */
                                    v48 = v53 + 1;
                                }
                                sub_4273f0(9, v29, v0, v1);
                            }
                            idx = *((int *)&v18[1924].padding_5a4[836]);
                            if (g_4cee78 & 0x8000)
                            {
                                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                                g_4cf221 = g_4cf221 + 1;
                            }
                            idx->field_130 = idx->field_130 + 1;
                            idx1 = sub_4621c0();
                            idx1->field_480 = idx1->field_480 | 1;
                            idx1->field_20 = 23;
                            if (!v29)
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                idx1->field_430 = v7;
                                idx1->field_434 = v8;
                                idx1->field_438 = v9;
                            }
                            else
                            {
                                v56 = v48 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v57 = v56 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v57 = v56 - 1;
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
                                    v58 = v57 + 1;
                                }
                                else
                                {
                                    idx1->field_430 = nan;
                                    /* unsupported instruction */
                                    v58 = v57 + 1;
                                }
                                v59 = v58 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v60 = v59 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v60 = v59 - 1;
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
                                    v61 = v60 + 1;
                                }
                                else
                                {
                                    idx1->field_434 = nan;
                                    /* unsupported instruction */
                                    v61 = v60 + 1;
                                }
                                v62 = v61 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v63 = v62 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v63 = v62 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                if (/* unsupported instruction */)
                                {
                                    idx1->field_438 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v48 = v63 + 1;
                                }
                                else
                                {
                                    idx1->field_438 = nan;
                                    /* unsupported instruction */
                                    v48 = v63 + 1;
                                }
                            }
                            sub_454d10(idx1, 96);
                            v64 = *((int *)sub_461250(idx1, 96));
                            if (g_4cee78 & 0x8000)
                            {
                                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                                g_4cf221 = g_4cf221 - 1;
                            }
                            v65 = sub_461920(v64);
                            v66 = *((int *)(iter + 0x3fc));
                            if (v66)
                            {
                                v67 = v48 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v68 = v67 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v68 = v67 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                if (/* unsupported instruction */)
                                {
                                    v69 = CmpF(/* unsupported instruction */, v66->field_38) * 0x100 & 0x4500;
                                    /* unsupported instruction */
                                    v48 = v68 + 1;
                                }
                                else
                                {
                                    v69 = CmpF(nan, v66->field_38) * 0x100 & 0x4500;
                                    /* unsupported instruction */
                                    v48 = v68 + 1;
                                }
                                if (!((((unsigned short)v48 & 7) * 0x800 | (unsigned short)v69 & 0x4700) >> 8 & 1))
                                {
                                    v70 = g_4b0bd0[*((short *)(iter + 2550))];
                                }
                                else
                                {
                                    v71 = v48 - 1;
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
                                        v73 = CmpF(/* unsupported instruction */, v66->field_38) * 0x100 & 0x4500;
                                        /* unsupported instruction */
                                        v48 = v72 + 1;
                                    }
                                    else
                                    {
                                        v73 = CmpF(nan, v66->field_38) * 0x100 & 0x4500;
                                        /* unsupported instruction */
                                        v48 = v72 + 1;
                                    }
                                    v74 = *((short *)(iter + 2550));
                                    v70 = (!((char)((((unsigned short)v48 & 7) * 0x800 | (unsigned short)v73 & 0x4700) >> 8) & 1) ? g_4b0c10[v74] : g_4b0c30[v74]);
                                }
                                v65->field_3bc = v70;
                            }
                            v75 = v48 - 1;
                            if (/* unsupported instruction */)
                            {
                                v26 = v75 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v26 = v75 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            v18 = v2;
                            *((unsigned int *)(iter + 1340)) = 0;
                        }
                    }
                }
            }
        }
    } while ((iter += 2552, v5 = v4 - 1, v4 != 1));
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return 0;
}
