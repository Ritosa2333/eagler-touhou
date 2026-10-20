// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411420; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411420_1 {
    char padding_0[4];
    unsigned int field_4;
} st_411420_1;

typedef struct st_411420_0 {
    char padding_0[32];
    char field_20;
} st_411420_0;

extern unsigned int g_4b0ca8;
extern st_411420_0 *g_4b43d8;
extern unsigned int g_4ce55c;
extern unsigned int g_4d48b8;
extern unsigned int g_4d48c4;

unsigned int sub_411420(void* idx)
{
    unsigned int v18;  // ebx
    unsigned int v19;  // edi
    void* iter;  // esi
    unsigned int v28;  // edi
    unsigned int v29;  // edi
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    void* v32;  // ecx
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ecx
    char *v36;  // ecx
    unsigned int node;  // eax
    char v38;  // 4101
    unsigned int v39;  // cc_dep1
    unsigned int v40;  // cc_dep2
    char v41;  // 4103
    unsigned int v42;  // eax
    unsigned int v43;  // 4111
    unsigned int v44;  // 4120
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v20;  // ftop
    void* iter1;  // esi
    unsigned int v48;  // edi
    st_411420_1 *v49;  // esi
    unsigned int v50;  // edx
    unsigned int v51;  // 4112
    void* v52;  // eax
    unsigned int v53;  // edx
    unsigned int *v54;  // ecx
    unsigned short v56;  // ftop
    void* index;  // eax
    unsigned short v57;  // ftop
    unsigned int v22;  // ecx
    void* v23;  // ebx
    unsigned int v24;  // eax
    int v25;  // eax
    int v26;  // eax
    int v0;  // [bp-0x54]
    unsigned int v1;  // [bp-0x50]
    int v2;  // [bp-0x4c]
    int v3;  // [bp-0x48]
    int v4;  // [bp-0x44], Other Possible Types: unsigned int
    int v5;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x38]
    unsigned int v8;  // [bp-0x34]
    unsigned int v9;  // [bp-0x30]
    unsigned int v10;  // [bp-0x2c]
    char v11;  // [bp-0x28]
    unsigned int v12;  // [bp-0x24]
    unsigned int v13;  // [bp-0x24]
    unsigned int v14;  // [bp-0x1c]
    void* v15;  // [bp-0x14], Other Possible Types: unsigned int
    void* v16;  // [bp-0xc]
    void* v17;  // [bp-0xc]

    if ((char)idx[116] & 4)
        return 0;
    v14 = v18;
    v13 = v19;
    if ((int)idx[28] >= *((short *)(int)idx[84]))
    {
        do
        {
            index = (int)idx[84];
            v22 = (char)index[2];
            switch (v22)
            {
            case 3:
                if (!(int)idx[120])
                {
                    v23 = idx + 64;
                    v15 = v23;
                    v10 = 5;
                    v16 = v17;
                    while (1)
                    {
                        v9 = *((int *)v23);
                        if (!sub_461920(*((int *)v23)))
                            *((int *)v23) = 0;
                        sub_460760(0xffffff, 0, 0, 0, " ");
                        v8 = *((int *)v23);
                        sub_461970(*((int *)v23));
                        v16 += 4;
                        v12 = v13 - 1;
                        if (v13 == 1)
                            break;
                        v23 = v16;
                        v13 = v12;
                    }
                    v7 = (int)idx[64];
                    v24 = sub_461920((int)idx[64]);
                    if (!v24)
                        *((unsigned int *)&idx[64]) = v24;
                    v25 = sub_420e40();
                    sub_460760((int)idx[124], 0, 0, 0, v25);
                    v6 = (int)idx[64];
                    sub_461970((int)idx[64]);
                    *((unsigned int *)&idx[120]) = (int)idx[120] + 1;
                    v13 = v12;
                    goto LABEL_41194d;
                }
                else
                {
                    v26 = sub_420e40();
                    sub_461c50((int)idx[124], 0, 0, 0, v26);
                    sub_460760((int)idx[124], 0, 0, 0, v26);
                    sub_461970(*((int *)((char *)idx + 4 * idx[120] + 64)));
                    *((unsigned int *)&idx[120]) = (int)idx[120] + 1;
                    if ((int)idx[120] >= 5)
                    {
                        *((int *)&idx[120]) = 0;
                        goto LABEL_41194d;
                    }
                }
            case 4:
                iter = idx + 64;
                v28 = 5;
                do
                {
                    v29 = v28;
                    v10 = *((int *)iter);
                    sub_461970(*((int *)iter));
                    iter += 4;
                    v28 = v29 - 1;
                } while (v29 != 1);
                goto LABEL_41194d;
            case 5:
                if ((int)idx[48] <= NULL)
                    sub_4067e0((int)index[4]);
                v30 = v20 - 1;
                if (/* unsupported instruction */)
                {
                    v31 = v30 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    break;
                }
                else
                {
                    v31 = v30 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    break;
                }
                v10 = v22;
                if (/* unsupported instruction */)
                {
                    v10 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v20 = v31 + 1;
                }
                else
                {
                    v10 = nan;
                    /* unsupported instruction */
                    v20 = v31 + 1;
                }
                sub_464a20();
                v32 = (int)idx[84];
                if (*((int *)((int)idx[84] + 4)) < NULL)
                {
                    v9 = 999;
                    sub_4067e0(999);
                }
                if (!(!(g_4d48c4 & 524289) && (int)idx[48] > NULL))
                {
                    sub_453d90(v32, 0);
                    v8 = 0;
                    sub_4067e0(0);
                    goto LABEL_41194d;
                }
                else if (g_4b43d8->field_20 & 1)
                {
                    return 0;
                }
                else if (!(g_4d48b8 & 0x200))
                {
                    return 0;
                }
                else if (!((int)idx[48] % 6))
                {
                    v8 = 0;
                    sub_4067e0(0);
                    goto LABEL_41194d;
                }
                else
                {
                    return 0;
                }
            case 6:
                if ((int)idx[48] <= NULL)
                    sub_4067e0((int)index[4]);
                v33 = v20 - 1;
                if (/* unsupported instruction */)
                {
                    v34 = v33 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v34 = v33 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v10 = v22;
                if (/* unsupported instruction */)
                {
                    v10 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v20 = v34 + 1;
                }
                else
                {
                    v10 = nan;
                    /* unsupported instruction */
                    v20 = v34 + 1;
                }
                sub_464a20();
                if (!(!(g_4d48c4 & 524289) && (int)idx[48] > NULL))
                {
                    sub_453d90(v35, 0);
                }
                else if (g_4b43d8->field_20 & 1 || !(g_4d48b8 & 0x200) || (int)idx[48] % 6)
                {
                    return 0;
                }
                v9 = 0;
                sub_4067e0(0);
                *((int *)&idx[120]) = 0;
                g_4ce55c = 0;
                goto LABEL_41194d;
            case 7:
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
                    v5 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v5 = nan;
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
                    v4 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v4 = nan;
                    /* unsupported instruction */
                }
                sub_411b80();
                v51 = _ccall(8, 3, *((int *)((int)idx[84] + 4)), 28, 0);
                if (!(v51 & 1))
                    sub_4604a0();
                v52 = (int)idx[84];
                *((unsigned int *)&idx[116]) = (int)idx[116] | 4;
                *((void* *)&idx[112]) = v52 + 8;
                *((int *)&idx[236]) = (int)v52[4];
                sub_464cb0(idx);
                *((unsigned int *)&idx[84]) = *((char *)((int)idx[84] + 3)) + (int)idx[84] + 4;
                return 0;
            case 8:
                sub_461a70(*((int *)((char *)idx + 4 * index[4] + 144)));
                *((unsigned int *)((char *)idx + 4 * index[4] + 144)) = 0;
                sub_4615a0(*((int *)((char *)idx + 4 * *((int *)((int)idx[84] + 8)) + 128)), &v10, *((int *)((int)idx[84] + 12)), 0);
                *((unsigned int *)((char *)idx + 4 * *((int *)((int)idx[84] + 4)) + 144)) = v6;
                goto LABEL_41194d;
            case 9:
                *((int *)&idx[124]) = (int)index[4];
                goto LABEL_41194d;
            case 10:
                sub_4300d0(0, index + 4);
                v36 = "bgm/th12_20.wav";
                node = (int)idx[84] + 4;
                while (1)
                {
                    v38 = *((char *)node);
                    v39 = v38;
                    v40 = *(v36);
                    if (v38 != *(v36))
                        break;
                    if (v38)
                    {
                        v41 = *((char *)(node + 1));
                        v39 = v41;
                        v40 = v36[1];
                        if (v41 != v36[1])
                            break;
                        node += 2;
                        v36 += 2;
                        if (!v41)
                            goto LABEL_411874;
                    }
                    else
                    {
LABEL_411874:
                        v42 = 0;
                        goto LABEL_41187d;
                    }
                }
                v43 = _ccall(4, v39, v40, 0);
                v44 = _ccall(12, 0, v43 & 1, v43 & 1);
                v42 = -(v43 & 1) + 1 - (v44 & 1);
LABEL_41187d:
                if (!v42)
                {
                    sub_430150(0, 15);
                    goto LABEL_41194d;
                }
                else
                {
                    sub_430150(0, 16);
                    goto LABEL_41194d;
                }
            case 11:
                v45 = v20 - 1;
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
                v5 = v22;
                if (/* unsupported instruction */)
                {
                    v5 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v20 = v46 + 1;
                }
                else
                {
                    v5 = nan;
                    /* unsupported instruction */
                    v20 = v46 + 1;
                }
                sub_430270();
                *((unsigned int *)&idx[116]) = (int)idx[116] & 0xfffffffe;
                goto LABEL_41194d;
            case 13:
                v50 = (int)index[4];
                v5 = 70;
                v4 = 0;
                v3 = 0;
                v2 = 0;
                v1 = (int)index[4];
                v0 = 0;
                goto LABEL_411948;
            case 14:
                v50 = (int)index[4];
                v5 = 70;
                v4 = 0;
                v3 = 0;
                v2 = 0;
                v1 = (int)index[4];
                v0 = 5;
LABEL_411948:
                sub_4529a0(v0, v50, v2, v3, v4, v5);
                goto LABEL_41194d;
            case 15:
                if (g_4b0ca8 == 1)
                {
                    sub_461a70(*((int *)((char *)idx + 4 * index[4] + 144)));
                    *((unsigned int *)((char *)idx + 4 * index[4] + 144)) = 0;
                    sub_4615a0(*((int *)((char *)idx + 4 * *((int *)((int)idx[84] + 8)) + 128)), &v11, *((int *)((int)idx[84] + 12)), 0);
                    *((unsigned int *)((char *)idx + 4 * *((int *)((int)idx[84] + 4)) + 144)) = v7;
                    goto LABEL_41194d;
                }
                break;
            case 16:
                if (g_4b0ca8 == 2)
                {
                    sub_461a70(*((int *)((char *)idx + 4 * index[4] + 144)));
                    *((unsigned int *)((char *)idx + 4 * index[4] + 144)) = 0;
                    sub_4615a0(*((int *)((char *)idx + 4 * *((int *)((int)idx[84] + 8)) + 128)), &v13, *((int *)((int)idx[84] + 12)), 0);
                    *((unsigned int *)((char *)idx + 4 * *((int *)((int)idx[84] + 4)) + 144)) = v8;
                    goto LABEL_41194d;
                }
                break;
            case 17:
                if (g_4b0ca8 == 3)
                {
                    sub_461a70(*((int *)((char *)idx + 4 * index[4] + 144)));
                    *((unsigned int *)((char *)idx + 4 * index[4] + 144)) = 0;
                    sub_4615a0(*((int *)((char *)idx + 4 * *((int *)((int)idx[84] + 8)) + 128)), &esi, *((int *)((int)idx[84] + 12)), 0);
                    *((unsigned int *)((char *)idx + 4 * *((int *)((int)idx[84] + 4)) + 144)) = v9;
                    goto LABEL_41194d;
                }
                break;
            case 12:
                iter1 = idx + 64;
                v7 = 5;
                do
                {
                    v48 = v7;
                    v10 = *((int *)iter1);
                    sub_461a70(*((int *)iter1));
                    *((int *)iter1) = 0;
                    iter1 += 4;
                    v7 = v48 - 1;
                } while (v48 != 1);
                v49 = sub_410b30();
                if (v49)
                {
                    sub_477420(idx, v7, 240);
                    *((st_411420_1 **)&idx[84]) = &v49->padding_0[v49->field_4];
                    sub_4067e0(v7);
                    sub_4067e0(v7);
                    sub_4067e0(v7);
                    *((unsigned int *)&idx[116]) = (int)idx[116] | 2;
                    *((int *)&idx[124]) = 0xffffff;
                    v9 = v7;
                    v8 = v7;
                    continue;
                }
            case 0:
                return 0xffffffff;
            default:
LABEL_41194d:
                *((unsigned int *)&idx[84]) = *((char *)((int)idx[84] + 3)) + (int)idx[84] + 4;
                goto LABEL_41195b;
            }
LABEL_41195b:
        } while ((int)idx[28] >= *((short *)(int)idx[84]));
    }
    v53 = (int)idx[28];
    v54 = (int)idx[36];
    *((unsigned int *)&idx[24]) = v53;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)((((unsigned short)v20 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        v56 = v20 - 1;
        if (/* unsupported instruction */)
        {
            v57 = v56 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v57 = v56 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        if (!((char)(((v57 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v54)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
            if (/* unsupported instruction */)
            {
                *((int *)&idx[32]) = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                *((unsigned int *)&idx[32]) = nan;
                /* unsupported instruction */
            }
            *((unsigned int *)&idx[28]) = v53 + 1;
            return 0;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&idx[28]) = sub_4931e0();
    return 0;
}
