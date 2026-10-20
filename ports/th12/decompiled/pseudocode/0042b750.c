// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42b750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42b750_3 {
    struct st_42b750_1 *field_0;
    struct st_42b750_3 *field_4;
    struct st_42b750_3 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_42b750_3;

typedef struct st_42b750_4 {
    char padding_0[304];
    unsigned int field_130;
} st_42b750_4;

typedef struct st_42b750_2 {
    unsigned int field_0;
} st_42b750_2;

typedef struct st_42b750_1 {
    char padding_0[4];
    struct st_42b750_2 *field_4;
} st_42b750_1;

typedef struct st_42b750_9 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_42b750_9;

typedef struct st_42b750_5 {
    char padding_0[1124];
    struct st_42b750_3 *field_464;
    int field_468;
    int field_46c;
    char padding_470[24];
    struct st_42b750_4 *field_488;
} st_42b750_5;

typedef struct st_42b750_0 {
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
} st_42b750_0;

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

extern st_42b750_5 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_42b750(st_42b750_0 *a0, unsigned int a1, unsigned int a2, unsigned int a3, char a4, unsigned int a5)
{
    unsigned int v33;  // ebx
    st_42b750_0 *v34;  // esi
    unsigned int v43;  // ftop
    unsigned int v45;  // 4165
    unsigned int v48;  // 4165
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    st_42b750_0 *v35;  // edi
    unsigned int v53;  // 4246
    unsigned int node;  // ftop
    unsigned int v56;  // 4175
    unsigned int v59;  // 4165
    st_42b750_4 *idx;  // esi
    st_42b750_9 *v62;  // eax
    int iter;  // edi
    int v63;  // edx
    st_42b750_9 *index;  // ebx
    unsigned int v66;  // ftop
    int iter1;  // esi
    unsigned int v68;  // eax
    unsigned int v69;  // ftop
    unsigned int v70;  // eax
    unsigned int v72;  // ftop
    st_42b750_0 *idx1;  // ebx
    unsigned int v73;  // 4557
    unsigned int v76;  // 4179
    unsigned int v78;  // edi
    unsigned int v79;  // ebx
    st_42b750_3 *idx2;  // eax
    st_42b750_3 *v82;  // ecx
    unsigned int v38;  // eax
    unsigned int v83;  // eax
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v0;  // [bp-0x374]
    unsigned int v1;  // [bp-0x370]
    int v2;  // [bp-0x36c], Other Possible Types: unsigned int
    char *v3;  // [bp-0x368], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x364]
    unsigned int v5;  // [bp-0x360]
    st_42b750_0 *v6;  // [bp-0x35c]
    st_42b750_0 *v7;  // [bp-0x358], Other Possible Types: int, unsigned int
    unsigned int v8;  // [bp-0x354]
    unsigned int v9;  // [bp-0x350]
    unsigned int v10;  // [bp-0x34c]
    st_42b750_5 *v11;  // [bp-0x348], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x33c], Other Possible Types: unsigned long
    st_42b750_0 *v13;  // [bp-0x338], Other Possible Types: unsigned int
    unsigned int v14;  // [bp-0x334]
    unsigned int v15;  // [bp-0x330]
    unsigned int v16;  // [bp-0x32c]
    unsigned int v17;  // [bp-0x328]
    unsigned int v18;  // [bp-0x324]
    char v19;  // [bp-0x318]
    char v20;  // [bp-0x318]
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
    unsigned short v84;  // [bp-0x12c]
    char v32;  // [bp-0x10c]

    v8 = v33;
    v7 = v34;
    v6 = v35;
    iter = 0;
    idx1 = a0;
    v13 = idx1;
    if (a5 && idx1->field_44c)
        return v38;
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v25 = idx1->field_50;
    v3 = &v32;
    v12 = 0;
    v26 = idx1->field_54;
    v27 = idx1->field_58;
    sub_477420(&v32, 0, 0x100);
    /* unsupported instruction */
    /* unsupported instruction */
    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v9 = v16;
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
    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = v17;
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
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = v18;
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
    /* unsupported instruction */
    a3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)((((unsigned short)v40 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4030000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v41 = v40 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        while (1)
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v43 = v41 - 3;
            if ((char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                break;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            node = v43 + 2;
LABEL_42baa5:
            /* unsupported instruction */
            /* unsupported instruction */
            iter += 1;
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v66 = node + 1;
            if ((char)((((unsigned short)v66 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                goto LABEL_42baf2;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v41 = v66 - 0;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v6 = &v6->padding_0[1];
        *((char *)&v84 + iter) = 1;
        if (a4 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v45 = _ccall(10, 13, (char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v45 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v48 = _ccall(10, 13, (char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (v48 & 1)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        if ((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
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
                            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v51 = v43 + 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v52 = v51 + 1;
                            v53 = _ccall(10, 13, (char)((((unsigned short)v51 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                            if (!(v53 & 1))
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
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                sub_4273f0(9, &v3, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v52 -= 0;
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
                                goto LABEL_42b981;
                            }
                        }
                    }
                }
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v52 = v43 + 2;
LABEL_42b981:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        node = v52;
        v56 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
        if (v56 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if ((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v59 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v59 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
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
                        idx = g_4b44f4->field_488;
                        v4 = idx1->field_4a6 * 2 + 4;
                        if (g_4cee78 & 0x8000)
                        {
                            EnterCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 + 1;
                        }
                        idx->field_130 = idx->field_130 + 1;
                        v62 = sub_4621c0();
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v63 = idx1->field_4a6 * 2 + 4;
                        index = v62;
                        index->field_480 = index->field_480 | 1;
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
                        sub_454d10(index, v63);
                        sub_461250(index, v63);
                        if (g_4cee78 & 0x8000)
                        {
                            LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 - 1;
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx1 = v7;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        node -= 0;
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
                        goto LABEL_42baa5;
                    }
                }
            }
        }
LABEL_42baf2:
        v2 = iter;
        if (v6)
        {
            iter1 = 0;
            if (iter > 0)
            {
                do
                {
                } while (*((char *)&v84 + iter1) && (iter1 = (int)(iter1 + 1), iter1 < iter));
                if (!iter1)
                    goto LABEL_42bb2e;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v66 -= 0;
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_42bb6f;
            }
LABEL_42bb2e:
            v68 = 0;
            if (iter <= 0)
                goto LABEL_42bb44;
            while (!*((char *)&v84 + iter1))
            {
                v68 += 1;
                if (iter1 + 1 >= iter)
                    goto LABEL_42bb44;
            }
            v1 = v68;
            if (iter1 >= iter)
                goto LABEL_42bb44;
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_42bb6f:
            idx1->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v69 = v66 + 1;
LABEL_42bb72:
            v20 = v19;
            if (iter1 < iter)
            {
                do
                {
                    if ((&v31)[iter1])
                    {
                        iter1 += 1;
                        goto LABEL_42bb72;
                    }
                    else
                    {
                        if (iter1 >= iter)
                            break;
                        v70 = 0;
                        v7 = iter1;
                        do
                        {
                        } while (!(&v31)[iter1] && (iter1 = (int)(iter1 + 1), v70 += 1, iter1 < iter));
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v0 = v70;
                        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
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
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v72 = v69 - 1;
                        v73 = _ccall(10, 13, (char)((((unsigned short)v72 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v73 & 1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v69 = v72 + 1;
                            if (!((char)((((unsigned short)v69 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                                continue;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v72 = v69 - 1;
                            v76 = _ccall(10, 13, (char)((((unsigned short)v72 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                            if (v76 & 1)
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v69 = v72 + 1;
                                if ((((unsigned short)v69 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                                {
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
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
                                    v20 = v2;
                                    v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    v28 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    v29 = idx1->field_4a4;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v30 = idx1->field_4a6;
                                    v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v27 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v78 = &g_4b44f4->field_468;
                                    v11 = g_4b44f4;
                                    v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    if (*((int *)v78) < 0x100)
                                    {
                                        g_4b44f4->field_46c = g_4b44f4->field_46c + 1;
                                        v79 = &g_4b44f4->field_46c;
                                        if (*((int *)v79) < 0x10000)
                                            *((int *)v79) = 0x10000;
                                        idx2 = (!sub_46c9ea(4004) ? NULL : sub_428520());
                                        idx2->field_80 = *((int *)v79);
                                        v82 = v11->field_464;
                                        idx2->field_4 = v82;
                                        v82->field_8 = idx2;
                                        *((unsigned int *)v78) = *((int *)v78) + 1;
                                        v11->field_464 = idx2;
                                        idx2->field_0->field_4(&v20);
                                    }
                                    v69 -= 0;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    iter = v1;
                                    idx1 = v6;
                                    continue;
                                }
                            }
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v69 = v72 + 1;
                    }
                } while (iter1 < iter);
            }
        }
        else
        {
LABEL_42bb44:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    _security_check_cookie();
    return v83;
}
