// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e210; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44e210_0 {
    char padding_0[256];
    unsigned int field_100;
    unsigned int field_104;
    unsigned int field_108;
    char padding_10c[4];
    unsigned int field_110;
    char padding_114[8];
    unsigned int field_11c;
    void* field_120;
} st_44e210_0;

typedef struct st_44e210_2 {
    unsigned int field_0;
} st_44e210_2;

typedef struct st_44e210_1 {
    char padding_0[48];
    struct st_44e210_2 *field_30;
    struct st_44e210_2 *field_34;
} st_44e210_1;

typedef struct st_44e210_4 {
    char padding_0[56];
    struct st_44e210_2 *field_38;
} st_44e210_4;

int sub_44e210(void)
{
    int i;  // ebp
    st_44e210_0 *idx;  // ecx
    int v22;  // edx
    int v23;  // eax
    unsigned int v24;  // edx
    unsigned int v25;  // esi
    char v26;  // al
    unsigned int v27;  // eax
    unsigned int v28;  // eax
    int v29;  // eax
    unsigned int v30;  // eax
    unsigned int v16;  // esi
    unsigned int v31;  // eax
    unsigned int v32;  // ebp
    unsigned short *iter;  // edx
    int j;  // ebp
    int v35;  // eax
    int v36;  // edi
    int v37;  // esi
    int v38;  // esi
    unsigned int v39;  // eax
    unsigned int v40;  // eax
    void* iter1;  // ecx
    char *node;  // esi
    int v42;  // ebp
    int v43;  // edx
    int l;  // ebp
    unsigned int v45;  // edi
    unsigned int v46;  // eax
    unsigned int v47;  // eax
    unsigned int v48;  // eax
    unsigned int v18;  // edi
    unsigned int v50;  // eax
    unsigned int v19;  // edi
    unsigned int v20;  // edi
    char *iter2;  // edi
    unsigned int v0;  // [bp-0x5c]
    int v1;  // [bp-0x54]
    char v2;  // [bp-0x40]
    unsigned int v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30]
    unsigned int v6;  // [bp-0x2c]
    char v7;  // [bp-0x20]
    st_44e210_4 **v8;  // [bp-0x14]
    unsigned int k;  // [bp-0x10], Other Possible Types: int
    int v10;  // [bp-0xc]
    unsigned int v11;  // [bp-0x8]
    st_44e210_1 **v12;  // [bp+0x4]
    int v13;  // [bp+0x10]

    i = v13;
    if (!idx->field_11c)
        return;
    *(v12)->field_30(v12, &v7, v16);
    v6 = idx->field_108;
    v3 = 0;
    v4 = 0;
    v5 = idx->field_104;
    if (*(v12)->field_34(v12, &v2, &v3, 0))
        return;
    iter1 = idx->field_120;
    v18 = idx->field_100;
    v0 = v19;
    v11 = idx->field_110;
    if (v3 == v18)
    {
        v20 = v18 - 21;
        if (v18 == 21)
        {
            node = v31 * v19 + v32 + k * 4;
            if (i > 0)
            {
                k = i;
                do
                {
                    v42 = v10;
                    v43 = 0;
                    if (v42 > 0)
                    {
                        v43 = v42;
                        do
                        {
                            l = v42;
                            v45 = (char)iter1[3];
                            v46 = *(node);
                            v47 = node[1];
                            *(node) = (char)((int)(v45 * (*((char *)iter1) - v46)) >> 8) + (char)v46;
                            v48 = node[2];
                            node[1] = (char)((int)(v45 * ((char)iter1[1] - v47)) >> 8) + (char)v47;
                            node[2] = (char)((int)(v45 * ((char)iter1[2] - v48)) >> 8) + (char)v48;
                            node += 4;
                            iter1 += 4;
                            v42 = l - 1;
                        } while (l != 1);
                    }
                    v50 = v43 * 4;
                    iter1 += v11 - v50;
                    node = &node[v19 + -1 * v50];
                    k -= 1;
                } while (k != 1);
            }
        }
        else if (v20 == 4)
        {
            iter = v31 * v19 + v32 + k * 2;
            if (i > 0)
            {
                do
                {
                    j = i;
                    v35 = v10;
                    v36 = 0;
                    if (v35 > 0)
                    {
                        v37 = v35;
                        v36 = v37;
                        do
                        {
                            v38 = v37;
                            v39 = *((short *)iter1);
                            if (v39 & 0x8000)
                                *(iter) = v39;
                        } while ((iter += 2, iter1 += 2, v37 = (int)(v38 - 1), v38 != 1));
                    }
                    v40 = v36 * 2;
                    iter1 += v11 - v40;
                    iter = (char *)iter + v19 - v40;
                    i = j - 1;
                } while (j != 1);
            }
        }
        else if (v20 != 5)
        {
            return;
        }
        else
        {
            iter2 = v31 * v19 + v32 + k * 2;
            if (i > 0)
            {
                i = i;
                do
                {
                    v22 = v10;
                    v23 = 0;
                    if (v22 > 0)
                    {
                        v23 = v22;
                        v1 = v23;
                        k = v23;
                        do
                        {
                            v24 = *(iter2) >> 4;
                            v25 = (char)((char)iter1[1]) >> 4;
                            v26 = (int)(v25 * (((char)(*((char *)iter1)) >> 4) - v24)) >> 4;
                            *(iter2) = (v26 + (char)v24) * 16;
                            v27 = (v26 + (char)v24) * 16 & 15;
                            v28 = iter2[1];
                            *(iter2) = *(iter2) | (char)((int)(v25 * ((*((char *)iter1) & 15) - v27)) >> 4) + (char)v27;
                            if ((v28 >> 2 & 60) + v25 >= 16)
                                v29 = 15;
                        } while ((iter2[1] = (char)((int)(v25 * ((unsigned int)(char)((char)iter1[1] & 15) - (v28 & 15))) >> 4) + (char)(v28 & 15) | (char)v29 * 16, iter2 += 2, iter1 += 2, k = (int)(k - 1), k != 1));
                    }
                    v30 = v23 * 2;
                    iter1 += v11 - v30;
                    iter2 = &iter2[v19 + -1 * v30];
                    i -= 1;
                } while (i != 1);
            }
        }
    }
    *(v8)->field_38(v8);
    return;
}
