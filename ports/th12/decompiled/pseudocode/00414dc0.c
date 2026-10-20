// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x414dc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_414dc0_6 {
    char padding_0[304];
    unsigned int field_130;
} st_414dc0_6;

typedef struct st_414dc0_1 {
    struct st_414dc0_0 *field_0;
    struct st_414dc0_1 *field_4;
} st_414dc0_1;

typedef struct st_414dc0_0 {
    char padding_0[4724];
    unsigned int field_1274;
    char padding_1278[5188];
    int field_26bc;
    unsigned int field_26c0;
    char padding_26c4[52];
    unsigned int field_26f8;
} st_414dc0_0;

typedef struct st_414dc0_2 {
    char padding_0[104];
    struct st_414dc0_1 *field_68;
} st_414dc0_2;

typedef struct st_414dc0_3 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_414dc0_3;

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

extern st_414dc0_2 *g_4b43dc;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

void sub_414dc0(unsigned int a0)
{
    st_414dc0_1 *i;  // eax
    unsigned int v9;  // ebx
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    int v24;  // edi
    int v25;  // esi
    unsigned int v10;  // ftop
    st_414dc0_1 *v11;  // ecx
    st_414dc0_0 *index;  // edi
    unsigned int v13;  // eax
    st_414dc0_6 *idx;  // esi
    st_414dc0_3 *idx1;  // ebx
    unsigned int v16;  // ftop
    int v0;  // [bp-0x38]
    int v1;  // [bp-0x34]
    int v2;  // [bp-0x24], Other Possible Types: unsigned int
    st_414dc0_1 *v3;  // [bp-0x20]
    st_414dc0_2 *v4;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x4]

    i = g_4b43dc->field_68;
    v2 = v9;
    v4 = g_4b43dc;
    if (i)
    {
        do
        {
            v11 = i->field_4;
            index = i->field_0;
            v13 = index->field_26f8;
            v3 = v11;
            if (!((char)v13 & 160) && !(v13 & 0x600400) || (i = v11, v13 & 0x100))
            {
                i = v11;
                if (index->field_1274 == v7)
                {
                    v2 = index->field_26bc;
                    if (index->field_26bc >= NULL)
                    {
                        idx = *((int *)&g_4b43dc->padding_0[64 + 4 * index->field_26c0]);
                        if (g_4cee78 & 0x8000)
                        {
                            EnterCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 + 1;
                        }
                        idx->field_130 = idx->field_130 + 1;
                        idx1 = sub_4621c0();
                        idx1->field_480 = idx1->field_480 | 1;
                        *((unsigned int *)&idx1->padding_0[32]) = 4;
                        if (index == 0xffffef8c)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            idx1->field_430 = v4;
                            idx1->field_434 = v5;
                            idx1->field_438 = v6;
                        }
                        else
                        {
                            v16 = v10 - 1;
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
                                v18 = v17 + 1;
                            }
                            else
                            {
                                idx1->field_430 = nan;
                                /* unsupported instruction */
                                v18 = v17 + 1;
                            }
                            v19 = v18 - 1;
                            if (/* unsupported instruction */)
                            {
                                v20 = v19 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v20 = v19 - 1;
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
                                v21 = v20 + 1;
                            }
                            else
                            {
                                idx1->field_434 = nan;
                                /* unsupported instruction */
                                v21 = v20 + 1;
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
                            if (/* unsupported instruction */)
                            {
                                idx1->field_438 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v10 = v23 + 1;
                            }
                            else
                            {
                                idx1->field_438 = nan;
                                /* unsupported instruction */
                                v10 = v23 + 1;
                            }
                        }
                        sub_454d10(idx1, v2);
                        sub_461250(idx1, v2);
                        if (g_4cee78 & 0x8000)
                        {
                            LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 - 1;
                        }
                    }
                    index->field_26f8 = index->field_26f8 | 0x1000000;
                    i = v3;
                }
            }
        } while (i);
    }
    sub_464a80(v0, v1, v24, v25);
    return;
}
