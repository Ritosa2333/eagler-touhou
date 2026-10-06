// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4099e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4099e0_0 {
    char padding_0[304];
    unsigned int field_130;
} st_4099e0_0;

typedef struct st_4099e0_1 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4099e0_1;

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

extern unsigned int g_4b43c8;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_4099e0(void* index)
{
    unsigned int v11;  // ebx
    unsigned int v12;  // esi
    unsigned int v20;  // ftop
    int v21;  // eax
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ecx
    st_4099e0_0 *idx;  // edi
    st_4099e0_1 *idx1;  // esi
    unsigned int iter;  // ebx
    unsigned int v29;  // ecx
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    void* idx2;  // ebx, Other Possible Types: int, unsigned int
    void* v44;  // ebx
    int v45;  // eax
    void* v46;  // esi
    st_4099e0_0 *v47;  // edi
    int v48;  // edi
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    int v0;  // [bp-0x38]
    int v1;  // [bp-0x34], Other Possible Types: unsigned int
    int v2;  // [bp-0x2c], Other Possible Types: unsigned int
    unsigned int v3;  // [bp-0x24]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0x10]
    unsigned int v8;  // [bp-0xc]
    int v9;  // [bp-0x8], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x4]

    v4 = v11;
    v3 = v12;
    if (*((char *)index) & 8)
    {
        sub_409350(v1, idx2, v2, v48);
        return 0xffffffff;
    }
    v13 = (short)index[1330];
    v14 = v13 - 1;
    if (v13 == 1)
    {
        do
        {
LABEL_409c9a:
            sub_40aa60();
            if (!(int)index[1320])
                break;
            iter = 0;
            if ((char)(int)index[1320] & 1)
                iter = sub_40b670();
            if ((char)index[1320] & 4)
                iter += sub_40b6f0();
            if ((int)index[1320] & 0x20000000)
                iter += sub_40b800();
            if ((char)index[1320] & 8)
                iter += sub_40b920();
            if ((char)index[1320] & 16)
                iter += sub_40b9c0();
            if ((char)index[1320] & 64)
                iter += sub_40baf0();
            if ((char)index[1320] & 32)
                iter += sub_40bc20();
            if ((int)index[1320] & 0x100)
                iter += sub_40bef0();
            if ((int)index[1320] & 0x800000)
                iter += sub_40c170();
            if ((int)index[1320] & 0x2000000)
                iter += sub_40c230();
            if ((int)index[1320] & 0x8000000)
                iter += sub_40c3b0();
            if ((int)index[1320] & 0x400)
                iter += sub_40c480();
            if ((int)index[1320] & 0x1000)
            {
                if ((int)index[0x808] <= 0)
                {
                    *((unsigned int *)&index[1320]) = (int)index[1320] ^ 0x1000;
                    iter += 1;
                }
                else
                {
                    v41 = v20 - 1;
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
                    idx2 = v29;
                    if (/* unsupported instruction */)
                    {
                        idx2 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v20 = v42 + 1;
                    }
                    else
                    {
                        idx2 = (unsigned int)nan;
                        /* unsupported instruction */
                        v20 = v42 + 1;
                    }
                    sub_464a20();
                }
            }
            if ((int)index[4])
                *((unsigned int *)&index[4]) = (int)index[4] - 1;
        } while (iter);
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
        v44 = index + 1212;
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)v44) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v44[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v44[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (!((char)*((int *)index) & 2))
            goto LABEL_409f6e;
        if (!((char)*((int *)index) & 16))
        {
            v45 = sub_437810();
        }
        else
        {
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
            v1 = v29;
            if (/* unsupported instruction */)
            {
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
            }
            v45 = sub_437980();
        }
        if (v45 == 1)
        {
            v46 = index + 8;
            *((unsigned short *)&index[1330]) = 3;
            if ((int)v46[1172])
                (int)v46[1172]();
            *((unsigned short *)&v46[964]) = 1;
            v9 = (int)index[1316];
            if ((int)index[1316] >= 0)
            {
                v47 = *((int *)(g_4b43c8 + 5106652));
                if (g_4cee78 & 0x8000)
                {
                    EnterCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 + 1;
                }
                v47->field_130 = v47->field_130 + 1;
                idx1 = sub_4621c0();
                idx1->field_480 = idx1->field_480 | 1;
                idx1->field_20 = 23;
                if (!v44)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx1->field_430 = v5;
                    idx1->field_434 = v6;
                    idx1->field_438 = v7;
                    v0 = v9;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = v9;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx1->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx1->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx1->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
LABEL_409c33:
                sub_454d10(idx1);
                sub_461250(idx1);
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                    g_4cf221 = g_4cf221 - 1;
                }
            }
        }
        else
        {
            if (v45 == 2 && !(*((char *)index) & 4))
            {
                sub_4391c0(v44);
                *((unsigned int *)index) = *((int *)index) | 4;
            }
        }
    }
    else if (v14 == 1)
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
        idx2 = index + 1212;
        v18 = v17 - 1;
        if (/* unsupported instruction */)
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v19 = v18 - 1;
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
        *((int *)idx2) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&idx2[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v20 = v19 + 2;
        if ((int)index[1256] >= 8 && (char)*((int *)index) & 2)
        {
            if (!((char)*((int *)index) & 16))
            {
                v21 = sub_437810();
            }
            else
            {
                v22 = v20 - 1;
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
                v2 = v24;
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v20 = v23 + 1;
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                    v20 = v23 + 1;
                }
                v21 = sub_437980();
            }
            if (v21 != 1 && v21 == 2 && !(*((char *)index) & 4))
            {
                sub_4391c0(idx2);
                *((unsigned int *)index) = *((int *)index) | 4;
                goto LABEL_409c81;
            }
            *((unsigned short *)&index[1330]) = 3;
            sub_40d6e0();
            idx2 = (int)index[1316];
            if ((int)index[1316] < 0)
                goto LABEL_409f6e;
            idx = *((int *)(g_4b43c8 + 5106652));
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            idx->field_130 = idx->field_130 + 1;
            idx1 = sub_4621c0();
            idx1->field_480 = idx1->field_480 | 1;
            idx1->field_20 = 23;
            if (!idx2)
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
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx1->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx1->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx1->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            goto LABEL_409c33;
        }
LABEL_409c81:
        if (!(int)index[0x404])
            goto LABEL_409f6e;
        *((unsigned short *)&index[1330]) = 1;
        goto LABEL_409c9a;
    }
    else if (v14 == 2)
    {
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
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[1212]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[1216]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[1220]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
LABEL_409f6e:
    if ((int)index[0x3fc])
    {
        if ((int)index[1320] & 0x20000)
            sub_40c000();
        if ((int)index[1320] & 0x40000)
            sub_40c0b0();
        if (!((int)index[1320] & 0x400) && (int)index[1312] < 1)
        {
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
                idx2 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                idx2 = nan;
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
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
            }
            if (sub_409900())
            {
                sub_409350(v1, idx2, v2, v48);
                return 0xffffffff;
            }
        }
    }
    if ((int)index[4])
        *((unsigned int *)&index[4]) = (int)index[4] - 1;
    if ((int)index[1312] > 0)
        *((unsigned int *)&index[1312]) = (int)index[1312] - 1;
    if (!sub_455630(index + 8))
        return 0;
    sub_409350(v1, idx2, v2, v48);
    return 0xffffffff;
}
