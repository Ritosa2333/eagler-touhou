// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44e130; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44e130_0 {
    char padding_0[256];
    unsigned int field_100;
    unsigned int field_104;
    int field_108;
    char padding_10c[4];
    int field_110;
    char padding_114[8];
    unsigned int field_11c;
    int field_120;
} st_44e130_0;

typedef struct st_44e130_2 {
    unsigned int field_0;
} st_44e130_2;

typedef struct st_44e130_1 {
    char padding_0[48];
    struct st_44e130_2 *field_30;
    struct st_44e130_2 *field_34;
} st_44e130_1;

typedef struct st_44e130_4 {
    char padding_0[56];
    struct st_44e130_2 *field_38;
} st_44e130_4;

char sub_44e130(st_44e130_1 **a0)
{
    st_44e130_0 *idx;  // esi
    unsigned int v11;  // edi
    int v12;  // edi
    int iter;  // [bp-0x58]
    st_44e130_1 **v1;  // [bp-0x54]
    st_44e130_1 **v2;  // [bp-0x4c]
    char v3;  // [bp-0x40]
    unsigned int v4;  // [bp-0x38]
    unsigned int v5;  // [bp-0x34]
    unsigned int v6;  // [bp-0x30]
    int v7;  // [bp-0x2c]
    st_44e130_1 *v8;  // [bp-0x20]
    st_44e130_4 **v9;  // [bp-0x14]

    if (!idx->field_11c)
        return 0;
    v2 = &v8;
    *(a0)->field_30(a0, &v8, v11);
    v6 = idx->field_104;
    v7 = idx->field_108;
    v4 = 0;
    v5 = 0;
    if (*(a0)->field_34(a0, &v3, &v4, 0))
        return 0;
    v12 = idx->field_120;
    v1 = a0;
    if (v4 == idx->field_100)
    {
        iter = 0;
        if (idx->field_108 > 0)
        {
            do
            {
                sub_47a330(v2, v12, idx->field_110);
                v2 += a0;
                iter += 1;
                v12 += idx->field_110;
            } while (iter < idx->field_108);
        }
    }
    *(v9)->field_38(v9);
    return 1;
}
