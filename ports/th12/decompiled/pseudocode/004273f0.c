// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4273f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4273f0_2 {
    char padding_0[304];
    unsigned int field_130;
} st_4273f0_2;

typedef struct st_4273f0_1 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    int field_438;
    char padding_43c[68];
    unsigned int field_480;
} st_4273f0_1;

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

typedef struct st_4273f0_0 {
    char padding_0[20];
    char field_14;
    char padding_15[955];
    unsigned int field_3d0;
    char padding_3d4[220];
    char field_4b0;
    char field_4b1;
    char padding_4b2[22];
    char field_4c8;
    char padding_4c9[1179];
    char field_964;
    char field_965;
    char padding_966[22];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
    unsigned int field_988;
    char padding_98c[8];
    unsigned int field_994;
    unsigned int field_998;
    unsigned int field_99c;
    unsigned int field_9a0;
    unsigned int field_9a4;
    char padding_9a8[4];
    unsigned int field_9ac;
    char padding_9b0[20];
    unsigned int field_9c4;
    int field_9c8;
    unsigned int field_9cc;
    unsigned int field_9d0;
    char padding_9d4[24];
    char field_9ec;
    char padding_9ed[1509479];
    char field_171254;
    char padding_171255[955];
    unsigned int field_171610;
    char padding_171614[220];
    char field_1716f0;
    char field_1716f1;
    char padding_1716f2[1226];
    int field_171bbc;
    unsigned int field_171bc0;
    unsigned int field_171bc4;
    unsigned int field_171bc8;
    char padding_171bcc[8];
    unsigned int field_171bd4;
    unsigned int field_171bd8;
    unsigned int field_171bdc;
    int field_171be0;
    unsigned int field_171be4;
    char padding_171be8[4];
    unsigned int field_171bec;
    unsigned int field_171bf0;
    int field_171bf4;
    unsigned int field_171bf8;
    char padding_171bfc[4];
    unsigned int field_171c00;
    unsigned int field_171c04;
    int field_171c08;
    int field_171c0c;
    char padding_171c10[8];
    int field_171c18;
    char padding_171c1c[16];
    char field_171c2c;
} st_4273f0_0;

extern char g_4b2ed0;
extern unsigned int g_4b43c8;
extern st_4273f0_0 *g_4b44f0;
extern unsigned int g_4cee78;
extern CRITICAL_SECTION g_4cf1d0;
extern char g_4cf221;

unsigned int * sub_4273f0(int a0, unsigned int *idx, unsigned int a2, unsigned int a3)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int v17;  // fc3210
    unsigned int v18;  // 4155
    unsigned int v19;  // edi
    unsigned int v20;  // eax
    int v21;  // edx
    unsigned int v22;  // eax
    unsigned int *v23;  // esi
    int v24;  // eax
    unsigned int *index;  // esi
    unsigned short v26;  // ftop
    unsigned int v9;  // edi
    unsigned short v27;  // ftop
    unsigned short v29;  // ftop
    unsigned short v30;  // ftop
    unsigned int v31;  // fc3210
    unsigned int v32;  // 4155
    unsigned int v34;  // eax
    int v35;  // edx
    unsigned int v36;  // eax
    unsigned int v10;  // ecx
    unsigned int v37;  // ecx
    unsigned int *idx1;  // esi
    int v39;  // ecx
    unsigned int v40;  // eax
    unsigned int v41;  // eax
    unsigned int v42;  // eax
    unsigned int v43;  // eax
    unsigned int v44;  // edx
    unsigned int v45;  // edx
    unsigned int *v46;  // edi
    unsigned int *v11;  // esi
    int v47;  // eax
    unsigned int *idx2;  // edi
    unsigned short v49;  // ftop
    unsigned short v50;  // ftop
    unsigned int v51;  // ebx
    unsigned int v53;  // fc3210
    unsigned int v54;  // 4153
    unsigned int v55;  // eax
    unsigned int v56;  // edx
    int v12;  // eax
    int v57;  // ebp
    st_4273f0_2 *v58;  // ebp
    st_4273f0_1 *v59;  // esi
    unsigned int v60;  // edx
    unsigned int v61;  // esi
    unsigned int *v13;  // esi
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    int v5;  // [bp-0xc]
    int v6;  // [bp-0x4]

    v4 = v7;
    v2 = v8;
    v1 = v9;
    if (a0 - 10 <= 5)
    {
        *((unsigned int *)&g_4b44f0[4].padding_9ed[652607]) = *((int *)&g_4b44f0[4].padding_9ed[652607]) + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = v10;
        /* unsupported instruction */
        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v11 = &g_4b44f0->field_171254;
        v12 = 0;
        while (1)
        {
            v13 = v11;
            if (!v13[620])
                break;
            v12 += 1;
            v11 = v13 + 630;
            if (v12 >= 16)
                return v13 + 630;
        }
        v15 = v14 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v13[620] = 6;
        v13[603] = *(idx);
        v13[604] = idx[1];
        v13[605] = idx[2];
        if (!(((v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v13[603]) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            v13[603] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v13[603]) * 0x100 & 0x4500;
            v18 = _ccall(10, 13, (char)(((v15 - 0 & 7) * 0x800 | (unsigned short)v17 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v18 & 1))
            {
                v13[603] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        v19 = *((int *)&g_4b44f0[4].padding_9ed[652607]) & 1;
        if (v19)
        {
            if (v19 != 1)
                goto LABEL_4274d5;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
LABEL_4274d5:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        v13[608] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v20 = v13[614];
        if (!((char)v13[614] & 1))
        {
            v13[612] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v13[611] = v21;
            v13[610] = 0xfff0bdc1;
            v13[613] = &g_4b2ed0;
            v13[614] = v20 | 1;
        }
        v13[612] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v13[611] = v21;
        v13[610] = 0xffffffff;
        v22 = v13[619];
        if (!((char)v13[619] & 1))
        {
            v13[617] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            v13[616] = v21;
            v13[615] = 0xfff0bdc1;
            v13[618] = &g_4b2ed0;
            v13[619] = v22 | 1;
        }
        v13[616] = v21;
        v13[617] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v13[615] = 0xffffffff;
        v13[609] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v13[602] = v21;
        v13[622] = v21;
        v13[625] = v21;
        v13[621] = a0;
        sub_402520();
        *((char *)&v13[295] + 1) = 16;
        *((char *)&v13[295]) = 16;
        sub_454d10(v13, a0 + 165);
        v13[239] = 0xffffffff;
        return v13;
    }
    else if (a0 - 16 <= 2)
    {
        v23 = &g_4b44f0->field_171254;
        v24 = 0;
        while (1)
        {
            index = v23;
            if (!index[620])
                break;
            v24 += 1;
            v23 = index + 630;
            if (v24 >= 16)
                return index + 630;
        }
        v27 = v26 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        index[620] = 8;
        index[603] = *(idx);
        index[604] = idx[1];
        index[605] = idx[2];
        if (!(((v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), index[603]) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            index[603] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v29 = v27 + 1;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v30 = v27 - 0;
            /* unsupported instruction */
            /* unsupported instruction */
            v31 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), index[603]) * 0x100 & 0x4500;
            v32 = _ccall(10, 13, (char)(((v30 & 7) * 0x800 | (unsigned short)v31 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v32 & 1))
            {
                index[603] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v29 = v30 + 1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v29 = v30 + 1;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        if (!(((v29 - 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), index[604]) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
            index[604] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        index[608] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v34 = index[614];
        if (!((char)index[614] & 1))
        {
            index[612] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            index[611] = v35;
            index[610] = 0xfff0bdc1;
            index[613] = &g_4b2ed0;
            index[614] = v34 | 1;
        }
        index[612] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        index[611] = v35;
        index[610] = 0xffffffff;
        index[609] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        index[621] = a0;
        index[622] = v35;
        sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &a0, a0 + 165, v35);
        index[602] = v5;
        index[239] = 0xffffffff;
        return index;
    }
    else if (a0 == 9)
    {
        v36 = *((int *)&g_4b44f0[4].padding_9ed[652599]);
        *((unsigned int *)&g_4b44f0[4].padding_9ed[652603]) = *((int *)&g_4b44f0[4].padding_9ed[652603]) + 1;
        v37 = v36 * 2520;
        idx1 = &g_4b44f0->padding_0[v37 + 1552340];
        v39 = *((int *)&g_4b44f0[4].padding_9ed[652603]);
        if (!*((int *)&g_4b44f0[1].padding_9ed[37738 + v37]))
        {
            if (v39 >= 0x400)
            {
                v40 = v36 & 0x8000001f;
                if ((v36 & 0x8000001f) < 0)
                    v40 = (v40 - 1 | 0xffffffe0) + 1;
                v41 = v40 + 16;
            }
            else if (v39 >= 0x200)
            {
                v42 = v36 & 0x8000000f;
                if ((v36 & 0x8000000f) < 0)
                    v42 = (v42 - 1 | 0xfffffff0) + 1;
                v41 = v42 + 8;
            }
            else if (v39 >= 0x100)
            {
                v43 = v36 & 0x80000007;
                if ((v36 & 0x80000007) < 0)
                    v43 = (v43 - 1 | 0xfffffff8) + 1;
                v41 = v43 + 4;
            }
            else
            {
                v41 = v36 & 0x80000003;
                if ((v36 & 0x80000003) < 0)
                    v41 = (v41 - 1 | 0xfffffffc) + 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            idx1[624] = v41;
            idx1[620] = 5;
            idx1[621] = 9;
            idx1[622] = 9;
            idx1[603] = *(idx);
            idx1[604] = idx[1];
            /* unsupported instruction */
            idx1[605] = idx[2];
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            idx1[608] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_4067e0(0);
            /* unsupported instruction */
            /* unsupported instruction */
            idx1[609] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx1[602] = 0;
        }
        v44 = *((int *)&g_4b44f0[4].padding_9ed[652599]) + 1;
        v45 = v44 & 0x800007ff;
        if ((v44 & 0x800007ff) < 0)
            v45 = (v45 - 1 | 0xfffff800) + 1;
        *((unsigned int *)&g_4b44f0[4].padding_9ed[652599]) = v45;
        return idx1;
    }
    else
    {
        v46 = &g_4b44f0->field_14;
        v47 = 0;
        while (1)
        {
            idx2 = v46;
            if (!idx2[620])
                break;
            v47 += 1;
            v46 = idx2 + 630;
            if (v47 >= 600)
                return idx2 + 630;
        }
        v50 = v49 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        idx2[620] = 1;
        v51 = idx2 + 603;
        *((unsigned int *)v51) = *(idx);
        *((unsigned int *)(v51 + 4)) = idx[1];
        *((unsigned int *)(v51 + 8)) = idx[2];
        if (!(((v50 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)v51)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            *((int *)v51) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v53 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)v51)) * 0x100 & 0x4500;
            v54 = _ccall(10, 13, (char)(((v50 - 0 & 7) * 0x800 | (unsigned short)v53 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v54 & 1))
            {
                *((int *)v51) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        idx2[608] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v55 = idx2[614];
        if (!((char)idx2[614] & 1))
        {
            idx2[612] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            idx2[611] = v56;
            idx2[610] = 0xfff0bdc1;
            idx2[613] = &g_4b2ed0;
            idx2[614] = v55 | 1;
        }
        v57 = v6;
        idx2[612] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        idx2[611] = v56;
        idx2[610] = 0xffffffff;
        idx2[609] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        idx2[623] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx2[602] = v56;
        if (v57 == 4 || v57 == 5 || v57 == 6 || v57 == 7)
        {
            v58 = *((int *)(g_4b43c8 + 5106652));
            if (g_4cee78 & 0x8000)
            {
                EnterCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 + 1;
            }
            v58->field_130 = v58->field_130 + 1;
            v59 = sub_4621c0();
            v59->field_480 = v59->field_480 | 1;
            v59->field_20 = 23;
            if (!v51)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v59->field_430 = v3;
                v59->field_434 = v4;
                v59->field_438 = v5;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v59->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v59->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v59->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            sub_454d10(v59, 100);
            sub_461250(v59, 100);
            if (g_4cee78 & 0x8000)
            {
                LeaveCriticalSection(&g_4cf1d0.DebugInfo);
                g_4cf221 = g_4cf221 - 1;
            }
            sub_453d90();
            v57 = v5;
            v60 = 0;
        }
        idx2[622] = v60;
        idx2[621] = v57;
        sub_402520();
        *((char *)&idx2[295] + 1) = 16;
        *((char *)&idx2[295]) = 16;
        sub_454d10(idx2, v57 + 165);
        v61 = idx2 + 301;
        sub_402520(idx2, v57 + 165);
        *((char *)(v61 + 1181)) = 16;
        *((char *)(v61 + 1180)) = 16;
        sub_454d10(v61, v57 + 185);
        idx2[239] = 0xffffffff;
        return idx2;
    }
}
