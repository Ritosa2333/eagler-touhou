// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412050; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412050_0 {
    unsigned int field_0;
    char padding_4[8];
    unsigned int field_c;
    unsigned int field_10;
    char field_14;
} st_412050_0;

int sub_412050(void)
{
    st_412050_0 *idx;  // ecx
    unsigned int v2;  // eax
    unsigned int v3;  // esi
    int v4;  // edx
    unsigned int v5;  // eax

    if (idx->field_14 & 1)
    {
        idx->field_c = idx->field_c - v2;
        v3 = idx->field_10;
        v4 = (int)((unsigned int)(2454267027 * (idx->field_c - v3 * 7) >> 32) + idx->field_c - v3 * 7) >> 2;
        idx->field_0 = ((unsigned int)(v4) >> 31) + v4 + v3;
    }
    else
    {
        idx->field_0 = idx->field_0 - v5;
    }
    return;
}
