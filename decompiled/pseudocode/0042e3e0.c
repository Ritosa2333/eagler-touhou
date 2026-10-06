// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42e3e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42e3e0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[8];
    char field_14;
} st_42e3e0_0;

typedef struct st_42e3e0_2 {
    char padding_0[304];
    unsigned int field_130;
} st_42e3e0_2;

typedef struct st_42e3e0_1 {
    char padding_0[12];
    unsigned int field_c;
    char padding_10[1084];
    unsigned int field_44c;
    char padding_450[30];
    short field_46e;
    int field_470;
    char padding_474[2856];
    struct st_42e3e0_0 *field_f9c;
} st_42e3e0_1;

typedef struct st_42e3e0_4 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42e3e0_4;

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

typedef struct st_42e3e0_3 {
    char padding_0[1160];
    struct st_42e3e0_2 *field_488;
} st_42e3e0_3;

extern st_42e3e0_3 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_42e3e0(st_42e3e0_1 *a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v11;  // esi
    st_42e3e0_1 *index;  // esi
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v25;  // 4165
    unsigned int v27;  // ftop
    unsigned int v29;  // 4165
    unsigned int i;  // ecx
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    st_42e3e0_2 *idx;  // esi
    int v38;  // edi
    st_42e3e0_4 *v39;  // eax
    unsigned int v40;  // ftop
    st_42e3e0_0 *iter;  // ebp
    unsigned int v41;  // ftop
    st_42e3e0_4 *idx1;  // ebx
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // edx
    unsigned int v19;  // eax
    unsigned int v20;  // ftop
    unsigned int v0;  // [bp-0x4c]
    char *v1;  // [bp-0x48]
    unsigned int v2;  // [bp-0x44]
    unsigned int v3;  // [bp-0x40]
    st_42e3e0_1 *v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x18]
    st_42e3e0_1 *v9;  // [bp-0x14]
    unsigned int v10;  // [bp-0x10]

    v8 = v11;
    index = a0;
    v9 = index;
    if (a3 && index->field_44c)
        return 0;
    i = 0;
    iter = index->field_f9c;
    a3 = 0;
    if (index->field_470 > 0)
    {
        v16 = v15 - 1;
        if (/* unsupported instruction */)
        {
            v17 = v16 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v17 = v16 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        do
        {
            v5 = iter->field_0;
            v7 = iter->field_8;
            v6 = iter->field_4;
            v18 = ((1431655766 * i >> 63) + (unsigned int)(1431655766 * i >> 32)) * 3;
            v19 = i;
            if (v19 != v18)
                continue;
            v20 = v17 - 1;
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
            if (v9 != v19 - v18)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v25 = _ccall(10, 13, (char)((((unsigned short)v23 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v25 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v27 = v23 + 1;
                    if ((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v29 = _ccall(10, 13, (char)((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v29 & 1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v31 = v27 + 2;
                            if ((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                            {
                                v32 = v31 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v33 = v32 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v33 = v32 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                if (/* unsupported instruction */)
                                {
                                    v3 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v34 = v33 + 1;
                                }
                                else
                                {
                                    v3 = nan;
                                    /* unsupported instruction */
                                    v34 = v33 + 1;
                                }
                                v35 = v34 - 1;
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
                                    v2 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v31 = v36 + 1;
                                }
                                else
                                {
                                    v2 = nan;
                                    /* unsupported instruction */
                                    v31 = v36 + 1;
                                }
                                v1 = &v5;
                                v0 = 9;
                                sub_4273f0();
                                goto LABEL_42e4c7;
                            }
                        }
                    }
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v27 = v23 + 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v31 = v27 + 2;
LABEL_42e4c7:
            idx = g_4b44f4->field_488;
            v38 = index->field_46e * 2 + 4;
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            idx->field_130 = idx->field_130 + 1;
            v39 = sub_4621c0();
            v40 = v31 - 1;
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
            idx1 = v39;
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
                idx1->field_430 = /* unsupported instruction */;
                /* unsupported instruction */
                v43 = v41 + 1;
            }
            else
            {
                idx1->field_430 = nan;
                /* unsupported instruction */
                v43 = v41 + 1;
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
                v46 = v45 + 1;
            }
            else
            {
                idx1->field_434 = nan;
                /* unsupported instruction */
                v46 = v45 + 1;
            }
            v47 = v46 - 1;
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
            if (/* unsupported instruction */)
            {
                idx1->field_438 = /* unsupported instruction */;
                /* unsupported instruction */
                v49 = v48 + 1;
            }
            else
            {
                idx1->field_438 = nan;
                /* unsupported instruction */
                v49 = v48 + 1;
            }
            sub_454d10();
            sub_461250(idx1, v38);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
            }
            v50 = v49 - 1;
            if (/* unsupported instruction */)
            {
                v17 = v50 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v17 = v50 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            index = v4;
            i = v10;
            i += 1;
            iter = &iter->field_14;
            v10 = i;
        } while (i < index->field_470);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    index->field_c = 1;
    return 0;
}
