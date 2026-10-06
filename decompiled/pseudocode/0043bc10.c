// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43bc10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43bc10_6 {
    char padding_0[4];
    int field_4;
    int field_8;
    int field_c;
    unsigned int field_10;
    int field_14;
} st_43bc10_6;

typedef struct st_43bc10_3 {
    char padding_0[5400];
    unsigned int field_1518;
} st_43bc10_3;

typedef struct st_43bc10_1 {
    char padding_0[5400];
    unsigned int field_1518;
    char field_151c;
    char padding_151d[899];
    unsigned int field_18a0;
} st_43bc10_1;

typedef struct st_43bc10_2 {
    struct st_43bc10_1 *field_0;
    struct st_43bc10_2 *field_4;
} st_43bc10_2;

typedef struct st_43bc10_5 {
    unsigned int field_0;
    unsigned int field_4;
    char field_8;
} st_43bc10_5;

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

extern int g_4a12f0;
extern int g_4a13b4;
extern int g_4a13e8;
extern unsigned int g_4ad138;
extern unsigned int g_4ae590;
extern unsigned int g_4aee20[4];
extern unsigned int g_4aee38[4];
extern unsigned int g_4b0c44;
extern void* g_4b4518;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf128;
extern char g_4cf21a;

void sub_43bc10(int a0, char *a1, unsigned int a2)
{
    unsigned int v17;  // esi
    unsigned int v18;  // edi
    int j;  // eax
    st_43bc10_3 *idx;  // eax
    st_43bc10_3 **v30;  // eax
    int k;  // ecx
    unsigned int v32;  // esi
    unsigned int v33;  // ebx
    st_43bc10_2 *node;  // edi
    st_43bc10_1 *index;  // ecx
    unsigned int v36;  // edx
    char *v19;  // edi
    unsigned int v37;  // edx
    unsigned int v38;  // edx
    unsigned int *v39;  // eax
    void* l;  // esi
    unsigned int *v41;  // edx
    unsigned int v42;  // ecx
    unsigned int v43;  // eax
    unsigned int *iter;  // edi
    int iter1;  // ebx
    unsigned int *n;  // esi
    void* iter2;  // ecx
    unsigned int *v47;  // edi
    unsigned int v48;  // ecx
    st_43bc10_2 *i0;  // edi
    st_43bc10_1 *v50;  // esi
    unsigned int v51;  // edx
    unsigned int v52;  // edx
    st_43bc10_2 *i1;  // edi
    st_43bc10_1 *v54;  // esi
    unsigned int count;  // ebx
    st_43bc10_5 *idx1;  // edi
    char *v21;  // eax
    st_43bc10_5 *v57;  // ebx
    int v58;  // esi
    int v59;  // esi
    int v60;  // esi
    st_43bc10_6 *v61;  // eax
    int v62;  // esi
    int v63;  // esi
    int v64;  // esi
    unsigned int v65;  // eax
    unsigned int v66;  // esi
    char i;  // 4103
    unsigned int v67;  // esi
    unsigned int v68;  // eax
    unsigned int v69;  // eax
    unsigned int v70;  // eax
    unsigned int v71;  // 4101
    unsigned int count1[4];  // esi
    unsigned int v73;  // esi
    unsigned int v74;  // eax
    unsigned int v75;  // eax
    unsigned int v76;  // eax
    char *v23;  // eax
    unsigned int v77;  // 4101
    unsigned int v78;  // ebx
    unsigned int count2[4];  // esi
    char *v24;  // eax
    char *v25;  // eax
    char *v26;  // eax
    unsigned int v0;  // [bp-0x170]
    unsigned int v1;  // [bp-0x16c]
    double v2;  // [bp-0x168], Other Possible Types: unsigned long
    unsigned int v3;  // [bp-0x14c]
    unsigned int v4[2];  // [bp-0x148]
    unsigned int v5[4];  // [bp-0x140]
    unsigned int v6;  // [bp-0x130]
    unsigned int v7;  // [bp-0x12c]
    int v8;  // [bp-0x128], Other Possible Types: unsigned int
    int <0x43bc10[is_1]|Stack bp-0x11c, 1 B>;  // [bp-0x11c]
    int v9;  // [bp-0x11c], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x118]
    unsigned int m;  // [bp-0x114]
    unsigned int *v12;  // [bp-0x110], Other Possible Types: unsigned int
    int v13;  // [bp-0x10c], Other Possible Types: unsigned int
    int v14;  // [bp-0x108]
    char v15;  // [bp-0x104]
    unsigned int v16;  // [bp-0x4]

    v16 = g_4ad138 ^ &<0x43bc10[is_1]|Stack bp-0x11c, 1 B>;
    v8 = v17;
    v7 = v18;
    v19 = a1;
    iter2 = (int)g_4b4518[28];
    v14 = 0;
    v13 = 0;
    v21 = v19;
    do
    {
        i = *(v21);
        *((char *)iter2) = *(v21);
        v21 += 1;
        iter2 += 1;
    } while (i);
    v23 = v19;
    v24 = v23;
    do
    {
        v25 = v24;
        v26 = v25 + 1;
        v24 = v26;
    } while (*(v25));
    j = v26 - (v23 + 1);
    if (j < 8)
    {
        do
        {
            *((char *)(j + (int)g_4b4518[28])) = 32;
            j += 1;
        } while (j < 8);
    }
    if (!((char)g_4b4518[476] & 1) && a2)
    {
        idx = *((int *)(int)g_4b4518[160]);
        memset(idx->field_1518, -0x1, 6);
        idx->field_1518 = idx->field_1518 + 6;
        if ((715827883 * (idx->field_1518 - (char *)idx) >> 63) + (715827883 * (idx->field_1518 - (char *)idx) >> 32) >= 900)
        {
            v30 = sub_43cbb0();
            *((st_43bc10_3 ***)&g_4b4518[160]) = v30;
        }
    }
    sub_46d585("replay");
    sub_46cbbb(&v15, "replay/%s", a0);
    k = 0;
    v12 = 0;
    v32 = 112;
    v9 = 0;
    v33 = g_4b4518 + 32;
    v10 = g_4b4518 + 0x44;
    do
    {
        if (*((int *)v33))
        {
            if (!v14)
                v14 = k;
            v13 = k;
            if (!((char)g_4b4518[476] & 1))
                *((unsigned int *)(*((int *)v33) + 8)) = 0;
            node = *((int *)v10);
            v32 += 160;
            if (node)
            {
                do
                {
                    index = node->field_0;
                    v36 = 715827883 * (index->field_1518 - (char *)index) >> 32;
                    m = v32 + index->field_18a0 + 6 * (v36 >> 31) + 6 * v36 - (char *)index - 5404;
                    if (!((char)g_4b4518[476] & 1))
                    {
                        v37 = 715827883 * (index->field_1518 - (char *)index) >> 32;
                        *((st_43bc10_1 **)(*((int *)v33) + 8)) = *((int *)(*((int *)v33) + 8)) + index->field_18a0 + 6 * (v37 >> 31) + 6 * v37 - (char *)index - 5404;
                        v38 = 715827883 * (node->field_0->field_1518 - (char *)node->field_0) >> 32;
                        *((unsigned int *)(*((int *)v33) + 4)) = *((int *)(*((int *)v33) + 4)) + (v38 >> 31) + v38;
                    }
                } while ((node = (st_43bc10_2 *)node->field_4, v32 = m, node));
                k = v9;
                v12 += 1;
            }
            else
            {
                v12 += 1;
            }
        }
        v10 += 12;
        k += 1;
        v33 += 4;
        v9 = k;
    } while (k < 8);
    *((unsigned int *)((int)g_4b4518[28] + 88)) = v12;
    *((unsigned int *)((int)g_4b4518[28] + 20)) = g_4b0c44;
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
    v6 = v32;
    if (/* unsupported instruction */)
    {
        v9 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v9 = nan;
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)((int)g_4b4518[28] + 84)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v39 = sub_46d04a();
    l = (int)g_4b4518[28];
    v41 = v39;
    v10 = g_4b4518 + 0x44;
    v42 = 28;
    v43 = g_4b4518 + 32;
    v12 = v41;
    for (iter = v41; v42; l += 4)
    {
        v42 -= 1;
        *(iter) = *((int *)l);
        iter += 1;
    }
    iter1 = 112;
    v9 = v43;
    m = 8;
    do
    {
        n = *((int *)v43);
        if (*((int *)v43))
        {
            v47 = (char *)v41 + iter1;
            for (v48 = 40; v48; n += 1)
            {
                v48 -= 1;
                *(v47) = *(n);
                v47 += 1;
            }
            i0 = *((int *)v10);
            iter1 += 160;
            if (*((int *)v10))
            {
                do
                {
                    v50 = i0->field_0;
                    v51 = 715827883 * (v50->field_1518 - (char *)v50) >> 32;
                    sub_47a330((char *)v12 + iter1, v50, ((v51 >> 31) + v51) * 6);
                    i0 = i0->field_4;
                    v52 = 715827883 * (v50->field_1518 - (char *)v50) >> 32;
                    iter1 += ((v52 >> 31) + v52) * 6;
                } while (i0);
            }
            i1 = *((int *)v10);
            if (*((int *)v10))
            {
                do
                {
                    v54 = i1->field_0;
                    sub_47a330((char *)v12 + iter1, &v54->field_151c, v54->field_18a0 - (char *)v54 - 5404);
                    i1 = i1->field_4;
                    iter1 = iter1 + v54->field_18a0 - (char *)v54 - 5404;
                } while (i1);
            }
        }
        v10 += 12;
        v41 = v12;
        v43 = v9 + 4;
        m -= 1;
        v9 = v43;
    } while (m != 1);
    v8 = sub_44c3f0(v41, iter1, &m);
    sub_46c941(v9);
    sub_463af0(v8, count, 58, 64, count);
    sub_463af0(v8, count, 225, 0x800, count);
    *((int *)((int)g_4b4518[24] + 32)) = iter1;
    *((unsigned int *)((int)g_4b4518[24] + 28)) = count;
    *((unsigned int *)((int)g_4b4518[24] + 12)) = *((int *)((int)g_4b4518[24] + 28)) + 36;
    sub_463fb0(v8, count, 225, 0x800, count);
    if (g_4ae590 != 0xffffffff)
    {
        WriteFile(g_4ae590, (int)g_4b4518[24], 36, v4, NULL);
        if (v4 != 36)
        {
            CloseHandle(g_4ae590);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf128.DebugInfo);
                g_4cf21a = g_4cf21a - 1;
            }
        }
        if (g_4ae590 != 0xffffffff)
        {
            WriteFile(g_4ae590, count, count, &v3, NULL);
            if (count != v3)
            {
                CloseHandle(g_4ae590);
                if (g_4cee78 & 0x8000)
                {
                    LeaveCriticalSection(&g_4cf128.DebugInfo);
                    g_4cf21a = g_4cf21a - 1;
                }
            }
        }
    }
    if (count)
        sub_46c941(count);
    idx1 = sub_46d04a(0xffff);
    sub_477420(idx1, 0, 0xffff);
    v57 = (char *)&idx1[1].field_0 + 3;
    idx1->field_0 = 1380275029;
    idx1->field_8 = 0;
    v58 = (char *)v57 + sub_46cbbb(v57, &g_4a12f0);
    v59 = v58 + sub_46cbbb(v58, "Version %s\r\n", "1.00b");
    v60 = v59 + sub_46cbbb(v59, "Name %s\r\n", (int)g_4b4518[28]);
    v61 = sub_46d8e4((int)g_4b4518[28] + 12);
    v62 = v60 + sub_46cbbb(v60, "Date %.2d/%.2d/%.2d %.2d:%.2d\r\n", v61->field_14 % 100, v61->field_10 + 1, v61->field_c, v61->field_8, v61->field_4);
    v63 = v62 + sub_46cbbb(v62, "Chara %s\r\n", g_4aee20[2 * *((int *)((int)g_4b4518[28] + 92)) + *((int *)((int)g_4b4518[28] + 96))]);
    v64 = v63 + sub_46cbbb(v63, "Rank %s\r\n", g_4aee38[*((int *)((int)g_4b4518[28] + 100))]);
    v65 = (*((int *)((int)g_4b4518[28] + 104)) <= 7 ? (count == 64 ? (count == 7 ? sub_46cbbb(v64, "Extra Stage\r\n") : sub_46cbbb(v64, "Stage %d\r\n", count)) : sub_46cbbb(v64, &g_4a13b4, count, 64)) : (count == 7 ? sub_46cbbb(v64, "Extra Stage Clear\r\n") : sub_46cbbb(v64, "Stage All Clear\r\n")));
    v66 = v64 + v65 + sub_46cbbb(v64 + v65, "Score %d\r\n", *((int *)((int)g_4b4518[28] + 20)));
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = (double)nan;
        /* unsupported instruction */
    }
    v1 = "Slow Rate %2.2f\r\n";
    v0 = v66;
    v67 = v66 + sub_46cbbb() + 1;
    v68 = v67 - (char *)idx1;
    v69 = v68 & 0x80000003;
    if ((v68 & 0x80000003) < 0)
    {
        v70 = v69 - 1 | 0xfffffffc;
        v69 = v70 + 1;
        v71 = _ccall(4, 18, v70 + 1, 0, 0);
        if (!(v71 & 1))
            goto LABEL_43c20a;
    }
    else if (v68 & 0x80000003)
    {
LABEL_43c20a:
        v67 += 4 - v69;
    }
    count1 = (unsigned int (32 bits)[4])(struct st_43bc10_5 *)(v67 - (char *)idx1);
    *((unsigned int [4])&idx1->field_4) = count1;
    if (g_4ae590 != 0xffffffff)
    {
        WriteFile(g_4ae590, idx1, count1, v5, NULL);
        if (count1 != v5)
        {
            CloseHandle(g_4ae590);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf128.DebugInfo);
                g_4cf21a = g_4cf21a - 1;
            }
        }
    }
    sub_477420(idx1, 0, 0xffff);
    idx1->field_0 = 1380275029;
    idx1->field_8 = 1;
    v73 = (char *)v57 + sub_46cbbb(v57, &g_4a13e8) + 1;
    v74 = v73 - (char *)idx1;
    v75 = v74 & 0x80000003;
    if ((v74 & 0x80000003) < 0)
    {
        v76 = v75 - 1 | 0xfffffffc;
        v75 = v76 + 1;
        v77 = _ccall(4, 18, v76 + 1, 0, 0);
        if (!(v77 & 1))
            goto LABEL_43c29c;
    }
    else if (v74 & 0x80000003)
    {
LABEL_43c29c:
        v73 += 4 - v75;
    }
    v78 = g_4ae590;
    count2 = (unsigned int (32 bits)[4])(struct st_43bc10_5 *)(v73 - (char *)idx1);
    *((unsigned int [4])&idx1->field_4) = count2;
    if (g_4ae590 != 0xffffffff)
    {
        WriteFile(g_4ae590, idx1, count2, v5, NULL);
        if (count2 != v5)
        {
            CloseHandle(g_4ae590);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf128.DebugInfo);
                g_4cf21a = g_4cf21a - 1;
            }
        }
        v78 = g_4ae590;
    }
    sub_46c941(idx1);
    if (v78 != 0xffffffff)
    {
        CloseHandle(v78);
        if (g_4cee78 & 0x8000)
        {
            LeaveCriticalSection(&g_4cf128.DebugInfo);
            g_4cf21a = g_4cf21a - 1;
        }
    }
    *((unsigned int *)&g_4b4518[476]) = (int)g_4b4518[476] | 1;
    _security_check_cookie();
    return;
}
