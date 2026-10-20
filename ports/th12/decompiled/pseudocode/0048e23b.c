// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e23b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48e23b_1 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
    int field_c;
} st_48e23b_1;

typedef struct st_48e23b_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48e23b_0;


unsigned int sub_48e23b(int a0, int a1, int a2, int a3)
{
    int v4;  // ebx
    int v5;  // esi
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    char v0;  // [bp-0x14]
    st_48e23b_1 *v1;  // [bp-0x10]
    st_48e23b_0 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]

    sub_46d3b4(a3, v4);
    if (!a2)
    {
        if (!v3)
            return 0;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return 0;
    }
    else
    {
        if (a0 && a1)
        {
            if (a2 > 0x7fffffff)
            {
                *((unsigned int *)sub_46e96f(v5)) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
LABEL_48e306:
                if (v3)
                    idx->field_70 = idx->field_70 & 0xfffffffd;
                v6 = 0x7fffffff;
            }
            else
            {
                if (!v1->field_8)
                {
                    v6 = sub_490bac(a0, a1, a2, a3);
                }
                else
                {
                    v7 = sub_490b6a(&v0, v1->field_c, 4097, a0, a2, a1, a2, v1->field_4);
                    if (!v7)
                        goto LABEL_48e306;
                    v6 = v7 - 2;
                }
                if (v3)
                    idx->field_70 = idx->field_70 & 0xfffffffd;
            }
            return v6;
        }
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        if (!v3)
            return 0x7fffffff;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return 0x7fffffff;
    }
}
