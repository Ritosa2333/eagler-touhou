// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4018f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4018f0_2 {
    char padding_0[268];
    unsigned int field_10c;
} st_4018f0_2;

typedef struct st_4018f0_1 {
    char field_0;
    char padding_1[255];
    unsigned int field_100;
    unsigned int field_104;
    unsigned int field_108;
    unsigned int field_10c;
    unsigned int field_110;
    char padding_114[12];
    unsigned int field_120;
    unsigned int field_124;
    char padding_128[8];
    unsigned int field_130;
    unsigned int field_134;
} st_4018f0_1;

typedef struct st_4018f0_0 {
    char padding_0[84];
    unsigned int field_54;
    unsigned int field_58;
    char padding_5c[16];
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[860];
    char field_3d0[4];
    char field_3d3;
    char padding_3d4[100];
    unsigned int field_438;
    unsigned int field_43c;
    unsigned int field_440;
    char padding_444[76];
    unsigned int field_490;
    unsigned int field_494;
} st_4018f0_0;

extern int g_4d47e8;
extern int g_4d4804;

unsigned int sub_4018f0(st_4018f0_0 *idx, st_4018f0_1 *a1)
{
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    unsigned int v17;  // ftop
    unsigned int v18;  // fc3210
    unsigned int v19;  // ftop
    unsigned int v20;  // 4144
    unsigned int v21;  // ftop
    st_4018f0_1 *node;  // eax
    unsigned int v23;  // ftop
    st_4018f0_1 *iter;  // eax
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    st_4018f0_1 *idx1;  // edi
    st_4018f0_1 *iter1;  // eax
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    st_4018f0_1 *iter2;  // eax
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // eax
    unsigned int v35;  // eax
    unsigned int v36;  // ftop
    st_4018f0_1 *v10;  // eax
    st_4018f0_1 *v37;  // ecx
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    char v40;  // al
    st_4018f0_1 *v41;  // ebx
    st_4018f0_1 *idx2;  // eax
    unsigned int v43;  // edx
    st_4018f0_1 *v11;  // eax
    st_4018f0_1 *v12;  // eax
    st_4018f0_1 *v13;  // eax
    st_4018f0_0 *index;  // esi
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    st_4018f0_1 *v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0xc]
    st_4018f0_1 *v5;  // [bp-0x8], Other Possible Types: unsigned int
    st_4018f0_2 *v6;  // [bp-0x4]

    v2 = v7;
    v1 = v8;
    idx1 = a1;
    v10 = idx1;
    v11 = v10;
    do
    {
        v12 = v11;
        v13 = v12->padding_1;
        v11 = v13;
    } while (v12->field_0);
    v5 = v13 - v10->padding_1;
    *((unsigned int *)&idx->padding_444[72]) = *((int *)&idx->padding_444[72]) & 0xffafffff | 2621441;
    *((unsigned int *)&idx->padding_3d4[99]) = idx1->field_100;
    idx->field_438 = idx1->field_104;
    idx->field_43c = idx1->field_108;
    /* unsupported instruction */
    /* unsupported instruction */
    idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    index = &idx->padding_0[20];
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&index->padding_444[52]) = *((int *)&index->padding_444[52]) | 8;
    *((int *)&index->padding_0[64]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&index->padding_0[0x44]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = v15 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    switch (idx1->field_120)
    {
    case 1:
        idx->field_490 = idx->field_490 & 0xfffffffd;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v17 = v16 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    case 2:
        idx->field_490 = idx->field_490 & 0xfffffffd;
        goto LABEL_4019b5;
    case 3:
        idx->field_490 = idx->field_490 & 0xfffffffd;
        goto LABEL_4019d0;
    case 4:
        idx->field_490 = idx->field_490 | 2;
LABEL_4019d0:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v17 = v16 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    case 5:
        idx->field_490 = idx->field_490 | 2;
LABEL_4019b5:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v17 = v16 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    default:
        /* unsupported instruction */
        /* unsupported instruction */
        v18 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx1->field_110) * 0x100 & 0x4500;
        /* unsupported instruction */
        v19 = v16;
        v20 = _ccall(10, 13, (char)((((unsigned short)v19 & 7) * 0x800 | (unsigned short)v18 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v20 & 1)
            idx->field_490 = idx->field_490 | 2;
        else
            idx->field_490 = idx->field_490 & 0xfffffffd;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v17 = v19 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        break;
    }
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v21 = v17 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    if (idx1->field_130)
    {
        if (idx1->field_130 == 2)
        {
            switch (idx1->field_120)
            {
            case 2: case 5:
                node = idx1;
                if (idx1->field_0)
                {
                    do
                    {
                        v23 = v21 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        if (node->field_0 == 46)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    } while ((node += 1, *((int *)&idx->padding_3d4[99]) = (int)(/* unsupported instruction */ ? /* unsupported instruction */ : nan), /* unsupported instruction */
, v21 = v23 + 1, node->field_0));
                }
                break;
            case 3: case 4:
                iter = idx1;
                if (idx1->field_0)
                {
                    do
                    {
                        v25 = v21 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        if (iter->field_0 == 44)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    } while ((iter += 1, *((int *)&idx->padding_3d4[99]) = (int)(/* unsupported instruction */ ? /* unsupported instruction */ : nan), /* unsupported instruction */
, v21 = v25 + 1, iter->field_0));
                }
                break;
            default:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = v21 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_401b79;
            }
        }
    }
    else
    {
        if (idx1->field_120 == 2)
        {
            iter2 = idx1;
            if (idx1->field_0)
            {
                do
                {
                    v31 = v21 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (iter2->field_0 == 46)
                    {
                        v32 = v31 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v32 = v31 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    iter2 = iter2->padding_1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v21 = v32 + 2;
                } while (iter2->field_0);
            }
        }
        else if (idx1->field_120 - 3 <= 1)
        {
            iter1 = idx1;
            if (idx1->field_0)
            {
                do
                {
                    v28 = v21 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (iter1->field_0 == 44)
                    {
                        v29 = v28 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v29 = v28 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    iter1 = iter1->padding_1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v21 = v29 + 2;
                } while (iter1->field_0);
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v26 = v21 + 1;
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
            *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            goto LABEL_401b79;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v26 = v21 + 1;
LABEL_401b79:
    v33 = v26 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v34 = idx1->field_134;
    if (idx1->field_134)
    {
        v35 = v34 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v36 = v33 + 1;
        if (v34 != 2)
            goto LABEL_401bac;
        v33 = v36 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v34 = v35;
    }
    else
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
    }
    idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v36 = v33 + 1;
    v35 = v34;
LABEL_401bac:
    v37 = idx1;
    v5 = idx1;
    if (idx1->field_0)
    {
        do
        {
            v35 = _INSERT(v35, 0, v37->field_0);
            if (v37->field_0 == 10)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else if ((char)v35 == 32)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v38 = v36 + 2;
                switch (idx1->field_120)
                {
                case 0:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v39 = v38 + 2;
                    sub_402390();
                    break;
                case 1:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v39 = v38 + 2;
                    sub_402390();
                    break;
                case 2: case 5:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v39 = v38 + 2;
                    if ((char)v35 == 47)
                    {
                        sub_402390();
                        break;
                    }
                    else if ((char)v35 == 58)
                    {
                        sub_402390();
                        break;
                    }
                    else if ((char)v35 == 45)
                    {
                        sub_402390();
                        break;
                    }
                    else if ((char)v35 == 42)
                    {
                        sub_402390();
                        break;
                    }
                    else if ((char)v35 == 37)
                    {
                        sub_402390();
                        break;
                    }
                    else if ((char)v35 == 36)
                    {
                        sub_402390();
                        break;
                    }
                    else if ((char)v35 == 46)
                    {
                        sub_402390();
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        break;
                    }
                    else
                    {
                        sub_402390();
                        break;
                    }
                case 3: case 4:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v39 = v38 + 2;
                    v40 = v37->field_0;
                    if (v37->field_0 == 47)
                    {
                        sub_402390();
                        break;
                    }
                    else if (v40 == 46)
                    {
                        sub_402390();
                        break;
                    }
                    else if (v40 == 115)
                    {
                        sub_402390();
                        break;
                    }
                    else if (v40 == 42)
                    {
                        sub_402390();
                        break;
                    }
                    else if (v40 == 44)
                    {
                        sub_402390();
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
                        idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        break;
                    }
                    else
                    {
                        sub_402390();
                        break;
                    }
                default:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v39 = v38 + 2;
                    break;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)&index->padding_444[52]) = *((int *)&index->padding_444[52]) | 8;
                index->field_58 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&index->padding_5c[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if (idx1->field_124)
                {
                    idx2 = v41;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((unsigned int *)&idx->field_3d0[0]) = idx2->field_10c & 0xff000000;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v43 = idx2->field_10c;
                    *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx->field_3d0[3] = v43 >> 25;
                    sub_45a570(&g_4d47e8, &g_4d4804);
                    sub_459e50(1);
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
                    *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
                *((unsigned int *)&idx->field_3d0[0]) = v6->field_10c;
                sub_45a570(&g_4d47e8, &g_4d4804);
                v35 = sub_459e50(1);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v37 = v0;
                idx1 = v41;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx->padding_3d4[99]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v36 = v39 - 4;
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
            }
            v37 = v37->padding_1;
            v0 = v37;
        } while (v37->field_0);
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
    return v35;
}
