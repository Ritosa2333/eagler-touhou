// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490bac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_490bac_0 {
    char padding_0[112];
    unsigned int field_70;
} st_490bac_0;


unsigned int sub_490bac(unsigned int a0, unsigned int a1, int a2, int a3)
{
    int v3;  // ebx
    int v4;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    int *v0;  // [bp-0x14]
    st_490bac_0 *idx;  // [bp-0xc]
    char v2;  // [bp-0x8]

    sub_46d3b4(a3, v3);
    if (!a2)
    {
        if (!v2)
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
                *((unsigned int *)sub_46e96f(v4)) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
                goto LABEL_490c83;
            }
            if (!v0[4])
            {
                v5 = sub_48d85c(a0, a1, a2, &v0);
                goto LABEL_490c96;
            }
            else
            {
                v6 = sub_490b6a(&v0, v0[4], 4097, a0, a2, a1, a2, v0[2]);
                if (!v6)
                {
                    *((unsigned int *)sub_46e96f()) = 22;
LABEL_490c83:
                    if (v2)
                        idx->field_70 = idx->field_70 & 0xfffffffd;
                    v5 = 0x7fffffff;
                }
                else
                {
                    v5 = v6 - 2;
LABEL_490c96:
                    if (v2)
                        idx->field_70 = idx->field_70 & 0xfffffffd;
                }
            }
            return v5;
        }
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        if (!v2)
            return 0x7fffffff;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return 0x7fffffff;
    }
}
