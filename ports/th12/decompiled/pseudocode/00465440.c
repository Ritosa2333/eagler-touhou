// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465440; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465440_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[276];
    unsigned int field_12c;
} st_465440_0;


void sub_465440(st_465440_0 *idx)
{
    unsigned int v1;  // edx
    unsigned int v2;  // esi
    st_465440_0 *iter;  // eax
    unsigned int v4;  // edi
    unsigned int v5;  // edi
    unsigned int v6;  // eax
    unsigned int v7;  // edx

    v1 = idx->field_0;
    v2 = 1;
    idx->field_8 = 0;
    idx->field_12c = 0;
    iter = &idx->field_14;
    v4 = 32;
    do
    {
        v5 = v4;
        if ((char)v1 & 1)
        {
            iter->field_0 = iter->field_0 + 1;
            if (iter->field_0 >= 8)
                idx->field_12c = idx->field_12c | v2;
            if (iter->field_0 >= 26)
            {
                idx->field_8 = idx->field_8 | v2;
                iter->field_0 = iter->field_0 - 8;
            }
        }
        else
        {
            iter->field_0 = 0;
        }
    } while ((iter += 4, v1 >>= 1, v2 *= 2, v4 = v5 - 1, v5 != 1));
    v6 = idx->field_0;
    v7 = idx->field_4 ^ v6;
    idx->field_c = v7 & v6;
    idx->field_10 = ~(v6) & v7;
    return;
}
