// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x413840; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CRITICAL_SECTION {
    struct CRITICAL_SECTION_DEBUG *DebugInfo;
    int LockCount;
    int RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    UIntPtr SpinCount;
} CRITICAL_SECTION;

typedef struct st_413840_0 {
    char padding_0[304];
    unsigned int field_130;
} st_413840_0;

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

typedef struct st_413840_2 {
    unsigned int field_0;
    unsigned int field_4;
} st_413840_2;

typedef struct LIST_ENTRY {
    struct LIST_ENTRY *Flink;
    struct LIST_ENTRY *Blink;
} LIST_ENTRY;

typedef struct st_413840_1 {
    char padding_0[4];
    struct st_413840_2 *field_4;
} st_413840_1;

typedef struct st_413840_13 {
    char padding_0[1072];
    int field_430;
    unsigned int field_434;
    unsigned int field_438;
} st_413840_13;

typedef struct st_413840_14 {
    char padding_0[960];
    unsigned int field_3c0;
    char padding_3c4[184];
    unsigned int field_47c;
} st_413840_14;

typedef struct st_413840_12 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[1024];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[64];
    unsigned int field_47c;
} st_413840_12;

typedef struct st_413840_11 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_413840_11;

typedef struct st_413840_15 {
    char padding_0[8];
    unsigned int field_8;
} st_413840_15;

typedef struct st_413840_8 {
    char padding_0[2428];
    int field_97c;
    char padding_980[168];
    unsigned int field_a28;
    char padding_a2c[47592];
    char field_c414;
} st_413840_8;

typedef struct st_413840_10 {
    char padding_0[27656];
    unsigned int field_6c08;
    char padding_6c0c[292];
    unsigned int field_6d30;
} st_413840_10;

typedef struct st_413840_9 {
    char padding_0[72];
    unsigned int field_48;
} st_413840_9;

typedef struct st_413840_6 {
    char padding_0[124];
    struct st_413840_1 *field_7c;
} st_413840_6;

typedef struct st_413840_7 {
    char padding_0[60];
    unsigned int field_3c;
} st_413840_7;

extern st_413840_7 *g_4b43c4;
extern st_413840_6 *g_4b43cc;
extern st_413840_9 *g_4b43dc;
extern st_413840_10 *g_4b43e4;
extern st_413840_8 *g_4b4514;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int sub_413840(void* a0)
{
    void* index;  // esi
    unsigned int v41;  // edi
    unsigned int v50;  // ftop
    unsigned int v135;  // 4129
    unsigned long long v136;  // 4229
    unsigned int v137;  // ftop
    unsigned int v138;  // ecx
    unsigned int *v139;  // eax, Other Possible Types: void*, unsigned int
    st_413840_15 *iter;  // edx, Other Possible Types: unsigned int
    st_413840_0 *idx;  // eax, Other Possible Types: unsigned int
    unsigned int v142;  // edx
    unsigned int *v143;  // ecx
    unsigned int v51;  // 4206
    unsigned short v145;  // ftop
    unsigned int v53;  // ftop
    unsigned int v55;  // ftop
    unsigned int v56;  // 4177
    unsigned int v58;  // ftop
    st_413840_1 *v59;  // ecx
    void* idx1;  // eax
    void* v60;  // esi
    void* idx2;  // eax
    int v62;  // eax
    int v63;  // esi
    void* node;  // edi
    st_413840_1 *v65;  // esi
    unsigned int v66;  // eax
    unsigned int v43;  // 4120
    unsigned int *v67;  // ecx
    void* v69;  // edi
    st_413840_1 *v70;  // esi
    unsigned int v71;  // eax
    unsigned int *v72;  // ecx
    unsigned int v73;  // eax
    unsigned int v74;  // edi
    unsigned int v76;  // 4148
    unsigned int v44;  // ftop
    unsigned int v77;  // eax
    unsigned int v79;  // ecx
    unsigned int *v80;  // eax
    unsigned int v81;  // eax
    st_413840_0 *v82;  // ebx
    st_413840_11 *v83;  // esi
    unsigned int v84;  // esi
    unsigned int v85;  // ftop
    unsigned int v86;  // ftop
    unsigned int iter1;  // ftop
    st_413840_12 *v87;  // edi
    unsigned int v88;  // esi
    unsigned int v89;  // eax
    unsigned int v90;  // ftop
    void* iter2;  // edi
    unsigned int v92;  // esi
    unsigned int v93;  // esi
    st_413840_13 *v94;  // eax
    unsigned int v46;  // ecx
    st_413840_14 *v96;  // eax
    unsigned int v97;  // ecx
    unsigned int v98;  // 4101
    st_413840_1 *v99;  // ecx
    int v100;  // eax
    st_413840_1 *v101;  // eax
    unsigned int v102;  // eax
    unsigned int v103;  // esi
    unsigned int v104;  // 4101
    unsigned int v105;  // esi
    void* v47;  // edi
    unsigned int v106;  // ecx
    unsigned int v107;  // ftop
    unsigned int v108;  // ftop
    unsigned int v110;  // ftop
    unsigned int v111;  // ftop
    unsigned int v113;  // ftop
    unsigned int v115;  // fpround
    void* v48;  // ebx
    unsigned int v116;  // 4143
    unsigned long long v117;  // 4203
    unsigned int v118;  // fpround
    unsigned int *v119;  // esi
    void* v120;  // esi
    unsigned int v121;  // ftop
    int v122;  // edi
    unsigned int v123;  // ftop
    st_413840_15 *v124;  // ecx
    unsigned int v125;  // ftop
    unsigned int v127;  // ftop
    unsigned int v128;  // 4685
    unsigned long long v129;  // 4716
    unsigned int v130;  // 4723
    unsigned long long v131;  // 4755
    unsigned int v132;  // 4762
    unsigned int v133;  // ftop
    unsigned long long v134;  // 4801
    unsigned int v0;  // [bp-0xf4]
    unsigned int v1;  // [bp-0xf0]
    int v2;  // [bp-0xec], Other Possible Types: unsigned int
    int v3;  // [bp-0xe8], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0xe4]
    st_413840_15 *v5;  // [bp-0xe0]
    char v6;  // [bp-0xd8]
    st_413840_15 *v7;  // [bp-0xd4], Other Possible Types: unsigned int, unsigned short
    unsigned int v8;  // [bp-0xd0]
    unsigned int v9;  // [bp-0xcc]
    st_413840_1 *v10;  // [bp-0xc4], Other Possible Types: unsigned int
    unsigned int v11;  // [bp-0xc0]
    unsigned int v12;  // [bp-0xbc]
    CRITICAL_SECTION *i;  // [bp-0xb8], Other Possible Types: int, unsigned int, unsigned short
    unsigned int *v14;  // [bp-0xb4], Other Possible Types: unsigned int
    unsigned int v15;  // [bp-0xb0]
    unsigned int *v16;  // [bp-0xac], Other Possible Types: unsigned long, unsigned int
    unsigned int v17;  // [bp-0xa8]
    unsigned int v18;  // [bp-0xa4]
    void* v19;  // [bp-0x9c], Other Possible Types: unsigned int
    unsigned int v20;  // [bp-0x94]
    unsigned int v21;  // [bp-0x90]
    st_413840_1 *v22;  // [bp-0x88], Other Possible Types: unsigned int
    unsigned int v23;  // [bp-0x80]
    unsigned int v24;  // [bp-0x7c]
    unsigned int v25;  // [bp-0x78]
    unsigned int v26;  // [bp-0x74]
    void* v27;  // [bp-0x70], Other Possible Types: unsigned int
    unsigned int v28;  // [bp-0x6c]
    void* v29;  // [bp-0x68]
    void* v30;  // [bp-0x64]
    void* v31;  // [bp-0x5c]
    unsigned int v32;  // [bp-0x58]
    unsigned int v33;  // [bp-0x54]
    unsigned int v34;  // [bp-0x50]
    unsigned int v35;  // [bp-0x4c]
    unsigned int v36;  // [bp-0x48]
    unsigned int v37;  // [bp-0x20]
    unsigned int v38;  // [bp-0x1c]
    unsigned int v39;  // [bp-0x18]

    index = a0;
    v28 = v41;
    if ((int)index[5816] & 0x20000)
        return 0;
    *((unsigned int *)&index[5816]) = (int)index[5816] | 0x20000;
    idx1 = index + 52;
    *((int *)index) = (int)index[52];
    *((int *)&index[4]) = (int)idx1[4];
    *((int *)&index[8]) = (int)idx1[8];
    *((int *)&index[12]) = (int)idx1[12];
    *((int *)&index[16]) = (int)idx1[16];
    *((int *)&index[20]) = (int)idx1[20];
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v31 = idx1;
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[28]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v43 = (int)index[856];
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[36]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[40]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index[44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    iter1 = v44;
    *((int *)&index[48]) = (int)v31[48];
    if (v43)
    {
        sub_41c020();
        if (!((char)index[152] & 3))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v27 = v46;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v26 = v46;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&index[132]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            iter1 += 2;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[128]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    if ((int)index[976])
    {
        sub_41c020();
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[0x88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[140]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    if ((int)index[916])
    {
        sub_41c020();
        if (!((char)index[0xcc] & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v25 = v46;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v24 = v46;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&index[184]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            iter1 += 2;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[180]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    if ((int)index[1036])
    {
        sub_41c020();
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[188]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&index[192]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    if ((int)index[720])
    {
        v29 = index + 104;
        sub_405900();
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v37 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v38 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&index[116]) = v37;
        *((unsigned int *)&index[120]) = v38;
        v39 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&index[124]) = v39;
    }
    else
    {
        v29 = index + 104;
        sub_464d50();
        index = a0;
    }
    if ((int)index[796])
    {
        v30 = index + 156;
        sub_405900();
        /* unsupported instruction */
        /* unsupported instruction */
        v47 = v30;
        /* unsupported instruction */
        /* unsupported instruction */
        v37 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v38 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&index[168]) = v37;
        *((unsigned int *)&index[172]) = v38;
        v39 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&index[176]) = v39;
    }
    else
    {
        v30 = index + 156;
        sub_464d50();
        index = a0;
        v47 = v30;
    }
    sub_464db0();
    if ((int)index[5816] & 0x2000000)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)v47) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v47[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v47[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    sub_464db0();
    sub_413700();
    v48 = a0;
    v27 = v48 + 224;
    if (!sub_461920((int)v48[224]))
    {
        *((int *)&v48[224]) = 0;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v48[5612]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&v48[5616]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    /* unsupported instruction */
    /* unsupported instruction */
    v50 = iter1 - 2;
    v51 = _ccall(10, 13, (char)((((unsigned short)v50 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
    if (v51 & 1)
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
        v53 = v50 + 1;
        if (!((char)((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            goto LABEL_413c00;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v55 = v53;
        v56 = _ccall(10, 13, (char)((((unsigned short)v55 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (v56 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v58 = v55 + 1;
            if (!((char)((((unsigned short)v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                goto LABEL_413be1;
            *((unsigned int *)&v48[5816]) = (int)v48[5816] | 0x8000;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v58 = v55 + 1;
LABEL_413be1:
            if ((int)v48[5816] & 0x8000 && !((char)(int)v48[5816] & 8))
                return 0xffffffff;
        }
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v53 = v50 + 1;
LABEL_413c00:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v58 = v53 + 1;
        if ((int)v48[5816] & 0x8000 && !((char)(int)v48[5816] & 4))
            return 0xffffffff;
    }
    v59 = (int)v48[5816];
    if (v59 & 0x8000000)
    {
        if (!g_4b43c4->field_3c)
        {
LABEL_413c23:
            if (v59 & 0x10000000)
            {
                idx = (int)v48[5824];
                *((st_413840_0 **)&v48[556]) = idx;
                sub_461f40(idx);
                *((unsigned int *)&v48[5816]) = (int)v48[5816] & 0xeffffffe;
            }
        }
        else if (!(v59 & 0x10000000))
        {
            idx = (int)v48[5820];
            *((st_413840_0 **)&v48[556]) = idx;
            sub_461f40(idx);
            *((unsigned int *)&v48[5816]) = (int)v48[5816] | 268435457;
        }
        else if (!g_4b43c4->field_3c)
        {
            goto LABEL_413c23;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v32 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v22 = v59;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (sub_468d90((/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
        return 0xffffffff;
    if ((int)v48[5992] && (int)v48[5992]())
        return 0xffffffff;
    if ((int)v48[5816] & 0x400000 && !((char)(int)v48[5816] & 32))
    {
        if ((char)g_4b43cc->field_7c & 1 && *((int *)&g_4b43cc->padding_0[120]) >= 103 && *((int *)&g_4b43cc->padding_0[120]) <= 112 && g_4b43c4->field_3c && !(g_4b4514->field_c414 & 4))
        {
            if (!sub_41c890())
            {
                iter = g_4b43dc->field_48;
                sub_4615a0(iter, &v31, 50, 0);
                *((unsigned int *)&v48[284]) = v28;
            }
            sub_461e30();
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v48[208]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            *((int *)&v48[212]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            v60 = v48 + 284;
            if (sub_41c890())
            {
                sub_461970(*((int *)v60));
                /* unsupported instruction */
                /* unsupported instruction */
                idx2 = a0;
                *((int *)&idx2[208]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((int *)v60) = 0;
                *((int *)&idx2[212]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v48 = idx2;
            }
        }
    }
    *((unsigned int *)&v48[5816]) = (int)v48[5816] & 0xffefffff;
    if (!((char)(int)v48[5816] & 33))
    {
        if ((int)v48[5996])
        {
            v62 = (int)v48[5996]();
        }
        else
        {
            v139 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v62 = sub_439ed0(v139, v48 + 208);
        }
        v63 = v62;
        if (g_4b4514->field_a28 == 2 || !g_4b4514->field_a28)
            v63 = ((unsigned int)((int)(1717986919 * v63 >> 33)) >> 31) + (int)(1717986919 * v63 >> 33);
        if (g_4b43e4->field_6d30)
            v63 = 0;
        if (g_4b43cc->field_7c & 0x1 && *((int *)&g_4b43cc->padding_0[120]) >= 93 && *((int *)&g_4b43cc->padding_0[120]) <= 99 && (g_4b43c4->field_3c || sub_412690()) && !(g_4b4514->field_c414 & 4))
            v63 = ((unsigned int)((int)(1717986919 * v63 >> 33)) >> 31) + (int)(1717986919 * v63 >> 33);
        if ((!(g_4b43cc->field_7c & 0x1) || *((int *)&g_4b43cc->padding_0[120]) < 103 || *((int *)&g_4b43cc->padding_0[120]) > 112 || !g_4b43c4->field_3c || !((int)v48[5816] & 0x400000) || g_4b4514->field_c414 & 4) && v63)
        {
            if (!((char)v48[5816] & 16) && (int)v48[5780] <= 0)
                sub_412050();
            node = sub_41a6f0();
            if (node)
            {
                v18 = 0;
                sub_4067e0(0);
                sub_411df0(0);
                sub_411e40();
                v65 = (int)v48[5964];
                v66 = sub_4694f0(node);
                /* unsupported instruction */
                /* unsupported instruction */
                v67 = &v65->field_4->field_0;
                v67[1] = v66;
                v65->field_4->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v16 = v67;
                /* unsupported instruction */
                /* unsupported instruction */
                v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if (sub_468d90((/* unsupported instruction */ ? /* unsupported instruction */ : nan)))
                    return 0xffffffff;
            }
            if ((char)((int)v48[5640] <= 0 & ~((unsigned int)((int)v48[5816]) >> 7)) & 1 && sub_414af0())
                return 1;
            *((unsigned int *)&v48[5816]) = (int)v48[5816] | 0x100000;
        }
    }
    if ((char)v48[5660] & 2 && sub_414af0())
        return 1;
    v69 = sub_41a6f0();
    if (v69)
    {
        v18 = 0;
        sub_4067e0(0);
        sub_411df0(0);
        sub_411e40();
        v70 = (int)v48[5964];
        v71 = sub_4694f0(v69);
        /* unsupported instruction */
        /* unsupported instruction */
        v72 = &v70->field_4->field_0;
        v72[1] = v71;
        v70->field_4->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        node = v69;
    }
    if (!((char)(int)v48[5816] & 0x22) && (int)v48[5800] <= 0 && !((int)v48[5816] & 0x2000000))
    {
        if ((int)v48[6000])
        {
            (int)v48[6000]();
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = v72;
            v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v73 = sub_437980((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            if ((int)v48[5816] & 0x200 && v73 == 2 && !(int)((int)v48[624] % 3))
            {
                v15 = &g_4b4514->field_97c;
                sub_4391c0(&g_4b4514->field_97c);
            }
        }
    }
    if ((int)v48[5816] & 0x80000)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v74 = 0;
        /* unsupported instruction */
        v76 = _ccall(10, 13, (char)((((unsigned short)v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xbf9eb851e0000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v76 & 1))
        {
            v77 = 0xffffffff;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v77 = (!((char)((((unsigned short)v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3f9eb851e0000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65) ? 1 : 0);
        }
        v79 = (int)v48[560];
        v20 = v77;
        if (v79 != v77)
        {
            if (v79 == 0xffffffff)
            {
                v74 = -(0 < v77) + 3;
            }
            else if (!v79)
            {
                v74 = (unsigned int)((v77 != 0xffffffff) + 1);
            }
            else if (v79 == 1)
            {
                v74 = (-(0 < v77) & 0xfffffffd) + 4;
            }
            v23 = *((int *)(sub_461c50() + 1016));
            v80 = sub_461c50();
            v81 = v80[267];
            v34 = v80[265];
            v35 = v80[266];
            *((unsigned int *)&v48[560]) = v77;
            v15 = *(v139);
            v36 = v81;
            sub_461a70(*(v139));
            *(v139) = 0;
            v82 = (int)v48[556];
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            idx->field_130 = idx->field_130 + 1;
            v83 = sub_4621c0();
            v83->field_480 = v83->field_480 | 1;
            v83->field_430 = v33;
            v14 = &v82->padding_0[v74];
            v83->field_434 = v34;
            v83->field_20 = 8;
            v83->field_438 = v35;
            sub_454df0(&v82->padding_0[v74]);
            v84 = *((int *)sub_461250(&v82->padding_0[v74]));
            if (g_4cee78 & 0x8000)
            {
                i = &g_4cf1d0.DebugInfo;
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
            }
            v48 = a0;
            *((unsigned int *)node) = v84;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v85 = v58 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((int)v48[5816] & 0x2000000))
    {
        v86 = v85 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v139 = v48 + 480;
        v19 = v48 + 296;
        v21 = 14;
        do
        {
            v87 = sub_461920(*((int *)((char *)node - 0x100)));
            if (!v87)
            {
                *((st_413840_12 **)((char *)node - 0x100)) = v87;
            }
            else
            {
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
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v88 = *(v16);
                v26 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if (v88 >= 0)
                {
                    if (!sub_461920(*((int *)((char *)v48 + 4 * v88 + 224))))
                        *((unsigned int *)((char *)v48 + 4 * v88 + 224)) = 0;
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
                    v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v87->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v87->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v87->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v89 = v87->field_47c;
                if (v87->field_47c & 0x20000000)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v87->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v87->field_47c = v89 | 4;
                }
            }
        } while ((v18 += 12, node += 4, iter -= 1, iter != 1));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v90 = v86 + 3;
    }
    else
    {
        iter2 = node;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v90 = v85 + 2;
        v92 = 14;
        do
        {
            v93 = v92;
            v94 = sub_461920(*((int *)iter2));
            if (v94)
            {
                v94->field_430 = *(v16);
                v94->field_434 = v16[1];
                v94->field_438 = v16[2];
            }
        } while ((iter2 += 4, v92 = v93 - 1, v93 != 1));
    }
    if (!((char)(int)v48[5816] & 33) && !((int)v48[5816] & 0x6000000))
    {
        if (*((int *)&g_4b4514->padding_a2c[32596]))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if ((char)((((unsigned short)v90 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                goto LABEL_4143b0;
        }
        sub_412660();
        g_4b4514->padding_a2c[32600] = 1;
    }
LABEL_4143b0:
    v96 = sub_461920(*((int *)v15));
    if (!v96)
    {
        *((unsigned int *)v15) = 0;
    }
    else if (!(int)v48[5764])
    {
        if ((int)v48[5816] & 0x40000000)
        {
            v97 = (int)v48[624];
            if ((v97 & 0x80000003) < 0)
            {
                v98 = _ccall(4, 18, ((v97 & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
                if (!(v98 & 1))
                    goto LABEL_414417;
LABEL_414405:
                v96->field_47c = v96->field_47c | 0x10000;
                v96->field_3c0 = 0xffff00ff;
            }
            else
            {
                if (!(v97 & 0x80000003))
                    goto LABEL_414405;
LABEL_414417:
                v96->field_47c = v96->field_47c & 0xfffeffff;
            }
        }
        v99 = (int)v48[5816];
        if (v99 & 0x100000)
        {
            v96->field_47c = v96->field_47c | 0x10000;
            v96->field_3c0 = 0xff0000ff;
            v100 = (int)v48[5772];
            *((unsigned int *)&v48[5764]) = 4;
            if (v100 < 0)
            {
                if ((int)v48[5816] & 0x20400000 && !(v99 = (st_413840_1 *)g_4b43cc->field_7c, v101 = v99, v102 = (unsigned int)(v101 & 1), v101 & 0x1 && *((char *)(void*)&v99) & 8))
                {
                    if (v102)
                    {
                        v99 = (int)v48[5964];
                        if (v99[1226].padding_0 < 200)
                            goto LABEL_4144a0;
                        if (!v102)
                            goto LABEL_41448e;
                    }
                    else
                    {
LABEL_41448e:
                        if (*((int *)((int)v48[5964] + 9808)) < 900)
                        {
LABEL_4144a0:
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v10 = v99;
                            /* unsupported instruction */
                            sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                            goto LABEL_41457e;
                        }
                    }
                }
                v10 = v99;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = v99;
                /* unsupported instruction */
                sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            }
        }
        else
        {
            v103 = (int)v48[624];
            if ((v103 & 0x80000003) < 0)
            {
                v104 = _ccall(4, 18, ((v103 & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
                if (!(v104 & 1))
                    goto LABEL_414553;
LABEL_4144fb:
                if (v99 & 0x20400000)
                {
                    v105 = g_4b43cc->field_7c & 1;
                    if (!(g_4b43cc->field_7c & 0x1) || !(*((char *)&g_4b43cc->field_7c) & 8))
                    {
                        if (v105)
                        {
                            if (*((int *)((int)v48[5964] + 9808)) < 100)
                                goto LABEL_414541;
                            if (v105)
                                goto LABEL_41457e;
                        }
                        if (*((int *)((int)v48[5964] + 9808)) < 500)
                        {
LABEL_414541:
                            v96->field_47c = v96->field_47c | 0x10000;
                            v96->field_3c0 = 0xff0000ff;
                        }
                    }
                }
            }
            else
            {
                if (!(v103 & 0x80000003))
                    goto LABEL_4144fb;
LABEL_414553:
                v96->field_47c = v96->field_47c & 0xfffeffff;
            }
        }
    }
    else
    {
        v96->field_47c = v96->field_47c & 0xfffeffff;
        if ((int)v48[5816] & 0x400000)
        {
            g_4b43e4->field_6c08 = g_4b43e4->field_6c08 & 0xfffeffff;
            *((unsigned int *)&v48[5764]) = (int)v48[5764] - 1;
        }
        else
        {
            *((unsigned int *)&v48[5764]) = (int)v48[5764] - 1;
        }
    }
LABEL_41457e:
    v106 = (int)v48[5968];
    if (v106)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v107 = v90;
        v18 = (int)v48[5984];
        v16 = *((int *)(v106 + 20));
        v139 = (int)v48[5988];
        if (*((int *)&g_4b43cc->padding_0[120]) == 111)
        {
            v108 = v107 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v108 = v107 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v110 = v108 + 1;
        v111 = v110 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v110 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&v48[5976]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        *((char *)&i) = 0xff;
        /* unsupported instruction */
        v113 = v111;
        if (!((char)((((unsigned short)v113 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v48[5976]) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            *((char *)&i) = 0;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v113 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)v48[5976]) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v116 = _ccall(v115);
                i = v116;
                /* unsupported instruction */
                /* unsupported instruction */
                iter = v116 & 0xffff | 0xc00;
                /* unsupported instruction */
                /* unsupported instruction */
                iter = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v117 = _ccall(v116 & 0xffff);
                v118 = v117;
                i = _INSERT(CONCAT((unsigned short)v116, 0), 0, (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v119 = v14;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v27 = v119[1];
        /* unsupported instruction */
        /* unsupported instruction */
        v26 = *(v119);
        v28 = v119[2];
        iter = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        iter = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        iter = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_410020((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v17 = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v120 = *((int *)((int)v48[5968] + 16));
        v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v121 = v113 + 1;
        if (*((int *)(int)v48[5968]) > 0)
        {
            do
            {
                v122 = 0;
                if (*((int *)((int)v48[5968] + 4)) > 0)
                {
                    v123 = v121 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    while (1)
                    {
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
                        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v139 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v124 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        iter = v124;
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
                        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v125 = v123 - 2;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v127 = v125 + 1;
                        if (!((((unsigned short)v125 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((int *)&v120[16]) = (int)v48[5980];
                            v7 = 0xff - (char)v120[18];
                            v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                            v128 = _ccall(v118);
                            v7 = v128;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v14 = v128 & 0xffff | 0xc00;
                            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v14 = 0xff - (char)v120[0x11];
                            *((char *)&v120[18]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            v129 = _ccall(v128 & 0xffff);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v130 = _ccall((unsigned int)v129);
                            v7 = v130;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v14 = v130 & 0xffff | 0xc00;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v14 = 0xff - (char)v120[16];
                            *((char *)&v120[0x11]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            v131 = _ccall(v130 & 0xffff);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v132 = _ccall((unsigned int)v131);
                            v7 = _INSERT(CONCAT((unsigned short)v130, 0), 0, v132);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v14 = *((unsigned short *)&v7) | 0xc00;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v133 = v127 + 1;
                            *((char *)&v120[16]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            v134 = _ccall(*((unsigned short *)&v7));
                            v118 = v134;
                            if (*((int *)&g_4b43cc->padding_0[120]) == 111)
                            {
                                v135 = _ccall(v118);
                                *((unsigned short *)&v7) = v135;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v14 = v6;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v14 = *((unsigned short *)&v7) | 0xc00;
                                v3 = &v139;
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v2 = &v139;
                                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                *((char *)&v120[19]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                v136 = _ccall(*((unsigned short *)&v7));
                                v118 = v136;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v3 = &v139;
                                *((char *)&v120[19]) = 0xff;
                                v2 = &v139;
                            }
                            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            D3DXVec3Normalize(v2, v3);
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
                            v139 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            v137 = v133 + 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            sub_4938c0();
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
                            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            sub_4938c0();
                            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v124 = v7;
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
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((int *)v120) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((int *)&v120[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((int *)&v120[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            v124->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((char *)&v120[19]) = 0;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v137 = v127 + 2;
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = v124;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v4 = v46;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                        v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v121 = v137 + 2;
                        v9 += 12;
                        v122 += 1;
                        v120 += 28;
                        if (v122 >= *((int *)((int)v48[5968] + 4)))
                            break;
                        v123 = v121 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                }
                v138 = (int)v48[5968];
                i += 1;
            } while (i < *((int *)v138));
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = v138;
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        *((int *)&v48[5984]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v2 = v46;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        i = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        *((int *)&v48[5988]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v90 = v121 + 2;
    }
    if ((int)v48[5780] > 0)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = v106;
        /* unsupported instruction */
        sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    }
    if ((int)v48[5800] > 0)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = v106;
        /* unsupported instruction */
        sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    }
    v142 = (int)v48[624];
    v143 = (int)v48[632];
    *((unsigned int *)&v48[620]) = v142;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v145 = v90;
    if (!((char)(((v145 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v145 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v143)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&v48[624]) = v142 + 1;
            *((int *)&v48[628]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            return 0;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&v48[628]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&v48[624]) = sub_4931e0();
    return 0;
}
