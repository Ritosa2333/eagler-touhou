// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ce10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ce10_0 {
    char padding_0[8];
    int field_8;
    unsigned int field_c;
} st_44ce10_0;

int sub_44ce10(st_44ce10_0 *idx, unsigned int a1, int a2, int a3)
{
    int v0;  // eax
    int v1;  // eax
    int v2;  // eax

    if (a3)
    {
        v0 = a3 - 1;
        if (a3 != 1)
        {
            v1 = v0 - 1;
            if (v0 == 1)
            {
                a2 = a2;
                if (a2 <= 0)
                {
                    v1 = a2;
                    if (a2 < 0)
                        v1 = -(v1);
                    if (v1 < *((int *)&idx->padding_0[4]))
                    {
                        v1 = idx->field_c + *((int *)&idx->padding_0[4]);
                        v2 = v1 + a2;
                        idx->field_8 = v2;
                        return _INSERT(v2, 0, 1);
                    }
                }
            }
        }
        else
        {
            v1 = idx->field_8;
            if (idx->field_c + *((int *)&idx->padding_0[4]) - v1 > a2)
            {
                v2 = v1 + a2;
                idx->field_8 = v2;
                return _INSERT(v2, 0, 1);
            }
        }
    }
    else
    {
        v1 = a2;
        if (v1 >= 0 && v1 < *((int *)&idx->padding_0[4]))
        {
            idx->field_8 = idx->field_c + v1;
            return _INSERT(v1, 0, 1);
        }
    }
    return _INSERT(v1, 0, 0);
}
