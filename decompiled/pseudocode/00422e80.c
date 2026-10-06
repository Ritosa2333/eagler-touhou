// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422e80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_422e80_0 {
    int field_0;
    int field_4;
    int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    int field_18;
} st_422e80_0;

int sub_422e80(unsigned int a0)
{
    int v2;  // edi
    st_422e80_0 *idx;  // esi
    int v4;  // 4098
    int v5;  // ecx
    int v6;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    if (v2 <= 0)
        return 0;
    v4 = idx->field_8;
    idx->field_14 = 0;
    if (v4)
        return 0;
    v5 = idx->field_0;
    if (!v5)
    {
        idx->field_c = 1;
        idx->field_10 = 1;
        idx->field_0 = v2;
        return 0;
    }
    v6 = idx->field_4;
    if (!v6)
    {
        idx->field_c = 2;
        idx->field_10 = 2;
        idx->field_4 = v2;
        return 0;
    }
    idx->field_8 = v2;
    idx->field_c = 3;
    idx->field_10 = 3;
    if (v5 == v2)
    {
        if (v6 == v2)
        {
            idx->field_18 = v2;
            return v2;
        }
    }
    else
    {
        if (v6 != v2 && v5 != v6)
        {
            idx->field_18 = -0x1;
            return -0x1;
        }
    }
    sub_421a60();
    idx->field_0 = idx->field_4;
    idx->field_8 = 0;
    idx->field_c = 2;
    idx->field_10 = 4;
    idx->field_4 = v2;
    return 0;
}
