// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42bd90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42bd90_2 {
    char padding_0[304];
    unsigned int field_130;
} st_42bd90_2;

typedef struct st_42bd90_0 {
    char padding_0[1100];
    unsigned int field_44c;
} st_42bd90_0;

typedef struct st_42bd90_4 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42bd90_4;

typedef struct st_42bd90_1 {
    char padding_0[1190];
    short field_4a6;
} st_42bd90_1;

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

typedef struct st_42bd90_3 {
    char padding_0[1160];
    struct st_42bd90_2 *field_488;
} st_42bd90_3;

extern st_42bd90_3 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_42bd90(st_42bd90_0 *a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    unsigned int v19;  // ebx
    unsigned int v20;  // esi
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // 4199
    unsigned int v33;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // 4192
    unsigned int v21;  // edi
    unsigned int v39;  // ftop
    st_42bd90_2 *index;  // ebx
    int v41;  // edi
    st_42bd90_4 *idx;  // esi
    unsigned int v43;  // ftop
    unsigned int v46;  // ftop
    unsigned int v22;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // 4179
    unsigned int v23;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // 4629
    st_42bd90_1 *v27;  // edx
    st_42bd90_1 *v0;  // [bp-0x74]
    st_42bd90_1 *v1;  // [bp-0x64]
    unsigned int v2;  // [bp-0x60]
    unsigned int v3;  // [bp-0x54], Other Possible Types: unsigned long
    unsigned int v4;  // [bp-0x50]
    unsigned int v5;  // [bp-0x4c], Other Possible Types: unsigned long
    unsigned int v6;  // [bp-0x44]
    unsigned int v7;  // [bp-0x40]
    st_42bd90_0 *v8;  // [bp-0x3c], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x38]
    unsigned int v10;  // [bp-0x34]
    unsigned int v11;  // [bp-0x30]
    unsigned int v12;  // [bp-0x2c]
    unsigned int v13;  // [bp-0x24]
    unsigned int v14;  // [bp-0x20]
    unsigned int v15;  // [bp-0x1c]
    unsigned int v16;  // [bp-0x18]
    unsigned int v17;  // [bp-0x14]
    unsigned int v18;  // [bp-0x10]

    v5 = v19;
    v4 = v20;
    v3 = v21;
    v8 = a0;
    if (a3 && a0->field_44c)
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v7 = 0;
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
    v13 = v10;
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
    v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = v11;
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
    v15 = v12;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
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
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v23 = v22 - 3;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v25 = v23 + 1;
    v26 = _ccall(10, 13, (char)((((unsigned short)v23 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
    if (v26 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        while (1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v2 += 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v29 = v25 - 1;
            v30 = v29 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v31 = _ccall(10, 13, (char)((((unsigned short)v29 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v31 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v33 = v30 + 1;
                if (!((char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                    goto LABEL_42c024;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v35 = v33;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v36 = v35 + 1;
                v37 = _ccall(10, 13, (char)((((unsigned short)v35 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (!(v37 & 1))
                    goto LABEL_42c026;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v39 = v36 + 1;
                if (!((char)((((unsigned short)v39 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                    goto LABEL_42c028;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                index = g_4b44f4->field_488;
                v41 = v27->field_4a6 * 2 + 4;
                if (g_4cee78 & 0x8000)
                {
                    EnterCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 + 1;
                }
                index->field_130 = index->field_130 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx = sub_4621c0();
                idx->field_480 = idx->field_480 | 1;
                idx->field_20 = 23;
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v43 = v39 + 1;
                sub_454d10(idx, v41);
                sub_461250(idx, v41);
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 - 1;
                }
                if (a2)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v3) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
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
                        v46 = v43 - 1;
                        if ((((unsigned short)v46 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v48 = v46 - 1;
                            v49 = _ccall(10, 13, (char)((((unsigned short)v48 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                            if (!(v49 & 1))
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v46 = v48 + 1;
                                goto LABEL_42c014;
                            }
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v43 = v48 + 2;
                            if ((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                sub_4273f0(9, &v6, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                v39 = v43 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v27 = v0;
                                goto LABEL_42c028;
                            }
                        }
                        else
                        {
LABEL_42c014:
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v43 = v46 + 1;
                        }
                    }
                }
                v39 = v43 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v27 = v1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v33 = v30 + 1;
LABEL_42c024:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v36 = v33 + 1;
LABEL_42c026:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v39 = v36 + 1;
            }
LABEL_42c028:
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
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            if ((char)((((unsigned short)v39 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                break;
            v25 = v39 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&v27->padding_0[12]) = 1;
    return v2;
}
