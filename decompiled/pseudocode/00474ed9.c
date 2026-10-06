// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474ed9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474ed9_3 {
    unsigned int field_0;
} st_474ed9_3;

typedef struct st_474ed9_0 {
    char padding_0[4];
    unsigned int field_4;
} st_474ed9_0;

typedef struct st_474ed9_2 {
    struct st_474ed9_0 *field_0;
    struct st_474ed9_3 *field_4;
} st_474ed9_2;

st_474ed9_2 * sub_474ed9(int a0, unsigned int a1)
{
    unsigned int v1;  // edi
    st_474ed9_2 *idx;  // esi
    st_474ed9_0 *v3;  // eax
    unsigned int *v4;  // eax
    unsigned int v0;  // [bp-0xc]

    v0 = v1;
    if (a0 > 5 || !a1)
        return NULL;
    idx = sub_477813(8, 1);
    if (idx)
    {
        v3 = sub_477813(216, 1);
        idx->field_0 = v3;
        if (!v3)
        {
            sub_46c941(idx);
        }
        else
        {
            v4 = sub_477813(544, 1);
            idx->field_4 = v4;
            if (!v4)
            {
                sub_46c941(idx->field_0);
                sub_46c941(idx);
            }
            else
            {
                sub_47411d();
                if (!sub_474cbe(a0))
                {
                    sub_474084(idx->field_0);
                    sub_473eac(idx->field_0);
                    sub_46c941(idx);
                    goto LABEL_474fb9;
                }
                else if (sub_473ac5(idx->field_0->field_4, idx->field_4))
                {
                    sub_46c941(idx->field_4);
                    sub_474084(idx->field_0);
                    sub_473eac(idx->field_0);
                    sub_46c941(idx);
LABEL_474fb9:
                    idx = NULL;
                }
                else
                {
                    idx->field_4->field_0 = 1;
                    idx->field_4->field_0 = 1;
                }
                return idx;
            }
        }
    }
    *((unsigned int *)sub_46e96f()) = 12;
    return NULL;
}
