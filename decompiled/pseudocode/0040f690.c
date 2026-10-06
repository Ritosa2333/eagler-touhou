// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40f690; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40f690_0 {
    char padding_0[148];
    unsigned int field_94;
    char padding_98[128];
    unsigned int field_118;
    unsigned int field_11c;
    unsigned int field_120;
    unsigned int field_124;
    unsigned int field_128;
    char padding_12c[4];
    unsigned int field_130;
} st_40f690_0;

void sub_40f690(st_40f690_0 *idx)
{
    unsigned int v1;  // edx
    unsigned int v2;  // esi
    st_40f690_0 *iter;  // eax
    unsigned int v4;  // edi
    unsigned int v5;  // edi
    unsigned int v6;  // eax
    unsigned int v7;  // edx

    v1 = idx->field_118;
    v2 = 1;
    idx->field_120 = 0;
    idx->field_130 = 0;
    iter = &idx->field_94;
    v4 = 32;
    do
    {
        v5 = v4;
        if ((char)v1 & 1)
        {
            *((unsigned int *)&iter->padding_0[0]) = *((int *)&iter->padding_0[0]) + 1;
            if (*((int *)&iter->padding_0[0]) >= 8)
                idx->field_130 = idx->field_130 | v2;
            if (*((int *)&iter->padding_0[0]) >= 26)
            {
                idx->field_120 = idx->field_120 | v2;
                *((unsigned int *)&iter->padding_0[0]) = *((int *)&iter->padding_0[0]) - 8;
            }
        }
        else
        {
            *((unsigned int *)&iter->padding_0[0]) = 0;
        }
    } while ((iter += 4, v1 >>= 1, v2 *= 2, v4 = v5 - 1, v5 != 1));
    v6 = idx->field_118;
    v7 = idx->field_11c ^ v6;
    idx->field_124 = v7 & v6;
    idx->field_128 = ~(v6) & v7;
    return;
}
