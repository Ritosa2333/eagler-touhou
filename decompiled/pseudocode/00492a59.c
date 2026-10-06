// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492a59; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492a59_4 {
    char padding_0[144];
    unsigned int field_90;
} st_492a59_4;

typedef struct st_492a59_1 {
    char padding_0[12];
    struct st_492a59_0 *field_c;
} st_492a59_1;

typedef struct st_492a59_2 {
    unsigned int field_0;
    char padding_4[12];
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    struct st_492a59_1 *field_1c;
} st_492a59_2;

typedef struct st_492a59_3 {
    char padding_0[8];
    char field_8;
} st_492a59_3;

typedef struct st_492a59_0 {
    int field_0;
    unsigned int field_4;
    char field_8;
} st_492a59_0;


unsigned int sub_492a59(st_492a59_2 **a0, st_492a59_3 *a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned int a8, unsigned int a9, unsigned int a10, unsigned int a11, unsigned int a12, unsigned int a13, unsigned int a14, unsigned int a15, unsigned int a16, unsigned int a17, unsigned int a18, unsigned int a19, unsigned int a20, unsigned int a21, unsigned int a22, unsigned int a23, unsigned int a24, unsigned int a25, unsigned int a26, unsigned int a27, unsigned int a28, unsigned int a29, unsigned int a30)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    st_492a59_2 *idx;  // esi
    unsigned int v8;  // edi
    int v10;  // edi
    unsigned int iter;  // ebx
    st_492a59_4 *idx1;  // eax
    unsigned int v13;  // eax
    st_492a59_4 *index;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    st_492a59_3 *v4;  // [bp-0x10]

    if (!a0)
        return 0;
    v2 = v5;
    v1 = v6;
    idx = *(a0);
    v0 = v8;
    if (a1 && a1->field_8 || idx->field_0 != 3762507597 && (char)a2 & 64)
    {
        if (idx->field_0 == 3765269347 && idx->field_10 == 3 && (idx->field_14 == 429065504 || idx->field_14 == 429065505 || idx->field_14 == 429065506))
        {
            if (!idx->field_1c)
            {
                if (*((int *)(sub_475467() + 0x88)))
                    idx = *((int *)(sub_475467() + 0x88));
                else
                    return 0;
            }
            v4 = a1;
            v10 = idx->field_1c->field_c->field_0;
            v3 = a2 | 0x80000000;
            for (iter = &idx->field_1c->field_c->field_4; v10 > NULL; iter += 4)
            {
                a0 = *((int *)iter);
                if (!sub_491fb3(&v3, *((int *)iter), idx->field_1c))
                {
                    v10 -= 1;
                }
                else
                {
                    idx1 = sub_475467();
                    idx1->field_90 = idx1->field_90 + 1;
                    if (!a3)
                        return 1;
                    sub_4929c7(idx, a3, &v3, a0);
                    return 1;
                }
            }
        }
    }
    else
    {
        if (idx->field_0 != 3765269347 || idx->field_10 != 3 || !(v13 = idx->field_14, (idx->field_14 == 429065504 || idx->field_14 == 429065505 || idx->field_14 == 429065506) && !idx->field_1c && !*((int *)(sub_475467() + 0x88))))
        {
            index = sub_475467();
            index->field_90 = index->field_90 + 1;
            return 1;
        }
    }
    return 0;
}
