// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x429bc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_429bc0_3 {
    struct st_429bc0_1 *field_0;
    struct st_429bc0_3 *field_4;
    struct st_429bc0_3 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_429bc0_3;

typedef struct st_429bc0_4 {
    char padding_0[304];
    unsigned int field_130;
} st_429bc0_4;

typedef struct st_429bc0_2 {
    unsigned int field_0;
} st_429bc0_2;

typedef struct st_429bc0_1 {
    char padding_0[4];
    struct st_429bc0_2 *field_4;
} st_429bc0_1;

typedef struct st_429bc0_0 {
    char padding_0[80];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[16];
    unsigned int field_6c;
    char padding_70[8];
    unsigned int field_78;
    char field_7c;
    char padding_7d[975];
    unsigned int field_44c;
    char padding_450[4];
    unsigned int field_454;
    char field_458;
    char padding_459[11];
    unsigned int field_464;
    char padding_468[18];
    short field_47a;
} st_429bc0_0;

typedef struct st_429bc0_9 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_429bc0_9;

typedef struct st_429bc0_5 {
    char padding_0[1124];
    struct st_429bc0_3 *field_464;
    int field_468;
    int field_46c;
    char padding_470[24];
    struct st_429bc0_4 *field_488;
} st_429bc0_5;

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

extern st_429bc0_5 *g_4b44f4;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

int sub_429bc0(st_429bc0_0 *a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    unsigned int v30;  // esi
    int v31;  // edi
    unsigned int v40;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // 4162
    unsigned int v47;  // ftop
    unsigned int v48;  // 4153
    unsigned int v49;  // ftop
    int v32;  // edi
    int v51;  // ecx
    st_429bc0_4 *idx;  // ebx
    st_429bc0_9 *index;  // esi
    unsigned int v55;  // ftop
    unsigned int v56;  // eax
    unsigned int v57;  // ftop
    int iter;  // ebx
    st_429bc0_0 *idx1;  // ecx
    st_429bc0_0 *idx2;  // ebx
    unsigned int v60;  // ftop
    unsigned int v62;  // ftop
    unsigned int v63;  // eax
    unsigned int v65;  // ftop
    unsigned int v66;  // 4291
    st_429bc0_5 *v67;  // edx
    unsigned int v68;  // eax
    unsigned int j;  // esi
    unsigned int v34;  // eax
    unsigned int v70;  // ecx
    unsigned int *node;  // edi
    unsigned int v73;  // ftop
    unsigned int v74;  // edi
    unsigned int v75;  // ftop
    unsigned int v76;  // esi
    st_429bc0_3 *v78;  // eax
    st_429bc0_3 *v79;  // ecx
    st_429bc0_0 *v80;  // ebx
    unsigned int v81;  // eax
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v39;  // ftop
    unsigned int v0;  // [bp-0x380]
    int v1;  // [bp-0x37c], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x378]
    unsigned int v3;  // [bp-0x374]
    unsigned int v4;  // [bp-0x370]
    unsigned int v5;  // [bp-0x36c]
    unsigned int v6;  // [bp-0x368]
    int v7;  // [bp-0x364]
    st_429bc0_5 *v8;  // [bp-0x360], Other Possible Types: int, unsigned int
    st_429bc0_0 *v9;  // [bp-0x358]
    int v10;  // [bp-0x354], Other Possible Types: unsigned int
    unsigned int v11;  // [bp-0x350]
    st_429bc0_0 *v12;  // [bp-0x34c]
    int v13;  // [bp-0x348]
    unsigned int v14;  // [bp-0x344]
    unsigned int v15;  // [bp-0x340]
    unsigned int v16;  // [bp-0x338]
    st_429bc0_0 *v17;  // [bp-0x330], Other Possible Types: unsigned int
    unsigned int v18;  // [bp-0x32c]
    unsigned int v19;  // [bp-0x328]
    unsigned int v20;  // [bp-0x320]
    unsigned int v21;  // [bp-0x31c]
    unsigned int v22;  // [bp-0x318]
    unsigned int v23;  // [bp-0x314]
    unsigned int v24;  // [bp-0x310]
    unsigned int v25;  // [bp-0x30c]
    unsigned int v26;  // [bp-0x304]
    unsigned int v27;  // [bp-0x300]
    char v28;  // [bp-0x138]
    int v82;  // [bp-0x134]
    char v29;  // [bp-0x10c]

    v8 = v30;
    v7 = v31;
    v32 = 0;
    idx2 = a0;
    v17 = idx2;
    v16 = a3;
    if (a5 && idx2->field_44c)
        return v34;
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v21 = idx2->field_50;
    v18 = 0;
    v22 = idx2->field_54;
    v23 = idx2->field_58;
    sub_477420(&v29, 0, 0x100);
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v27 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v11 = v14;
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
    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v37 = v36;
    if ((((unsigned short)v37 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4030000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
    {
        _security_check_cookie();
        return v81;
    }
    while (1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = v37 - 2;
        v40 = v39 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if ((char)((((unsigned short)v39 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v42 = v40;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v43 = v42 + 1;
            v44 = _ccall(10, 13, (char)((((unsigned short)v42 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v44 & 1)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if ((char)((((unsigned short)v43 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v47 = v43 + 1;
                    v48 = _ccall(10, 13, (char)((((unsigned short)v47 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (v48 & 1)
                    {
                        v10 += 1;
                        *((char *)&v82 + v32) = 1;
                        if (a4)
                        {
                            /* unsupported instruction */
                            v49 = v47 + 1;
                            if (!sub_42e820((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                sub_4273f0(9, v51, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            }
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v49 = v47 + 1;
                        }
                        idx = g_4b44f4->field_488;
                        idx2 = idx2->field_47a * 2 + 4;
                        if (g_4cee78 & 0x8000)
                        {
                            EnterCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 + 1;
                        }
                        idx->field_130 = idx->field_130 + 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        index = sub_4621c0();
                        index->field_480 = index->field_480 | 1;
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
                        sub_454d10(index, idx2);
                        sub_461250(index, idx2);
                        if (g_4cee78 & 0x8000)
                        {
                            LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                            g_4cf221 = g_4cf221 - 1;
                        }
                        v9 = idx2;
                        goto LABEL_429ea9;
                    }
                }
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v43 = v40 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v47 = v43 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v49 = v47 + 1;
LABEL_429ea9:
        /* unsupported instruction */
        /* unsupported instruction */
        v32 += 1;
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
        /* unsupported instruction */
        /* unsupported instruction */
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
        v55 = v49 - 4;
        if ((((unsigned short)v55 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
            break;
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
        v37 = v55 + 4;
    }
    v8 = v32;
    if (v10)
    {
        if (v10 >= v32)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx2->field_7c = 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            _security_check_cookie();
            return v56;
        }
        v57 = v55 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        iter = 0;
        if (v32 > 0)
        {
            do
            {
            } while (*((char *)&v82 + iter) && (iter = (int)(iter + 1), iter < v32));
            v1 = iter;
            if (!iter)
                goto LABEL_42a00b;
            /* unsupported instruction */
            /* unsupported instruction */
            idx1 = v9;
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
            /* unsupported instruction */
            /* unsupported instruction */
            idx1->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx1->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx1->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v60 = v57 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            idx1->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            if (!((char)((((unsigned short)v60 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                idx1->field_464 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                idx1->field_78 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v62 = v60 + 2;
LABEL_42a011:
                v63 = 0;
                if (iter < v32)
                {
                    do
                    {
                        if (*((char *)&v82 + iter))
                        {
                            v1 = v63;
                            if (iter < v32)
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
                                idx1->field_464 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                idx1->field_6c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                v66 = _ccall(10, 13, (char)((((unsigned short)v62 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                if (!(v66 & 1))
                                    idx1->field_7c = 1;
                                v67 = g_4b44f4;
                                while (iter < v32)
                                {
                                    if ((&v28)[iter])
                                    {
                                        iter += 1;
                                    }
                                    else
                                    {
                                        if (iter >= v32)
                                            break;
                                        v68 = 0;
                                        v8 = iter;
                                        do
                                        {
                                        } while (!(&v28)[iter] && (iter = (int)(iter + 1), v68 += 1, iter < v32));
                                        v0 = v68;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        j = &idx1->field_454;
                                        v70 = 122;
                                        for (node = &v20; v70; j += 4)
                                        {
                                            v70 -= 1;
                                            *(node) = *((int *)j);
                                            node += 1;
                                        }
                                        v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                        /* unsupported instruction */
                                        v73 = v65 + 1;
                                        if (!((char)((((unsigned short)v73 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v74 = &v67->field_468;
                                            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v8 = v67;
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
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                                            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v20 = v1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v21 = v2;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            v75 = v73 + 2;
                                            v22 = v3;
                                            if (*((int *)v74) < 0x100)
                                            {
                                                v67->field_46c = v67->field_46c + 1;
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                v76 = &v67->field_46c;
                                                if (v67->field_46c < 0x10000)
                                                    *((int *)v76) = 0x10000;
                                                v78 = (!sub_46c9ea(4004) ? NULL : sub_428520());
                                                v78->field_80 = *((int *)v76);
                                                v79 = v8->field_464;
                                                v78->field_4 = v79;
                                                v79->field_8 = v78;
                                                *((unsigned int *)v74) = *((int *)v74) + 1;
                                                v8->field_464 = v78;
                                                v78->field_0->field_4(&v20);
                                                v75 -= 0;
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                v67 = g_4b44f4;
                                            }
                                        }
                                        else
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v75 = v73 + 2;
                                        }
                                        if (iter >= v7)
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            _security_check_cookie();
                                            return v81;
                                        }
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v32 = v7;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        idx1 = v80;
                                        v65 = v75 - 3;
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
                                    }
                                }
                            }
                        }
                    } while ((iter = (int)(iter + 1), v63 += 1, iter < v32));
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx1->field_7c = 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        else
        {
LABEL_42a00b:
            idx1 = v9;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v62 = v57 + 1;
            goto LABEL_42a011;
        }
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    _security_check_cookie();
    return v81;
}
