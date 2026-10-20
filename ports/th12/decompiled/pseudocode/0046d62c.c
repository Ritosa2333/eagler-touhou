// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d62c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d62c_0 {
    int field_0;
    int field_4;
    int field_8;
    int field_c;
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
} st_46d62c_0;

typedef struct st_46d62c_1 {
    char padding_0[4];
    int field_4;
} st_46d62c_1;


unsigned int sub_46d62c(st_46d62c_0 *idx, st_46d62c_1 *index)
{
    int v9;  // esi
    int v10;  // ebx
    unsigned int v19;  // edi
    int v20;  // ebx
    unsigned int v21;  // eax
    int v22;  // eax
    unsigned int v25;  // edi
    int v26;  // eax
    int v27;  // edi
    int v28;  // edx
    int v11;  // edi
    int v29;  // ebx
    int v30;  // eax
    unsigned int v33;  // edi
    int v34;  // eax
    int v35;  // edi
    int v36;  // ebx
    int v37;  // eax
    int v12;  // eax
    unsigned int v40;  // ecx
    unsigned int v41;  // 4101
    unsigned int v42;  // eax
    unsigned int v43;  // eax
    int v44;  // eax
    unsigned int v13;  // eax
    int v14;  // ecx
    unsigned int v15;  // edi
    unsigned int v16;  // eax
    unsigned int v17;  // eax
    unsigned int v18;  // ecx
    unsigned int v0;  // [bp-0x88]
    unsigned int v1;  // [bp-0x28]
    unsigned int v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]

    v7 = 0;
    v6 = 0;
    v8 = 0;
    if (!idx)
    {
        v2 = 22;
        *((unsigned int *)sub_46e96f(v9, v10)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 22;
    }
    sub_477420(idx, 0xff, 36, v11);
    if (!index)
    {
        v1 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        goto LABEL_46d6bf;
    }
    v12 = index->field_4;
    if (v12 <= NULL && (v12 < NULL || index->padding_0 < NULL) || v12 >= 7 && (v12 > 7 || index->padding_0 > 0x93406fff))
    {
        v1 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
LABEL_46d6bf:
        v13 = 22;
    }
    else
    {
        sub_476f80();
        if (sub_4772b2(&v7))
            sub_470dc6(0, 0, 0, 0, 0);
        if (sub_4772eb(&v6))
            sub_470dc6(0, 0, 0, 0, 0);
        if (sub_477324(&v8))
            sub_470dc6(0, 0, 0, 0, 0);
        v14 = index->field_4;
        v15 = (unsigned int)index->padding_0;
        if (v14 >= NULL && (v14 > NULL || v15 > 259200))
        {
            v16 = v8;
            v4 = v15 - v16;
            v5 = v14 - ((int)(v16) >> 31) - (v15 < v16);
            v13 = sub_477048(idx, &v4);
            if (!v13)
            {
                if (!v7 || !sub_477007(idx))
                    goto LABEL_46d89e;
                v17 = v6;
                v3 = v4 - v17;
                v5 = v5 - ((int)(v17) >> 31) - (v4 < v17);
                v13 = sub_477048(idx, &v3);
                if (!v13)
                {
                    idx->field_20 = 1;
LABEL_46d89e:
                    v13 = 0;
                }
            }
        }
        else
        {
            v13 = sub_477048(idx, index);
            if (v13)
                return v13;
            if (v7 && sub_477007(idx))
            {
                v18 = v8 + v6;
                idx = (int)(v18) >> 31;
                v19 = idx->field_0 - v18;
                v20 = (idx->field_0 >> 31) - idx - (idx->field_0 < v18);
                idx->field_20 = 1;
            }
            else
            {
                v21 = v8;
                v19 = idx->field_0 - v21;
                v20 = (idx->field_0 >> 31) - ((int)(v21) >> 31) - (idx->field_0 < v21);
            }
            v22 = sub_477550(v19, v20, 60, 0);
            idx->field_0 = v22;
            if (v22 < NULL)
            {
                idx->field_0 = v22 + 60;
                v20 = v20 - 1 + (v19 - 60 < v19);
                v19 -= 60;
            }
            v25 = sub_4774a0(v19, v20, 60, 0);
            v26 = idx->field_4;
            v27 = v25 + v26;
            v29 = v28 + (v26 >> 31) + (v25 + v26 < v25);
            v30 = sub_477550(v27, v29, 60, 0);
            idx->field_4 = v30;
            if (v30 < NULL)
            {
                idx->field_4 = v30 + 60;
                v29 = v29 - 1 + (v27 - 60 < v27);
                v27 -= 60;
            }
            v33 = sub_4774a0(v27, v29, 60, 0);
            v34 = idx->field_8;
            v35 = v33 + v34;
            v36 = v28 + (v34 >> 31) + (v33 + v34 < v33);
            v37 = sub_477550(v35, v36, 24, 0);
            idx->field_8 = v37;
            if (v37 < NULL)
            {
                idx->field_8 = v37 + 24;
                v36 = v36 - 1 + (v35 - 24 < v35);
                v35 -= 24;
            }
            v40 = sub_4774a0(v35, v36, 24, 0);
            if (v28 >= NULL)
            {
                v41 = _ccall(14, 15, v28, 0, 0);
                if (!(v41 & 1) || v40)
                {
                    v42 = idx->field_18 + v40;
                    v0 = 7;
                    idx->field_c = idx->field_c + v40;
                    idx->field_18 = v42 % v0;
LABEL_46d89b:
                    idx->field_1c = idx->field_1c + v40;
                    goto LABEL_46d89e;
                }
                else
                {
                    if (v28 <= 0 && v28 < 0)
                        goto LABEL_46d8af;
                    goto LABEL_46d89e;
                }
            }
            else
            {
LABEL_46d8af:
                v43 = v40 + idx->field_18 + 7;
                v0 = 7;
                idx->field_c = idx->field_c + v40;
                v44 = idx->field_c;
                idx->field_18 = v43 % v0;
                if (v44 > 0)
                    goto LABEL_46d89b;
                idx->field_1c = idx->field_1c + v40 + 365;
                idx->field_14 = idx->field_14 - 1;
                idx->field_c = v44 + 31;
                idx->field_10 = 11;
                goto LABEL_46d89e;
            }
        }
    }
    return v13;
}
