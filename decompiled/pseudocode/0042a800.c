// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42a800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42a800_1 {
    char padding_0[304];
    unsigned int field_130;
} st_42a800_1;

typedef struct st_42a800_0 {
    char padding_0[12];
    unsigned int field_c;
    char padding_10[1084];
    unsigned int field_44c;
    char padding_450[42];
    short field_47a;
} st_42a800_0;

typedef struct st_42a800_3 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42a800_3;

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

typedef struct st_42a800_2 {
    char padding_0[1160];
    struct st_42a800_1 *field_488;
} st_42a800_2;

extern st_42a800_2 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_42a800(st_42a800_0 *a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v26;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ebx
    unsigned int v30;  // esi
    unsigned int v31;  // edi
    unsigned int v32;  // ebx
    st_42a800_1 *index;  // edi
    st_42a800_3 *v34;  // eax
    unsigned int v35;  // ftop
    unsigned int v18;  // ftop
    unsigned int v36;  // ftop
    st_42a800_3 *idx;  // esi
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v19;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // fc3210
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // ftop
    unsigned int v55;  // ftop
    unsigned int v20;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // 4165
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int v62;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // ftop
    unsigned int v65;  // ftop
    unsigned int v21;  // ftop
    unsigned int v66;  // ftop
    unsigned int v67;  // ftop
    unsigned int v68;  // ftop
    unsigned int v69;  // ftop
    unsigned int v70;  // ftop
    unsigned int v71;  // ftop
    unsigned int v72;  // ftop
    unsigned int v73;  // ftop
    unsigned int v74;  // ftop
    unsigned int v75;  // ftop
    unsigned int v22;  // ftop
    unsigned int v76;  // ftop
    unsigned int v77;  // ftop
    unsigned int v78;  // ftop
    unsigned int v79;  // ftop
    unsigned int v80;  // ftop
    unsigned int v81;  // fc3210
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v0;  // [bp-0x5c]
    unsigned int v1;  // [bp-0x58]
    unsigned int v2;  // [bp-0x44]
    unsigned int v3;  // [bp-0x40]
    unsigned long long v4;  // [bp-0x3c], Other Possible Types: unsigned long
    unsigned int v5[2];  // [bp-0x34], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x30]
    unsigned int v7;  // [bp-0x2c]
    unsigned int v8;  // [bp-0x28]
    unsigned int v9;  // [bp-0x24]
    unsigned int v10;  // [bp-0x20]
    unsigned int v11;  // [bp-0x1c]
    unsigned int v12;  // [bp-0x18]
    unsigned int v13;  // [bp-0x14]
    unsigned int v14;  // [bp-0x10]
    unsigned int v15;  // [bp-0xc]

    if (a3 && a0->field_44c)
        return 0;
    v17 = v16 - 1;
    if (/* unsupported instruction */)
    {
        v18 = v17 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v18 = v17 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
        a3 = /* unsupported instruction */;
    else
        a3 = nan;
    if (/* unsupported instruction */)
    {
        v5 = /* unsupported instruction */;
        /* unsupported instruction */
        v19 = v18 + 1;
    }
    else
    {
        v5 = nan;
        /* unsupported instruction */
        v19 = v18 + 1;
    }
    v7 = 0;
    v20 = v19 - 1;
    if (/* unsupported instruction */)
    {
        v21 = v20 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v21 = v20 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        (&v4)[1] = /* unsupported instruction */;
        /* unsupported instruction */
        v22 = v21 + 1;
    }
    else
    {
        (&v4)[1] = nan;
        /* unsupported instruction */
        v22 = v21 + 1;
    }
    sub_42e880();
    v23 = v22 - 1;
    if (/* unsupported instruction */)
    {
        v24 = v23 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v24 = v23 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v25 = v24 - 1;
    if (/* unsupported instruction */)
    {
        v26 = v25 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v26 = v25 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
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
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = v7;
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
    v11 = v8;
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
    v12 = v9;
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
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v28 = v26 + 2;
    if (!((char)((((unsigned short)v28 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4030000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        *((unsigned int *)&v4) = v29;
        v3 = v30;
        v2 = v31;
        do
        {
            v32 = a0->field_47a;
            index = g_4b44f4->field_488;
            *((unsigned int *)&v4) = (unsigned int)v4 + 1;
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            index->field_130 = index->field_130 + 1;
            v34 = sub_4621c0();
            v35 = v28 - 1;
            if (/* unsupported instruction */)
            {
                v36 = v35 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v36 = v35 - 1;
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
            idx = v34;
            idx->field_480 = idx->field_480 | 1;
            idx->field_20 = 23;
            v5 = (unsigned int (32 bits)[2])(/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v5 = v5;
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
                idx->field_430 = /* unsupported instruction */;
                /* unsupported instruction */
                v38 = v36 + 1;
            }
            else
            {
                idx->field_430 = nan;
                /* unsupported instruction */
                v38 = v36 + 1;
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
                idx->field_434 = /* unsupported instruction */;
                /* unsupported instruction */
                v41 = v40 + 1;
            }
            else
            {
                idx->field_434 = nan;
                /* unsupported instruction */
                v41 = v40 + 1;
            }
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
            if (/* unsupported instruction */)
            {
                idx->field_438 = /* unsupported instruction */;
                /* unsupported instruction */
                v44 = v43 + 1;
            }
            else
            {
                idx->field_438 = nan;
                /* unsupported instruction */
                v44 = v43 + 1;
            }
            sub_454d10();
            sub_461250(idx, v32 * 2 + 4);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
            }
            if (!v13)
                continue;
            v45 = v44 - 1;
            if (/* unsupported instruction */)
            {
                v46 = v45 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v46 = v45 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v47 = CmpF(/* unsupported instruction */, v4) * 0x100 & 0x4500;
                /* unsupported instruction */
                v44 = v46 + 1;
            }
            else
            {
                v47 = CmpF(nan, v4) * 0x100 & 0x4500;
                /* unsupported instruction */
                v44 = v46 + 1;
            }
            if (!((char)((((unsigned short)v44 & 7) * 0x800 | (unsigned short)v47 & 0x4700) >> 8) & 1))
                continue;
            v48 = v44 - 1;
            if (/* unsupported instruction */)
            {
                v49 = v48 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v49 = v48 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v50 = v49 - 1;
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v53 = v51 + 1;
            if ((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
            {
                v54 = v53 - 1;
                if (/* unsupported instruction */)
                {
                    v55 = v54 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v55 = v54 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v57 = v55;
                v58 = _ccall(10, 13, (char)((((unsigned short)v57 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v58 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v44 = v57 + 2;
                    if ((((unsigned short)v44 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                    {
                        v60 = v44 - 1;
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
                            v1 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v62 = v61 + 1;
                        }
                        else
                        {
                            v1 = nan;
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
                            v0 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v44 = v64 + 1;
                        }
                        else
                        {
                            v0 = nan;
                            /* unsupported instruction */
                            v44 = v64 + 1;
                        }
                        sub_4273f0(9, &v5[1], v0, v1);
                        goto LABEL_42a9fb;
                    }
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v53 = v57 + 1;
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v44 = v53 + 1;
LABEL_42a9fb:
            v65 = v44 - 1;
            if (/* unsupported instruction */)
            {
                v66 = v65 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v66 = v65 - 1;
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
                v6 = /* unsupported instruction */;
                /* unsupported instruction */
                v67 = v66 + 1;
            }
            else
            {
                v6 = nan;
                /* unsupported instruction */
                v67 = v66 + 1;
            }
            v68 = v67 - 1;
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
            if (/* unsupported instruction */)
            {
                v7 = /* unsupported instruction */;
                /* unsupported instruction */
                v70 = v69 + 1;
            }
            else
            {
                v7 = nan;
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
                v8 = /* unsupported instruction */;
                /* unsupported instruction */
                v73 = v72 + 1;
            }
            else
            {
                v8 = nan;
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
                v14 = /* unsupported instruction */;
                /* unsupported instruction */
                v76 = v75 + 1;
            }
            else
            {
                v14 = nan;
                /* unsupported instruction */
                v76 = v75 + 1;
            }
            v77 = v76 - 1;
            if (/* unsupported instruction */)
            {
                v78 = v77 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v78 = v77 - 1;
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
            v79 = v78 - 1;
            if (/* unsupported instruction */)
            {
                v80 = v79 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v80 = v79 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v81 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
            /* unsupported instruction */
            /* unsupported instruction */
            v28 = v80 + 2;
        } while (!((char)((((unsigned short)v28 & 7) * 0x800 | (unsigned short)v81 & 0x4700) >> 8) & 65));
    }
    a0->field_c = 1;
    return v2;
}
