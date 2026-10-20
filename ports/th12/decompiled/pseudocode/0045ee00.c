// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45ee00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45ee00_2 {
    struct st_45ee00_0 *field_0;
} st_45ee00_2;

typedef struct st_45ee00_1 {
    unsigned int field_0;
} st_45ee00_1;

typedef struct st_45ee00_3 {
    char padding_0[48];
    struct st_45ee00_1 *field_30;
    char padding_34[4];
    struct st_45ee00_1 *field_38;
} st_45ee00_3;

typedef struct st_45ee00_0 {
    char padding_0[72];
    struct st_45ee00_1 *field_48;
} st_45ee00_0;


int sub_45ee00(void)
{
    unsigned int iter;  // ebx
    st_45ee00_2 **v10;  // eax
    void* iter1;  // ecx
    unsigned int iter2;  // esi
    void* v21;  // eax
    void* v22;  // eax
    unsigned int v23;  // eax
    unsigned int v24;  // edx
    void* v25;  // ebp
    unsigned int v26;  // esi
    unsigned int v27;  // ecx
    unsigned int v28;  // edi
    unsigned int v11;  // edi
    unsigned int v29;  // eax
    unsigned int v30;  // eax
    unsigned int v31;  // eax
    unsigned int v32;  // esi
    unsigned int v33;  // eax
    unsigned int v34;  // eax
    unsigned int v35;  // eax
    unsigned int v36;  // eax
    unsigned int v37;  // eax
    unsigned int v38;  // edx
    st_45ee00_3 **v12;  // esi
    void* v39;  // ebp
    unsigned int v40;  // esi
    unsigned int v41;  // ecx
    unsigned int v42;  // edi
    unsigned int v43;  // eax
    unsigned int v44;  // eax
    unsigned int v45;  // eax
    unsigned int v46;  // esi
    unsigned int v47;  // eax
    unsigned int v48;  // eax
    unsigned int v13;  // ebp
    unsigned int v49;  // eax
    unsigned int v50;  // eax
    unsigned int v51;  // eax
    unsigned int v52;  // edx
    void* v53;  // esi
    unsigned int v54;  // edi
    unsigned int v55;  // ecx
    unsigned int v56;  // ebp
    unsigned int v57;  // eax
    unsigned int v58;  // edi
    unsigned int v14;  // ebx
    void* v59;  // eax
    unsigned int v60;  // eax
    unsigned int v61;  // eax
    unsigned int v62;  // eax
    unsigned int v15;  // ecx
    unsigned int v16;  // eax
    void* v17;  // edi
    unsigned int v18;  // ebp
    unsigned int v0;  // [bp-0x64]
    unsigned int v1;  // [bp-0x60]
    unsigned int v2;  // [bp-0x50]
    char node;  // [bp-0x4c], Other Possible Types: unsigned int
    char v5;  // [bp-0x44]
    char v6;  // [bp-0x30], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0x2c]

    iter = 0;
    *(v10)->field_0->field_48(*(v10), 0, &v5, v11, v12, v13, v14, 0);
    *(v12)->field_30(v12, &v6);
    (*((int *)(*((int *)&v5) + 52)))(&v5, &node, 0, 0);
    switch (v14)
    {
    case 0: case 21:
        v15 = 0;
        v0 = 0;
        if (v7 > 0)
        {
            v16 = v6;
            do
            {
                v17 = &(&v5)[0 * v15];
                v18 = 0;
                v1 = 0;
                if (v16 > 0)
                {
                    iter1 = v17 - 2;
                    do
                    {
                        if (!(char)iter1[5])
                        {
                            iter2 = 0;
                            node = 0;
                            v2 = 0;
                            if (v18 && (char)iter1[1])
                            {
                                iter = *((char *)iter1);
                                v2 = *((char *)iter1 - 1);
                                node = *((char *)iter1 - 2);
                                iter2 = 1;
                            }
                            if (v18 < v16 - 1 && (char)iter1[9])
                            {
                                v2 += (char)iter1[7];
                                iter += (char)iter1[8];
                                node += (char)iter1[6];
                                iter2 += 1;
                            }
                            if (v0 > 0)
                            {
                                v21 = v17 - 0;
                                if ((char)v21[3])
                                {
                                    v18 = v1;
                                    iter += (char)v21[2];
                                    v2 += (char)v21[1];
                                    node += *((char *)v21);
                                    iter2 += 1;
                                }
                            }
                            if (v0 < v7 - 1)
                            {
                                v22 = v17;
                                if ((char)v17[3])
                                {
                                    v18 = v1;
                                    iter += (char)v22[2];
                                    v2 += (char)v22[1];
                                    node += *((char *)v22);
                                    iter2 += 1;
                                }
                            }
                            if (iter2 > 1)
                            {
                                iter /= iter2;
                                v2 /= iter2;
                                node /= iter2;
                            }
                            *((char *)&iter1[4]) = iter;
                            *((char *)&iter1[3]) = v2;
                            *((char *)v17) = node;
                            iter = 0;
                            v16 = v6;
                        }
                        v1 = v18 + 1;
                        v17 += 4;
                        iter1 += 4;
                        v18 = v1;
                    } while (v18 < v16);
                    v15 = v0;
                }
                v0 = v15 + 1;
                v15 = v0;
            } while (v15 < v7);
        }
        break;
    case 25:
        v23 = 0;
        v0 = 0;
        if (v7 > 0)
        {
            v24 = v6;
            do
            {
                v25 = &(&v5)[0 * v23];
                v26 = 0;
                v1 = 0;
                if (v24 > 0)
                {
                    do
                    {
                        if (!(*((short *)v25) & 0x8000))
                        {
                            v27 = 0;
                            v28 = 0;
                            node = 0;
                            if (v26)
                            {
                                v29 = *((short *)((char *)v25 - 2));
                                if (v29 & 0x8000)
                                {
                                    v30 = (unsigned short)v29;
                                    v27 = v30 >> 10 & 31;
                                    iter = v30 >> 5 & 31;
                                    node = v30 & 31;
                                    v28 = 1;
                                }
                            }
                            if (v26 < v24 - 1)
                            {
                                v31 = (short)v25[2];
                                if (v31 & 0x8000)
                                {
                                    v32 = (unsigned short)v31;
                                    node += v32 & 31;
                                    v27 += v32 >> 10 & 31;
                                    iter += v32 >> 5 & 31;
                                    v28 += 1;
                                }
                            }
                            if (v0 > 0)
                            {
                                v33 = *((short *)((char *)v25 + -2 * 0 / 2));
                                if (v33 & 0x8000)
                                {
                                    v34 = (unsigned short)v33;
                                    v27 += v34 >> 10 & 31;
                                    node += v34 & 31;
                                    iter += v34 >> 5 & 31;
                                    v28 += 1;
                                }
                            }
                            if (v0 < v7 - 1)
                            {
                                v35 = *((short *)((char *)v25 + 2 * 0 / 2));
                                if (v35 & 0x8000)
                                {
                                    v36 = (unsigned short)v35;
                                    v27 += v36 >> 10 & 31;
                                    node += v36 & 31;
                                    iter += v36 >> 5 & 31;
                                    v28 += 1;
                                }
                            }
                            if (v28 > 1)
                            {
                                v27 /= v28;
                                iter /= v28;
                                node /= v28;
                            }
                            v26 = v1;
                            *((unsigned short *)v25) = ((((char)v27 & 31) * 32 | (char)iter & 31) * 32 | *((short *)v25) & 32799) & 0xffe0 | (char)node & 31;
                            iter = 0;
                            v24 = v6;
                        }
                        v1 = v26 + 1;
                        v25 += 2;
                        v26 = v1;
                    } while (v26 < v24);
                    v23 = v0;
                }
                v0 = v23 + 1;
                v23 = v0;
            } while (v23 < v7);
        }
        break;
    case 26:
        v37 = 0;
        v0 = 0;
        if (v7 > 0)
        {
            v38 = v6;
            do
            {
                v39 = &(&v5)[0 * v37];
                v40 = 0;
                v1 = 0;
                if (v38 > 0)
                {
                    do
                    {
                        if (!(*((short *)v39) & 0xf000))
                        {
                            v41 = 0;
                            v42 = 0;
                            node = 0;
                            if (v40)
                            {
                                v43 = *((short *)((char *)v39 - 2));
                                if (v43 & 0xf000)
                                {
                                    v44 = (unsigned short)v43;
                                    v41 = v44 >> 8 & 15;
                                    iter = v44 >> 4 & 15;
                                    node = v44 & 15;
                                    v42 = 1;
                                }
                            }
                            if (v40 < v38 - 1)
                            {
                                v45 = (short)v39[2];
                                if (v45 & 0xf000)
                                {
                                    v46 = (unsigned short)v45;
                                    node += v46 & 15;
                                    v41 += v46 >> 8 & 15;
                                    iter += v46 >> 4 & 15;
                                    v42 += 1;
                                }
                            }
                            if (v0 > 0)
                            {
                                v47 = *((short *)((char *)v39 + -2 * 0 / 2));
                                if (v47 & 0xf000)
                                {
                                    v48 = (unsigned short)v47;
                                    v41 += v48 >> 8 & 15;
                                    node += v48 & 15;
                                    iter += v48 >> 4 & 15;
                                    v42 += 1;
                                }
                            }
                            if (v0 < v7 - 1)
                            {
                                v49 = *((short *)((char *)v39 + 2 * 0 / 2));
                                if (v49 & 0xf000)
                                {
                                    v50 = (unsigned short)v49;
                                    v41 += v50 >> 8 & 15;
                                    node += v50 & 15;
                                    iter += v50 >> 4 & 15;
                                    v42 += 1;
                                }
                            }
                            if (v42 > 1)
                            {
                                v41 /= v42;
                                iter /= v42;
                                node /= v42;
                            }
                            v40 = v1;
                            *((unsigned short *)v39) = ((((char)v41 & 15) * 16 | (char)iter & 15) * 16 | *((short *)v39) & 61455) & 0xfff0 | (char)node & 15;
                            iter = 0;
                            v38 = v6;
                        }
                        v1 = v40 + 1;
                        v39 += 2;
                        v40 = v1;
                    } while (v40 < v38);
                    v37 = v0;
                }
                v0 = v37 + 1;
                v37 = v0;
            } while (v37 < v7);
        }
        break;
    case 29:
        v51 = 0;
        v0 = 0;
        if (v7 > 0)
        {
            v52 = v6;
            do
            {
                v53 = &(&v5)[0 * v51];
                v54 = 0;
                v1 = 0;
                if (v52 > 0)
                {
                    do
                    {
                        if (!(char)v53[1])
                        {
                            v55 = 0;
                            v56 = 0;
                            node = 0;
                            if (v54 && *((char *)v53 - 1))
                            {
                                v57 = *((short *)((char *)v53 - 2));
                                v55 = v57 >> 5 & 7;
                                iter = v57 >> 2 & 7;
                                node = v57 & 3;
                                v56 = 1;
                            }
                            if (v54 < v52 - 1 && (char)v53[3])
                            {
                                v58 = (short)v53[2];
                                node += v58 & 3;
                                v54 = v1;
                                v55 += v58 >> 5 & 7;
                                iter += v58 >> 2 & 7;
                                v56 += 1;
                            }
                            if (v0 > 0)
                            {
                                v59 = v53 - 0 / 2 * 2;
                                if ((char)v59[1])
                                {
                                    v60 = *((short *)v59);
                                    v55 += v60 >> 5 & 7;
                                    node += v60 & 3;
                                    iter += v60 >> 2 & 7;
                                    v56 += 1;
                                }
                            }
                            if (v0 < v7 - 1)
                            {
                                v61 = 0 / 2;
                                if (*((char *)v53 + 2 * v61 + 1))
                                {
                                    v62 = *((short *)((char *)v53 + 2 * v61));
                                    v55 += v62 >> 5 & 7;
                                    node += v62 & 3;
                                    iter += v62 >> 2 & 7;
                                    v56 += 1;
                                }
                            }
                            if (v56 > 1)
                            {
                                v55 /= v56;
                                iter /= v56;
                                node /= v56;
                            }
                            *((unsigned short *)v53) = ((((char)v55 & 7) * 8 | (char)iter & 7) * 4 | *((short *)v53) & 0xff03) & 0xfffc | (char)node & 3;
                            iter = 0;
                            v52 = v6;
                        }
                        v1 = v54 + 1;
                        v53 += 2;
                        v54 = v1;
                    } while (v54 < v52);
                    v51 = v0;
                }
                v0 = v51 + 1;
                v51 = v0;
            } while (v51 < v7);
        }
        break;
    }
    *(v12)->field_38(v12);
    (*((int *)(*((int *)NULL) + 8)))(0);
    return;
}
