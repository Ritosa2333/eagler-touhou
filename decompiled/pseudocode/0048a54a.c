// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a54a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48a54a_1 {
    char padding_0[188];
    struct st_48a54a_2 *field_bc;
} st_48a54a_1;

typedef struct st_48a54a_2 {
    struct st_48a54a_0 *field_0;
} st_48a54a_2;

typedef struct st_48a54a_4 {
    char padding_0[4];
    unsigned int field_4;
} st_48a54a_4;

typedef struct st_48a54a_0 {
    char field_0;
} st_48a54a_0;

typedef struct st_48a54a_3 {
    char padding_0[112];
    unsigned int field_70;
} st_48a54a_3;


unsigned int sub_48a54a(st_48a54a_4 *a0, char *a1, int a2, int a3, unsigned int a4, int a5)
{
    int v10;  // edi
    int v11;  // esi
    char *v20;  // esi
    unsigned int v21;  // ebx
    char *v22;  // esi
    char *iter;  // esi
    unsigned int v24;  // eax
    unsigned int v25;  // ecx
    char *iter1;  // eax
    char *v27;  // esi
    unsigned int v29;  // eax
    char *iter2;  // esi
    unsigned int v30;  // 4106
    int v31;  // eax
    int v32;  // edx
    unsigned int v33;  // 4140
    unsigned int v34;  // 4101
    char *node;  // esi
    char *v37;  // edi
    unsigned int v39;  // edx
    unsigned int *v13;  // eax
    int v40;  // ecx
    unsigned int v41;  // 4101
    unsigned int v43;  // 4101
    unsigned int v14;  // ebx
    char *v15;  // ebx
    unsigned int v16;  // eax
    char *v17;  // esi
    char *v18;  // eax
    char *v19;  // esi
    unsigned int v0;  // [bp-0x3c]
    unsigned int v1;  // [bp-0x38]
    st_48a54a_1 *v2;  // [bp-0x28]
    st_48a54a_3 *idx;  // [bp-0x20]
    char v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0x10]
    unsigned int v8;  // [bp-0xc]
    unsigned int v9;  // [bp-0x8]

    v5 = 0x3ff;
    v9 = 48;
    sub_46d3b4(a5, v10, v11);
    if (a3 < 0)
        a3 = 0;
    iter2 = a1;
    if (!iter2 || a2 <= 0)
    {
        v13 = sub_46e96f();
        v1 = 22;
    }
    else
    {
        *(iter2) = 0;
        if (a2 <= a3 + 11)
        {
            v13 = sub_46e96f();
            v1 = 0x22;
        }
        else
        {
            v7 = (unsigned int)a0->padding_0;
            v1 = v14;
            if ((a0->field_4 & 0x7ff00000) == 0x7ff00000)
            {
                v15 = iter2 + 2;
                v16 = sub_48a52a(a0, v15, (a2 == -0x1 ? 0xffffffff : a2 - 2), a3, 0);
                if (v16)
                {
                    *(iter2) = 0;
                    if (v4)
                        idx->field_70 = idx->field_70 & 0xfffffffd;
                }
                else
                {
                    if (*(v15) == 45)
                    {
                        *(iter2) = 45;
                        iter2 += 1;
                    }
                    *(iter2) = 48;
                    v17 = iter2 + 1;
                    *(v17) = ((char)((!a4) + 1) & 224) + 120;
                    v18 = sub_46d260(v17 + 1, 101);
                    if (v18)
                    {
                        *(v18) = ((char)((!a4) + 1) & 224) + 112;
                        v18[3] = 0;
                    }
LABEL_48a8a9:
                    if (v4)
                        idx->field_70 = idx->field_70 & 0xfffffffd;
                    v16 = 0;
                }
            }
            else
            {
                if (a0->field_4 & 0x80000000)
                {
                    *(iter2) = 45;
                    iter2 += 1;
                }
                *(iter2) = 48;
                v19 = iter2 + 1;
                *(v19) = ((char)((!a4) + 1) & 224) + 120;
                v20 = v19 + 1;
                v21 = (-(0 < a4) & 0xffffffe0) + 39;
                if (!(a0->field_4 & 0x7ff00000))
                {
                    *(v20) = 48;
                    v22 = v20 + 1;
                    v5 = (!a0->padding_0 && !(a0->field_4 & 0xfffff) ? 0 : 0x3fe);
                }
                else
                {
                    *(v20) = 49;
                    v22 = v20 + 1;
                }
                iter = v22 + 1;
                a1 = v22;
                if (!a3)
                    *(a1) = 0;
                else
                    *(a1) = v2->field_bc->field_0->field_0;
                v8 = a0->field_4 & 0xfffff;
                if (a0->field_4 & 0xfffff || a0->padding_0 > 0)
                {
                    v7 = 0;
                    v8 = 0xf0000;
                    do
                    {
                        if (a3 <= 0)
                            break;
                        v24 = (unsigned short)sub_48e1b0() + 48;
                        if ((unsigned short)v24 > 57)
                            v24 += v21;
                        v25 = v8;
                        v9 -= 4;
                        *(iter) = v24;
                        iter += 1;
                        a3 -= 1;
                        v7 = CONCAT(v25, v7) >> 4;
                        v8 = v25 >> 4;
                    } while ((unsigned short)v9 >= 0);
                    if ((unsigned short)v9 >= 0 && (unsigned short)sub_48e1b0() > 8)
                    {
                        for (iter1 = iter - 1; *(iter1) == 0x66 || *(iter1) == 70; iter1 -= 1)
                        {
                            *(iter1) = 48;
                        }
                        if (iter1 == a1)
                        {
                            *(iter1 - 1) = *(iter1 - 1) + 1;
                        }
                        else if (*(iter1) == 57)
                        {
                            *(iter1) = (char)v21 + 58;
                        }
                        else
                        {
                            *(iter1) = *(iter1) + 1;
                        }
                    }
                }
                if (a3 > 0)
                {
                    sub_477420(iter, 48, a3);
                    iter = &iter[a3];
                }
                if (!*(a1))
                    iter = a1;
                *(iter) = ((char)((!a4) + 1) & 224) + 112;
                v27 = iter + 1;
                v29 = sub_48e1b0() & 0x7ff;
                v30 = v5;
                v31 = v29 - v30;
                v0 = 0;
                v32 = 0xffffffff - (v29 < v30);
                v33 = _ccall(8, 12, 0, 0 ^ v29 < v30, v29 < v30);
                if (!(v33 & 1) && !(v34 = (unsigned int)_ccall(14, 12, 0, 0 ^ (unsigned int)(char)(v29 < v30), (unsigned int)(char)(v29 < v30)), v34 & 1 && v31 < 0))
                {
                    *(v27) = 43;
                    node = v27 + 1;
                }
                else
                {
                    *(v27) = 45;
                    node = v27 + 1;
                    v32 = -(v32 + (0 < v31));
                    v31 = -(v31);
                }
                v37 = node;
                *(node) = 48;
                if (v32 >= 0 && (v32 > 0 || v31 >= 1000))
                {
                    *(node) = (char)sub_48e0d0(v31, v32, 1000, 0) + 48;
                    node += 1;
                    v6 = v39;
                    v31 = v40;
                    v32 = 0;
                    if (node != v37)
                        goto LABEL_48a863;
                }
                if (v32 < 0 || (v41 = (unsigned int)_ccall(14, 15, v32, 0, 0), v41 & 1 && v31 < 100))
                    goto LABEL_48a87a;
LABEL_48a863:
                *(node) = (char)sub_48e0d0(v31, v32, 100, 0) + 48;
                v6 = v39;
                node += 1;
                v31 = v40;
                v32 = 0;
LABEL_48a87a:
                if (node != v37 || v32 >= 0 && !(v43 = (unsigned int)_ccall(14, 15, v32, 0, 0), v43 & 1 && v31 < 10))
                {
                    *(node) = (char)sub_48e0d0(v31, v32, 10, 0) + 48;
                    v6 = v39;
                    node += 1;
                    v31 = v40;
                    v6 = 0;
                }
                *(node) = (char)v31 + 48;
                node[1] = 0;
                goto LABEL_48a8a9;
            }
            return v16;
        }
    }
    *(v13) = v1;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v4)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v1;
}
